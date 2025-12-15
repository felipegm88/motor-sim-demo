#ifndef FAULT_MONITOR_H_
#define FAULT_MONITOR_H_

/**
 * @brief Start the periodic fault monitor.
 *
 * The fault monitor runs in the system workqueue and periodically checks
 * the speed error. If the error is larger than a configured threshold,
 * it logs a warning.
 */
void fault_monitor_start(void);

#endif /* FAULT_MONITOR_H_ */

