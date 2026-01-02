/**
  * @file    pwm_driver.cpp
  * @brief   PWM output driver implementation for STM32F407VET6
  */

#include "pwm_driver.h"
#include "stm32f407xx.h"

/* Timer register offsets */
#define TIM_CR1_OFFSET      0x00
#define TIM_CCMR1_OFFSET    0x18
#define TIM_CCMR2_OFFSET    0x1C
#define TIM_CCER_OFFSET     0x20
#define TIM_CNT_OFFSET      0x24
#define TIM_PSC_OFFSET      0x28
#define TIM_ARR_OFFSET      0x2C
#define TIM_CCR1_OFFSET     0x34
#define TIM_CCR2_OFFSET     0x38
#define TIM_CCR3_OFFSET     0x3C
#define TIM_CCR4_OFFSET     0x40

/* Timer register access macros */
#define TIM_CR1(base)       (*(volatile uint32_t *)((base) + TIM_CR1_OFFSET))
#define TIM_CCMR1(base)     (*(volatile uint32_t *)((base) + TIM_CCMR1_OFFSET))
#define TIM_CCMR2(base)     (*(volatile uint32_t *)((base) + TIM_CCMR2_OFFSET))
#define TIM_CCER(base)      (*(volatile uint32_t *)((base) + TIM_CCER_OFFSET))
#define TIM_CNT(base)       (*(volatile uint32_t *)((base) + TIM_CNT_OFFSET))
#define TIM_PSC(base)       (*(volatile uint32_t *)((base) + TIM_PSC_OFFSET))
#define TIM_ARR(base)       (*(volatile uint32_t *)((base) + TIM_ARR_OFFSET))
#define TIM_CCR(base, ch)   (*(volatile uint32_t *)((base) + TIM_CCR1_OFFSET + (ch * 4)))

/* Timer base addresses */
#define TIM1_BASE           0x40010000UL
#define TIM2_BASE           0x40000000UL
#define TIM3_BASE           0x40000400UL
#define TIM4_BASE           0x40000800UL
#define TIM5_BASE           0x40000C00UL
#define TIM8_BASE           0x40010400UL
#define TIM9_BASE           0x40014000UL
#define TIM10_BASE          0x40014400UL
#define TIM11_BASE          0x40014800UL
#define TIM12_BASE          0x40009000UL
#define TIM13_BASE          0x40009400UL
#define TIM14_BASE          0x40009800UL

/* RCC APB1 Timer Enable bits */
#define RCC_APB1ENR_TIM2EN  (1U << 0)
#define RCC_APB1ENR_TIM3EN  (1U << 1)
#define RCC_APB1ENR_TIM4EN  (1U << 2)
#define RCC_APB1ENR_TIM5EN  (1U << 3)
#define RCC_APB1ENR_TIM12EN (1U << 6)
#define RCC_APB1ENR_TIM13EN (1U << 7)
#define RCC_APB1ENR_TIM14EN (1U << 8)

/* RCC APB2 Timer Enable bits */
#define RCC_APB2ENR_TIM1EN  (1U << 0)
#define RCC_APB2ENR_TIM8EN  (1U << 1)
#define RCC_APB2ENR_TIM9EN  (1U << 16)
#define RCC_APB2ENR_TIM10EN (1U << 17)
#define RCC_APB2ENR_TIM11EN (1U << 18)

/* Timer Control Register 1 (CR1) bits */
#define TIM_CR1_CEN         (1U << 0)   /* Counter enable */

/* Timer Capture/Compare Mode Register bits */
#define TIM_CCMR_PWM_MODE1  (0x6U << 4) /* PWM mode 1 for channel 1,2 */
#define TIM_CCMR_PWM_MODE2  (0x7U << 4) /* PWM mode 2 for channel 1,2 */
#define TIM_CCMR_OC_PRE     (1U << 3)   /* Output compare preload enable */

/* Timer Capture/Compare Enable Register (CCER) bits */
#define TIM_CCER_CCxE(ch)   (1U << (ch * 4))      /* Capture/Compare output enable */
#define TIM_CCER_CCxP(ch)   (1U << ((ch * 4) + 1)) /* Polarity */

/**
 * @brief Enable timer clock based on timer base address
 */
static void EnableTimerClock(uint32_t timer_base) {
    if (timer_base == TIM1_BASE) {
        RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
    } else if (timer_base == TIM2_BASE) {
        RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    } else if (timer_base == TIM3_BASE) {
        RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    } else if (timer_base == TIM4_BASE) {
        RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
    } else if (timer_base == TIM5_BASE) {
        RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
    } else if (timer_base == TIM8_BASE) {
        RCC->APB2ENR |= RCC_APB2ENR_TIM8EN;
    } else if (timer_base == TIM9_BASE) {
        RCC->APB2ENR |= RCC_APB2ENR_TIM9EN;
    } else if (timer_base == TIM10_BASE) {
        RCC->APB2ENR |= RCC_APB2ENR_TIM10EN;
    } else if (timer_base == TIM11_BASE) {
        RCC->APB2ENR |= RCC_APB2ENR_TIM11EN;
    } else if (timer_base == TIM12_BASE) {
        RCC->APB1ENR |= RCC_APB1ENR_TIM12EN;
    } else if (timer_base == TIM13_BASE) {
        RCC->APB1ENR |= RCC_APB1ENR_TIM13EN;
    } else if (timer_base == TIM14_BASE) {
        RCC->APB1ENR |= RCC_APB1ENR_TIM14EN;
    }
}

/**
 * @brief Get timer clock frequency
 */
static uint32_t GetTimerClock(uint32_t timer_base) {
    /* APB2 timers run at 2x PCLK2 when prescaler != 1 */
    /* APB1 timers run at 2x PCLK1 when prescaler != 1 */
    /* For simplicity, assume 84MHz for APB1/APB2 (STM32F407 at 168MHz) */
    if (timer_base == TIM1_BASE || timer_base == TIM8_BASE || 
        timer_base == TIM9_BASE || timer_base == TIM10_BASE || 
        timer_base == TIM11_BASE) {
        return 84000000UL;  /* APB2 @ 84MHz */
    } else {
        return 84000000UL;  /* APB1 @ 84MHz */
    }
}

/**
 * @brief Configure GPIO pins for PWM output
 */
static void ConfigureGPIO(GPIO_TypeDef *gpio, uint8_t pin, uint8_t af_number) {
    /* Set pin to alternate function mode */
    gpio->MODER &= ~(0x3U << (pin * 2));
    gpio->MODER |= (0x2U << (pin * 2));
    
    /* Set to push-pull output type */
    gpio->OTYPER &= ~(1U << pin);
    
    /* Set speed to high */
    gpio->OSPEEDR |= (0x3U << (pin * 2));
    
    /* No pull-up or pull-down */
    gpio->PUPDR &= ~(0x3U << (pin * 2));
    
    /* Set alternate function */
    if (pin < 8) {
        gpio->AFR[0] &= ~(0xFU << (pin * 4));
        gpio->AFR[0] |= (af_number << (pin * 4));
    } else {
        gpio->AFR[1] &= ~(0xFU << ((pin - 8) * 4));
        gpio->AFR[1] |= (af_number << ((pin - 8) * 4));
    }
}

/**
 * @brief Initialize PWM output
 */
bool PWM_Init(PWM_Handle *handle, const PWM_Config *config) {
    if (handle == nullptr || config == nullptr) {
        return false;
    }
    
    if (config->channel > PWM_CHANNEL_4 || config->frequency_hz == 0) {
        return false;
    }
    
    /* Copy configuration */
    handle->config = *config;
    handle->timer_clock_hz = GetTimerClock(config->timer_base);
    
    /* Enable timer clock */
    EnableTimerClock(config->timer_base);
    
    /* Enable GPIO clock */
    uint32_t gpio_offset = (config->gpio_base - GPIOA_BASE) / 0x400;
    RCC->AHB1ENR |= (1U << gpio_offset);
    
    /* Configure GPIO pin */
    ConfigureGPIO((GPIO_TypeDef *)config->gpio_base, config->pin, config->af_number);
    
    /* Disable timer */
    TIM_CR1(config->timer_base) &= ~TIM_CR1_CEN;
    
    /* Calculate period and prescaler */
    uint32_t prescaler = 0;
    handle->period = (handle->timer_clock_hz / (config->frequency_hz * (prescaler + 1))) - 1;
    
    /* If period is too large, increase prescaler */
    while (handle->period > 0xFFFF && prescaler < 0xFFFF) {
        prescaler++;
        handle->period = (handle->timer_clock_hz / (config->frequency_hz * (prescaler + 1))) - 1;
    }
    
    if (handle->period > 0xFFFF) {
        return false;  /* Frequency too low */
    }
    
    /* Set prescaler */
    TIM_PSC(config->timer_base) = prescaler;
    
    /* Set auto-reload value */
    TIM_ARR(config->timer_base) = handle->period;
    
    /* Configure PWM mode */
    if (config->channel < 2) {
        /* Configure CCMR1 for channels 1 and 2 */
        uint32_t ccmr1 = TIM_CCMR1(config->timer_base);
        ccmr1 &= ~(0xFF << (config->channel * 8));
        ccmr1 |= (TIM_CCMR_PWM_MODE1 | TIM_CCMR_OC_PRE) << (config->channel * 8);
        TIM_CCMR1(config->timer_base) = ccmr1;
    } else {
        /* Configure CCMR2 for channels 3 and 4 */
        uint32_t ccmr2 = TIM_CCMR2(config->timer_base);
        ccmr2 &= ~(0xFF << ((config->channel - 2) * 8));
        ccmr2 |= (TIM_CCMR_PWM_MODE1 | TIM_CCMR_OC_PRE) << ((config->channel - 2) * 8);
        TIM_CCMR2(config->timer_base) = ccmr2;
    }
    
    /* Enable output for the channel */
    TIM_CCER(config->timer_base) |= TIM_CCER_CCxE(config->channel);
    
    /* Set initial compare value (0% duty) */
    TIM_CCR(config->timer_base, config->channel) = 0;
    
    /* Enable timer */
    TIM_CR1(config->timer_base) |= TIM_CR1_CEN;
    
    return true;
}

/**
 * @brief Set PWM duty cycle (0-100%)
 */
void PWM_SetDuty(PWM_Handle *handle, float duty_percent) {
    if (handle == nullptr) {
        return;
    }
    
    /* Clamp duty cycle to 0-100% */
    if (duty_percent < 0.0f) {
        duty_percent = 0.0f;
    } else if (duty_percent > 100.0f) {
        duty_percent = 100.0f;
    }
    
    /* Calculate compare value */
    uint32_t compare_value = (uint32_t)((handle->period + 1) * duty_percent / 100.0f);
    
    /* Set compare register */
    TIM_CCR(handle->config.timer_base, handle->config.channel) = compare_value;
}

/**
 * @brief Set PWM duty cycle using raw value
 */
void PWM_SetCompare(PWM_Handle *handle, uint32_t value) {
    if (handle == nullptr) {
        return;
    }
    
    /* Clamp value to period */
    if (value > handle->period + 1) {
        value = handle->period + 1;
    }
    
    /* Set compare register */
    TIM_CCR(handle->config.timer_base, handle->config.channel) = value;
}

/**
 * @brief Get current PWM duty cycle
 */
float PWM_GetDuty(PWM_Handle *handle) {
    if (handle == nullptr) {
        return 0.0f;
    }
    
    uint32_t compare_value = TIM_CCR(handle->config.timer_base, handle->config.channel);
    
    return (100.0f * compare_value) / (handle->period + 1);
}

/**
 * @brief Set PWM frequency
 */
bool PWM_SetFrequency(PWM_Handle *handle, uint32_t frequency_hz) {
    if (handle == nullptr || frequency_hz == 0) {
        return false;
    }
    
    /* Disable timer */
    TIM_CR1(handle->config.timer_base) &= ~TIM_CR1_CEN;
    
    /* Calculate period and prescaler */
    uint32_t prescaler = 0;
    uint32_t period = (handle->timer_clock_hz / (frequency_hz * (prescaler + 1))) - 1;
    
    /* If period is too large, increase prescaler */
    while (period > 0xFFFF && prescaler < 0xFFFF) {
        prescaler++;
        period = (handle->timer_clock_hz / (frequency_hz * (prescaler + 1))) - 1;
    }
    
    if (period > 0xFFFF) {
        /* Re-enable timer and return false */
        TIM_CR1(handle->config.timer_base) |= TIM_CR1_CEN;
        return false;  /* Frequency too low */
    }
    
    /* Update configuration */
    handle->config.frequency_hz = frequency_hz;
    handle->period = period;
    
    /* Set prescaler and period */
    TIM_PSC(handle->config.timer_base) = prescaler;
    TIM_ARR(handle->config.timer_base) = period;
    
    /* Re-enable timer */
    TIM_CR1(handle->config.timer_base) |= TIM_CR1_CEN;
    
    return true;
}

/**
 * @brief Enable PWM output
 */
void PWM_Enable(PWM_Handle *handle) {
    if (handle != nullptr) {
        TIM_CR1(handle->config.timer_base) |= TIM_CR1_CEN;
    }
}

/**
 * @brief Disable PWM output
 */
void PWM_Disable(PWM_Handle *handle) {
    if (handle != nullptr) {
        TIM_CR1(handle->config.timer_base) &= ~TIM_CR1_CEN;
    }
}

/**
 * @brief Check if PWM is enabled
 */
bool PWM_IsEnabled(PWM_Handle *handle) {
    if (handle == nullptr) {
        return false;
    }
    
    return (TIM_CR1(handle->config.timer_base) & TIM_CR1_CEN) != 0;
}
