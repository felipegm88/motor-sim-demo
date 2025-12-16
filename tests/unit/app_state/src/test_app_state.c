#include <errno.h>
#include <zephyr/ztest.h>

#include "app_state.h"

ZTEST(app_state, test_init_defaults)
{
    zassert_equal(app_state_init(), 0, NULL);

    struct motor_state s;
    zassert_equal(app_state_get_snapshot(&s), 0, NULL);

    zassert_true(s.setpoint_rpm == 1500.0f, "default setpoint");
    zassert_true(s.measured_rpm == 0.0f, "default measured");
    zassert_true(s.control_output_pct == 0.0f, "default output");
    zassert_true(s.temperature_c == 25.0f, "default temp");
}

ZTEST(app_state, test_set_setpoint_validation)
{
    zassert_equal(app_state_init(), 0, NULL);

    zassert_equal(app_state_set_setpoint(-1.0f), -ERANGE, NULL);
    zassert_equal(app_state_set_setpoint(3000.1f), -ERANGE, NULL);

    zassert_equal(app_state_set_setpoint(0.0f), 0, NULL);
    zassert_equal(app_state_set_setpoint(3000.0f), 0, NULL);

    struct motor_state s;
    zassert_equal(app_state_get_snapshot(&s), 0, NULL);
    zassert_true(s.setpoint_rpm == 3000.0f, NULL);
}

ZTEST(app_state, test_update_feedback_and_sample_sem)
{
    zassert_equal(app_state_init(), 0, NULL);

    zassert_equal(app_state_update_feedback(123.0f, 45.0f, 67.0f), 0, NULL);

    zassert_equal(app_state_wait_for_sample(), 0, NULL);

    struct motor_state s;
    zassert_equal(app_state_get_snapshot(&s), 0, NULL);
    zassert_true(s.measured_rpm == 123.0f, NULL);
    zassert_true(s.control_output_pct == 45.0f, NULL);
    zassert_true(s.temperature_c == 67.0f, NULL);
}

ZTEST(app_state, test_get_snapshot_null)
{
    zassert_equal(app_state_init(), 0, NULL);
    zassert_equal(app_state_get_snapshot(NULL), -EINVAL, NULL);
}

ZTEST_SUITE(app_state, NULL, NULL, NULL, NULL, NULL);
