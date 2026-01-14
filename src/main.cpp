/**
  * @file    main.cpp
  * @brief   Main program for STM32F407VET6 Black Board with FreeRTOS
  *          Blinks LED on PA6 and PA7 using FreeRTOS tasks
  */


#include <stdio.h>
#include "uart_driver.h"
#include <stdint.h>
#include "pwm_driver.h"
#include "stm32f407xx.h"
#include "timer_common.h"
#include "system_stm32f4xx.h"
#include "FreeRTOS.h" // IWYU pragma: keep - Must include FreeRTOS.h before task.h
#include "task.h"
#include "led.h"
#include "uart_driver.h"
#include "motor_controller.h"
#include "pwm_driver.h"

static MotorController g_motor_controller;
//static PWM_Handle g_pwm[2]{};

static const PWM_Config kMotorPwmCfg[2] = {
    {
    TIM3_BASE,   /* timer_base */
    GPIOC_BASE,  /* gpio_base */
    9,           /* pin: PC9 -> TIM3_CH3 (AF2) */
    2,           /* af_number */
    PWM_CHANNEL_4,
    20000U       /* frequency_hz: 20 kHz */
    },
    {
    TIM3_BASE,   /* timer_base */
    GPIOC_BASE,  /* gpio_base */
    8,           /* pin: PC8 -> TIM3_CH4 (AF2) */
    2,           /* af_number */
    PWM_CHANNEL_3,
    20000U       /* frequency_hz: 20 kHz */
    }};

static const QuadEncoder_Config kMotorEncoderCfg = {
    TIM4_BASE,   /* timer_base */
    GPIOB_BASE,  /* gpio_base */
    6,           /* pin_a: PB6 -> TIM4_CH1 (AF2) */
    7,           /* pin_b: PB7 -> TIM4_CH2 (AF2) */
    2,           /* af_number */
    2048         /* counts_per_rev: adjust to your encoder */
};

static const LimitSwitch_Config kLimitSwitchCfg = {
    GPIOB_BASE,  /* gpio_base */
    8,           /* pin_min: PB8 */
    9,           /* pin_max: PB9 */
    true,        /* active_low: switches pull to ground when active */
    true         /* enable_pullup: use internal pull-up resistors */
};

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
    UART_FlushTx();
    
    /* Create LED tasks */
    if (LED_CreateTask1() != pdPASS) {
        UART_WriteString("ERROR: Failed to create LED1 task\r\n");
    }
    if (LED_CreateTask2() != pdPASS) {
        UART_WriteString("ERROR: Failed to create LED2 task\r\n");
    }

    g_motor_controller = MotorController();
    if (!g_motor_controller.init(kMotorPwmCfg, kMotorEncoderCfg, kLimitSwitchCfg)) {
        UART_WriteString("ERROR: MotorController init failed\r\n");
        UART_FlushTx();
    } else {
        UART_WriteString("MotorController init ok with limit switches\r\n");
        UART_FlushTx();
    }

    g_motor_controller.startPositionControlTask(1.0, .4, 10,6400);

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
