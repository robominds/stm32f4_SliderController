/**
 * @file    motor_controller.h
 * @brief   Simple motor controller wrapper combining PWM output and quadrature encoder feedback.
 */

#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "pwm_driver.h"
#include "quadrature_decoder.h"
#include "FreeRTOS.h" // IWYU pragma: keep - Must include FreeRTOS.h before task.h
#include "task.h"
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

class MotorController {
public:
    MotorController();

    /**
     * @brief Initialize motor controller.
     * @param pwm_cfg PWM configuration (timer/pin/channel/frequency).
     * @param enc_cfg Encoder configuration (timer/pins/counts-per-rev).
     * @return true on success.
     */
    bool init(const PWM_Config pwm_cfg[], const QuadEncoder_Config &enc_cfg);

    /** Enable PWM output. */
    void enable();

    /** Disable PWM output. */
    void disable();

    /** Set duty cycle in percent [0,100]. */
    void setDuty(uint32_t chan, float duty_percent);

    /** Get current duty cycle percent. */
    float getDuty(uint32_t chan) const;

    /** Get encoder position in counts. */
    int32_t getPositionCounts();

    /** Get encoder position in revolutions. */
    float getPositionRevolutions();

    /** Reset encoder position to zero. */
    void resetPosition();

    /** Manually set encoder position. */
    void setPosition(int32_t counts);

    /**
     * @brief Create and start a FreeRTOS task that holds position using a proportional controller.
     * @param kp Proportional gain that maps position error (counts) to duty percent.
     * @param sample_time_ms Task period in milliseconds.
     * @param target_counts Initial target position in encoder counts.
     * @param max_duty_percent Duty saturation limit (0-100).
     * @return true if the task was created.
     */
    bool startPositionControlTask(float kp,
                                  uint32_t sample_time_ms,
                                  int32_t target_counts,
                                  float max_duty_percent = 100.0f);

    /** Stop the position control task if running. */
    void stopPositionControlTask();

    /** Update the target position while the task is running. */
    void setTargetPositionCounts(int32_t target_counts);

    /** Get the currently commanded target position in counts. */
    int32_t getTargetPositionCounts() const;

    /** Check whether the position task is active. */
    bool positionTaskRunning() const;

private:
    static void PositionTaskThunk(void *param);
    void positionTaskLoop();

    PWM_Handle pwm_[2]{};
    QuadEncoder_Handle encoder_{};
    bool initialized_ {false};
    TaskHandle_t position_task_{nullptr};
    float kp_{0.05f};
    uint32_t sample_time_ms_{10};
    int32_t target_counts_{0};
    float max_duty_percent_{100.0f};
};

#endif /* __cplusplus */

#endif /* MOTOR_CONTROLLER_H */
