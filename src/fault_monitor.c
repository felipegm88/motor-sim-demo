/**
 * @file fault_monitor.c
 * @brief Fault monitor work item implementation.
 *
 * Implements a delayable work handler that periodically evaluates
 * speed/temperature conditions and logs faults.
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "fault_monitor.h"
#include "app_state.h"

LOG_MODULE_REGISTER(fault_monitor, LOG_LEVEL_INF);

/** Period of the fault monitor work item. */
#ifdef MOTOR_SIM_DEMO_UNIT_TEST
#define FAULT_MONITOR_PERIOD K_MSEC(20)
#else
#define FAULT_MONITOR_PERIOD K_SECONDS(2)
#endif

/** Absolute speed error threshold (RPM). */
#define FAULT_SPEED_ERROR_RPM 300.0f

/** Soft temperature threshold (Celsius). */
#define SOFT_LIMIT_TEMP_C 60.0f

/** Hard temperature threshold (Celsius). */
#define HARD_LIMIT_TEMP_C 70.0f

/** Fault logs interval (msecs). */
#ifdef MOTOR_SIM_DEMO_UNIT_TEST
#define FAULT_LOG_PERIOD_MS 0
#else
#define FAULT_LOG_PERIOD_MS 10000
#endif

/**
 * @brief Internal fault monitor context.
 *
 * Holds the delayable work item and the thresholds used for evaluation.
 */
struct fault_monitor_ctx {
    /** Delayable work item used for periodic checks. */
    struct k_work_delayable dwork;
    /** Speed error threshold in RPM. */
    float speed_error_threshold_rpm;
    /** Soft temperature threshold in Celsius. */
    float soft_temp_threshold_c;
    /** Hard temperature threshold in Celsius. */
    float hard_temp_threshold_c;
    /** Last time a fault was logged (ms since boot). */
    int64_t last_log_ms;
};

/** Single static context for the demo. */
static struct fault_monitor_ctx fault_ctx = {
    .speed_error_threshold_rpm = FAULT_SPEED_ERROR_RPM,
    .soft_temp_threshold_c = SOFT_LIMIT_TEMP_C,
    .hard_temp_threshold_c = HARD_LIMIT_TEMP_C,
    .last_log_ms = 0,
};

/**
 * @brief Evaluate fault flags for a given motor state snapshot.
 *
 * This helper is used by the periodic work handler. It is also unit-tested.
 *
 * @param state Motor state snapshot to evaluate.
 * @param speed_err_th_rpm Speed error threshold in RPM (absolute diff).
 * @param soft_temp_c Soft temperature threshold in Celsius.
 * @param hard_temp_c Hard temperature threshold in Celsius.
 *
 * @return Bitmask of @ref fault_flags.
 */
uint32_t fault_monitor_eval(const struct motor_state *state, float speed_err_th_rpm,
                            float soft_temp_c, float hard_temp_c)
{
    uint32_t flags = FAULT_NONE;

    float speed_diff = state->setpoint_rpm - state->measured_rpm;
    if (speed_diff < 0.0f) {
        speed_diff = -speed_diff;
    }

    if (speed_diff > speed_err_th_rpm) {
        flags |= FAULT_SPEED_ERROR;
    }

    if (state->temperature_c > hard_temp_c) {
        flags |= FAULT_TEMP_HARD;
    } else if (state->temperature_c > soft_temp_c) {
        flags |= FAULT_TEMP_SOFT;
    }

    return flags;
}

/**
 * @brief Periodic work handler that checks and logs fault conditions.
 *
 * @param work Base work pointer from the workqueue.
 */
static void fault_monitor_work_handler(struct k_work *work)
{
    struct k_work_delayable *dwork = k_work_delayable_from_work(work);
    struct fault_monitor_ctx *ctx = CONTAINER_OF(dwork, struct fault_monitor_ctx, dwork);

    struct motor_state state;
    int ret = app_state_get_snapshot(&state);
    if (ret != 0) {
        LOG_ERR("fault_monitor: app_state_get_snapshot failed: %d", ret); /* GCOVR_EXCL_LINE */
        goto reschedule;                                                  /* GCOVR_EXCL_LINE */
    }

    uint32_t flags = fault_monitor_eval(&state,
                                        ctx->speed_error_threshold_rpm,
                                        ctx->soft_temp_threshold_c,
                                        ctx->hard_temp_threshold_c);
    int64_t now_ms = k_uptime_get();
    bool allow_log = (flags != FAULT_NONE) && ((now_ms - ctx->last_log_ms) >= 10000);
    if (allow_log) {
        ctx->last_log_ms = now_ms;

        if (flags & FAULT_SPEED_ERROR) {
            float diff = state.setpoint_rpm - state.measured_rpm;
            if (diff < 0.0f) {
                diff = -diff;
            }
            LOG_WRN("Fault(speed): |SP-MEAS|=%d rpm (OUT=%d%%, T=%d C)",
                    (int)diff,
                    (int)state.control_output_pct,
                    (int)state.temperature_c);
        }

        if (flags & FAULT_TEMP_HARD) {
            LOG_ERR("Fault(temp hard): T=%d C (OUT=%d%%, SP=%d rpm)",
                    (int)state.temperature_c,
                    (int)state.control_output_pct,
                    (int)state.setpoint_rpm);
        } else if (flags & FAULT_TEMP_SOFT) {
            LOG_WRN("Fault(temp soft): T=%d C (OUT=%d%%, SP=%d rpm)",
                    (int)state.temperature_c,
                    (int)state.control_output_pct,
                    (int)state.setpoint_rpm);
        }
    }

reschedule:
    (void)k_work_reschedule(&ctx->dwork, FAULT_MONITOR_PERIOD);
}

void fault_monitor_start(void)
{
    k_work_init_delayable(&fault_ctx.dwork, fault_monitor_work_handler);
    (void)k_work_schedule(&fault_ctx.dwork, FAULT_MONITOR_PERIOD);
    LOG_INF("Fault monitor scheduled");
}

#ifdef MOTOR_SIM_DEMO_UNIT_TEST
void fault_monitor_stop(void)
{
    (void)k_work_cancel_delayable(&fault_ctx.dwork);
}
#endif
