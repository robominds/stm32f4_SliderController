# Quadrature Encoder Decoder

Hardware timer-based quadrature encoder interface for STM32F407VET6.

## Features

- Hardware encoder mode using STM32 timers (TIM2-TIM5)
- 4x resolution (counts on both edges of both channels)
- 32-bit position tracking with automatic overflow/underflow handling
- Direction detection
- Configurable GPIO pins and alternate functions
- Pull-up resistors on inputs for noise immunity

## Supported Timers and Pins

### TIM2 (Alternate Function: AF1)
| Channel | Available Pins |
|---------|----------------|
| CH1     | PA0, PA5, PA15 |
| CH2     | PA1, PB3       |

**Common pairs:** PA0/PA1, PA15/PB3

### TIM3 (Alternate Function: AF2)
| Channel | Available Pins |
|---------|----------------|
| CH1     | PA6, PB4, PC6  |
| CH2     | PA7, PB5, PC7  |

**Common pairs:** PA6/PA7, PB4/PB5, PC6/PC7

### TIM4 (Alternate Function: AF2)
| Channel | Available Pins |
|---------|----------------|
| CH1     | PB6, PD12      |
| CH2     | PB7, PD13      |

**Common pairs:** PB6/PB7, PD12/PD13

### TIM5 (Alternate Function: AF2)
| Channel | Available Pins |
|---------|----------------|
| CH1     | PA0, PH10      |
| CH2     | PA1, PH11      |

**Common pairs:** PA0/PA1, PH10/PH11

## API Reference

### Initialization

```cpp
QuadEncoder_Config config = {
    .timer_base = TIM3_BASE,      // Timer peripheral
    .gpio_base = GPIOA_BASE,      // GPIO port
    .pin_a = 6,                   // Channel A pin (PA6)
    .pin_b = 7,                   // Channel B pin (PA7)
    .af_number = 2,               // Alternate function (AF2 for TIM3)
    .counts_per_rev = 1024        // Encoder resolution
};

QuadEncoder_Handle encoder;
if (QuadEncoder_Init(&encoder, &config)) {
    // Initialization successful
}
```

### Reading Position

```cpp
// Get current position in counts
int32_t position = QuadEncoder_GetPosition(&encoder);

// Position automatically tracks overflow/underflow
// Range: -2,147,483,648 to 2,147,483,647
```

### Resetting Position

```cpp
// Reset position to zero
QuadEncoder_Reset(&encoder);

// Or set to specific value
QuadEncoder_SetPosition(&encoder, 1000);
```

### Direction Detection

```cpp
// Get rotation direction
int8_t direction = QuadEncoder_GetDirection(&encoder);
// Returns:
//   1  = forward (up-counting)
//  -1  = reverse (down-counting)
//   0  = stationary
```

## Resolution and Counts Per Revolution

The encoder operates in **4x mode**, counting on both edges of both channels:

| Encoder PPR | Counts Per Revolution |
|-------------|----------------------|
| 256         | 1024                 |
| 512         | 2048                 |
| 1024        | 4096                 |
| 2048        | 8192                 |

**Formula:** `counts_per_rev = encoder_PPR × 4`

## Configuration Examples

### Example 1: TIM2 on PA0/PA1
```cpp
QuadEncoder_Config config = {
    .timer_base = TIM2_BASE,
    .gpio_base = GPIOA_BASE,
    .pin_a = 0,        // PA0 (TIM2_CH1)
    .pin_b = 1,        // PA1 (TIM2_CH2)
    .af_number = 1,    // AF1 for TIM2
    .counts_per_rev = 2048  // 512 PPR encoder
};
```

### Example 2: TIM3 on PB4/PB5
```cpp
QuadEncoder_Config config = {
    .timer_base = TIM3_BASE,
    .gpio_base = GPIOB_BASE,
    .pin_a = 4,        // PB4 (TIM3_CH1)
    .pin_b = 5,        // PB5 (TIM3_CH2)
    .af_number = 2,    // AF2 for TIM3
    .counts_per_rev = 4096  // 1024 PPR encoder
};
```

### Example 3: TIM4 on PB6/PB7
```cpp
QuadEncoder_Config config = {
    .timer_base = TIM4_BASE,
    .gpio_base = GPIOB_BASE,
    .pin_a = 6,        // PB6 (TIM4_CH1)
    .pin_b = 7,        // PB7 (TIM4_CH2)
    .af_number = 2,    // AF2 for TIM4
    .counts_per_rev = 1024  // 256 PPR encoder
};
```

## Typical Usage Pattern

```cpp
// 1. Initialize encoder
QuadEncoder_Config config = { /* ... */ };
QuadEncoder_Handle encoder;
QuadEncoder_Init(&encoder, &config);

// 2. Reset to known position
QuadEncoder_Reset(&encoder);

// 3. In main loop or periodic callback
while (1) {
    int32_t position = QuadEncoder_GetPosition(&encoder);
    int8_t direction = QuadEncoder_GetDirection(&encoder);
    
    // Use position and direction for control
    // ...
    
    delay_ms(10);
}
```

## Hardware Connections

### Standard Quadrature Encoder
```
Encoder A → MCU Pin (Channel A)
Encoder B → MCU Pin (Channel B)
Encoder GND → MCU GND
Encoder VCC → 3.3V or 5V (depending on encoder)
```

### Pull-up Configuration
The driver automatically configures internal pull-up resistors on both input pins. For high-speed or noisy environments, consider adding external pull-up resistors (1kΩ-10kΩ).

## Notes

- Timer counter is 16-bit (0-65535), but position tracking is 32-bit
- Automatic overflow/underflow handling maintains accurate position
- Both pins must be on the same GPIO port
- Maximum count frequency depends on system clock (typically several MHz)
- For best noise immunity, keep encoder wires short and use shielded cable

## Files

- `inc/quadrature_decoder.h` - Header file with API
- `src/quadrature_decoder.cpp` - Implementation
- `inc/timer_common.h` - Shared timer register definitions
