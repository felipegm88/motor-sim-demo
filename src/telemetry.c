/**
 * @file telemetry.c
 * @brief Telemetry thread implementation.
 *
 * Implements a telemetry thread that waits for new samples from app_state and
 * logs a snapshot every N samples.
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "app_state.h"
#include "telemetry.h"

LOG_MODULE_REGISTER(telemetry, LOG_LEVEL_DBG);

#define TELEMETRY_THREAD_STACK_SIZE 1024
#define TELEMETRY_THREAD_PRIORITY   3

#define TELEMETRY_THREAD_NAME "telemetry"

static void telemetry_thread(void *p1, void *p2, void *p3);

K_THREAD_STACK_DEFINE(telemetry_stack, TELEMETRY_THREAD_STACK_SIZE);
static struct k_thread telemetry_thread_data;
static k_tid_t telemetry_tid;

void telemetry_start(void)
{
    telemetry_tid = k_thread_create(&telemetry_thread_data,
                                    telemetry_stack,
                                    K_THREAD_STACK_SIZEOF(telemetry_stack),
                                    telemetry_thread,
                                    NULL,
                                    NULL,
                                    NULL,
                                    TELEMETRY_THREAD_PRIORITY,
                                    0,
                                    K_NO_WAIT);

    (void)k_thread_name_set(telemetry_tid, TELEMETRY_THREAD_NAME);

    LOG_INF("Thread '%s' started (tid=%p)", TELEMETRY_THREAD_NAME, (void *)telemetry_tid);
}

bool telemetry_should_log(int *counter)
{
    (*counter)++;
    return ((*counter % 10) == 0);
}

/**
 * @brief Telemetry loop.
 *
 * This thread blocks until app_state signals that a new sample is
 * available, then logs a snapshot of the current motor state every
 * N samples to avoid flooding the log output.
 */
static void telemetry_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    int counter = 0;

    while (true) {
        int ret = app_state_wait_for_sample();
        /* GCOVR_EXCL_START */
        if (ret != 0) {
            LOG_ERR("T[%s] app_state_wait_for_sample failed: %d", TELEMETRY_THREAD_NAME, ret);
            continue;
        }
        /* GCOVR_EXCL_STOP */

        /* Reduce log volume by printing every 10th sample. */
        if (!telemetry_should_log(&counter)) {
            continue;
        }

        struct motor_state state;
        ret = app_state_get_snapshot(&state);
        /* GCOVR_EXCL_START */
        if (ret != 0) {
            LOG_ERR("T[%s] app_state_get_snapshot failed: %d", TELEMETRY_THREAD_NAME, ret);
            continue;
        }
        /* GCOVR_EXCL_STOP */

        LOG_INF("T[%s] SP=%d rpm, MEAS=%d rpm, OUT=%d%%, T=%d C",
                TELEMETRY_THREAD_NAME,
                (int)state.setpoint_rpm,
                (int)state.measured_rpm,
                (int)state.control_output_pct,
                (int)state.temperature_c);
    }
}

#ifdef MOTOR_SIM_DEMO_UNIT_TEST
void telemetry_stop(void)
{
    if (telemetry_tid != NULL) {
        k_thread_abort(telemetry_tid);
        telemetry_tid = NULL;
    }
}
#endif
