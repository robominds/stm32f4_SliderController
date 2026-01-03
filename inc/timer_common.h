/**
 * @file    timer_common.h
 * @brief   Common timer register definitions and access helpers for STM32F4xx
 */

#ifndef TIMER_COMMON_H
#define TIMER_COMMON_H

#include <stdint.h>

/* Register access helper */
#define REG32(base, offset) (*((volatile uint32_t *)((uintptr_t)((base) + (offset)))))

/* Timer register offsets */
#define TIM_CR1_OFFSET      0x00
#define TIM_SMCR_OFFSET     0x08
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
#define TIM_CR1(base)       REG32((base), TIM_CR1_OFFSET)
#define TIM_SMCR(base)      REG32((base), TIM_SMCR_OFFSET)
#define TIM_CCMR1(base)     REG32((base), TIM_CCMR1_OFFSET)
#define TIM_CCMR2(base)     REG32((base), TIM_CCMR2_OFFSET)
#define TIM_CCER(base)      REG32((base), TIM_CCER_OFFSET)
#define TIM_CNT(base)       REG32((base), TIM_CNT_OFFSET)
#define TIM_PSC(base)       REG32((base), TIM_PSC_OFFSET)
#define TIM_ARR(base)       REG32((base), TIM_ARR_OFFSET)
#define TIM_CCR(base, ch)   REG32((base), TIM_CCR1_OFFSET + ((ch) * 4U))

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

#endif /* TIMER_COMMON_H */
