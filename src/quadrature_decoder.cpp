/**
  * @file    quadrature_decoder.cpp
  * @brief   Quadrature encoder decoder implementation for STM32F407VET6
  */

#include "quadrature_decoder.h"
#include "stm32f407xx.h"
#include "timer_common.h"

/* Timer Control Register 1 (CR1) bits */
#define TIM_CR1_CEN         (1U << 0)   /* Counter enable */

/* Timer Slave Mode Control Register (SMCR) bits */
#define TIM_SMCR_SMS_ENC3   (3U << 0)   /* Encoder mode 3 (both edges) */

/* Timer Capture/Compare Mode Register 1 (CCMR1) bits */
#define TIM_CCMR1_CC1S_TI1  (1U << 0)   /* CC1 channel is input, IC1 mapped on TI1 */
#define TIM_CCMR1_CC2S_TI2  (1U << 8)   /* CC2 channel is input, IC2 mapped on TI2 */
#define TIM_CCMR1_IC1F_8    (3U << 4)   /* Input capture 1 filter: fSAMPLING=fDTS, N=8 */
#define TIM_CCMR1_IC2F_8    (3U << 12)  /* Input capture 2 filter: fSAMPLING=fDTS, N=8 */

/* Timer Capture/Compare Enable Register (CCER) bits */
#define TIM_CCER_CC1E       (1U << 0)   /* Capture/Compare 1 output enable */
#define TIM_CCER_CC2E       (1U << 4)   /* Capture/Compare 2 output enable */
#define TIM_CCER_CC1P       (1U << 1)   /* Capture/Compare 1 output polarity */
#define TIM_CCER_CC2P       (1U << 5)   /* Capture/Compare 2 output polarity */

/**
 * @brief Enable timer clock based on timer base address
 */
static void EnableTimerClock(uint32_t timer_base) {
    if (timer_base == TIM2_BASE) {
        RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    } else if (timer_base == TIM3_BASE) {
        RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    } else if (timer_base == TIM4_BASE) {
        RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
    } else if (timer_base == TIM5_BASE) {
        RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
    }
}

/**
 * @brief Configure GPIO pins for encoder inputs
 */
static void ConfigureEncoderGPIO(GPIO_TypeDef *gpio, uint8_t pin_a, uint8_t pin_b, uint8_t af_number) {
    /* Set pins to alternate function mode */
    gpio->MODER &= ~(0x3U << (pin_a * 2));
    gpio->MODER |= (0x2U << (pin_a * 2));
    
    gpio->MODER &= ~(0x3U << (pin_b * 2));
    gpio->MODER |= (0x2U << (pin_b * 2));
    
    /* Set to push-pull output type */
    gpio->OTYPER &= ~(1U << pin_a);
    gpio->OTYPER &= ~(1U << pin_b);
    
    /* Set speed to high */
    gpio->OSPEEDR |= (0x3U << (pin_a * 2));
    gpio->OSPEEDR |= (0x3U << (pin_b * 2));
    
    /* Set pull-up */
    gpio->PUPDR &= ~(0x3U << (pin_a * 2));
    gpio->PUPDR |= (0x1U << (pin_a * 2));
    
    gpio->PUPDR &= ~(0x3U << (pin_b * 2));
    gpio->PUPDR |= (0x1U << (pin_b * 2));
    
    /* Set alternate function */
    if (pin_a < 8) {
        gpio->AFR[0] &= ~(0xFU << (pin_a * 4));
        gpio->AFR[0] |= (af_number << (pin_a * 4));
    } else {
        gpio->AFR[1] &= ~(0xFU << ((pin_a - 8) * 4));
        gpio->AFR[1] |= (af_number << ((pin_a - 8) * 4));
    }
    
    if (pin_b < 8) {
        gpio->AFR[0] &= ~(0xFU << (pin_b * 4));
        gpio->AFR[0] |= (af_number << (pin_b * 4));
    } else {
        gpio->AFR[1] &= ~(0xFU << ((pin_b - 8) * 4));
        gpio->AFR[1] |= (af_number << ((pin_b - 8) * 4));
    }
}

/**
 * @brief Initialize quadrature encoder
 */
bool QuadEncoder_Init(QuadEncoder_Handle *handle, const QuadEncoder_Config *config) {
    if (handle == nullptr || config == nullptr) {
        return false;
    }
    
    /* Copy configuration */
    handle->config = *config;
    handle->position = 0;
    handle->last_count = 0;
    
    /* Enable timer clock */
    EnableTimerClock(config->timer_base);
    
    /* Enable GPIO clock */
    uint32_t gpio_offset = (config->gpio_base - GPIOA_BASE) / 0x400;
    RCC->AHB1ENR |= (1U << gpio_offset);
    
    /* Configure GPIO pins */
    ConfigureEncoderGPIO(reinterpret_cast<GPIO_TypeDef *>(static_cast<uintptr_t>(config->gpio_base)),
                         config->pin_a,
                         config->pin_b,
                         config->af_number);
    
    /* Configure timer for encoder mode */
    /* Disable timer */
    TIM_CR1(config->timer_base) &= ~TIM_CR1_CEN;
    
    /* Set prescaler to 0 (no prescaling) */
    TIM_PSC(config->timer_base) = 0;
    
    /* Set auto-reload value to maximum (16-bit) */
    TIM_ARR(config->timer_base) = 0xFFFF;
    
    /* Configure inputs with filtering to reduce noise */
    /* CC1 channel is input, IC1 mapped on TI1 with filter */
    /* CC2 channel is input, IC2 mapped on TI2 with filter */
    /* ICxF = 0011b: fSAMPLING=fDTS, N=8 (good balance of filtering and response) */
    TIM_CCMR1(config->timer_base) = TIM_CCMR1_CC1S_TI1 | TIM_CCMR1_CC2S_TI2 | 
                                     TIM_CCMR1_IC1F_8 | TIM_CCMR1_IC2F_8;
    
    /* Enable capture/compare channels with non-inverted polarity */
    /* CC1P=0 and CC2P=0 for non-inverted (rising edge)d on TI1 */
    /* CC2 channel is input, IC2 mapped on TI2 */
    TIM_CCMR1(config->timer_base) = TIM_CCMR1_CC1S_TI1 | TIM_CCMR1_CC2S_TI2;
    
    /* Enable capture/compare channels */
    TIM_CCER(config->timer_base) = TIM_CCER_CC1E | TIM_CCER_CC2E;
    
    /* Set encoder mode 3 (count on both TI1 and TI2 edges) */
    TIM_SMCR(config->timer_base) = TIM_SMCR_SMS_ENC3;
    
    /* Reset counter */
    TIM_CNT(config->timer_base) = 0;
    
    /* Enable timer */
    TIM_CR1(config->timer_base) |= TIM_CR1_CEN;
    
    return true;
}

/**
 * @brief Get current encoder position
 */
int32_t QuadEncoder_GetPosition(QuadEncoder_Handle *handle) {
    if (handle == nullptr) {
        return 0;
    }
    
    /* Read current counter value */
    uint16_t current_count = (uint16_t)(TIM_CNT(handle->config.timer_base) & 0xFFFF);
    
    /* Calculate delta (handling overflow/underflow) */
    int16_t delta = (int16_t)(current_count - (uint16_t)handle->last_count);
    
    /* Update position */
    handle->position += delta;
    handle->last_count = current_count;
    
    return handle->position;
}

/**
 * @brief Reset encoder position to zero
 */
void QuadEncoder_Reset(QuadEncoder_Handle *handle) {
    if (handle != nullptr) {
        handle->position = 0;
        handle->last_count = TIM_CNT(handle->config.timer_base) & 0xFFFF;
    }
}

/**
 * @brief Set encoder position to specific value
 */
void QuadEncoder_SetPosition(QuadEncoder_Handle *handle, int32_t position) {
    if (handle != nullptr) {
        handle->position = position;
        handle->last_count = TIM_CNT(handle->config.timer_base) & 0xFFFF;
    }
}

/**
 * @brief Get encoder direction
 */
int8_t QuadEncoder_GetDirection(QuadEncoder_Handle *handle) {
    if (handle == nullptr) {
        return 0;
    }
    
    /* Read direction bit from control register */
    /* Bit 4 of CR1 is DIR bit (0 = up-counting, 1 = down-counting) */
    uint32_t cr1 = TIM_CR1(handle->config.timer_base);
    
    if (cr1 & (1U << 4)) {
        return -1;  /* Down-counting (reverse) */
    } else {
        return 1;   /* Up-counting (forward) */
    }
}

/**
 * @brief Get encoder speed (counts per second)
 */
float QuadEncoder_GetSpeed(QuadEncoder_Handle *handle, uint32_t time_ms) {
    if (handle == nullptr || time_ms == 0) {
        return 0.0f;
    }
    
    /* Get current position */
    int32_t current_pos = QuadEncoder_GetPosition(handle);
    
    /* This is a simplified implementation */
    /* In a real application, you would need to track position over time */
    /* and calculate the difference divided by the time interval */
    
    /* For now, return 0 - this would need proper timing implementation */
    // TODO: implement real speed calculation using the provided time_ms interval
    return current_pos*1.0;
}
