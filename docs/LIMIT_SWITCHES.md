# Limit Switch Feature Documentation

## Overview

The motor controller now supports hardware limit switches for position boundaries and safety. This feature provides:

- **Hardware safety stops** - Prevents motor from driving past physical limits
- **Debouncing** - Filters out switch bounce and electrical noise
- **Automatic homing** - Find and zero position at minimum limit
- **Flexible configuration** - Supports active-low or active-high switches with optional pull-ups

## Hardware Configuration

### Typical Wiring

**Active-Low Configuration (Common):**
```
                    VCC
                     |
                    [R] 10kΩ pull-up (or use internal)
                     |
    STM32 GPIO ------+------ Switch ------ GND
                     |
                 (pin_min/pin_max)
```

When switch is **open**: Pin reads HIGH (not active)
When switch is **pressed**: Pin reads LOW (active)

**Active-High Configuration:**
```
    STM32 GPIO ------ Switch ------ VCC
         |
        [R] 10kΩ pull-down
         |
        GND
```

### Recommended Pin Assignments

For STM32F407VET6, any GPIO pins can be used. Suggested configurations:

| Configuration | Min Limit | Max Limit | GPIO Port |
|--------------|-----------|-----------|-----------|| **Current** | **PB8** | **PB9** | **GPIOB** || Option 1 | PC0 | PC1 | GPIOC |
| Option 2 | PD0 | PD1 | GPIOD |
| Option 3 | PE0 | PE1 | GPIOE |

**Note:** Avoid using pins that conflict with:
- Timer channels (TIM3, TIM4)
- UART pins (PA9/PA10 for USART1)
- LED pins (PA6/PA7)

## Software Configuration

### Basic Configuration Structure

```cpp
#include "motor_controller.h"

// Define limit switch configuration
const LimitSwitch_Config limit_cfg = {
    GPIOC_BASE,     // gpio_base - GPIO port (GPIOA-GPIOE)
    0,              // pin_min - Pin for minimum limit (0-15)
    1,              // pin_max - Pin for maximum limit (0-15)
    true,           // active_low - true for active-low switches
    true            // enable_pullup - true to use internal pull-ups
};

// Motor PWM configuration (example)
const PWM_Config motor_pwm_cfg[2] = {
    {TIM3_BASE, GPIOB_BASE, 0, 2, PWM_CHANNEL_3, 20000U},  // Channel 0 (reverse)
    {TIM3_BASE, GPIOB_BASE, 1, 2, PWM_CHANNEL_4, 20000U}   // Channel 1 (forward)
};

// Encoder configuration
const QuadEncoder_Config encoder_cfg = {
    TIM4_BASE,   // timer_base
    GPIOB_BASE,  // gpio_base
    6,           // pin_a: PB6
    7,           // pin_b: PB7
    2,           // af_number
    2048         // counts_per_rev
};

// Initialize motor controller with limit switches
MotorController motor;
bool init_ok = motor.init(motor_pwm_cfg, encoder_cfg, limit_cfg);
```

### Configuration Parameters

#### `gpio_base`
Base address of GPIO port:
- `GPIOA_BASE` for Port A
- `GPIOB_BASE` for Port B
- `GPIOC_BASE` for Port C
- `GPIOD_BASE` for Port D
- `GPIOE_BASE` for Port E

#### `pin_min` / `pin_max`
Pin numbers (0-15) for minimum and maximum limit switches.

#### `active_low`
- `true`: Switch active when grounded (common configuration)
- `false`: Switch active when connected to VCC

#### `enable_pullup`
- `true`: Enable internal pull-up resistors (~40kΩ)
- `false`: No internal pull-up (use external resistor)

**Recommendation:** Use `enable_pullup = true` for active-low switches unless external pull-ups are already present.

## API Usage

### Initialization

**Without Limit Switches:**
```cpp
bool init(const PWM_Config pwm_cfg[], const QuadEncoder_Config &enc_cfg);
```

**With Limit Switches:**
```cpp
bool init(const PWM_Config pwm_cfg[], const QuadEncoder_Config &enc_cfg, 
          const LimitSwitch_Config &limit_cfg);
```

### Checking Limit Status

```cpp
// Check individual limits
if (motor.isMinLimitActive()) {
    UART_WriteString("At minimum limit!\r\n");
}

if (motor.isMaxLimitActive()) {
    UART_WriteString("At maximum limit!\r\n");
}

// Check if any limit is active
if (motor.isAnyLimitActive()) {
    UART_WriteString("Limit switch triggered!\r\n");
}
```

### Homing Sequence

The homing function automatically moves to the minimum limit and zeros the position:

```cpp
// Perform homing at 20% duty with 30 second timeout
bool homed = motor.homeToMinLimit(20.0f, 30000);

if (homed) {
    UART_WriteString("Homing successful!\r\n");
    // Position is now zero at minimum limit
} else {
    UART_WriteString("Homing failed - check hardware\r\n");
}
```

**Homing Parameters:**
- `homing_duty`: Duty cycle percentage (typically 10-30%)
- `timeout_ms`: Maximum time to find limit in milliseconds

**Homing Behavior:**
1. Validates limit switches are not active at start (error if triggered)
2. If already at min limit, backs off slightly
3. Moves toward minimum limit at specified duty
4. Stops when limit switch activates
5. Resets encoder position to zero
6. Returns `true` if successful, `false` on timeout or if max limit hit

**Important:** The position control task (`startPositionControlTask`) automatically calls `homeToMinLimit()` at startup with 20% duty and 15 second timeout.

### Enable/Disable Limit Checking

```cpp
// Enable limit switch checking
motor.setLimitSwitchEnabled(true);

// Disable limit switch checking (for testing/override)
motor.setLimitSwitchEnabled(false);
```

**Warning:** Disabling limit switches removes safety protection. Only disable for testing or recovery operations.

## Safety Features

### Automatic Motion Blocking

The position control loop automatically prevents motion that would violate limits:

```cpp
// Position control loop checks limits before applying duty
if (motor.isMinLimitActive() && duty_reverse > 0) {
    // Block reverse motion - set duty to 0
}

if (motor.isMaxLimitActive() && duty_forward > 0) {
    // Block forward motion - set duty to 0
}
```

### Debouncing

Built-in software debouncing filters switch bounce:
- Requires 1 consecutive read to confirm limit active (configurable)
- Prevents false triggers from electrical noise
- Adjustable via `DEBOUNCE_THRESHOLD` constant (currently set to 1 in header)
- **Note:** Lower threshold = faster response but less noise immunity

### Direction Assumptions

**Default configuration:**
- Channel 0 (pwm_[0]): **Reverse/Negative** direction (toward minimum limit)
- Channel 1 (pwm_[1]): **Forward/Positive** direction (toward maximum limit)

Adjust motor wiring or swap channel assignments if your motor moves opposite direction.

## Example Application

### Complete Initialization and Homing

```cpp
#include "motor_controller.h"
#include "uart_driver.h"

void setupMotorSystem() {
    // Configure limit switches on PB8 (min) and PB9 (max) - current project configuration
    const LimitSwitch_Config limit_cfg = {
        GPIOB_BASE,     // Port B
        8,              // PB8 for minimum limit
        9,              // PB9 for maximum limit
        true,           // Active-low (switch grounds pin)
        true            // Enable internal pull-ups
    };
    
    // Configure PWM for H-bridge control
    const PWM_Config motor_pwm[2] = {
        {TIM3_BASE, GPIOC_BASE, 9, 2, PWM_CHANNEL_4, 20000U},  // PC9
        {TIM3_BASE, GPIOC_BASE, 8, 2, PWM_CHANNEL_3, 20000U}   // PC8
    };
    
    // Configure encoder
    const QuadEncoder_Config encoder_cfg = {
        TIM4_BASE, GPIOB_BASE, 6, 7, 2, 2048
    };
    
    // Initialize motor controller
    MotorController motor;
    if (!motor.init(motor_pwm, encoder_cfg, limit_cfg)) {
        UART_WriteString("ERROR: Motor init failed\r\n");
        return;
    }
    
    UART_WriteString("Motor initialized, starting homing...\r\n");
    
    // Home to minimum limit
    if (motor.homeToMinLimit(20.0f, 30000)) {
        UART_WriteString("Homing complete!\r\n");
        
        // Start position control
        motor.startPositionControlTask(0.1f, 0.05f, 10, 1000, 50.0f);
        UART_WriteString("Position control active\r\n");
    } else {
        UART_WriteString("Homing failed!\r\n");
    }
}
```

### Monitoring Limits During Operation

```cpp
void monitorLimits(MotorController &motor) {
    static bool last_min_state = false;
    static bool last_max_state = false;
    
    bool min_active = motor.isMinLimitActive();
    bool max_active = motor.isMaxLimitActive();
    
    // Report state changes
    if (min_active && !last_min_state) {
        UART_WriteString("Min limit activated!\r\n");
    }
    if (max_active && !last_max_state) {
        UART_WriteString("Max limit activated!\r\n");
    }
    
    last_min_state = min_active;
    last_max_state = max_active;
}
```

## Troubleshooting

### Issue: Limit switches always read as active

**Causes:**
- `active_low` setting incorrect
- Switches wired backwards
- Pull-up/pull-down incorrect

**Solutions:**
1. Check physical switch state with multimeter
2. Toggle `active_low` in configuration
3. Verify pull-up resistors (internal or external)

### Issue: Limit switches never trigger

**Causes:**
- Wrong GPIO port/pin configured
- Switch not making contact
- Active-low setting incorrect

**Solutions:**
1. Verify pin numbers match physical connections (current: PB8/PB9)
2. Test switch continuity with multimeter
3. Check `active_low` configuration matches physical wiring
4. Current `DEBOUNCE_THRESHOLD` is 1, so debouncing is minimal

### Issue: Homing times out

**Causes:**
- Motor not moving (wrong channel?)
- Limit switch not working
- Motor stalled or blocked

**Solutions:**
1. Test motor without limit switches first
2. Verify limit switch triggers manually
3. Increase `timeout_ms` parameter
4. Check mechanical obstructions

### Issue: Motor jitters at limit

**Causes:**
- Control loop fighting the limit
- Position target beyond limit
- Debouncing insufficient

**Solutions:**
1. Set target position away from limits
2. Increase `DEBOUNCE_THRESHOLD`
3. Add deadband around limits in control loop

## Technical Details

### Memory Usage

Adds minimal overhead:
- Configuration struct: 8 bytes
- State variables: 12 bytes
- Total: ~20 bytes RAM

### Performance

- Debouncing requires 1 consecutive read (configurable via `DEBOUNCE_THRESHOLD`)
- Limit checks add ~5-10 CPU cycles per control loop
- Negligible impact on control performance
- **Note:** Position control task automatically performs homing on startup

### Thread Safety

- Limit reading is thread-safe (read-only GPIO operations)
- Position control task handles synchronization
- No mutex needed for limit status queries

## References

- [Motor Controller Implementation](../src/motor_controller.cpp)
- [Motor Controller Header](../inc/motor_controller.h)
- [STM32F407 GPIO Documentation](STM32F407_TIMERS.md)
