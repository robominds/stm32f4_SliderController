/**
  * @file    led.h
  * @brief   LED driver for STM32F407VET6 Black Board
  *          Controls LEDs on PA6 and PA7 using FreeRTOS tasks
  */

#ifndef LED_H
#define LED_H

#ifdef __cplusplus
extern "C" {
#endif

#include "FreeRTOS.h"
#include "task.h"

/**
 * @brief Initialize GPIO pins for LED control
 *        Configures PA6 and PA7 as output pins
 */
void LED_Init(void);

/**
 * @brief Create LED1 task
 * @return pdPASS if task created successfully, errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY otherwise
 */
BaseType_t LED_CreateTask1(void);

/**
 * @brief Create LED2 task
 * @return pdPASS if task created successfully, errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY otherwise
 */
BaseType_t LED_CreateTask2(void);

/**
 * @brief LED1 Task - Blinks LED on PA6
 * @param pvParameters Task parameters (unused)
 */
void vLED1Task(void *pvParameters);

/**
 * @brief LED2 Task - Blinks LED on PA7
 * @param pvParameters Task parameters (unused)
 */
void vLED2Task(void *pvParameters);

#ifdef __cplusplus
}
#endif

#endif /* LED_H */
