# Motor Controller

A lightweight C++ wrapper that ties PWM output and quadrature encoder feedback into a single interface, plus an optional FreeRTOS task that holds position with a proportional controller.

## Features
- Uses existing PWM driver (any timer channel) and quadrature decoder (TIM2–TIM5) configurations
- FreeRTOS position-hold task with tunable proportional gain, sample period, and duty saturation
- Exposes encoder position in counts and revolutions, and provides position reset/set helpers
- Designed for single-direction drive output (duty is clamped to 0–100%). Add an H-bridge/direction GPIO if bidirectional control is needed.

## Typical Setup
```cpp
#include "motor_controller.h"
#include "timer_common.h"

static MotorController motor;

static const PWM_Config kMotorPwmCfg[2] = {
    {
    TIM3_BASE,   // timer_base
    GPIOC_BASE,  // gpio_base
    9,           // pin: PC9 -> TIM3_CH4 (AF2)
    2,           // af_number
    PWM_CHANNEL_4,
    20000U       // frequency_hz: 20 kHz
    },
    {
    TIM3_BASE,   // timer_base
    GPIOC_BASE,  // gpio_base
    8,           // pin: PC8 -> TIM3_CH3 (AF2)
    2,           // af_number
    PWM_CHANNEL_3,
    20000U       // frequency_hz: 20 kHz
    }
};

static const QuadEncoder_Config kEncCfg = {
    TIM4_BASE,   // timer_base
    GPIOB_BASE,  // gpio_base
    6,           // pin_a (PB6 -> TIM4_CH1 AF2)
    7,           // pin_b (PB7 -> TIM4_CH2 AF2)
    2,           // af_number
    2048         // counts_per_rev (adjust to your encoder)
};

void app_init() {
    motor.init(kMotorPwmCfg, kEncCfg);

    // kp: proportional gain (duty% per count of error)
    // kd: derivative gain (duty% per count/sec of velocity)
    // sample_time_ms: loop period
    // target_counts: desired encoder position (counts)
    // max_duty_percent: saturate command
    motor.startPositionControlTask(1.0f, 0.4f, 10U, 6400, 75.0f);
}
```

## API Highlights
- `bool init(const PWM_Config[], const QuadEncoder_Config&)`
- `bool init(const PWM_Config[], const QuadEncoder_Config&, const LimitSwitch_Config&)`
- `void enable() / disable()`
- `void setDuty(uint32_t chan, float duty_percent)` / `float getDuty(uint32_t chan) const`
- `int32_t getPositionCounts()` / `float getPositionRevolutions()`
- `void resetPosition()` / `void setPosition(int32_t counts)`
- `bool startPositionControlTask(float kp, float kd, uint32_t sample_time_ms, int32_t target_counts, float max_duty_percent=100.0f)`
- `void stopPositionControlTask()`
- `void setTargetPositionCounts(int32_t target_counts)` / `int32_t getTargetPositionCounts() const`
- `bool positionTaskRunning() const`
- `bool isMinLimitActive() / isMaxLimitActive() / isAnyLimitActive() const`
- `void setLimitSwitchEnabled(bool enabled)`
- `bool homeToMinLimit(float homing_duty=20.0f, uint32_t timeout_ms=30000)`

## Control Loop Details
- Controller: proportional-derivative (PD); command = `kp * error_counts + kd * velocity`
- Saturation: duty is clamped to `[0, max_duty_percent]`; negative commands are forced to 0 (single-direction output)
- Timing: loop runs in a FreeRTOS task with period `sample_time_ms` (minimum enforced to 1 ms)
- Task priority: `tskIDLE_PRIORITY + 2`; stack size: `configMINIMAL_STACK_SIZE + 128`

## Tuning Tips
1. Start with small `kp` (e.g., 0.01–0.05), small `kd` (e.g., 0.1–0.5) and moderate `max_duty_percent` to avoid overshoot.
2. Increase `kp` until the motor reaches target briskly without oscillation.
3. Adjust `kd` to dampen oscillations and improve settling time.
4. If the system feels sluggish, shorten `sample_time_ms` (watch CPU usage) or raise `kp` slightly.
5. Set `counts_per_rev` accurately in the encoder config so position and rev calculations are correct.

## Notes
- For bidirectional control, add a direction GPIO or H-bridge driver and extend the controller to handle signed duty.
- Ensure PWM and encoder timers/pins are on compatible AF mappings; see the individual driver docs if you change them.
- You can retarget position on the fly with `setTargetPositionCounts` while the task is running.

## Files
- `inc/motor_controller.h`
- `src/motor_controller.cpp`
- Example integration in `src/main.cpp`
