#include <math.h>
#include <zephyr/ztest.h>

#include "app_state.h"
#include "motor_control.h"

static void assert_float_near(float a, float b, float eps, const char *msg)
{
    zassert_true(fabsf(a - b) <= eps, "%s (a=%f b=%f)", msg, (double)a, (double)b);
}

ZTEST(motor_control, test_step_increases_output_and_speed)
{
    struct motor_state s = {
        .setpoint_rpm = 3000.0f,
        .measured_rpm = 0.0f,
        .control_output_pct = 0.0f,
        .temperature_c = 25.0f,
    };

    motor_control_step(&s);

    /* Con KP_PERCENT=10 y error/MOTOR_MAX_RPM=1.0 => +10% */
    assert_float_near(s.control_output_pct, 10.0f, 0.01f, "control output step");
    zassert_true(s.measured_rpm > 0.0f, "measured rpm should rise");
    zassert_true(s.temperature_c >= 25.0f, "temp should not drop below ambient");
}

ZTEST(motor_control, test_output_clamps_0_to_100)
{
    struct motor_state s1 = {
        .setpoint_rpm = 0.0f,
        .measured_rpm = 3000.0f,
        .control_output_pct = 1.0f,
        .temperature_c = 25.0f,
    };
    motor_control_step(&s1);
    zassert_true(s1.control_output_pct >= 0.0f, NULL);

    struct motor_state s2 = {
        .setpoint_rpm = 3000.0f,
        .measured_rpm = 0.0f,
        .control_output_pct = 99.0f,
        .temperature_c = 25.0f,
    };
    motor_control_step(&s2);
    zassert_true(s2.control_output_pct <= 100.0f, NULL);
}

ZTEST(motor_control, test_temperature_clamps_ambient_and_max)
{
    struct motor_state low = {
        .setpoint_rpm = 0.0f,
        .measured_rpm = 0.0f,
        .control_output_pct = 0.0f,
        .temperature_c = 0.0f,
    };
    motor_control_step(&low);
    assert_float_near(low.temperature_c, 25.0f, 0.01f, "ambient clamp");

    struct motor_state high = {
        .setpoint_rpm = 0.0f,
        .measured_rpm = 0.0f,
        .control_output_pct = 0.0f,
        .temperature_c = 200.0f,
    };
    motor_control_step(&high);
    /* MAX_TEMP_C in this project is 130. */
    assert_float_near(high.temperature_c, 130.0f, 0.01f, "max clamp");
}

ZTEST(motor_control, test_soft_and_hard_temp_saturation)
{
    struct motor_state soft = {
        .setpoint_rpm = 3000.0f,
        .measured_rpm = 3000.0f,
        .control_output_pct = 90.0f,
        .temperature_c = 90.0f,
    };
    motor_control_step(&soft);
    assert_float_near(soft.control_output_pct, 60.0f, 0.01f, "soft saturation");

    struct motor_state hard = {
        .setpoint_rpm = 3000.0f,
        .measured_rpm = 3000.0f,
        .control_output_pct = 90.0f,
        .temperature_c = 110.0f,
    };
    motor_control_step(&hard);
    assert_float_near(hard.control_output_pct, 10.0f, 0.01f, "hard saturation");
}

ZTEST(motor_control, test_negative_measured_rpm_branch)
{
    struct motor_state s = {
        .setpoint_rpm = 0.0f,
        .measured_rpm = -100.0f, /* force speed_norm < 0 */
        .control_output_pct = 0.0f,
        .temperature_c = 25.0f,
    };

    motor_control_step(&s);

    /* We dont seek exact values, just execute the branch and stay healthy */
    zassert_true(s.temperature_c >= 25.0f, NULL);
}

ZTEST_SUITE(motor_control, NULL, NULL, NULL, NULL, NULL);
