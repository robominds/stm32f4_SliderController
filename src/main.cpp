/**
  * @file    main.cpp
  * @brief   Main program for STM32F407VET6 Black Board with FreeRTOS
  *          Blinks LED on PA6 and PA7 using FreeRTOS tasks
  */

#include <stdint.h>
#include "stm32f407xx.h"
#include "system_stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"

/* Task handles */
TaskHandle_t xLED1TaskHandle = NULL;
TaskHandle_t xLED2TaskHandle = NULL;

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

/* LED1 Task - Blink PA6 */
void vLED1Task(void *pvParameters) {
    (void)pvParameters;
    
    for (;;) {
        /* Toggle PA6 (LED1) */
        GPIOA->ODR ^= (1U << 6);
        /* Delay for 500ms */
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

/* LED2 Task - Blink PA7 */
void vLED2Task(void *pvParameters) {
    (void)pvParameters;
    
    for (;;) {
        /* Toggle PA7 (LED2) */
        GPIOA->ODR ^= (1U << 7);
        /* Delay for 300ms (different rate) */
        vTaskDelay(pdMS_TO_TICKS(300));
    }
}

int main(void) {
    /* Initialize system */
    SystemInit();
    
    /* Initialize GPIO for LED */
    GPIO_Init();
    
    /* Create LED1 task */
    xTaskCreate(
        vLED1Task,              /* Task function */
        "LED1",                 /* Task name */
        128,                    /* Stack size (words) */
        NULL,                   /* Task parameters */
        tskIDLE_PRIORITY + 1,   /* Task priority */
        &xLED1TaskHandle        /* Task handle */
    );
    
    /* Create LED2 task */
    xTaskCreate(
        vLED2Task,              /* Task function */
        "LED2",                 /* Task name */
        128,                    /* Stack size (words) */
        NULL,                   /* Task parameters */
        tskIDLE_PRIORITY + 1,   /* Task priority */
        &xLED2TaskHandle        /* Task handle */
    );
    
    /* Start the scheduler */
    vTaskStartScheduler();
    
    /* Should never reach here */
    while (1) {
    }
    
    return 0;
}

/* C++ support - required functions */
extern "C" {
    void __cxa_pure_virtual() {
        while(1);
    }
    
    /* FreeRTOS hook functions */
    void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
        /* This function will get called if a stack overflow is detected during
         * the context switch. Set configCHECK_FOR_STACK_OVERFLOW to 2 to also
         * check for stack problems within nested interrupts, but only do this for
         * debug purposes as it will increase the context switch time. */
        (void)xTask;
        (void)pcTaskName;
        
        taskDISABLE_INTERRUPTS();
        for(;;); /* Hang here */
    }
    
    void vApplicationMallocFailedHook(void) {
        /* This function will get called if a call to pvPortMalloc() fails because
         * there is insufficient free memory available in the FreeRTOS heap. */
        taskDISABLE_INTERRUPTS();
        for(;;); /* Hang here */
    }
}
