/**
  * @file    main.cpp
  * @brief   Main program for STM32F407VET6 Black Board with FreeRTOS
  *          Blinks LED on PA6 and PA7 using FreeRTOS tasks
  */


#include <stdint.h>
#include "system_stm32f4xx.h"
#include "FreeRTOS.h" // IWYU pragma: keep - Must include FreeRTOS.h before task.h
#include "task.h"
#include "led.h"
#include "uart_driver.h"

int main(void) {
    /* Initialize system */
    SystemInit();
    
    /* Initialize GPIO for LED */
    LED_Init();
    
    /* Initialize interrupt-driven UART at 115200 baud */
    UART_Init(115200);
    
    /* Send startup message */
    UART_WriteString("\n\r");
    UART_WriteString("STM32F407 Interrupt-Driven UART Initialized\r\n");
    UART_WriteString("System Ready\r\n");
    
    /* Create LED tasks */
    if (LED_CreateTask1() != pdPASS) {
        UART_WriteString("ERROR: Failed to create LED1 task\r\n");
    }
    if (LED_CreateTask2() != pdPASS) {
        UART_WriteString("ERROR: Failed to create LED2 task\r\n");
    }
    
    /* Start the FreeRTOS scheduler */
    vTaskStartScheduler();
    
    /* Should never reach here */
    while(1) {
        /* If we get here, scheduler failed */
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
