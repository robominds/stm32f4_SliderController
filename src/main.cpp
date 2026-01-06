/**
  * @file    main.cpp
  * @brief   Main program for STM32F407VET6 Black Board with FreeRTOS
  *          Blinks LED on PA6 and PA7 using FreeRTOS tasks
  */


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
static PWM_Handle g_pwm{};

static const PWM_Config kMotorPwmCfg = {
    TIM3_BASE,   /* timer_base */
    GPIOB_BASE,  /* gpio_base */
    0,           /* pin: PB0 -> TIM3_CH3 (AF2) */
    2,           /* af_number */
    PWM_CHANNEL_3,
    20000U       /* frequency_hz: 20 kHz */
};

static const QuadEncoder_Config kMotorEncoderCfg = {
    TIM4_BASE,   /* timer_base */
    GPIOB_BASE,  /* gpio_base */
    6,           /* pin_a: PB6 -> TIM4_CH1 (AF2) */
    7,           /* pin_b: PB7 -> TIM4_CH2 (AF2) */
    2,           /* af_number */
    2048         /* counts_per_rev: adjust to your encoder */
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

    
    MotorController mc = MotorController();
    bool mc_ok = mc.init(kMotorPwmCfg, kMotorEncoderCfg);
    if (!mc_ok) {
        UART_WriteString("ERROR: MotorController init failed\r\n");
        UART_FlushTx();
    } else {
        UART_WriteString("MotorController init ok\r\n");
        UART_FlushTx();
    }

    mc.setDuty(50.0f);
    mc.startPositionControlTask(.1, 10, 1000);

    if(0) {    
    /* For now, just test PWM standalone at 50% duty (skip motor controller/encoder). */
    bool pwm_ok = PWM_Init(&g_pwm, &kMotorPwmCfg);
    if (!pwm_ok) {
        UART_WriteString("ERROR: PWM init failed\r\n");
        UART_FlushTx();
    } else {
        UART_WriteString("PWM init ok\r\n");
        UART_FlushTx();

        UART_WriteString("About to set duty (float + int diag)\r\n");
        UART_FlushTx();
        PWM_SetDuty(&g_pwm, 50.0f);
        UART_WriteString("Duty set via PWM_SetDuty\r\n");
        UART_FlushTx();

        UART_WriteString("About to enable PWM\r\n");
        UART_FlushTx();
        PWM_Enable(&g_pwm);
        UART_WriteString("PWM enabled\r\n");
        UART_FlushTx();
    }
    
    g_motor_controller.setDuty(50.0);
    }

    UART_WriteString("xpost\r\n");
    UART_FlushTx();

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
