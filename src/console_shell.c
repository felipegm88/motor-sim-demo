/**
 * @file console_shell.c
 * @brief Shell command handlers.
 *
 * Registers shell commands used by the demo to set the target speed and print
 * the current motor state.
 */

#include <stdlib.h>
#include <errno.h>

#include <zephyr/shell/shell.h>
#include <zephyr/logging/log.h>

#include "app_state.h"

LOG_MODULE_REGISTER(console_shell, LOG_LEVEL_INF);

/**
 * @brief Shell command: set motor speed setpoint.
 *
 * Usage:
 *   motor_set <rpm>
 */
static int cmd_motor_set(const struct shell *shell, size_t argc, char **argv)
{
    if (argc != 2) {
        shell_print(shell, "Usage: motor_set <rpm>");
        return -EINVAL;
    }

    char *end = NULL;
    long rpm_long = strtol(argv[1], &end, 10);

    if ((argv[1] == end) || (*end != '\0')) {
        shell_error(shell, "Invalid rpm value: %s", argv[1]);
        return -EINVAL;
    }

    if (rpm_long < 0) {
        shell_error(shell, "rpm must be non-negative");
        return -ERANGE;
    }

    float rpm = (float)rpm_long;

    int ret = app_state_set_setpoint(rpm);
    if (ret == -ERANGE) {
        shell_error(shell, "rpm out of allowed range");
        return ret;
    } else if (ret != 0) {
        shell_error(shell, "Failed to set setpoint (err=%d)", ret); /* GCOVR_EXCL_LINE */
        return ret;                                                 /* GCOVR_EXCL_LINE */
    }

    shell_print(shell, "Setpoint set to %ld rpm", rpm_long);
    LOG_INF("Shell: motor_set %ld rpm", rpm_long);

    return 0;
}

/**
 * @brief Shell command: print current motor state snapshot.
 *
 * Usage:
 *   motor_info
 */
static int cmd_motor_info(const struct shell *shell, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    struct motor_state state;
    int ret = app_state_get_snapshot(&state);
    if (ret != 0) {
        shell_error(shell, "Failed to get motor state (err=%d)", ret); /* GCOVR_EXCL_LINE */
        return ret;                                                    /* GCOVR_EXCL_LINE */
    }

    shell_print(shell,
                "SP=%d rpm, MEAS=%d rpm, OUT=%d%%, T=%d C",
                (int)state.setpoint_rpm,
                (int)state.measured_rpm,
                (int)state.control_output_pct,
                (int)state.temperature_c);

    return 0;
}

/* Register shell commands. */
SHELL_CMD_REGISTER(motor_set, NULL, "Set motor speed setpoint (rpm)", cmd_motor_set);

SHELL_CMD_REGISTER(motor_info, NULL, "Print current motor state snapshot", cmd_motor_info);
