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

#endif /* MOTOR_CONTROL_H_ */
