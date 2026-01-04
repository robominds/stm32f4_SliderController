/**
  * @file    uart_driver.cpp
  * @brief   Interrupt-driven UART driver implementation for STM32F407VET6
  *          USART1 driver with TX on PA9 and RX on PA10
  */

#include "uart_driver.h"
#include "stm32f407xx.h"
#include "system_stm32f4xx.h"

/* Circular buffer structures */
typedef struct {
    uint8_t buffer[UART_RX_BUFFER_SIZE];
    volatile uint32_t head;
    volatile uint32_t tail;
    volatile uint32_t count;
} CircularBuffer_t;

/* RX and TX buffers */
static CircularBuffer_t rxBuffer;
static CircularBuffer_t txBuffer;

/* Helper functions for circular buffer */
static inline bool CircularBuffer_Put(CircularBuffer_t *cb, uint8_t data) {
    if (cb->count >= UART_RX_BUFFER_SIZE) {
        return false;  /* Buffer full */
    }
    
    cb->buffer[cb->head] = data;
    cb->head = (cb->head + 1) % UART_RX_BUFFER_SIZE;
    cb->count++;
    return true;
}

static inline bool CircularBuffer_Get(CircularBuffer_t *cb, uint8_t *data) {
    if (cb->count == 0) {
        return false;  /* Buffer empty */
    }
    
    *data = cb->buffer[cb->tail];
    cb->tail = (cb->tail + 1) % UART_RX_BUFFER_SIZE;
    cb->count--;
    return true;
}

static inline uint32_t CircularBuffer_Count(CircularBuffer_t *cb) {
    return cb->count;
}

static inline uint32_t CircularBuffer_FreeSpace(CircularBuffer_t *cb) {
    return UART_TX_BUFFER_SIZE - cb->count;
}

static inline void CircularBuffer_Clear(CircularBuffer_t *cb) {
    cb->head = 0;
    cb->tail = 0;
    cb->count = 0;
}

/**
 * @brief Initialize USART1
 * @param baudrate Desired baud rate (e.g., 9600, 115200)
 *
 * Notes:
 * - System clock is configured to 168 MHz
 * - APB2 prescaler divides by 2, so APB2 = 84 MHz
 * - BRR format: [15:4]=mantissa, [3:0]=fraction (for 16x oversampling)
 */
static inline uint32_t UART_GetApb2Clock(void) {
    /* APB2 = SystemCoreClock / 2 (prescaler /2 is working correctly) */
    return SystemCoreClock / 2;  /* 168 MHz / 2 = 84 MHz */
}

void UART_Init(uint32_t baudrate) {

    /* Initialize circular buffers */
    CircularBuffer_Clear(&rxBuffer);
    CircularBuffer_Clear(&txBuffer);
    
    /* Enable GPIOA clock */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    
    /* Enable USART1 clock (APB2) */
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    
    /* Configure PA9 (TX) and PA10 (RX) as alternate function */
    /* Clear and set PA9 mode to alternate function (10) */
    GPIOA->MODER &= ~(0x3U << 18);  /* Clear bits [19:18] for PA9 */
    GPIOA->MODER |= (0x2U << 18);   /* Set to 10 (alternate function) */
    
    /* Clear and set PA10 mode to alternate function (10) */
    GPIOA->MODER &= ~(0x3U << 20);  /* Clear bits [21:20] for PA10 */
    GPIOA->MODER |= (0x2U << 20);   /* Set to 10 (alternate function) */
    
    /* Set alternate function to AF7 (USART1) for both pins */
    /* AFR[1] handles pins 8-15 */
    /* PA9 uses bits [7:4] in AFR[1] */
    GPIOA->AFR[1] &= ~(0xFU << 4);  /* Clear PA9 AF bits */
    GPIOA->AFR[1] |= (7U << 4);     /* Set PA9 to AF7 */
    
    /* PA10 uses bits [11:8] in AFR[1] */
    GPIOA->AFR[1] &= ~(0xFU << 8);  /* Clear PA10 AF bits */
    GPIOA->AFR[1] |= (7U << 8);     /* Set PA10 to AF7 */
    
    /* Disable pull-ups/pull-downs (no pull) */
    GPIOA->PUPDR &= ~(0x3U << 18);  /* Clear PA9 PUPDR bits */
    GPIOA->PUPDR &= ~(0x3U << 20);  /* Clear PA10 PUPDR bits */
    
    /* Configure output type as push-pull for TX (default is already push-pull, but make sure) */
    GPIOA->OTYPER &= ~(1U << 9);    /* PA9 push-pull */
    
    /* Set speed to high */
    GPIOA->OSPEEDR &= ~(0x3U << 18);
    GPIOA->OSPEEDR |= (0x3U << 18);  /* PA9 high speed */
    
    GPIOA->OSPEEDR &= ~(0x3U << 20);
    GPIOA->OSPEEDR |= (0x3U << 20);  /* PA10 high speed */
    
    /* Reset USART1 */
    USART1->CR1 = 0;
    USART1->CR2 = 0;
    USART1->CR3 = 0;
    
    /* Calculate BRR with proper mantissa.fraction format for 16x oversampling
     * USARTDIV = PCLK / (16 * baud)
     * BRR[15:4] = mantissa, BRR[3:0] = fraction * 16
     * 
     * For 84 MHz APB2 @ 115200 baud: BRR = 0x2D9
     */
    USART1->BRR = 0x2D9;  /* Hardcoded for 84 MHz @ 115200 baud */
    
    /* Enable USART first, then TE - critical order! */
    USART1->CR1 = USART_CR1_UE;  /* Enable USART */
    for (volatile int i = 0; i < 1000; i++);  /* Small delay */
    
    USART1->CR1 |= USART_CR1_TE | USART_CR1_RE;  /* Enable TX and RX */
    
    /* Wait for TE to be acknowledged */
    for (volatile int i = 0; i < 100000; i++);
}

/**
 * @brief Transmit a single byte via USART1 (interrupt-driven)
 * @param data Byte to transmit
 * @return true if byte queued successfully, false if buffer full
 */
bool UART_WriteByte(uint8_t data) {
    /* Just do simple polling */
    for (int timeout = 0; timeout < 10000000; timeout++) {
        if (USART1->SR & USART_SR_TXE) {
            USART1->DR = data;
            return true;
        }
    }
    return false;
}

/**
 * @brief Transmit a string via USART1 (interrupt-driven)
 * @param str Null-terminated string to transmit
 * @return Number of bytes queued for transmission
 */
uint32_t UART_WriteString(const char *str) {
    uint32_t count = 0;
    while (*str && UART_WriteByte((uint8_t)*str)) {
        str++;
        count++;
    }
    return count;
}

/**
 * @brief Receive a single byte via USART1 (non-blocking)
 * @param data Pointer to store received byte
 * @return true if byte received, false if buffer empty
 */
bool UART_ReadByte(uint8_t *data) {
    return CircularBuffer_Get(&rxBuffer, data);
}

/**
 * @brief Check if data is available to read
 * @return Number of bytes available in RX buffer
 */
uint32_t UART_DataAvailable(void) {
    return CircularBuffer_Count(&rxBuffer);
}

/**
 * @brief Transmit data buffer via USART1 (interrupt-driven)
 * @param data Pointer to data buffer
 * @param length Number of bytes to transmit
 * @return Number of bytes queued for transmission
 */
uint32_t UART_Write(const uint8_t *data, uint32_t length) {
    uint32_t count = 0;
    for (uint32_t i = 0; i < length; i++) {
        if (UART_WriteByte(data[i])) {
            count++;
        } else {
            break;  /* Buffer full */
        }
    }
    return count;
}

/**
 * @brief Receive data into buffer via USART1 (non-blocking)
 * @param data Pointer to receive buffer
 * @param length Maximum number of bytes to receive
 * @return Number of bytes actually received
 */
uint32_t UART_Read(uint8_t *data, uint32_t length) {
    uint32_t count = 0;
    for (uint32_t i = 0; i < length; i++) {
        if (!UART_ReadByte(&data[i])) {
            break;  /* Buffer empty */
        }
        count++;
    }
    return count;
}

/**
 * @brief Get free space in TX buffer
 * @return Number of bytes that can be written
 */
uint32_t UART_GetTxFreeSpace(void) {
    return CircularBuffer_FreeSpace(&txBuffer);
}

/**
 * @brief Flush TX buffer (wait until all data is transmitted)
 */
void UART_FlushTx(void) {
    /* Wait until TX buffer is empty and transmission complete */
    while (txBuffer.count > 0 || !(USART1->SR & USART_SR_TC)) {
        /* Busy wait */
    }
}

/**
 * @brief Clear RX buffer
 */
void UART_FlushRx(void) {
    CircularBuffer_Clear(&rxBuffer);
}

/**
 * @brief USART1 interrupt handler
 * @note Must be called from USART1_IRQHandler
 */
void UART_IRQHandler(void) {
    uint32_t sr = USART1->SR;
    
    /* Handle receive interrupt */
    if (sr & USART_SR_RXNE) {
        uint8_t data = (uint8_t)(USART1->DR & 0xFF);
        CircularBuffer_Put(&rxBuffer, data);
    }
    
    /* Handle transmit interrupt */
    if (sr & USART_SR_TXE) {
        uint8_t data;
        if (CircularBuffer_Get(&txBuffer, &data)) {
            USART1->DR = data;
        } else {
            /* No more data to send, disable TX interrupt */
            USART1->CR1 &= ~USART_CR1_TXEIE;
        }
    }
    
    /* Clear overrun error if present */
    if (sr & USART_SR_ORE) {
        /* Reading SR followed by DR clears the ORE flag */
        (void)USART1->DR;
    }
}

/* USART1 interrupt handler (ISR) */
extern "C" {
    void USART1_IRQHandler(void) {
        UART_IRQHandler();
    }
}
