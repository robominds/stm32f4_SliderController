/**
  * @file    system_stm32f4xx.h
  * @brief   CMSIS Cortex-M4 Device System Header File for STM32F4xx devices.
  */

#ifndef __SYSTEM_STM32F4XX_H
#define __SYSTEM_STM32F4XX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* System Clock Frequency (Core Clock) */
extern uint32_t SystemCoreClock;

/**
  * @brief  Setup the microcontroller system
  *         Initialize the FPU setting, vector table location and External memory 
  *         configuration.
  */
extern void SystemInit(void);

/**
  * @brief  Update SystemCoreClock variable according to Clock Register Values.
  */
extern void SystemCoreClockUpdate(void);

#ifdef __cplusplus
}
#endif

#endif /* __SYSTEM_STM32F4XX_H */
