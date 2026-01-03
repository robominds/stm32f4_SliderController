/**
  * @file    uart_driver.h
  * @brief   Interrupt-driven UART driver for STM32F407VET6
  *          USART1 driver with TX on PA9 and RX on PA10
  */

#ifndef UART_DRIVER_H
#define UART_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/* Circular buffer size for RX and TX */
#define UART_RX_BUFFER_SIZE   256
#define UART_TX_BUFFER_SIZE   256

/**
 * @brief Initialize USART1 with interrupt-driven operation
 * @param baudrate Desired baud rate (e.g., 9600, 115200)
 * @note Uses PA9 for TX and PA10 for RX
 * @note Assumes APB2 clock is 84 MHz (with 168 MHz system clock)
 */
void UART_Init(uint32_t baudrate);

/**
 * @brief Transmit a single byte via USART1 (interrupt-driven)
 * @param data Byte to transmit
 * @return true if byte queued successfully, false if buffer full
 */
bool UART_WriteByte(uint8_t data);

/**
 * @brief Transmit a string via USART1 (interrupt-driven)
 * @param str Null-terminated string to transmit
 * @return Number of bytes queued for transmission
 */
uint32_t UART_WriteString(const char *str);

/**
 * @brief Receive a single byte via USART1 (non-blocking)
 * @param data Pointer to store received byte
 * @return true if byte received, false if buffer empty
 */
bool UART_ReadByte(uint8_t *data);

/**
 * @brief Check if data is available to read
 * @return Number of bytes available in RX buffer
 */
uint32_t UART_DataAvailable(void);

/**
 * @brief Transmit data buffer via USART1 (interrupt-driven)
 * @param data Pointer to data buffer
 * @param length Number of bytes to transmit
 * @return Number of bytes queued for transmission
 */
uint32_t UART_Write(const uint8_t *data, uint32_t length);

/**
 * @brief Receive data into buffer via USART1 (non-blocking)
 * @param data Pointer to receive buffer
 * @param length Maximum number of bytes to receive
 * @return Number of bytes actually received
 */
uint32_t UART_Read(uint8_t *data, uint32_t length);

/**
 * @brief Get free space in TX buffer
 * @return Number of bytes that can be written
 */
uint32_t UART_GetTxFreeSpace(void);

/**
 * @brief Flush TX buffer (wait until all data is transmitted)
 */
void UART_FlushTx(void);

/**
 * @brief Clear RX buffer
 */
void UART_FlushRx(void);

/**
 * @brief USART1 interrupt handler
 * @note Must be called from USART1_IRQHandler
 */
void UART_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* UART_DRIVER_H */
