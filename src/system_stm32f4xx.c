/**
  * @file    system_stm32f4xx.c
  * @brief   CMSIS Cortex-M4 Device Peripheral Access Layer System Source File.
  */

#include <stdint.h>
#include "system_stm32f4xx.h"

/* System Clock Frequency (HSI = 16MHz) */
uint32_t SystemCoreClock = 16000000;

/* Constants for clock configuration */
#define HSI_VALUE    16000000U  /*!< Value of the Internal oscillator in Hz */
#define HSE_VALUE    8000000U   /*!< Value of the External oscillator in Hz (STM32F407VET6 Black Board typically uses 8MHz) */

/* RCC register addresses */
#define RCC_BASE              0x40023800UL
#define RCC_CR                (*(volatile uint32_t *)(RCC_BASE + 0x00))
#define RCC_PLLCFGR           (*(volatile uint32_t *)(RCC_BASE + 0x04))
#define RCC_CFGR              (*(volatile uint32_t *)(RCC_BASE + 0x08))
#define RCC_CIR               (*(volatile uint32_t *)(RCC_BASE + 0x0C))

/* FLASH register addresses */
#define FLASH_BASE            0x40023C00UL
#define FLASH_ACR             (*(volatile uint32_t *)(FLASH_BASE + 0x00))

/* SCB register addresses for FPU */
#define SCB_BASE              0xE000ED00UL
#define SCB_CPACR             (*(volatile uint32_t *)(SCB_BASE + 0x88))

/**
  * @brief  Setup the microcontroller system
  *         Initialize the FPU setting and vector table location.
  */
void SystemInit(void)
{
  /* FPU settings: Enable CP10 and CP11 coprocessors */
  #if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
    SCB_CPACR |= ((3UL << 10*2)|(3UL << 11*2));  /* set CP10 and CP11 Full Access */
  #endif

  /* Reset the RCC clock configuration to the default reset state */
  /* Set HSION bit */
  RCC_CR |= 0x00000001U;

  /* Reset CFGR register */
  RCC_CFGR = 0x00000000U;

  /* Reset HSEON, CSSON and PLLON bits */
  RCC_CR &= 0xFEF6FFFFU;

  /* Reset PLLCFGR register */
  RCC_PLLCFGR = 0x24003010U;

  /* Reset HSEBYP bit */
  RCC_CR &= 0xFFFBFFFFU;

  /* Disable all interrupts */
  RCC_CIR = 0x00000000U;

  /* Configure Flash prefetch, Instruction cache, Data cache and wait state */
  FLASH_ACR = 0x00000000U;
}

/**
  * @brief  Update SystemCoreClock variable according to Clock Register Values.
  */
void SystemCoreClockUpdate(void)
{
  uint32_t tmp = 0, pllm = 0, pllvco = 0, pllp = 2;
  
  /* Get SYSCLK source */
  tmp = RCC_CFGR & 0x0000000CU;
  
  switch (tmp)
  {
    case 0x00:  /* HSI used as system clock source */
      SystemCoreClock = HSI_VALUE;
      break;
    case 0x04:  /* HSE used as system clock source */
      SystemCoreClock = HSE_VALUE;
      break;
    case 0x08:  /* PLL used as system clock source */
      /* PLL_VCO = (HSE_VALUE or HSI_VALUE / PLLM) * PLLN
         SYSCLK = PLL_VCO / PLLP */
      pllm = RCC_PLLCFGR & 0x0000003FU;
      
      if ((RCC_PLLCFGR & 0x00400000U) != 0)
      {
        /* HSE used as PLL clock source */
        pllvco = (HSE_VALUE / pllm) * ((RCC_PLLCFGR & 0x00007FC0U) >> 6);
      }
      else
      {
        /* HSI used as PLL clock source */
        pllvco = (HSI_VALUE / pllm) * ((RCC_PLLCFGR & 0x00007FC0U) >> 6);
      }
      
      pllp = (((RCC_PLLCFGR & 0x00030000U) >> 16) + 1 ) * 2;
      SystemCoreClock = pllvco / pllp;
      break;
    default:
      SystemCoreClock = HSI_VALUE;
      break;
  }
}
