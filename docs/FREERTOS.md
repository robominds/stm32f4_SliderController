# FreeRTOS Integration

## Overview

This project now uses FreeRTOS v10.6.2 for task scheduling and real-time operations on the STM32F407VE microcontroller.

## Setup

### Prerequisites

1. ARM GCC toolchain (arm-none-eabi-gcc)
2. FreeRTOS kernel source code

### Getting Started

To set up FreeRTOS for this project:

```bash
# Clone FreeRTOS kernel
git clone --depth 1 --branch V10.6.2 https://github.com/FreeRTOS/FreeRTOS-Kernel.git FreeRTOS
```

## Configuration

### FreeRTOSConfig.h

The FreeRTOS configuration is located in `inc/FreeRTOSConfig.h`. Key settings include:

- **CPU Clock**: Uses `SystemCoreClock` (typically 168 MHz for STM32F407)
- **Tick Rate**: 1000 Hz (1ms tick period)
- **Total Heap Size**: 20 KB
- **Stack Overflow Detection**: Enabled (method 2)
- **Memory Allocation**: Dynamic allocation using heap_4

### Interrupt Priorities

FreeRTOS uses interrupt priorities 5-15. The kernel uses the SysTick, PendSV, and SVC interrupts:

- **configKERNEL_INTERRUPT_PRIORITY**: Lowest priority (15)
- **configMAX_SYSCALL_INTERRUPT_PRIORITY**: Priority 5

⚠️ **Important**: Any interrupt that calls FreeRTOS API functions must have a priority >= 5.

## Project Structure

```
├── FreeRTOS/                     # FreeRTOS kernel source (not in git)
│   ├── include/                  # FreeRTOS headers
│   ├── portable/
│   │   ├── GCC/ARM_CM4F/        # Cortex-M4F port
│   │   └── MemMang/heap_4.c     # Memory management
│   ├── tasks.c
│   ├── queue.c
│   ├── list.c
│   ├── timers.c
│   ├── event_groups.c
│   └── stream_buffer.c
├── inc/
│   └── FreeRTOSConfig.h         # FreeRTOS configuration
└── src/
    └── main.cpp                  # Application with FreeRTOS tasks
```

## Usage Example

The current implementation demonstrates basic FreeRTOS usage with two LED blinking tasks:

```cpp
/* LED1 Task - Blink PA6 every 500ms */
void vLED1Task(void *pvParameters) {
    for (;;) {
        GPIOA->ODR ^= (1U << 6);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

/* LED2 Task - Blink PA7 every 300ms */
void vLED2Task(void *pvParameters) {
    for (;;) {
        GPIOA->ODR ^= (1U << 7);
        vTaskDelay(pdMS_TO_TICKS(300));
    }
}
```

## Building

```bash
make clean
make
```

The build process automatically includes FreeRTOS sources and headers.

## Flashing

```bash
make flash
```

## Key FreeRTOS API Functions Used

- `xTaskCreate()`: Create a new task
- `vTaskStartScheduler()`: Start the FreeRTOS scheduler
- `vTaskDelay()`: Delay task execution for a specified number of ticks
- `pdMS_TO_TICKS()`: Convert milliseconds to ticks

## Hook Functions

The following hook functions are implemented in `main.cpp`:

- **vApplicationStackOverflowHook()**: Called when stack overflow is detected
- **vApplicationMallocFailedHook()**: Called when memory allocation fails

Both functions enter an infinite loop to halt execution for debugging.

## Adding New Tasks

To add a new task:

1. Define the task function with signature `void taskFunction(void *pvParameters)`
2. Create the task in `main()` before calling `vTaskStartScheduler()`

```cpp
TaskHandle_t xMyTaskHandle = NULL;

void vMyTask(void *pvParameters) {
    for (;;) {
        // Task code here
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

int main(void) {
    // ... initialization ...
    
    xTaskCreate(
        vMyTask,                // Task function
        "MyTask",               // Task name
        128,                    // Stack size (words)
        NULL,                   // Task parameters
        tskIDLE_PRIORITY + 1,   // Task priority
        &xMyTaskHandle          // Task handle
    );
    
    vTaskStartScheduler();
    // ... rest of main ...
}
```

## Resources

- [FreeRTOS Official Documentation](https://www.freertos.org/Documentation/RTOS_book.html)
- [FreeRTOS API Reference](https://www.freertos.org/a00106.html)
- [STM32F407 Reference Manual](https://www.st.com/resource/en/reference_manual/dm00031020.pdf)

## Notes

- The FreeRTOS directory is excluded from git. Clone it separately when setting up a new development environment.
- The ARM toolchain is also excluded from git. Install it using your system's package manager or download from ARM's website.
