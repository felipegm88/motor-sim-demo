#include <zephyr/kernel.h>
#include <zephyr/ztest.h>

#include "app_state.h"
#include "motor_control.h"
#include "telemetry.h"
#include "fault_monitor.h"

ZTEST(system, test_run_threads_and_work_paths)
{
    zassert_equal(app_state_init(), 0, NULL);

    /* Start modules (covers *_start() code paths) */
    telemetry_start();
    motor_control_start();
    fault_monitor_start();

    /* Drive telemetry: 25 samples -> ejecuta "skip" y "log" (cada 10) */
    for (int i = 0; i < 25; i++) {
        zassert_equal(app_state_update_feedback(100.0f + i, 10.0f, 25.0f), 0, NULL);
        k_msleep(20);
    }

    /* Forzar saturación soft/hard en el thread de motor_control */
    zassert_equal(app_state_set_setpoint(1500.0f), 0, NULL);

    zassert_equal(app_state_update_feedback(1500.0f, 90.0f, 90.0f), 0, NULL);
    k_msleep(150);

    zassert_equal(app_state_update_feedback(1500.0f, 90.0f, 110.0f), 0, NULL);
    k_msleep(150);

    /* Forzar fault_monitor (speed + temp soft/hard) */
    zassert_equal(app_state_set_setpoint(3000.0f), 0, NULL);

    zassert_equal(app_state_update_feedback(0.0f, 90.0f, 85.0f), 0, NULL);
    k_msleep(150);

    zassert_equal(app_state_update_feedback(0.0f, 90.0f, 105.0f), 0, NULL);
    k_msleep(150);

    /* Cubrir rama diff < 0 en fault_monitor: measured > setpoint, y |diff| > threshold */
    zassert_equal(app_state_set_setpoint(0.0f), 0, NULL);
    zassert_equal(app_state_update_feedback(500.0f, 50.0f, 25.0f), 0, NULL);
    k_msleep(150);

    /* Stop test-only (evita que queden corriendo y afecte salida del test) */
#ifdef MOTOR_SIM_DEMO_UNIT_TEST
    motor_control_stop();
    telemetry_stop();
    fault_monitor_stop();
#endif
}

ZTEST_SUITE(system, NULL, NULL, NULL, NULL, NULL);
