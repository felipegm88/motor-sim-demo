#include <errno.h>

#include <zephyr/ztest.h>
#include <zephyr/shell/shell.h>

#include "app_state.h"

static void reset_state(void)
{
    zassert_equal(app_state_init(), 0, NULL);
}

ZTEST(console_shell, test_motor_set_valid)
{
    reset_state();

    int ret = shell_execute_cmd(NULL, "motor_set 1234");
    zassert_equal(ret, 0, NULL);

    struct motor_state s;
    zassert_equal(app_state_get_snapshot(&s), 0, NULL);
    zassert_true(s.setpoint_rpm == 1234.0f, NULL);
}

ZTEST(console_shell, test_motor_set_out_of_range)
{
    reset_state();

    int ret = shell_execute_cmd(NULL, "motor_set 999999");
    zassert_equal(ret, -ERANGE, NULL);
}

ZTEST(console_shell, test_motor_set_missing_arg)
{
    reset_state();

    int ret = shell_execute_cmd(NULL, "motor_set");
    zassert_equal(ret, -EINVAL, NULL);
}

ZTEST(console_shell, test_motor_set_bad_arg)
{
    reset_state();

    int ret = shell_execute_cmd(NULL, "motor_set abc");
    zassert_equal(ret, -EINVAL, NULL);
}

ZTEST(console_shell, test_motor_info_smoke)
{
    reset_state();

    int ret = shell_execute_cmd(NULL, "motor_info");
    zassert_equal(ret, 0, NULL);
}

ZTEST_SUITE(console_shell, NULL, NULL, NULL, NULL, NULL);
