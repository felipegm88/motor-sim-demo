/**
 * @file telemetry.h
 * @brief Public API for telemetry logging.
 *
 * The telemetry module runs a thread that waits for new samples and
 * periodically logs a motor state snapshot.
 */

#ifndef TELEMETRY_H_
#define TELEMETRY_H_

/**
 * @brief Start the telemetry thread.
 *
 * The telemetry thread waits for new samples from app_state and
 * periodically logs a snapshot of the current motor state.
 */
void telemetry_start(void);

#ifdef MOTOR_SIM_DEMO_UNIT_TEST
#include <stdbool.h>

/**
 * @brief Decide whether telemetry should log on this sample (test-only).
 *
 * The internal sample counter is incremented and the function returns true
 * every 10th call (i.e., when the counter is divisible by 10).
 *
 * @param counter Pointer to a persistent counter variable.
 * @return true if this call corresponds to a log event, false otherwise.
 */
bool telemetry_should_log(int *counter);

/**
 * @brief Stop the telemetry thread (test-only).
 *
 * Aborts the internal thread created by @ref telemetry_start.
 */
void telemetry_stop(void);
#endif /* MOTOR_SIM_DEMO_UNIT_TEST */

#endif /* TELEMETRY_H_ */
