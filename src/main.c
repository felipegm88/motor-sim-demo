/**
 * @file main.c
 * @brief Application entry point.
 *
 * Initializes the application modules, starts background threads/work items,
 * and then idles forever.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>

#include "app_state.h"
#include "motor_control.h"
#include "telemetry.h"
#include "fault_monitor.h"

LOG_MODULE_REGISTER(motor_sim_main, LOG_LEVEL_INF);

/**
 * @brief Application entry point.
 *
 * This function initializes the global application state, starts the
 * control, telemetry and fault monitor components, and then sleeps
 * forever. The motor setpoint can be adjusted at runtime using the
 * shell command:
 *
 *   motor_set <rpm>
 *
 * and the current state can be inspected with:
 *
 *   motor_info
 */
int main(void)
{
    /* Banner: single printk, everything else uses LOG_* macros. */
    printk("motor-sim-demo starting (native_sim)\n");

    int ret = app_state_init();
    if (ret != 0) {
        LOG_ERR("app_state_init failed: %d", ret);
        return ret;
    }

    motor_control_start();
    telemetry_start();
    fault_monitor_start();

    LOG_INF("Use 'motor_set <rpm>' and 'motor_info' in the shell");

    while (true) {
        k_sleep(K_SECONDS(1));
    }

    /* Not reached, but keeps the compiler happy. */
    return 0;
}
