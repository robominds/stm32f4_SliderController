/**
 * @file    gpio.h
 * @brief   GPIO driver for STM32F407VET6
 *          Provides abstraction for GPIO pin control
 */

#ifndef GPIO_H
#define GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief GPIO Port enumeration
 * STM32F407 has GPIO ports A through E
 */
typedef enum {
    GPIO_PORT_A = 0,
    GPIO_PORT_B = 1,
    GPIO_PORT_C = 2,
    GPIO_PORT_D = 3,
    GPIO_PORT_E = 4
} GPIO_Port;

/**
 * @brief GPIO Mode enumeration
 */
typedef enum {
    GPIO_MODE_INPUT = 0,       /* 00 - Input */
    GPIO_MODE_OUTPUT = 1,      /* 01 - Output */
    GPIO_MODE_ALTERNATE = 2,   /* 10 - Alternate function */
    GPIO_MODE_ANALOG = 3       /* 11 - Analog */
} GPIO_Mode;

/**
 * @brief GPIO Output Type enumeration
 */
typedef enum {
    GPIO_OUTPUT_PUSH_PULL = 0, /* 0 - Push-pull */
    GPIO_OUTPUT_OPEN_DRAIN = 1 /* 1 - Open-drain */
} GPIO_OutputType;

/**
 * @brief GPIO Speed enumeration
 */
typedef enum {
    GPIO_SPEED_LOW = 0,        /* 00 - Low speed */
    GPIO_SPEED_MEDIUM = 1,     /* 01 - Medium speed */
    GPIO_SPEED_FAST = 2,       /* 10 - Fast speed */
    GPIO_SPEED_HIGH = 3        /* 11 - High speed */
} GPIO_Speed;

/**
 * @brief GPIO Pull-Up/Pull-Down enumeration
 */
typedef enum {
    GPIO_PULL_NONE = 0,        /* 00 - No pull */
    GPIO_PULL_UP = 1,          /* 01 - Pull-up */
    GPIO_PULL_DOWN = 2         /* 10 - Pull-down */
} GPIO_Pull;

/**
 * @brief GPIO Configuration structure
 */
typedef struct {
    GPIO_Port port;            /* GPIO port */
    uint8_t pin;               /* Pin number (0-15) */
    GPIO_Mode mode;            /* Pin mode */
    GPIO_OutputType otype;     /* Output type (only for output mode) */
    GPIO_Speed speed;          /* Output speed (only for output mode) */
    GPIO_Pull pull;            /* Pull-up/Pull-down */
    uint8_t alternate_func;    /* Alternate function number (only for alternate mode) */
} GPIO_Config;

/**
 * @brief Get GPIO port base address
 * @param port: GPIO port
 * @return Base address of the GPIO port
 */
uint32_t GPIO_GetPortBase(GPIO_Port port);

/**
 * @brief Enable GPIO port clock
 * @param port: GPIO port
 */
void GPIO_EnableClock(GPIO_Port port);

/**
 * @brief Disable GPIO port clock
 * @param port: GPIO port
 */
void GPIO_DisableClock(GPIO_Port port);

/**
 * @brief Initialize GPIO pin
 * @param config: Pointer to GPIO configuration structure
 * @retval true if successful, false otherwise
 */
bool GPIO_Init(const GPIO_Config *config);

/**
 * @brief Set GPIO pin output level
 * @param port: GPIO port
 * @param pin: Pin number (0-15)
 * @param state: true for high, false for low
 */
void GPIO_WritePin(GPIO_Port port, uint8_t pin, bool state);

/**
 * @brief Read GPIO pin input level
 * @param port: GPIO port
 * @param pin: Pin number (0-15)
 * @return true if pin is high, false if pin is low
 */
bool GPIO_ReadPin(GPIO_Port port, uint8_t pin);

/**
 * @brief Toggle GPIO pin output level
 * @param port: GPIO port
 * @param pin: Pin number (0-15)
 */
void GPIO_TogglePin(GPIO_Port port, uint8_t pin);

/**
 * @brief Write multiple pins at once
 * @param port: GPIO port
 * @param mask: Bitmask of pins to write
 * @param value: Value to write (0 or 1 for each bit in mask)
 */
void GPIO_WritePins(GPIO_Port port, uint16_t mask, uint16_t value);

/**
 * @brief Read all pins on a GPIO port
 * @param port: GPIO port
 * @return 16-bit value representing all pins
 */
uint16_t GPIO_ReadPort(GPIO_Port port);

#ifdef __cplusplus
}
#endif

#endif /* GPIO_H */
