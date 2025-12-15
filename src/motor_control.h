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
struct motor_state;
void motor_control_step(struct motor_state *state);
/** @brief Stop motor control thread (test-only helper). */
void motor_control_stop(void);
#endif /* MOTOR_SIM_DEMO_UNIT_TEST */

#endif /* MOTOR_CONTROL_H_ */
