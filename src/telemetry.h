#ifndef TELEMETRY_H_
#define TELEMETRY_H_

/**
 * @brief Start the telemetry thread.
 *
 * The telemetry thread waits for new samples from app_state and
 * periodically logs a snapshot of the current motor state.
 */
void telemetry_start(void);

#endif /* TELEMETRY_H_ */
