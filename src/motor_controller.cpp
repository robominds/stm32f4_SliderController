/**
 * @file    motor_controller.cpp
 * @brief   Motor controller wrapper for PWM output + quadrature encoder feedback.
 */

#include "motor_controller.h"
#include "stm32f407xx.h"
#include "uart_driver.h"
#include "FreeRTOS.h" // IWYU pragma: keep - Must include FreeRTOS.h before task.h
#include "task.h"
#include <stdio.h>

MotorController::MotorController() = default;

bool MotorController::init(const PWM_Config pwm_cfg[], const QuadEncoder_Config &enc_cfg) {
    if (!PWM_Init(&pwm_[0], &pwm_cfg[0])) {
        return false;
    }
    if (!PWM_Init(&pwm_[1], &pwm_cfg[1])) {
        return false;
    }
    if (!QuadEncoder_Init(&encoder_, &enc_cfg)) {
        return false;
    }
    initialized_ = true;
    limit_switches_enabled_ = false;
    return true;
}

bool MotorController::init(const PWM_Config pwm_cfg[], const QuadEncoder_Config &enc_cfg, const LimitSwitch_Config &limit_cfg) {
    if (!init(pwm_cfg, enc_cfg)) {
        return false;
    }
    
    limit_config_ = limit_cfg;
    initLimitSwitchGPIO();
    limit_switches_enabled_ = true;
    
    return true;
}

void MotorController::enable() {
    if (!initialized_) return;
    PWM_Enable(&pwm_[0]);
    PWM_Enable(&pwm_[1]);
}

void MotorController::disable() {
    if (!initialized_) return;
    PWM_Disable(&pwm_[0]);
    PWM_Disable(&pwm_[1]);
}

void MotorController::setDuty(uint32_t chan, float duty_percent) {
    if (!initialized_) return;
    PWM_SetDuty(&pwm_[chan], duty_percent);
}

float MotorController::getDuty(uint32_t chan) const {
    if (!initialized_) return 0.0f;
    return PWM_GetDuty(const_cast<PWM_Handle *>(&pwm_[chan]));
}

int32_t MotorController::getPositionCounts() {
    if (!initialized_) return 0;
    return QuadEncoder_GetPosition(&encoder_);
}

float MotorController::getPositionRevolutions() {
    if (!initialized_ || encoder_.config.counts_per_rev == 0) {
        return 0.0f;
    }
    return static_cast<float>(getPositionCounts()) /
           static_cast<float>(encoder_.config.counts_per_rev);
}

void MotorController::resetPosition() {
    if (!initialized_) return;
    QuadEncoder_Reset(&encoder_);
}

void MotorController::setPosition(int32_t counts) {
    if (!initialized_) return;
    QuadEncoder_SetPosition(&encoder_, counts);
}

void MotorController::initLimitSwitchGPIO() {
    /* Enable GPIO clock */
    uint32_t gpio_offset = (limit_config_.gpio_base - GPIOA_BASE) / 0x400;
    RCC->AHB1ENR |= (1U << gpio_offset);
    
    GPIO_TypeDef *gpio = reinterpret_cast<GPIO_TypeDef *>(static_cast<uintptr_t>(limit_config_.gpio_base));
    
    /* Configure pins as inputs */
    gpio->MODER &= ~(0x3U << (limit_config_.pin_min * 2));
    gpio->MODER &= ~(0x3U << (limit_config_.pin_max * 2));
    
    /* Configure pull-up/pull-down */
    if (limit_config_.enable_pullup) {
        /* Enable pull-up resistors */
        gpio->PUPDR &= ~(0x3U << (limit_config_.pin_min * 2));
        gpio->PUPDR |= (0x1U << (limit_config_.pin_min * 2));
        
        gpio->PUPDR &= ~(0x3U << (limit_config_.pin_max * 2));
        gpio->PUPDR |= (0x1U << (limit_config_.pin_max * 2));
    } else {
        /* No pull-up/pull-down */
        gpio->PUPDR &= ~(0x3U << (limit_config_.pin_min * 2));
        gpio->PUPDR &= ~(0x3U << (limit_config_.pin_max * 2));
    }
}

bool MotorController::readLimitSwitch(uint8_t pin) const {
    if (!limit_switches_enabled_) {
        return false;
    }
    
    GPIO_TypeDef *gpio = reinterpret_cast<GPIO_TypeDef *>(static_cast<uintptr_t>(limit_config_.gpio_base));
    bool pin_state = (gpio->IDR & (1U << pin)) != 0;
    
    /* If active_low, invert the reading */
    if (limit_config_.active_low) {
        pin_state = !pin_state;
    }
    
    return pin_state;
}

bool MotorController::isMinLimitActive() const {
    bool active = readLimitSwitch(limit_config_.pin_min);
    
    /* Simple debouncing */
    if (active) {
        min_limit_debounce_count_++;
        if (min_limit_debounce_count_ >= DEBOUNCE_THRESHOLD) {
            min_limit_debounce_count_ = DEBOUNCE_THRESHOLD;
            return true;
        }
    } else {
        min_limit_debounce_count_ = 0;
    }
    
    return false;
}

bool MotorController::isMaxLimitActive() const {
    bool active = readLimitSwitch(limit_config_.pin_max);
    
    /* Simple debouncing */
    if (active) {
        max_limit_debounce_count_++;
        if (max_limit_debounce_count_ >= DEBOUNCE_THRESHOLD) {
            max_limit_debounce_count_ = DEBOUNCE_THRESHOLD;
            return true;
        }
    } else {
        max_limit_debounce_count_ = 0;
    }
    
    return false;
}

bool MotorController::isAnyLimitActive() const {
    return isMinLimitActive() || isMaxLimitActive();
}

void MotorController::setLimitSwitchEnabled(bool enabled) {
    limit_switches_enabled_ = enabled;
}

bool MotorController::checkLimitSafety(float duty_chan0, float duty_chan1) const {
    if (!limit_switches_enabled_) {
        return true;  /* No limits, allow motion */
    }
    
    /* Check if motion would violate limits */
    /* Assuming chan0 is reverse/negative direction, chan1 is forward/positive */
    bool min_active = isMinLimitActive();
    bool max_active = isMaxLimitActive();
    
    /* If at minimum limit, don't allow reverse motion (chan0) */
    if (min_active && duty_chan0 > 0.1f) {
        return false;
    }
    
    /* If at maximum limit, don't allow forward motion (chan1) */
    if (max_active && duty_chan1 > 0.1f) {
        return false;
    }
    
    return true;
}

bool MotorController::homeToMinLimit(float homing_duty, uint32_t timeout_ms) {
    if (!initialized_ || !limit_switches_enabled_) {
        return false;
    }
    
    char msg[80];

    if(isMinLimitActive()) {
        UART_WriteString("Error Minimum limit switch not active at start of homing\r\n");
        disable();
        return false;
    }   

    if(isMaxLimitActive()) {
        UART_WriteString("Error Maximum limit switch not active at start of homing\r\n");
        disable();
        return false;
    }   

    snprintf(msg, sizeof(msg), "Homing to minimum limit at %.1f%% duty...\r\n", homing_duty);
    UART_WriteString(msg);
    
    /* If already at limit, back off slightly */
    if (isMinLimitActive()) {
        UART_WriteString("Already at min limit, backing off...\r\n");
        enable();
        setDuty(1, 10.0f);  /* Move forward */
        setDuty(0, 0.0f);
        vTaskDelay(pdMS_TO_TICKS(500));
        setDuty(1, 0.0f);
        
        /* Wait for limit to clear */
        uint32_t wait_count = 0;
        while ((isMinLimitActive() && !isMaxLimitActive()) && wait_count < 100) {
            vTaskDelay(pdMS_TO_TICKS(10));
            wait_count++;
        }
    }
    
    if(isMinLimitActive()) {
        UART_WriteString("Error Minimum limit switch not active at end of homing\r\n");
        disable();
        return false;
    }   

    if(isMaxLimitActive()) {
        UART_WriteString("Error Maximum limit switch not active at end of homing\r\n");
        disable();
        return false;
    }   

    /* Move toward minimum limit */
    enable();
    setDuty(0, homing_duty);  /* Reverse direction */
    setDuty(1, 0.0f);
    
    uint32_t start_time = xTaskGetTickCount();
    uint32_t timeout_ticks = pdMS_TO_TICKS(timeout_ms);
    
    /* Wait until limit is hit or timeout */
    while (!isMinLimitActive() && !isMaxLimitActive()) {
        if ((xTaskGetTickCount() - start_time) > timeout_ticks) {
            setDuty(0, 0.0f);
            disable();
            UART_WriteString("ERROR: Homing timeout\r\n");
            return false;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    
    /* Stop motion */
    setDuty(0, 0.0f);
    
    if(!isMinLimitActive() && isMaxLimitActive()) {
        UART_WriteString("ERROR: Hit maximum limit during homing\r\n");
        return false;
    } else { 
        /* Reset encoder position to zero at limit */
        resetPosition();
        UART_WriteString("Homing complete, position reset to zero\r\n");
    }
    return true;
}

bool MotorController::startPositionControlTask(float kp,
                                               float kd,
                                               uint32_t sample_time_ms,
                                               int32_t target_counts,
                                               float max_duty_percent) {
    if (!initialized_) {
        return false;
    }

    if (position_task_ != nullptr) {
        setTargetPositionCounts(target_counts);
        kp_ = kp;
        kd_ = kd;
        sample_time_ms_ = sample_time_ms == 0 ? 1U : sample_time_ms;
        max_duty_percent_ = max_duty_percent < 0.0f ? 0.0f : (max_duty_percent > 100.0f ? 100.0f : max_duty_percent);
        return true;
    }

    kp_ = kp;
    sample_time_ms_ = sample_time_ms == 0 ? 1U : sample_time_ms;
    target_counts_ = target_counts;
    max_duty_percent_ = max_duty_percent < 0.0f ? 0.0f : (max_duty_percent > 100.0f ? 100.0f : max_duty_percent);

    enable();

    BaseType_t created = xTaskCreate(PositionTaskThunk,
                                     "motor_pos",
                                     configMINIMAL_STACK_SIZE + 128,
                                     this,
                                     tskIDLE_PRIORITY + 2,
                                     &position_task_);
    if (created != pdPASS) {
        position_task_ = nullptr;
        disable();
        return false;
    }

    return true;
}

void MotorController::stopPositionControlTask() {
    if (position_task_ != nullptr) {
        vTaskDelete(position_task_);
        position_task_ = nullptr;
    }
    for(uint32_t chan = 0; chan < 2; ++chan) {
        setDuty(chan, 0.0f);
    }
    disable();
}

void MotorController::setTargetPositionCounts(int32_t target_counts) {
    target_counts_ = target_counts;
}

int32_t MotorController::getTargetPositionCounts() const {
    return target_counts_;
}

bool MotorController::positionTaskRunning() const {
    return position_task_ != nullptr;
}

void MotorController::PositionTaskThunk(void *param) {
    if (param == nullptr) {
        vTaskDelete(nullptr);
        return;
    }
    static_cast<MotorController *>(param)->positionTaskLoop();
}

void MotorController::positionTaskLoop() {
    static int32_t prevPosition = getPositionCounts();
    const TickType_t delay_ticks = pdMS_TO_TICKS(sample_time_ms_);

    if(!homeToMinLimit(20.0f, 15000)) {  /* Optional homing at start */
        UART_WriteString("ERROR: Homing failed\r\n");
        disable();
        vTaskDelete(nullptr);
    }

    for (;;) {
        int32_t position = getPositionCounts();
        int32_t error = target_counts_ - position;

        int32_t deltaPosition = position - prevPosition;
        prevPosition = position;
        float speed_cps = static_cast<float>(deltaPosition) * (1000.0f / static_cast<float>(sample_time_ms_));

        /* Simple proportional control; assumes external hardware handles direction. */
        float duty = kp_ * static_cast<float>(error) - (kd_ * speed_cps);  /* Simple velocity damping */ 

        float duty_chan0 = 0.0f;
        float duty_chan1 = 0.0f;
        
        if(duty < 0.0) {
            duty_chan0 = -duty;
        } else {
            duty_chan1 = duty;
        }
        
        /* Check limit switch safety before applying duty */
        if (checkLimitSafety(duty_chan0, duty_chan1)) {
            setDuty(0, duty_chan0);
            setDuty(1, duty_chan1);
        } else {
            /* Limit violated, stop motor */
            setDuty(0, 0.0f);
            setDuty(1, 0.0f);
        }

        vTaskDelay(delay_ticks);
    }
}
