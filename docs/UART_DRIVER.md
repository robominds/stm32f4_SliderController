# Interrupt-Driven UART Driver for STM32F407VET6

## Overview
This interrupt-driven UART driver provides efficient serial communication using USART1 on the STM32F407VET6 microcontroller. The driver uses circular buffers and interrupts to handle TX and RX operations without blocking the main application.

## Hardware Configuration

### Pin Mapping
- **PA9**: USART1_TX (Transmit)
- **PA10**: USART1_RX (Receive)

### Alternate Function
Both pins are configured to use Alternate Function 7 (AF7) for USART1.

## Features
- **Interrupt-driven operation**: Non-blocking TX and RX
- **Circular buffers**: 256-byte buffers for both TX and RX
- **Configurable baud rate**: Supports standard baud rates
- **8 data bits, 1 stop bit, no parity**
- **Thread-safe buffer operations**: Proper interrupt handling
- **Error handling**: Overrun error detection and recovery
- **Buffer management**: Query buffer status and flush operations

## API Reference

### Initialization
```c
void UART_Init(uint32_t baudrate);
```
Initializes USART1 with interrupt-driven operation at the specified baud rate.
- **Parameters**: 
  - `baudrate`: Desired baud rate (e.g., 9600, 115200)
- **Example**:
  ```c
  UART_Init(115200);  // Initialize at 115200 baud
  ```

### Transmit Functions (Non-blocking)

#### Send Single Byte
```c
bool UART_WriteByte(uint8_t data);
```
Queues a single byte for transmission (non-blocking).
- **Returns**: `true` if byte queued successfully, `false` if TX buffer full
- **Example**:
  ```c
  if (UART_WriteByte('A')) {
      // Byte queued successfully
  }
  ```

#### Send String
```c
uint32_t UART_WriteString(const char *str);
```
Queues a null-terminated string for transmission.
- **Returns**: Number of bytes queued (may be less than string length if buffer fills)
- **Example**:
  ```c
  uint32_t sent = UART_WriteString("Hello, World!\r\n");
  ```

#### Send Buffer
```c
uint32_t UART_Write(const uint8_t *data, uint32_t length);
```
Queues a data buffer for transmission.
- **Returns**: Number of bytes queued (may be less than requested if buffer fills)
- **Example**:
  ```c
  uint8_t buffer[] = {0x01, 0x02, 0x03};
  uint32_t sent = UART_Write(buffer, 3);
  ```

### Receive Functions (Non-blocking)

#### Receive Single Byte
```c
bool UART_ReadByte(uint8_t *data);
```
Reads a single byte from the RX buffer (non-blocking).
- **Parameters**:
  - `data`: Pointer to store the received byte
- **Returns**: `true` if byte read successfully, `false` if RX buffer empty
- **Example**:
  ```c
  uint8_t byte;
  if (UART_ReadByte(&byte)) {
      // Process received byte
  }
  ```

#### Receive Buffer
```c
uint32_t UART_Read(uint8_t *data, uint32_t length);
```
Reads multiple bytes from the RX buffer (non-blocking).
- **Returns**: Number of bytes actually read (may be less than requested)
- **Example**:
  ```c
  uint8_t buffer[10];
  uint32_t received = UART_Read(buffer, 10);
  ```

#### Check Data Availability
```c
uint32_t UART_DataAvailable(void);
```
Returns the number of bytes available in the RX buffer.
- **Returns**: Number of bytes available to read
- **Example**:
  ```c
  if (UART_DataAvailable() > 0) {
      uint8_t byte;
      UART_ReadByte(&byte);
  }
  ```

### Buffer Management

#### Get TX Free Space
```c
uint32_t UART_GetTxFreeSpace(void);
```
Returns the number of bytes that can be written to the TX buffer.
- **Returns**: Number of free bytes in TX buffer

#### Flush TX Buffer
```c
void UART_FlushTx(void);
```
Blocks until all queued data has been transmitted.
- **Example**:
  ```c
  UART_WriteString("Important message\r\n");
  UART_FlushTx();  // Wait until completely sent
  ```

#### Flush RX Buffer
```c
void UART_FlushRx(void);
```
Clears all data from the RX buffer.
- **Example**:
  ```c
  UART_FlushRx();  // Discard any pending received data
  ```

## Technical Details

### Circular Buffers
- **RX Buffer**: 256 bytes
- **TX Buffer**: 256 bytes
- Implemented as ring buffers with head/tail pointers
- Thread-safe with interrupt disable/enable protection

### Interrupt Operation
- **RX Interrupt (RXNE)**: Always enabled, fires when data received
- **TX Interrupt (TXE)**: Enabled only when data is in TX buffer
- **NVIC Priority**: Uses default priority
- **IRQ Number**: 37 (USART1_IRQn)

### Clock Configuration
The driver assumes:
- System Clock: 168 MHz
- APB2 Clock: 84 MHz (USART1 is on APB2 bus)
- Baud rate calculation: `BRR = 84000000 / baudrate`

If your system uses different clock settings, adjust the `apb2_clock` variable in `UART_Init()`.

## Usage Examples

### Basic Echo Example
```c
#include "uart_driver.h"

int main(void) {
    UART_Init(115200);
    UART_WriteString("Echo mode active\r\n");
    
    while (1) {
        uint8_t byte;
        if (UART_ReadByte(&byte)) {
            UART_WriteByte(byte);  // Echo back
        }
    }
}
```

### Command Processing Example
```c
#include "uart_driver.h"
#include <string.h>

#define CMD_BUFFER_SIZE 64

void processCommand(const char *cmd) {
    if (strcmp(cmd, "STATUS") == 0) {
        UART_WriteString("System OK\r\n");
    } else if (strcmp(cmd, "VERSION") == 0) {
        UART_WriteString("v1.0.0\r\n");
    } else {
        UART_WriteString("Unknown command\r\n");
    }
}

int main(void) {
    UART_Init(115200);
    UART_WriteString("Ready for commands\r\n");
    
    char cmdBuffer[CMD_BUFFER_SIZE];
    uint32_t cmdIndex = 0;
    
    while (1) {
        uint8_t byte;
        if (UART_ReadByte(&byte)) {
            if (byte == '\r' || byte == '\n') {
                if (cmdIndex > 0) {
                    cmdBuffer[cmdIndex] = '\0';
                    processCommand(cmdBuffer);
                    cmdIndex = 0;
                }
            } else if (cmdIndex < CMD_BUFFER_SIZE - 1) {
                cmdBuffer[cmdIndex++] = byte;
            }
        }
    }
}
```

### Using with FreeRTOS
```c
#include "uart_driver.h"
#include "FreeRTOS.h"
#include "task.h"

void uartTask(void *pvParameters) {
    UART_Init(115200);
    UART_WriteString("UART Task Started\r\n");
    
    while (1) {
        if (UART_DataAvailable() > 0) {
            uint8_t byte;
            UART_ReadByte(&byte);
            UART_WriteByte(byte);
        }
        vTaskDelay(pdMS_TO_TICKS(10));  // Yield to other tasks
    }
}
```

## Testing

### Hardware Setup
1. Connect PA9 (TX) to the RX pin of your serial adapter
2. Connect PA10 (RX) to the TX pin of your serial adapter
3. Connect GND pins together
4. Use a terminal program at the configured baud rate

### Terminal Setup (macOS/Linux)
```bash
screen /dev/tty.usbserial-* 115200
```

### Terminal Setup (Windows)
Use PuTTY or another serial terminal:
- Baud rate: 115200
- Data bits: 8
- Parity: None
- Stop bits: 1
- Flow control: None

## Advantages of Interrupt-Driven Approach

### vs. Polling/Blocking
- **No CPU blocking**: Main application continues running while data transfers
- **Better throughput**: Can handle high-speed communication without data loss
- **Lower latency**: Immediate response to incoming data
- **Power efficiency**: CPU can enter sleep modes between interrupts

### Buffer Benefits
- **Handles burst traffic**: Buffers smooth out temporary high data rates
- **Decouples timing**: TX/RX timing independent of application timing
- **Prevents data loss**: Data stored until application processes it

## Notes and Limitations

- **Buffer Size**: 256 bytes each for TX and RX. Increase if needed for your application
- **No DMA**: This implementation uses interrupts, not DMA (DMA would be even more efficient)
- **Thread Safety**: Buffer operations are protected against interrupt corruption
- **Error Handling**: Overrun errors are cleared automatically
- **No Flow Control**: Hardware flow control (RTS/CTS) is not implemented
- **ISR Context**: `UART_IRQHandler()` must be called from `USART1_IRQHandler()`

## Memory Usage

- **Code**: ~880 bytes
- **Static Data (BSS)**: 512 bytes (two 256-byte buffers)
- **Stack**: Minimal (interrupt context)

## Performance

At 115200 baud:
- **Maximum throughput**: ~11,520 bytes/second
- **Interrupt frequency**: Up to 11.52 kHz (one per byte)
- **CPU overhead**: <5% at maximum throughput
