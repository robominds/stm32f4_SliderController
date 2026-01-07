/**
 * @file    motor_controller.cpp
 * @brief   Motor controller wrapper for PWM output + quadrature encoder feedback.
 */

#include "motor_controller.h"
#include "FreeRTOS.h" // IWYU pragma: keep - Must include FreeRTOS.h before task.h
#include "task.h"

MotorController::MotorController() = default;

bool MotorController::init(const PWM_Config &pwm_cfg, const QuadEncoder_Config &enc_cfg) {
    if (!PWM_Init(&pwm_, &pwm_cfg)) {
        return false;
    }
    if (!QuadEncoder_Init(&encoder_, &enc_cfg)) {
        return false;
    }
    initialized_ = true;
    return true;
}

void MotorController::enable() {
    if (!initialized_) return;
    PWM_Enable(&pwm_);
}

void MotorController::disable() {
    if (!initialized_) return;
    PWM_Disable(&pwm_);
}

void MotorController::setDuty(float duty_percent) {
    if (!initialized_) return;
    PWM_SetDuty(&pwm_, duty_percent);
}

float MotorController::getDuty() const {
    if (!initialized_) return 0.0f;
    return PWM_GetDuty(const_cast<PWM_Handle *>(&pwm_));
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

bool MotorController::startPositionControlTask(float kp,
                                               uint32_t sample_time_ms,
                                               int32_t target_counts,
                                               float max_duty_percent) {
    if (!initialized_) {
        return false;
    }

    if (position_task_ != nullptr) {
        setTargetPositionCounts(target_counts);
        kp_ = kp;
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
    setDuty(0.0f);
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
    const TickType_t delay_ticks = pdMS_TO_TICKS(sample_time_ms_);

    for (;;) {
        int32_t position = getPositionCounts();
        int32_t error = target_counts_ - position;

        /* Simple proportional control; assumes external hardware handles direction. */
        float duty = kp_ * static_cast<float>(error);
        if (duty < 0.0f) {
            duty = 0.0f;
        }
        if (duty > max_duty_percent_) {
            duty = max_duty_percent_;
        }

        //setDuty(duty);
        setDuty(getPositionCounts()/10.0 + 50.0f);

        vTaskDelay(delay_ticks);
    }
}
