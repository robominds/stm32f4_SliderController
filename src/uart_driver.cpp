/**
  * @file    uart_driver.cpp
  * @brief   Interrupt-driven UART driver implementation for STM32F407VET6
  *          USART1 driver with TX on PA9 and RX on PA10
  */

#include "uart_driver.h"
#include "stm32f407xx.h"

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
 * @brief Initialize USART1 with interrupt-driven operation
 * @param baudrate Desired baud rate (e.g., 9600, 115200)
 * 
 * Configuration:
 * - PA9: USART1_TX (AF7)
 * - PA10: USART1_RX (AF7)
 * - 8 data bits, 1 stop bit, no parity
 * - Assumes APB2 clock = 84 MHz (with 168 MHz system clock)
 */
void UART_Init(uint32_t baudrate) {
    /* Initialize circular buffers */
    CircularBuffer_Clear(&rxBuffer);
    CircularBuffer_Clear(&txBuffer);
    
    /* Enable GPIOA clock */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    
    /* Enable USART1 clock (APB2) */
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    
    /* Configure PA9 (TX) and PA10 (RX) as alternate function */
    /* MODER: 00 = Input, 01 = Output, 10 = Alternate, 11 = Analog */
    GPIOA->MODER &= ~(0x3U << (9 * 2));   /* Clear PA9 mode bits */
    GPIOA->MODER |= (0x2U << (9 * 2));    /* Set PA9 as alternate function */
    
    GPIOA->MODER &= ~(0x3U << (10 * 2));  /* Clear PA10 mode bits */
    GPIOA->MODER |= (0x2U << (10 * 2));   /* Set PA10 as alternate function */
    
    /* Set alternate function to AF7 (USART1) for PA9 and PA10 */
    /* AFR[1] is for pins 8-15 */
    GPIOA->AFR[1] &= ~(0xFU << ((9 - 8) * 4));   /* Clear PA9 AF bits */
    GPIOA->AFR[1] |= (7U << ((9 - 8) * 4));      /* Set PA9 to AF7 */
    
    GPIOA->AFR[1] &= ~(0xFU << ((10 - 8) * 4));  /* Clear PA10 AF bits */
    GPIOA->AFR[1] |= (7U << ((10 - 8) * 4));     /* Set PA10 to AF7 */
    
    /* Configure output type as push-pull for TX */
    GPIOA->OTYPER &= ~(1U << 9);   /* PA9 push-pull */
    
    /* Set high speed */
    GPIOA->OSPEEDR |= (0x3U << (9 * 2));   /* PA9 high speed */
    GPIOA->OSPEEDR |= (0x3U << (10 * 2));  /* PA10 high speed */
    
    /* Set pull-up for both TX and RX */
    GPIOA->PUPDR &= ~(0x3U << (9 * 2));    /* Clear PA9 pull bits */
    GPIOA->PUPDR |= (0x1U << (9 * 2));     /* Set PA9 pull-up */
    
    GPIOA->PUPDR &= ~(0x3U << (10 * 2));   /* Clear PA10 pull bits */
    GPIOA->PUPDR |= (0x1U << (10 * 2));    /* Set PA10 pull-up */
    
    /* Configure USART1 */
    /* Disable USART1 before configuration */
    USART1->CR1 &= ~USART_CR1_UE;
    
    /* Configure baud rate */
    /* BRR format: [15:4]=mantissa, [3:0]=fraction for 16x oversampling */
    /* USARTDIV = PCLK / (16 * baudrate) */
    /* With APB2 = 84 MHz and baudrate = 115200: BRR = 0x2D9 */
    if (baudrate == 115200) {
        USART1->BRR = 0x2D9;  /* Verified working for 84 MHz @ 115200 baud */
    } else {
        /* Generic calculation for other baud rates */
        uint32_t apb2_clock = 84000000;
        uint32_t usartdiv_x100 = (100 * apb2_clock) / (16 * baudrate);
        uint32_t mantissa = usartdiv_x100 / 100;
        uint32_t fraction = ((usartdiv_x100 - (mantissa * 100)) * 16 + 50) / 100;
        if (fraction >= 16) {
            mantissa++;
            fraction = 0;
        }
        USART1->BRR = (mantissa << 4) | (fraction & 0x0F);
    }
    
    /* Configure USART1 CR1 */
    /* 8 data bits (M=0), no parity (PCE=0) */
    USART1->CR1 = 0;
    USART1->CR1 |= USART_CR1_TE;      /* Enable transmitter */
    USART1->CR1 |= USART_CR1_RE;      /* Enable receiver */
    USART1->CR1 |= USART_CR1_RXNEIE;  /* Enable RXNE interrupt */
    /* Note: TXEIE is enabled only when we have data to send */
    
    /* Configure USART1 CR2 */
    /* 1 stop bit (STOP[1:0] = 00) */
    USART1->CR2 = 0;
    
    /* Configure USART1 CR3 */
    /* No hardware flow control */
    USART1->CR3 = 0;
    
    /* Enable USART1 */
    USART1->CR1 |= USART_CR1_UE;
    
    /* Enable USART1 interrupt in NVIC */
    /* USART1_IRQn = 37 */
    NVIC->ISER[USART1_IRQn / 32] = (1U << (USART1_IRQn % 32));
}

/**
 * @brief Transmit a single byte via USART1 (interrupt-driven)
 * @param data Byte to transmit
 * @return true if byte queued successfully, false if buffer full
 */
bool UART_WriteByte(uint8_t data) {
    /* Disable TX interrupt temporarily */
    uint32_t cr1 = USART1->CR1;
    USART1->CR1 &= ~USART_CR1_TXEIE;
    
    bool result = CircularBuffer_Put(&txBuffer, data);
    
    /* Enable TX interrupt to start transmission */
    USART1->CR1 = cr1 | USART_CR1_TXEIE;
    
    return result;
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
