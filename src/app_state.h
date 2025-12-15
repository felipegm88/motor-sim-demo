/**
 * @file app_state.h
 * @brief Public API for the shared motor state.
 *
 * The app_state module owns the global motor state and exposes a small API to
 * update and read it. It also provides a sample-ready synchronization
 * primitive used by other threads.
 */

#ifndef APP_STATE_H_
#define APP_STATE_H_

#include <zephyr/kernel.h>

/**
 * @brief Global motor state snapshot.
 *
 * All values are represented in physical units (rpm, °C, percent).
 * This struct is owned and updated by the app_state module.
 */
struct motor_state {
    float setpoint_rpm;       /**< Target speed in rpm. */
    float measured_rpm;       /**< Simulated measured speed in rpm. */
    float control_output_pct; /**< Control output in percent (0..100). */
    float temperature_c;      /**< Simulated motor temperature in °C. */
};

/**
 * @brief Initialize the motor state and synchronization primitives.
 *
 * This must be called once at startup before any other app_state_* API
 * is used. It sets reasonable defaults and publishes the initial state.
 *
 * @return 0 on success, negative errno on error.
 */
int app_state_init(void);

/**
 * @brief Update the motor speed setpoint.
 *
 * Performs basic range validation on the requested setpoint.
 *
 * @param rpm New setpoint in rpm.
 *
 * @return 0 on success, -ERANGE if out of allowed range.
 */
int app_state_set_setpoint(float rpm);

/**
 * @brief Update measured values (feedback) from the control loop.
 *
 * This function is typically called by the motor control thread after
 * each control step. It updates the measured rpm, control output and
 * temperature in a thread-safe way, publishes the state on zbus, and
 * signals that a new sample is available for telemetry.
 *
 * @param measured_rpm       Simulated measured speed in rpm.
 * @param control_output_pct Control output in percent (0..100).
 * @param temperature_c      Simulated motor temperature in °C.
 *
 * @return 0 on success, negative errno on error.
 */
int app_state_update_feedback(float measured_rpm, float control_output_pct, float temperature_c);

/**
 * @brief Get a snapshot of the current motor state.
 *
 * @param out Pointer to a motor_state struct to fill with a copy of the
 *            current state. Must not be NULL.
 *
 * @return 0 on success, -EINVAL if out is NULL.
 */
int app_state_get_snapshot(struct motor_state *out);

/**
 * @brief Block until a new sample is available.
 *
 * This is intended for threads such as telemetry that want to react
 * whenever the control loop publishes a new feedback sample.
 *
 * @return 0 on success, negative errno on error.
 */
int app_state_wait_for_sample(void);

#endif /* APP_STATE_H_ */
