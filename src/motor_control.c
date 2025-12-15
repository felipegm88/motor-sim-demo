/**
 * @file motor_control.c
 * @brief Motor control loop implementation.
 *
 * Implements the periodic control thread and a simple motor/temperature model
 * used by the demo.
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "app_state.h"
#include "motor_control.h"

LOG_MODULE_REGISTER(motor_control, LOG_LEVEL_DBG);

#define CONTROL_THREAD_STACK_SIZE 1024
#define CONTROL_THREAD_PRIORITY   2

#define CONTROL_PERIOD_MS 50

#define MOTOR_MAX_RPM     3000.0f
#define AMBIENT_TEMP_C    25.0f
#define MAX_TEMP_C        130.0f
#define SOFT_LIMIT_TEMP_C 80.0f
#define HARD_LIMIT_TEMP_C 100.0f

/* Heat / cooling and dynamic response tuning constants. */
#define HEAT_GAIN          0.08f
#define COOL_GAIN          0.02f
#define SPEED_FILTER_ALPHA 0.2f
#define KP_PERCENT         10.0f /* proportional effect as % of full output */

#define MOTOR_CONTROL_THREAD_NAME "motor_ctrl"

static void control_thread(void *p1, void *p2, void *p3);

K_THREAD_STACK_DEFINE(control_stack, CONTROL_THREAD_STACK_SIZE);
static struct k_thread control_thread_data;
static k_tid_t control_tid;

void motor_control_start(void)
{
    control_tid = k_thread_create(&control_thread_data,
                                  control_stack,
                                  K_THREAD_STACK_SIZEOF(control_stack),
                                  control_thread,
                                  NULL,
                                  NULL,
                                  NULL,
                                  CONTROL_THREAD_PRIORITY,
                                  0,
                                  K_NO_WAIT);

    (void)k_thread_name_set(control_tid, MOTOR_CONTROL_THREAD_NAME);

    LOG_INF("Thread '%s' started (tid=%p)", MOTOR_CONTROL_THREAD_NAME, (void *)control_tid);
}

void motor_control_step(struct motor_state *state)
{
    /* Simple proportional control based on speed error. */
    float error = state->setpoint_rpm - state->measured_rpm;

    float step_pct = (error / MOTOR_MAX_RPM) * KP_PERCENT;
    state->control_output_pct += step_pct;

    if (state->control_output_pct < 0.0f) {
        state->control_output_pct = 0.0f;
    } else if (state->control_output_pct > 100.0f) {
        state->control_output_pct = 100.0f;
    }

    /* First order motor model: measured_rpm moves towards target_rpm. */
    float target_rpm = (state->control_output_pct / 100.0f) * MOTOR_MAX_RPM;
    state->measured_rpm += (target_rpm - state->measured_rpm) * SPEED_FILTER_ALPHA;

    /* Temperature model: heating depends on speed, cooling towards ambient. */
    float speed_norm = state->measured_rpm / MOTOR_MAX_RPM;
    if (speed_norm < 0.0f) {
        speed_norm = -speed_norm;
    }

    float heating = HEAT_GAIN * speed_norm * speed_norm;
    float cooling = COOL_GAIN * (state->temperature_c - AMBIENT_TEMP_C);
    state->temperature_c += (heating - cooling);

    if (state->temperature_c < AMBIENT_TEMP_C) {
        state->temperature_c = AMBIENT_TEMP_C;
    }
    if (state->temperature_c > MAX_TEMP_C) {
        state->temperature_c = MAX_TEMP_C;
    }

    /* Temperature-based saturation (safety), actual fault reporting is separate. */
    if ((state->temperature_c > SOFT_LIMIT_TEMP_C) && (state->control_output_pct > 60.0f)) {
        state->control_output_pct = 60.0f;
    }

    if ((state->temperature_c > HARD_LIMIT_TEMP_C) && (state->control_output_pct > 10.0f)) {
        state->control_output_pct = 10.0f;
    }
}

/**
 * @brief Main motor control loop.
 *
 * This thread:
 * - reads the current setpoint and feedback,
 * - computes a simple proportional correction,
 * - simulates first-order motor dynamics,
 * - updates temperature and applies overtemperature limits (saturation),
 * - publishes feedback back to app_state.
 */
static void control_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    while (true) {
        struct motor_state state;
        int ret = app_state_get_snapshot(&state);
        /* GCOVR_EXCL_START */
        if (ret != 0) {
            LOG_ERR("T[%s] app_state_get_snapshot failed: %d", MOTOR_CONTROL_THREAD_NAME, ret);
            k_msleep(CONTROL_PERIOD_MS);
            continue;
        }
        /* GCOVR_EXCL_STOP */

        motor_control_step(&state);

        ret = app_state_update_feedback(
            state.measured_rpm, state.control_output_pct, state.temperature_c);
        /* GCOVR_EXCL_START */
        if (ret != 0) {
            LOG_ERR("T[%s] app_state_update_feedback failed: %d", MOTOR_CONTROL_THREAD_NAME, ret);
        }
        /* GCOVR_EXCL_STOP */

        k_msleep(CONTROL_PERIOD_MS);
    }
}

#ifdef MOTOR_SIM_DEMO_UNIT_TEST
void motor_control_stop(void)
{
    if (control_tid != NULL) {
        k_thread_abort(control_tid);
        control_tid = NULL;
    }
}
#endif
