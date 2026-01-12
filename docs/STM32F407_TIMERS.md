# STM32F407 Timer Reference

Complete reference for all timer peripherals on the STM32F407VET6 microcontroller, including features, channels, and pin mappings.

## Timer Overview

The STM32F407 has 14 timer peripherals:
- 2 Advanced-control timers (TIM1, TIM8)
- 10 General-purpose timers (TIM2-TIM5, TIM9-TIM14)
- 2 Basic timers (TIM6, TIM7)

---

## Advanced-Control Timers

### TIM1 (APB2, up to 168 MHz)
**Features:**
- 16-bit auto-reload counter
- 4 independent channels for input capture/output compare/PWM
- Complementary outputs with dead-time insertion
- Brake input for motor control safety
- Encoder mode (quadrature decoder)
- DMA support

**Channels & Pin Mappings:**

| Channel | AF1 | AF2 | AF3 |
|---------|-----|-----|-----|
| CH1 | PA8 | PE9 | - |
| CH2 | PA9 | PE11 | - |
| CH3 | PA10 | PE13 | - |
| CH4 | PA11 | PE14 | - |
| CH1N | PA7 | PB13 | PE8 |
| CH2N | PB0 | PB14 | PE10 |
| CH3N | PB1 | PB15 | PE12 |
| ETR | PA12 | PE7 | - |
| BKIN | PA6 | PB12 | PE15 |

**Common Uses:**
- Motor control with complementary PWM
- High-frequency PWM generation
- Encoder interface with safety features

---

### TIM8 (APB2, up to 168 MHz)
**Features:**
- Same as TIM1
- 16-bit auto-reload counter
- 4 independent channels
- Complementary outputs
- Brake input
- Encoder mode

**Channels & Pin Mappings:**

| Channel | AF3 | Notes |
|---------|-----|-------|
| CH1 | PC6 | - |
| CH2 | PC7 | - |
| CH3 | PC8 | - |
| CH4 | PC9 | - |
| CH1N | PA5 | - |
| CH1N | PA7 | Shared with TIM1 |
| CH2N | PB0 | Shared with TIM1 |
| CH2N | PB14 | - |
| CH3N | PB1 | Shared with TIM1 |
| CH3N | PB15 | - |
| ETR | PA0 | - |
| BKIN | PA6 | Shared with TIM1 |

**Common Uses:**
- Secondary motor control
- Additional PWM channels
- Complementary PWM for H-bridges

---

## General-Purpose Timers (32-bit)

### TIM2 (APB1, up to 84 MHz)
**Features:**
- **32-bit** auto-reload counter
- 4 independent channels
- Encoder mode (quadrature decoder)
- Input capture, output compare, PWM
- DMA support

**Channels & Pin Mappings:**

| Channel | AF1 | AF2 | Notes |
|---------|-----|-----|-------|
| CH1 | PA0 | PA5, PA15 | - |
| CH2 | PA1 | PB3 | - |
| CH3 | PA2 | PB10 | - |
| CH4 | PA3 | PB11 | - |
| ETR | PA0 | PA15 | - |

**Common Uses:**
- High-resolution timing (32-bit)
- Long-duration pulse measurement
- Encoder interface for high count ranges

---

### TIM5 (APB1, up to 84 MHz)
**Features:**
- **32-bit** auto-reload counter
- 4 independent channels
- Same as TIM2 but different pins

**Channels & Pin Mappings:**

| Channel | AF2 | Notes |
|---------|-----|-------|
| CH1 | PA0 | Shared with TIM2 |
| CH2 | PA1 | Shared with TIM2 |
| CH3 | PA2 | Shared with TIM2 |
| CH4 | PA3 | Shared with TIM2 |

**Common Uses:**
- Independent 32-bit timing
- PWM with extended resolution

---

## General-Purpose Timers (16-bit)

### TIM3 (APB1, up to 84 MHz)
**Features:**
- 16-bit auto-reload counter
- 4 independent channels
- Encoder mode
- DMA support

**Channels & Pin Mappings:**

| Channel | AF2 | Notes |
|---------|-----|-------|
| CH1 | PA6 | Shared with TIM1 BKIN |
| CH1 | PB4 | - |
| CH1 | PC6 | Shared with TIM8 |
| CH2 | PA7 | - |
| CH2 | PB5 | - |
| CH2 | PC7 | Shared with TIM8 |
| CH3 | PB0 | - |
| CH3 | PC8 | Shared with TIM8 |
| CH4 | PB1 | - |
| CH4 | PC9 | Shared with TIM8 |
| ETR | PD2 | - |

**Common Uses:**
- Standard PWM generation (4 channels)
- Encoder interface
- Input capture timing

---

### TIM4 (APB1, up to 84 MHz)
**Features:**
- 16-bit auto-reload counter
- 4 independent channels
- Encoder mode
- DMA support

**Channels & Pin Mappings:**

| Channel | AF2 | Notes |
|---------|-----|-------|
| CH1 | PB6 | **Used for encoder A** |
| CH1 | PD12 | - |
| CH2 | PB7 | **Used for encoder B** |
| CH2 | PD13 | - |
| CH3 | PB8 | - |
| CH3 | PD14 | - |
| CH4 | PB9 | - |
| CH4 | PD15 | - |
| ETR | PE0 | - |

**Common Uses:**
- Quadrature encoder decoding (PB6/PB7)
- Multi-channel PWM
- Timing and counting

---

### TIM9 (APB2, up to 168 MHz)
**Features:**
- 16-bit auto-reload counter
- 2 channels only
- No encoder mode
- Basic PWM and input capture

**Channels & Pin Mappings:**

| Channel | AF3 | Notes |
|---------|-----|-------|
| CH1 | PA2 | - |
| CH1 | PE5 | - |
| CH2 | PA3 | - |
| CH2 | PE6 | - |

**Common Uses:**
- Basic PWM (2 channels)
- Simple timing applications

---

### TIM10 (APB2, up to 168 MHz)
**Features:**
- 16-bit auto-reload counter
- 1 channel only
- Basic PWM and input capture

**Channels & Pin Mappings:**

| Channel | AF3 | Notes |
|---------|-----|-------|
| CH1 | PB8 | - |
| CH1 | PF6 | - |

**Common Uses:**
- Single-channel PWM
- Basic timing

---

### TIM11 (APB2, up to 168 MHz)
**Features:**
- 16-bit auto-reload counter
- 1 channel only
- Basic PWM and input capture

**Channels & Pin Mappings:**

| Channel | AF3 | Notes |
|---------|-----|-------|
| CH1 | PB9 | - |
| CH1 | PF7 | - |

**Common Uses:**
- Single-channel PWM
- Basic timing

---

### TIM12 (APB1, up to 84 MHz)
**Features:**
- 16-bit auto-reload counter
- 2 channels only
- Basic PWM and input capture

**Channels & Pin Mappings:**

| Channel | AF9 | Notes |
|---------|-----|-------|
| CH1 | PB14 | - |
| CH1 | PH6 | - |
| CH2 | PB15 | - |
| CH2 | PH9 | - |

**Common Uses:**
- Basic PWM (2 channels)
- Timing applications

---

### TIM13 (APB1, up to 84 MHz)
**Features:**
- 16-bit auto-reload counter
- 1 channel only
- Basic PWM and input capture

**Channels & Pin Mappings:**

| Channel | AF9 | Notes |
|---------|-----|-------|
| CH1 | PA6 | - |
| CH1 | PF8 | - |

**Common Uses:**
- Single-channel PWM
- Basic timing

---

### TIM14 (APB1, up to 84 MHz)
**Features:**
- 16-bit auto-reload counter
- 1 channel only
- Basic PWM and input capture

**Channels & Pin Mappings:**

| Channel | AF9 | Notes |
|---------|-----|-------|
| CH1 | PA7 | - |
| CH1 | PF9 | - |

**Common Uses:**
- Single-channel PWM
- Basic timing

---

## Basic Timers (No I/O Pins)

### TIM6 (APB1, up to 84 MHz)
**Features:**
- 16-bit auto-reload counter
- No output channels
- Used for time base generation
- DAC trigger
- DMA support

**Common Uses:**
- Timebase for DAC
- General-purpose timing
- Periodic interrupts

---

### TIM7 (APB1, up to 84 MHz)
**Features:**
- 16-bit auto-reload counter
- No output channels
- Used for time base generation
- DAC trigger
- DMA support

**Common Uses:**
- Timebase for DAC
- General-purpose timing
- Periodic interrupts

---

## Feature Comparison Table

| Timer | Bits | Channels | Encoder | Complementary | Break | Bus | Max Freq |
|-------|------|----------|---------|---------------|-------|-----|----------|
| TIM1 | 16 | 4 | ✓ | ✓ | ✓ | APB2 | 168 MHz |
| TIM8 | 16 | 4 | ✓ | ✓ | ✓ | APB2 | 168 MHz |
| TIM2 | 32 | 4 | ✓ | ✗ | ✗ | APB1 | 84 MHz |
| TIM5 | 32 | 4 | ✓ | ✗ | ✗ | APB1 | 84 MHz |
| TIM3 | 16 | 4 | ✓ | ✗ | ✗ | APB1 | 84 MHz |
| TIM4 | 16 | 4 | ✓ | ✗ | ✗ | APB1 | 84 MHz |
| TIM9 | 16 | 2 | ✗ | ✗ | ✗ | APB2 | 168 MHz |
| TIM10 | 16 | 1 | ✗ | ✗ | ✗ | APB2 | 168 MHz |
| TIM11 | 16 | 1 | ✗ | ✗ | ✗ | APB2 | 168 MHz |
| TIM12 | 16 | 2 | ✗ | ✗ | ✗ | APB1 | 84 MHz |
| TIM13 | 16 | 1 | ✗ | ✗ | ✗ | APB1 | 84 MHz |
| TIM14 | 16 | 1 | ✗ | ✗ | ✗ | APB1 | 84 MHz |
| TIM6 | 16 | 0 | ✗ | ✗ | ✗ | APB1 | 84 MHz |
| TIM7 | 16 | 0 | ✗ | ✗ | ✗ | APB1 | 84 MHz |

---

## Encoder Mode (Quadrature Decoder)

**Supported Timers:** TIM1, TIM2, TIM3, TIM4, TIM5, TIM8

**Encoder Modes:**
1. **Mode 1**: Count on TI1 edges only
2. **Mode 2**: Count on TI2 edges only
3. **Mode 3**: Count on both TI1 and TI2 edges (most common)

**Recommended Configurations:**
- **TIM4 with PB6/PB7**: Commonly used, easily accessible on most boards
- **TIM3 with PA6/PA7**: Alternative if TIM4 is in use
- **TIM2 with PA0/PA1**: 32-bit counter for very high count ranges

---

## PWM Configuration Notes

**Maximum PWM Frequency:**
- APB2 timers (TIM1, TIM8-11): Up to 168 MHz / prescaler / ARR
- APB1 timers (TIM2-7, TIM12-14): Up to 84 MHz / prescaler / ARR

**Typical PWM Frequencies:**
- Servo control: 50 Hz (20 ms period)
- Motor control: 10-50 kHz
- LED dimming: 1-100 kHz
- Audio: 20-100 kHz

**Resolution vs Frequency Trade-off:**
- At 20 kHz with 84 MHz clock: ~4200 steps (12-bit resolution)
- At 100 kHz with 84 MHz clock: ~840 steps (10-bit resolution)

---

## Current Project Usage

### TIM3 - Motor PWM
- **Channel 3**: PB0 (AF2)
- **Purpose**: Motor speed control
- **Configuration**: 20 kHz PWM

### TIM4 - Encoder Interface
- **Channel 1**: PB6 (AF2) - Encoder A
- **Channel 2**: PB7 (AF2) - Encoder B
- **Purpose**: Quadrature decoder for position feedback
- **Configuration**: Encoder mode 3, 16-bit counter with filtering

---

## Pin Conflict Notes

**Common Pin Sharing Issues:**
- PA6: TIM1_BKIN, TIM3_CH1, TIM13_CH1
- PA7: TIM1_CH1N, TIM3_CH2, TIM14_CH1
- PB0: TIM1_CH2N, TIM3_CH3
- PB1: TIM1_CH3N, TIM3_CH4

**Tip**: Always check datasheet Table 9 (Alternate function mapping) for complete pin assignment details.

---

## References

- STM32F407xx Reference Manual (RM0090)
- STM32F407xx Datasheet (DS8626)
- AN4013: Introduction to TIM (STM32 timer)
