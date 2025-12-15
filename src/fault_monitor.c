#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "app_state.h"

LOG_MODULE_REGISTER(fault_monitor, LOG_LEVEL_INF);

#define FAULT_MONITOR_PERIOD_SEC 2
#define FAULT_SPEED_ERROR_RPM    300.0f
#define SOFT_LIMIT_TEMP_C        80.0f
#define HARD_LIMIT_TEMP_C        100.0f

struct fault_monitor_ctx {
    struct k_work_delayable dwork;
    float speed_error_threshold_rpm;
    float soft_temp_threshold_c;
    float hard_temp_threshold_c;
};

static struct fault_monitor_ctx fault_ctx = {
    .speed_error_threshold_rpm = FAULT_SPEED_ERROR_RPM,
    .soft_temp_threshold_c = SOFT_LIMIT_TEMP_C,
    .hard_temp_threshold_c = HARD_LIMIT_TEMP_C,
};

static void fault_monitor_work_handler(struct k_work *work);

void fault_monitor_start(void)
{
    k_work_init_delayable(&fault_ctx.dwork, fault_monitor_work_handler);

    /* Schedule first run a bit after boot. */
    (void)k_work_schedule(&fault_ctx.dwork, K_SECONDS(FAULT_MONITOR_PERIOD_SEC));

    LOG_INF("Fault monitor scheduled");
}

/**
 * @brief Periodic fault monitoring handler.
 *
 * This work item checks:
 * - absolute speed error,
 * - soft temperature limit,
 * - hard temperature limit.
 * It then reschedules itself for the next period.
 */
static void fault_monitor_work_handler(struct k_work *work)
{
    /* Recover the delayable work from the base k_work pointer. */
    struct k_work_delayable *dwork = k_work_delayable_from_work(work);

    /* Recover our context that embeds the delayable work. */
    struct fault_monitor_ctx *ctx = CONTAINER_OF(dwork, struct fault_monitor_ctx, dwork);

    struct motor_state state;
    int ret = app_state_get_snapshot(&state);
    if (ret != 0) {
        LOG_ERR("fault_monitor: app_state_get_snapshot failed: %d", ret);
        goto reschedule;
    }

    float speed_diff = state.setpoint_rpm - state.measured_rpm;
    if (speed_diff < 0.0f) {
        speed_diff = -speed_diff;
    }

    if (speed_diff > ctx->speed_error_threshold_rpm) {
        LOG_WRN("Fault(speed): |SP-MEAS|=%d rpm (OUT=%d%%, T=%d C)",
                (int)speed_diff,
                (int)state.control_output_pct,
                (int)state.temperature_c);
    }

    if (state.temperature_c > ctx->soft_temp_threshold_c) {
        LOG_WRN("Fault(temp soft): T=%d C (OUT=%d%%, SP=%d rpm)",
                (int)state.temperature_c,
                (int)state.control_output_pct,
                (int)state.setpoint_rpm);
    }

    if (state.temperature_c > ctx->hard_temp_threshold_c) {
        LOG_ERR("Fault(temp hard): T=%d C (OUT=%d%%, SP=%d rpm)",
                (int)state.temperature_c,
                (int)state.control_output_pct,
                (int)state.setpoint_rpm);
    }

reschedule:
    (void)k_work_reschedule(&ctx->dwork, K_SECONDS(FAULT_MONITOR_PERIOD_SEC));
}
