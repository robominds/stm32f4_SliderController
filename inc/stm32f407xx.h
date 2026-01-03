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

/* APB2 Peripherals */
#define USART1_BASE           (APB2PERIPH_BASE + 0x1000UL)

/* Core Peripherals */
#define SCS_BASE              0xE000E000UL
#define NVIC_BASE             (SCS_BASE + 0x0100UL)

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

/* USART Register Structure */
typedef struct
{
  volatile uint32_t SR;         /*!< USART Status register */
  volatile uint32_t DR;         /*!< USART Data register */
  volatile uint32_t BRR;        /*!< USART Baud rate register */
  volatile uint32_t CR1;        /*!< USART Control register 1 */
  volatile uint32_t CR2;        /*!< USART Control register 2 */
  volatile uint32_t CR3;        /*!< USART Control register 3 */
  volatile uint32_t GTPR;       /*!< USART Guard time and prescaler register */
} USART_TypeDef;

/* NVIC Register Structure */
typedef struct
{
  volatile uint32_t ISER[8];    /*!< Interrupt Set Enable Register */
  uint32_t RESERVED0[24];
  volatile uint32_t ICER[8];    /*!< Interrupt Clear Enable Register */
  uint32_t RESERVED1[24];
  volatile uint32_t ISPR[8];    /*!< Interrupt Set Pending Register */
  uint32_t RESERVED2[24];
  volatile uint32_t ICPR[8];    /*!< Interrupt Clear Pending Register */
  uint32_t RESERVED3[24];
  volatile uint32_t IABR[8];    /*!< Interrupt Active bit Register */
  uint32_t RESERVED4[56];
  volatile uint8_t  IP[240];    /*!< Interrupt Priority Register */
  uint32_t RESERVED5[644];
  volatile uint32_t STIR;       /*!< Software Trigger Interrupt Register */
} NVIC_TypeDef;

/* Peripheral Declarations */
#define GPIOA                 ((GPIO_TypeDef *) GPIOA_BASE)
#define GPIOB                 ((GPIO_TypeDef *) GPIOB_BASE)
#define GPIOC                 ((GPIO_TypeDef *) GPIOC_BASE)
#define GPIOD                 ((GPIO_TypeDef *) GPIOD_BASE)
#define GPIOE                 ((GPIO_TypeDef *) GPIOE_BASE)
#define RCC                   ((RCC_TypeDef *) RCC_BASE)
#define USART1                ((USART_TypeDef *) USART1_BASE)
#define NVIC                  ((NVIC_TypeDef *) NVIC_BASE)

/* RCC AHB1 Peripheral Clock Enable Bits */
#define RCC_AHB1ENR_GPIOAEN   (1U << 0)
#define RCC_AHB1ENR_GPIOBEN   (1U << 1)
#define RCC_AHB1ENR_GPIOCEN   (1U << 2)
#define RCC_AHB1ENR_GPIODEN   (1U << 3)
#define RCC_AHB1ENR_GPIOEEN   (1U << 4)

/* RCC APB2 Peripheral Clock Enable Bits */
#define RCC_APB2ENR_USART1EN  (1U << 4)

/* USART_SR Register Bits */
#define USART_SR_PE           (1U << 0)   /*!< Parity error */
#define USART_SR_FE           (1U << 1)   /*!< Framing error */
#define USART_SR_NF           (1U << 2)   /*!< Noise detected flag */
#define USART_SR_ORE          (1U << 3)   /*!< Overrun error */
#define USART_SR_IDLE         (1U << 4)   /*!< IDLE line detected */
#define USART_SR_RXNE         (1U << 5)   /*!< Read data register not empty */
#define USART_SR_TC           (1U << 6)   /*!< Transmission complete */
#define USART_SR_TXE          (1U << 7)   /*!< Transmit data register empty */
#define USART_SR_LBD          (1U << 8)   /*!< LIN break detection flag */
#define USART_SR_CTS          (1U << 9)   /*!< CTS flag */

/* USART_CR1 Register Bits */
#define USART_CR1_SBK         (1U << 0)   /*!< Send break */
#define USART_CR1_RWU         (1U << 1)   /*!< Receiver wakeup */
#define USART_CR1_RE          (1U << 2)   /*!< Receiver enable */
#define USART_CR1_TE          (1U << 3)   /*!< Transmitter enable */
#define USART_CR1_IDLEIE      (1U << 4)   /*!< IDLE interrupt enable */
#define USART_CR1_RXNEIE      (1U << 5)   /*!< RXNE interrupt enable */
#define USART_CR1_TCIE        (1U << 6)   /*!< Transmission complete interrupt enable */
#define USART_CR1_TXEIE       (1U << 7)   /*!< TXE interrupt enable */
#define USART_CR1_PEIE        (1U << 8)   /*!< PE interrupt enable */
#define USART_CR1_PS          (1U << 9)   /*!< Parity selection */
#define USART_CR1_PCE         (1U << 10)  /*!< Parity control enable */
#define USART_CR1_WAKE        (1U << 11)  /*!< Wakeup method */
#define USART_CR1_M           (1U << 12)  /*!< Word length */
#define USART_CR1_UE          (1U << 13)  /*!< USART enable */
#define USART_CR1_OVER8       (1U << 15)  /*!< Oversampling mode */

/* IRQ Numbers */
#define USART1_IRQn           37          /*!< USART1 global interrupt */

#ifdef __cplusplus
}
#endif

#endif /* __STM32F407XX_H */
