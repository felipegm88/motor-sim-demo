#ifndef FAULT_MONITOR_H_
#define FAULT_MONITOR_H_

#include <stdint.h>

/**
 * @brief Fault flags reported by the fault monitor.
 *
 * These flags are used in production code (work handler) and in unit tests.
 */
enum fault_flags {
    /** No fault condition. */
    FAULT_NONE        = 0,
    /** Absolute speed error exceeds threshold. */
    FAULT_SPEED_ERROR = (1u << 0),
    /** Temperature exceeds soft limit. */
    FAULT_TEMP_SOFT   = (1u << 1),
    /** Temperature exceeds hard limit. */
    FAULT_TEMP_HARD   = (1u << 2),
};

/**
 * @brief Start the periodic fault monitor.
 *
 * Initializes and schedules a delayable work item that periodically checks:
 * - absolute speed error
 * - soft temperature limit
 * - hard temperature limit
 *
 * The monitor logs warnings/errors based on the evaluated flags.
 */
void fault_monitor_start(void);

/* -------------------------------------------------------------------------- */
/* Unit-test API                                                               */
/* -------------------------------------------------------------------------- */
/*
 * We keep the public (production) header small. The evaluation function is
 * declared only for unit tests, but it is implemented and used internally by
 * fault_monitor.c in production as well (no need for a public prototype).
 */
#ifdef MOTOR_SIM_DEMO_UNIT_TEST

#include "app_state.h" /* for struct motor_state */

/**
 * @brief Evaluate fault flags for a given motor state snapshot.
 *
 * This is a pure helper (no Zephyr calls) and is unit-testable.
 *
 * @param state Motor state snapshot to evaluate.
 * @param speed_err_th_rpm Speed error threshold in RPM (absolute diff).
 * @param soft_temp_c Soft temperature threshold in Celsius.
 * @param hard_temp_c Hard temperature threshold in Celsius.
 *
 * @return Bitmask of @ref fault_flags.
 */
uint32_t fault_monitor_eval(const struct motor_state *state,
                            float speed_err_th_rpm,
                            float soft_temp_c,
                            float hard_temp_c);

#endif /* MOTOR_SIM_DEMO_UNIT_TEST */

#endif /* FAULT_MONITOR_H_ */

