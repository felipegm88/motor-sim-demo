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
bool telemetry_should_log(int *counter);
/** @brief Stop telemetry thread (test-only helper). */
void telemetry_stop(void);
#endif /* MOTOR_SIM_DEMO_UNIT_TEST */

#endif /* TELEMETRY_H_ */
