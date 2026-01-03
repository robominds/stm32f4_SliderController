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
#include "led.h"

int main(void) {
    /* Initialize system */
    SystemInit();
    
    /* Initialize GPIO for LED */
    LED_Init();
    
    /* Create LED tasks */
    LED_CreateTask1();
    LED_CreateTask2();
    
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
