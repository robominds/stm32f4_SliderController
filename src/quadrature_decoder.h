/**
  * @file    quadrature_decoder.h
  * @brief   Quadrature encoder decoder for STM32F407VET6
  *          Uses hardware timer in encoder mode for accurate position tracking
  */

#ifndef __QUADRATURE_DECODER_H
#define __QUADRATURE_DECODER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Encoder configuration structure
 */
typedef struct {
    uint32_t timer_base;        /* Timer peripheral base address */
    uint32_t gpio_base;         /* GPIO port base address */
    uint8_t pin_a;              /* GPIO pin for channel A */
    uint8_t pin_b;              /* GPIO pin for channel B */
    uint8_t af_number;          /* Alternate function number */
    uint16_t counts_per_rev;    /* Encoder counts per revolution */
} QuadEncoder_Config;

/**
 * @brief Encoder handle structure
 */
typedef struct {
    QuadEncoder_Config config;
    int32_t position;           /* Current position */
    int32_t last_count;         /* Last raw counter value */
} QuadEncoder_Handle;

/**
 * @brief Initialize quadrature encoder
 * @param handle: Pointer to encoder handle
 * @param config: Pointer to configuration structure
 * @retval true if successful, false otherwise
 */
bool QuadEncoder_Init(QuadEncoder_Handle *handle, const QuadEncoder_Config *config);

/**
 * @brief Get current encoder position
 * @param handle: Pointer to encoder handle
 * @retval Current position in counts
 */
int32_t QuadEncoder_GetPosition(QuadEncoder_Handle *handle);

/**
 * @brief Reset encoder position to zero
 * @param handle: Pointer to encoder handle
 */
void QuadEncoder_Reset(QuadEncoder_Handle *handle);

/**
 * @brief Set encoder position to specific value
 * @param handle: Pointer to encoder handle
 * @param position: New position value
 */
void QuadEncoder_SetPosition(QuadEncoder_Handle *handle, int32_t position);

/**
 * @brief Get encoder direction
 * @param handle: Pointer to encoder handle
 * @retval 1 for forward, -1 for reverse, 0 if not moving
 */
int8_t QuadEncoder_GetDirection(QuadEncoder_Handle *handle);

/**
 * @brief Get encoder speed (counts per second)
 * @param handle: Pointer to encoder handle
 * @param time_ms: Time interval in milliseconds for measurement
 * @retval Speed in counts per second
 */
float QuadEncoder_GetSpeed(QuadEncoder_Handle *handle, uint32_t time_ms);

#ifdef __cplusplus
}
#endif

#endif /* __QUADRATURE_DECODER_H */
