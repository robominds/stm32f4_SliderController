/**
 * @file    gpio.cpp
 * @brief   GPIO driver implementation for STM32F407VET6
 *          Provides abstraction for GPIO pin control
 */

#include "gpio.h"
#include "stm32f407xx.h"

/* GPIO Port base addresses */
static const uint32_t GPIO_PORT_BASES[] = {
    GPIOA_BASE,  /* GPIO_PORT_A */
    GPIOB_BASE,  /* GPIO_PORT_B */
    GPIOC_BASE,  /* GPIO_PORT_C */
    GPIOD_BASE,  /* GPIO_PORT_D */
    GPIOE_BASE   /* GPIO_PORT_E */
};

/**
 * @brief Get GPIO port base address
 * @param port: GPIO port
 * @return Base address of the GPIO port
 */
uint32_t GPIO_GetPortBase(GPIO_Port port) {
    if (port < 5) {  /* 5 ports: A-E */
        return GPIO_PORT_BASES[port];
    }
    return 0;
}

/**
 * @brief Enable GPIO port clock
 * @param port: GPIO port
 */
void GPIO_EnableClock(GPIO_Port port) {
    /* Enable corresponding GPIO clock in RCC_AHB1ENR */
    RCC->AHB1ENR |= (1U << port);
}

/**
 * @brief Disable GPIO port clock
 * @param port: GPIO port
 */
void GPIO_DisableClock(GPIO_Port port) {
    /* Disable corresponding GPIO clock in RCC_AHB1ENR */
    RCC->AHB1ENR &= ~(1U << port);
}

/**
 * @brief Initialize GPIO pin
 * @param config: Pointer to GPIO configuration structure
 * @retval true if successful, false otherwise
 */
bool GPIO_Init(const GPIO_Config *config) {
    if (!config || config->pin > 15) {
        return false;
    }

    /* Get GPIO port base address */
    uint32_t port_base = GPIO_GetPortBase(config->port);
    if (!port_base) {
        return false;
    }

    GPIO_TypeDef *GPIOx = (GPIO_TypeDef *)port_base;

    /* Enable GPIO port clock */
    GPIO_EnableClock(config->port);

    /* Configure MODE register */
    /* Clear mode bits for this pin */
    GPIOx->MODER &= ~(0x3U << (config->pin * 2));
    /* Set new mode */
    GPIOx->MODER |= (config->mode << (config->pin * 2));

    /* Configure OTYPER register (only relevant for output mode) */
    if (config->mode == GPIO_MODE_OUTPUT || config->mode == GPIO_MODE_ALTERNATE) {
        if (config->otype == GPIO_OUTPUT_OPEN_DRAIN) {
            GPIOx->OTYPER |= (1U << config->pin);
        } else {
            GPIOx->OTYPER &= ~(1U << config->pin);
        }
    }

    /* Configure OSPEEDR register (only relevant for output mode) */
    if (config->mode == GPIO_MODE_OUTPUT || config->mode == GPIO_MODE_ALTERNATE) {
        GPIOx->OSPEEDR &= ~(0x3U << (config->pin * 2));
        GPIOx->OSPEEDR |= (config->speed << (config->pin * 2));
    }

    /* Configure PUPDR register */
    GPIOx->PUPDR &= ~(0x3U << (config->pin * 2));
    GPIOx->PUPDR |= (config->pull << (config->pin * 2));

    /* Configure alternate function register (only for alternate mode) */
    if (config->mode == GPIO_MODE_ALTERNATE) {
        if (config->pin < 8) {
            GPIOx->AFR[0] &= ~(0xFU << (config->pin * 4));
            GPIOx->AFR[0] |= (config->alternate_func << (config->pin * 4));
        } else {
            GPIOx->AFR[1] &= ~(0xFU << ((config->pin - 8) * 4));
            GPIOx->AFR[1] |= (config->alternate_func << ((config->pin - 8) * 4));
        }
    }

    return true;
}

/**
 * @brief Set GPIO pin output level
 * @param port: GPIO port
 * @param pin: Pin number (0-15)
 * @param state: true for high, false for low
 */
void GPIO_WritePin(GPIO_Port port, uint8_t pin, bool state) {
    if (pin > 15) {
        return;
    }

    uint32_t port_base = GPIO_GetPortBase(port);
    if (!port_base) {
        return;
    }

    GPIO_TypeDef *GPIOx = (GPIO_TypeDef *)port_base;

    if (state) {
        /* Set bit using BSRR register */
        GPIOx->BSRR = (1U << pin);
    } else {
        /* Clear bit using BSRR register */
        GPIOx->BSRR = (1U << (pin + 16));
    }
}

/**
 * @brief Read GPIO pin input level
 * @param port: GPIO port
 * @param pin: Pin number (0-15)
 * @return true if pin is high, false if pin is low
 */
bool GPIO_ReadPin(GPIO_Port port, uint8_t pin) {
    if (pin > 15) {
        return false;
    }

    uint32_t port_base = GPIO_GetPortBase(port);
    if (!port_base) {
        return false;
    }

    GPIO_TypeDef *GPIOx = (GPIO_TypeDef *)port_base;

    return (GPIOx->IDR & (1U << pin)) ? true : false;
}

/**
 * @brief Toggle GPIO pin output level
 * @param port: GPIO port
 * @param pin: Pin number (0-15)
 */
void GPIO_TogglePin(GPIO_Port port, uint8_t pin) {
    if (pin > 15) {
        return;
    }

    uint32_t port_base = GPIO_GetPortBase(port);
    if (!port_base) {
        return;
    }

    GPIO_TypeDef *GPIOx = (GPIO_TypeDef *)port_base;

    /* XOR the pin bit to toggle it */
    GPIOx->ODR ^= (1U << pin);
}

/**
 * @brief Write multiple pins at once
 * @param port: GPIO port
 * @param mask: Bitmask of pins to write
 * @param value: Value to write (0 or 1 for each bit in mask)
 */
void GPIO_WritePins(GPIO_Port port, uint16_t mask, uint16_t value) {
    uint32_t port_base = GPIO_GetPortBase(port);
    if (!port_base) {
        return;
    }

    GPIO_TypeDef *GPIOx = (GPIO_TypeDef *)port_base;

    /* Use BSRR to atomically set/clear pins */
    /* Upper 16 bits clear, lower 16 bits set */
    GPIOx->BSRR = ((~value & mask) << 16) | (value & mask);
}

/**
 * @brief Read all pins on a GPIO port
 * @param port: GPIO port
 * @return 16-bit value representing all pins
 */
uint16_t GPIO_ReadPort(GPIO_Port port) {
    uint32_t port_base = GPIO_GetPortBase(port);
    if (!port_base) {
        return 0;
    }

    GPIO_TypeDef *GPIOx = (GPIO_TypeDef *)port_base;

    return (uint16_t)GPIOx->IDR;
}
