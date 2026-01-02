/**
  * @file    stm32f407xx.h
  * @brief   STM32F407xx device specific definitions and register map
  */

#ifndef __STM32F407XX_H
#define __STM32F407XX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* Peripheral Base Addresses */
#define PERIPH_BASE           0x40000000UL
#define APB1PERIPH_BASE       PERIPH_BASE
#define APB2PERIPH_BASE       (PERIPH_BASE + 0x00010000UL)
#define AHB1PERIPH_BASE       (PERIPH_BASE + 0x00020000UL)
#define AHB2PERIPH_BASE       (PERIPH_BASE + 0x10000000UL)

/* AHB1 Peripherals */
#define GPIOA_BASE            (AHB1PERIPH_BASE + 0x0000UL)
#define GPIOB_BASE            (AHB1PERIPH_BASE + 0x0400UL)
#define GPIOC_BASE            (AHB1PERIPH_BASE + 0x0800UL)
#define GPIOD_BASE            (AHB1PERIPH_BASE + 0x0C00UL)
#define GPIOE_BASE            (AHB1PERIPH_BASE + 0x1000UL)
#define RCC_BASE              (AHB1PERIPH_BASE + 0x3800UL)

/* GPIO Register Structure */
typedef struct
{
  volatile uint32_t MODER;    /*!< GPIO port mode register */
  volatile uint32_t OTYPER;   /*!< GPIO port output type register */
  volatile uint32_t OSPEEDR;  /*!< GPIO port output speed register */
  volatile uint32_t PUPDR;    /*!< GPIO port pull-up/pull-down register */
  volatile uint32_t IDR;      /*!< GPIO port input data register */
  volatile uint32_t ODR;      /*!< GPIO port output data register */
  volatile uint32_t BSRR;     /*!< GPIO port bit set/reset register */
  volatile uint32_t LCKR;     /*!< GPIO port configuration lock register */
  volatile uint32_t AFR[2];   /*!< GPIO alternate function registers */
} GPIO_TypeDef;

/* RCC Register Structure */
typedef struct
{
  volatile uint32_t CR;            /*!< RCC clock control register */
  volatile uint32_t PLLCFGR;       /*!< RCC PLL configuration register */
  volatile uint32_t CFGR;          /*!< RCC clock configuration register */
  volatile uint32_t CIR;           /*!< RCC clock interrupt register */
  volatile uint32_t AHB1RSTR;      /*!< RCC AHB1 peripheral reset register */
  volatile uint32_t AHB2RSTR;      /*!< RCC AHB2 peripheral reset register */
  volatile uint32_t AHB3RSTR;      /*!< RCC AHB3 peripheral reset register */
  uint32_t      RESERVED0;         /*!< Reserved */
  volatile uint32_t APB1RSTR;      /*!< RCC APB1 peripheral reset register */
  volatile uint32_t APB2RSTR;      /*!< RCC APB2 peripheral reset register */
  uint32_t      RESERVED1[2];      /*!< Reserved */
  volatile uint32_t AHB1ENR;       /*!< RCC AHB1 peripheral clock register */
  volatile uint32_t AHB2ENR;       /*!< RCC AHB2 peripheral clock register */
  volatile uint32_t AHB3ENR;       /*!< RCC AHB3 peripheral clock register */
  uint32_t      RESERVED2;         /*!< Reserved */
  volatile uint32_t APB1ENR;       /*!< RCC APB1 peripheral clock enable register */
  volatile uint32_t APB2ENR;       /*!< RCC APB2 peripheral clock enable register */
} RCC_TypeDef;

/* Peripheral Declarations */
#define GPIOA                 ((GPIO_TypeDef *) GPIOA_BASE)
#define GPIOB                 ((GPIO_TypeDef *) GPIOB_BASE)
#define GPIOC                 ((GPIO_TypeDef *) GPIOC_BASE)
#define GPIOD                 ((GPIO_TypeDef *) GPIOD_BASE)
#define GPIOE                 ((GPIO_TypeDef *) GPIOE_BASE)
#define RCC                   ((RCC_TypeDef *) RCC_BASE)

/* RCC AHB1 Peripheral Clock Enable Bits */
#define RCC_AHB1ENR_GPIOAEN   (1U << 0)
#define RCC_AHB1ENR_GPIOBEN   (1U << 1)
#define RCC_AHB1ENR_GPIOCEN   (1U << 2)
#define RCC_AHB1ENR_GPIODEN   (1U << 3)
#define RCC_AHB1ENR_GPIOEEN   (1U << 4)

#ifdef __cplusplus
}
#endif

#endif /* __STM32F407XX_H */
