/**
  * @file    led.cpp
  * @brief   LED driver implementation for STM32F407VET6 Black Board
  *          Controls LEDs on PA6 and PA7 using FreeRTOS tasks
  */

#include "led.h"
#include "stm32f407xx.h"

/* Task handles */
static TaskHandle_t xLED1TaskHandle = NULL;
static TaskHandle_t xLED2TaskHandle = NULL;

/**
 * @brief Initialize GPIO pins for LED control
 *        Configures PA6 and PA7 as output pins
 */
void LED_Init(void) {
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

/**
 * @brief Create LED1 task
 * @return pdPASS if task created successfully, errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY otherwise
 */
BaseType_t LED_CreateTask1(void) {
    return xTaskCreate(
        vLED1Task,              /* Task function */
        "LED1",                 /* Task name */
        128,                    /* Stack size (words) */
        NULL,                   /* Task parameters */
        tskIDLE_PRIORITY + 1,   /* Task priority */
        &xLED1TaskHandle        /* Task handle */
    );
}

/**
 * @brief Create LED2 task
 * @return pdPASS if task created successfully, errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY otherwise
 */
BaseType_t LED_CreateTask2(void) {
    return xTaskCreate(
        vLED2Task,              /* Task function */
        "LED2",                 /* Task name */
        128,                    /* Stack size (words) */
        NULL,                   /* Task parameters */
        tskIDLE_PRIORITY + 1,   /* Task priority */
        &xLED2TaskHandle        /* Task handle */
    );
}

/**
 * @brief LED1 Task - Blinks LED on PA6
 * @param pvParameters Task parameters (unused)
 */
void vLED1Task(void *pvParameters) {
    (void)pvParameters;
    
    for (;;) {
        /* Toggle PA6 (LED1) */
        GPIOA->ODR ^= (1U << 6);
        /* Delay for 500ms */
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

/**
 * @brief LED2 Task - Blinks LED on PA7
 * @param pvParameters Task parameters (unused)
 */
void vLED2Task(void *pvParameters) {
    (void)pvParameters;
    
    for (;;) {
        /* Toggle PA7 (LED2) */
        GPIOA->ODR ^= (1U << 7);
        /* Delay for 300ms (different rate) */
        vTaskDelay(pdMS_TO_TICKS(300));
    }
}
