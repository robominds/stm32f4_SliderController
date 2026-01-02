/**
  * @file    pwm_driver.h
  * @brief   PWM output driver for STM32F407VET6
  *          Uses hardware timers in PWM mode
  */

#ifndef __PWM_DRIVER_H
#define __PWM_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief PWM Channel enumeration
 */
typedef enum {
    PWM_CHANNEL_1 = 0,
    PWM_CHANNEL_2 = 1,
    PWM_CHANNEL_3 = 2,
    PWM_CHANNEL_4 = 3
} PWM_Channel;

/**
 * @brief PWM Configuration structure
 */
typedef struct {
    uint32_t timer_base;        /* Timer peripheral base address */
    uint32_t gpio_base;         /* GPIO port base address */
    uint8_t pin;                /* GPIO pin number */
    uint8_t af_number;          /* Alternate function number */
    PWM_Channel channel;        /* Timer channel (1-4) */
    uint32_t frequency_hz;      /* PWM frequency in Hz */
} PWM_Config;

/**
 * @brief PWM Handle structure
 */
typedef struct {
    PWM_Config config;
    uint32_t timer_clock_hz;    /* Timer clock frequency */
    uint32_t period;            /* Auto-reload value (ARR) */
} PWM_Handle;

/**
 * @brief Initialize PWM output
 * @param handle: Pointer to PWM handle
 * @param config: Pointer to configuration structure
 * @retval true if successful, false otherwise
 */
bool PWM_Init(PWM_Handle *handle, const PWM_Config *config);

/**
 * @brief Set PWM duty cycle (0-100%)
 * @param handle: Pointer to PWM handle
 * @param duty_percent: Duty cycle percentage (0-100)
 */
void PWM_SetDuty(PWM_Handle *handle, float duty_percent);

/**
 * @brief Set PWM duty cycle using raw value (0-period)
 * @param handle: Pointer to PWM handle
 * @param value: Raw compare value (0 to period)
 */
void PWM_SetCompare(PWM_Handle *handle, uint32_t value);

/**
 * @brief Get current PWM duty cycle
 * @param handle: Pointer to PWM handle
 * @retval Duty cycle percentage (0-100)
 */
float PWM_GetDuty(PWM_Handle *handle);

/**
 * @brief Set PWM frequency
 * @param handle: Pointer to PWM handle
 * @param frequency_hz: Desired frequency in Hz
 * @retval true if frequency was set successfully
 */
bool PWM_SetFrequency(PWM_Handle *handle, uint32_t frequency_hz);

/**
 * @brief Enable PWM output
 * @param handle: Pointer to PWM handle
 */
void PWM_Enable(PWM_Handle *handle);

/**
 * @brief Disable PWM output
 * @param handle: Pointer to PWM handle
 */
void PWM_Disable(PWM_Handle *handle);

/**
 * @brief Check if PWM is enabled
 * @param handle: Pointer to PWM handle
 * @retval true if PWM is enabled, false otherwise
 */
bool PWM_IsEnabled(PWM_Handle *handle);

#ifdef __cplusplus
}
#endif

#endif /* __PWM_DRIVER_H */
