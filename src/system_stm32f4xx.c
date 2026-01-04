/**
  * @file    system_stm32f4xx.c
  * @brief   CMSIS Cortex-M4 Device Peripheral Access Layer System Source File.
  */

#include <stdint.h>
#include "system_stm32f4xx.h"

/* System Clock Frequency */
uint32_t SystemCoreClock = 168000000;

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

  /* Reset RCC configuration */
  RCC_CR |= 0x00000001U;      /* HSION */
  RCC_CFGR = 0x00000000U;
  RCC_CR &= 0xFEF6FFFFU;      /* HSEON, CSSON, PLLON off */
  RCC_PLLCFGR = 0x24003010U;  /* Reset value */
  RCC_CR &= 0xFFFBFFFFU;      /* HSEBYP off */
  RCC_CIR = 0x00000000U;

  /* Enable HSE (8 MHz crystal) */
  RCC_CR |= (1U << 16); /* HSEON */
  uint32_t hse_timeout = 0;
  while ((RCC_CR & (1U << 17)) == 0 && hse_timeout < 100000) {
    hse_timeout++;
  }
  
  if ((RCC_CR & (1U << 17)) == 0) {
    /* HSE failed to start - hang here for debugging */
    while(1);
  }

  /* Configure bus prescalers EARLY, before PLL, while on HSE */
  /* PPRE2 = 4 (100b = /2) at bits [15:13]
     PPRE1 = 5 (101b = /4) at bits [12:10]  
     HPRE  = 0 (0000b = /1) at bits [7:4] */
  volatile uint32_t *cfgr_addr = (volatile uint32_t *)(0x40023800UL + 0x08UL);
  *cfgr_addr = (4U << 13) | (5U << 10) | (0U << 4) | 0x0U;
  
  /* Read back to verify write took effect */
  volatile uint32_t readback1 = *cfgr_addr;
  (void)readback1;

  /* Configure Flash latency and caches for 168MHz */
  FLASH_ACR = (1U << 8) | (1U << 9) | (1U << 10) | (5U << 0); /* prefetch, ICACHE, DCACHE, 5 WS */

  /* PLL configuration for 8 MHz HSE:
   * VCO = 8MHz / 8 * 336 = 336MHz
   * SYSCLK = VCO / 2 = 168MHz
   * USB = VCO / 7 = 48MHz
   */
  uint32_t pllcfgr = 0;
  pllcfgr |= (8U & 0x3FU);          /* PLLM = 8 */
  pllcfgr |= (336U << 6);           /* PLLN = 336 */
  pllcfgr |= (0U << 16);            /* PLLP = 2 (00b) */
  pllcfgr |= (7U << 24);            /* PLLQ = 7 */
  pllcfgr |= (1U << 22);            /* PLL source = HSE */
  RCC_PLLCFGR = pllcfgr;

  /* Enable PLL */
  RCC_CR |= (1U << 24);
  while ((RCC_CR & (1U << 25)) == 0) {
    /* wait for PLLRDY */
  }

  /* Switch to PLL as system clock (SW=2), reapply prescalers explicitly */
  RCC_CFGR = (4U << 13) | (5U << 10) | (0U << 4) | 0x2U;
  
  while (((RCC_CFGR >> 2) & 0x3U) != 0x2U) {
    /* wait for PLL as system clock */
  }

  /* Verify prescalers were applied by reading back */
  volatile uint32_t cfgr_verify = RCC_CFGR;
  (void)cfgr_verify;

  /* Update global clock */
  SystemCoreClock = 168000000U;
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
