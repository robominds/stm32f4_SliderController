/**
  * @file    main.cpp
  * @brief   Main program for STM32F407VET6 Black Board
  *          Blinks LED on PA6 and PA7 (common on STM32F407VET6 Black Board)
  */

#include <stdint.h>
#include "stm32f407xx.h"
#include "system_stm32f4xx.h"

/* Simple delay function */
void delay(volatile uint32_t count) {
    while (count--) {
        __asm("nop");
    }
}

/* GPIO initialization for LED */
void GPIO_Init() {
    /* Enable GPIOA clock */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    
    /* Configure PA6 and PA7 as output (LEDs on STM32F407VET6 Black Board) */
    /* MODER: 00 = Input, 01 = Output, 10 = Alternate, 11 = Analog */
    GPIOA->MODER &= ~(0x3U << (6 * 2));  /* Clear PA6 mode bits */
    GPIOA->MODER |= (0x1U << (6 * 2));   /* Set PA6 as output */
    
    GPIOA->MODER &= ~(0x3U << (7 * 2));  /* Clear PA7 mode bits */
    GPIOA->MODER |= (0x1U << (7 * 2));   /* Set PA7 as output */
    
    /* OTYPER: 0 = Push-pull, 1 = Open-drain */
    GPIOA->OTYPER &= ~(1U << 6);  /* PA6 push-pull */
    GPIOA->OTYPER &= ~(1U << 7);  /* PA7 push-pull */
    
    /* OSPEEDR: 00 = Low, 01 = Medium, 10 = Fast, 11 = High */
    GPIOA->OSPEEDR |= (0x2U << (6 * 2));  /* PA6 fast speed */
    GPIOA->OSPEEDR |= (0x2U << (7 * 2));  /* PA7 fast speed */
    
    /* PUPDR: 00 = No pull, 01 = Pull-up, 10 = Pull-down */
    GPIOA->PUPDR &= ~(0x3U << (6 * 2));  /* No pull for PA6 */
    GPIOA->PUPDR &= ~(0x3U << (7 * 2));  /* No pull for PA7 */
}

int main(void) {
    /* Initialize system */
    SystemInit();
    
    /* Initialize GPIO for LED */
    GPIO_Init();
    
    /* Main loop - Blink LEDs */
    while (1) {
        /* Toggle PA6 (LED1) */
        GPIOA->ODR ^= (1U << 6);
        delay(1000000);
        
        /* Toggle PA7 (LED2) */
        GPIOA->ODR ^= (1U << 7);
        delay(1000000);
    }
    
    return 0;
}

/* C++ support - required functions */
extern "C" {
    void __cxa_pure_virtual() {
        while(1);
    }
}
