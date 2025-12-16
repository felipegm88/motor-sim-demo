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

ZTEST(fault_monitor, test_process_logs_speed_fault_path)
{
    struct motor_state s = {
        .setpoint_rpm = 1000.0f,
        .measured_rpm = 0.0f,
        .control_output_pct = 80.0f,
        .temperature_c = 25.0f,
    };

    fault_monitor_test_set_log_period_ms(0);
    fault_monitor_test_set_last_log_ms(0);

    uint32_t flags = fault_monitor_test_process(&s, 1);
    zassert_true((flags & FAULT_SPEED_ERROR) != 0U, NULL);
}

ZTEST(fault_monitor, test_process_logs_soft_temp_path)
{
    struct motor_state s = {
        .setpoint_rpm = 0.0f,
        .measured_rpm = 0.0f,
        .control_output_pct = 80.0f,
        .temperature_c = 65.0f, /* entre soft y hard */
    };

    fault_monitor_test_set_log_period_ms(0);
    fault_monitor_test_set_last_log_ms(0);

    uint32_t flags = fault_monitor_test_process(&s, 1);
    zassert_true((flags & FAULT_TEMP_SOFT) != 0U, NULL);
    zassert_true((flags & FAULT_TEMP_HARD) == 0U, NULL);
}

ZTEST(fault_monitor, test_process_logs_hard_temp_path_without_soft)
{
    struct motor_state s = {
        .setpoint_rpm = 0.0f,
        .measured_rpm = 0.0f,
        .control_output_pct = 80.0f,
        .temperature_c = 105.0f, /* sobre hard */
    };

    fault_monitor_test_set_log_period_ms(0);
    fault_monitor_test_set_last_log_ms(0);

    uint32_t flags = fault_monitor_test_process(&s, 1);
    zassert_true((flags & FAULT_TEMP_HARD) != 0U, NULL);
    zassert_true((flags & FAULT_TEMP_SOFT) == 0U, NULL);
}

ZTEST(fault_monitor, test_process_returns_when_no_faults)
{
    struct motor_state s = {
        .setpoint_rpm = 1000.0f,
        .measured_rpm = 1000.0f, /* diff 0 -> no speed fault */
        .control_output_pct = 10.0f,
        .temperature_c = 25.0f, /* bajo soft/hard */
    };

    fault_monitor_test_set_log_period_ms(0);
    fault_monitor_test_set_last_log_ms(0);

    uint32_t flags = fault_monitor_test_process(&s, 100);
    zassert_equal(flags, FAULT_NONE, NULL);
}

ZTEST(fault_monitor, test_process_returns_when_rate_limited)
{
    struct motor_state s = {
        .setpoint_rpm = 1000.0f,
        .measured_rpm = 0.0f, /* diff grande -> speed fault */
        .control_output_pct = 80.0f,
        .temperature_c = 25.0f,
    };

    /* rate-limit: now_ms - last_log_ms < log_period_ms */
    fault_monitor_test_set_log_period_ms(10000);
    fault_monitor_test_set_last_log_ms(5000);

    uint32_t flags = fault_monitor_test_process(&s, 6000); /* diff=1000ms < 10s => return */
    zassert_true((flags & FAULT_SPEED_ERROR) != 0U, NULL);
}

ZTEST_SUITE(fault_monitor, NULL, NULL, NULL, NULL, NULL);
