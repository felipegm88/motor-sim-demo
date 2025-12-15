/**
 * @file app_state.c
 * @brief Shared motor state implementation.
 *
 * Implements the app_state module using a mutex for data protection and a
 * semaphore to signal new samples. Setpoint updates are broadcast using Zbus.
 */

#include <errno.h>

#include <zephyr/kernel.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/logging/log.h>

#include "app_state.h"

LOG_MODULE_REGISTER(app_state, LOG_LEVEL_DBG);

/* Maximum allowed setpoint, should match motor model full scale. */
#define APP_STATE_MAX_SETPOINT_RPM 3000.0f

/* Internal global state (owned by this module only). */
static struct motor_state g_state = {
    .setpoint_rpm = 1500.0f,
    .measured_rpm = 0.0f,
    .control_output_pct = 0.0f,
    .temperature_c = 25.0f,
};

/* Synchronization primitives used internally. */
static struct k_mutex state_mutex;
static struct k_sem sample_ready_sem;

/* Forward declaration of zbus listener callback. */
static void motor_state_listener_cb(const struct zbus_channel *chan);

/* Zbus listener that reacts to motor_state updates. */
ZBUS_LISTENER_DEFINE(motor_state_listener, motor_state_listener_cb);

/* Zbus channel used to broadcast motor_state updates. */
ZBUS_CHAN_DEFINE(motor_state_chan,   /* name */
                 struct motor_state, /* message type */
                 NULL,               /* validator */
                 NULL,               /* user data */
                 ZBUS_OBSERVERS(motor_state_listener),
                 ZBUS_MSG_INIT(.setpoint_rpm = 1500.0f, .measured_rpm = 0.0f,
                               .control_output_pct = 0.0f, .temperature_c = 25.0f));

/* Last setpoint value observed by the zbus listener. */
static float last_logged_setpoint = -1.0f;

/**
 * @brief Publish current state on zbus.
 *
 * This helper assumes the state mutex is already locked before calling.
 */
static void app_state_publish_locked(void)
{
    int err = zbus_chan_pub(&motor_state_chan, &g_state, K_NO_WAIT);
    if (err != 0) {
        LOG_WRN("zbus_chan_pub failed: %d", err); /* GCOVR_EXCL_LINE */
    }
}

/**
 * @brief Zbus listener callback for motor_state channel.
 *
 * This callback runs in the zbus listener context whenever a new
 * motor_state is published. Here we log setpoint changes as an example
 * of an event-driven consumer.
 */
static void motor_state_listener_cb(const struct zbus_channel *chan)
{
    const struct motor_state *msg = zbus_chan_const_msg(chan);
    /* GCOVR_EXCL_START */
    if (msg == NULL) {
        return;
    }
    /* GCOVR_EXCL_STOP */

    if (msg->setpoint_rpm != last_logged_setpoint) {
        LOG_INF("ZBUS: setpoint changed to %d rpm", (int)msg->setpoint_rpm);
        last_logged_setpoint = msg->setpoint_rpm;
    }
}

int app_state_init(void)
{
    k_mutex_init(&state_mutex);
    k_sem_init(&sample_ready_sem, 0, 1);

    k_mutex_lock(&state_mutex, K_FOREVER);
    app_state_publish_locked();
    k_mutex_unlock(&state_mutex);

    LOG_INF("app_state initialized: setpoint=%d rpm", (int)g_state.setpoint_rpm);

    return 0;
}

int app_state_set_setpoint(float rpm)
{
    if ((rpm < 0.0f) || (rpm > APP_STATE_MAX_SETPOINT_RPM)) {
        LOG_WRN("Setpoint out of range: %d rpm", (int)rpm);
        return -ERANGE;
    }

    k_mutex_lock(&state_mutex, K_FOREVER);

    g_state.setpoint_rpm = rpm;
    app_state_publish_locked();

    k_mutex_unlock(&state_mutex);

    /* No LOG_INF here: the zbus listener logs setpoint changes. */
    return 0;
}

int app_state_update_feedback(float measured_rpm, float control_output_pct, float temperature_c)
{
    k_mutex_lock(&state_mutex, K_FOREVER);

    g_state.measured_rpm = measured_rpm;
    g_state.control_output_pct = control_output_pct;
    g_state.temperature_c = temperature_c;

    app_state_publish_locked();

    /* Notify listeners (e.g. telemetry) there is a new sample. */
    k_sem_give(&sample_ready_sem);

    k_mutex_unlock(&state_mutex);

    return 0;
}

int app_state_get_snapshot(struct motor_state *out)
{
    if (out == NULL) {
        LOG_ERR("app_state_get_snapshot: out is NULL");
        return -EINVAL;
    }

    k_mutex_lock(&state_mutex, K_FOREVER);
    *out = g_state;
    k_mutex_unlock(&state_mutex);

    return 0;
}

int app_state_wait_for_sample(void)
{
    int ret = k_sem_take(&sample_ready_sem, K_FOREVER);
    if (ret != 0) {
        LOG_ERR("k_sem_take failed: %d", ret); /* GCOVR_EXCL_LINE */
    }

    return ret;
}
