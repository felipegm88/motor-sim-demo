/**
 * @file motor_control.h
 * @brief Public API for the motor control loop.
 *
 * The motor_control module runs a periodic control loop that updates the motor
 * output to follow a setpoint. In unit tests, a pure step function is exposed
 * to allow deterministic testing.
 */

#ifndef MOTOR_CONTROL_H_
#define MOTOR_CONTROL_H_

/**
 * @brief Start the motor control thread.
 *
 * This creates a dedicated thread that periodically:
 * - reads the motor state,
 * - updates the control output to follow the setpoint,
 * - simulates motor dynamics and temperature,
 * - writes feedback back into the shared state.
 */
void motor_control_start(void);

#ifdef MOTOR_SIM_DEMO_UNIT_TEST
#include "app_state.h"

/**
 * @brief Run a single control-loop step on a state snapshot (test-only).
 *
 * This function implements the pure control + model update logic without any
 * threading, sleeps, or synchronization. It is intended for deterministic unit tests.
 *
 * @param state In/out motor state snapshot to be updated.
 */
void motor_control_step(struct motor_state *state);

/**
 * @brief Stop the motor control thread (test-only).
 *
 * Aborts the internal thread created by @ref motor_control_start.
 */
void motor_control_stop(void);
#endif /* MOTOR_SIM_DEMO_UNIT_TEST */

#endif /* MOTOR_CONTROL_H_ */
