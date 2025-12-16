#include <zephyr/ztest.h>

#include "app_state.h"
#include "fault_monitor.h"

ZTEST(fault_monitor, test_no_faults_at_exact_thresholds)
{
    struct motor_state s = {
        .setpoint_rpm = 1000.0f,
        .measured_rpm = 700.0f, /* diff = 300 exact */
        .control_output_pct = 50.0f,
        .temperature_c = 80.0f, /* soft exact */
    };

    uint32_t flags = fault_monitor_eval(&s, 300.0f, 80.0f, 100.0f);
    zassert_equal(flags, FAULT_NONE, NULL);
}

ZTEST(fault_monitor, test_speed_fault_abs_branch)
{
    struct motor_state s = {
        .setpoint_rpm = 1000.0f,
        .measured_rpm = 1401.0f, /* |diff| = 401 */
        .temperature_c = 25.0f,
    };

    uint32_t flags = fault_monitor_eval(&s, 300.0f, 80.0f, 100.0f);
    zassert_true((flags & FAULT_SPEED_ERROR) != 0U, NULL);
}

ZTEST(fault_monitor, test_soft_temp_only)
{
    struct motor_state s = {
        .setpoint_rpm = 0.0f,
        .measured_rpm = 0.0f,
        .temperature_c = 85.0f,
    };

    uint32_t flags = fault_monitor_eval(&s, 300.0f, 80.0f, 100.0f);
    zassert_true((flags & FAULT_TEMP_SOFT) != 0U, NULL);
    zassert_true((flags & FAULT_TEMP_HARD) == 0U, NULL);
}

ZTEST(fault_monitor, test_hard_temp_sets_only_hard)
{
    struct motor_state s = {
        .temperature_c = 105.0f,
    };

    uint32_t flags = fault_monitor_eval(&s, 300.0f, 60.0f, 70.0f);

    zassert_true((flags & FAULT_TEMP_HARD) != 0U, NULL);
    zassert_true((flags & FAULT_TEMP_SOFT) == 0U, NULL);
}

ZTEST(fault_monitor, test_soft_temp_sets_only_soft)
{
    struct motor_state s = {
        .temperature_c = 65.0f,
    };

    uint32_t flags = fault_monitor_eval(&s, 300.0f, 60.0f, 70.0f);

    zassert_true((flags & FAULT_TEMP_SOFT) != 0U, NULL);
    zassert_true((flags & FAULT_TEMP_HARD) == 0U, NULL);
}

ZTEST_SUITE(fault_monitor, NULL, NULL, NULL, NULL, NULL);
