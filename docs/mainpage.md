# motor-sim-demo

This is a small **Zephyr RTOS** demo that runs on **`native_sim`** and simulates a basic motor control loop.

It is designed to be:
- small enough to read end-to-end,
- realistic enough to show common embedded patterns,
- testable (unit + integration),
- and well documented (Doxygen + Markdown pages).

## Modules

- **app_state**: Owns the global motor state (setpoint, measured RPM, output %, temperature). Provides snapshot/update APIs and synchronization.
- **motor_control**: Periodic control loop thread. Reads state, updates simulated dynamics and temperature, and publishes feedback.
- **telemetry**: Thread that waits for new samples and periodically logs snapshots.
- **fault_monitor**: Delayable work item that periodically checks speed/temperature and logs fault flags.
- **console_shell**: Shell commands `motor_set <rpm>` and `motor_info`.

## Quickstart

From the repository root in a Zephyr workspace:

```bash
west build -b native_sim -p always .
west build -t run
```

Shell commands:

- `motor_set <rpm>` (0..3000)
- `motor_info`

## More documentation

- [Quickstart](quickstart.md)
- [Testing](testing.md)
- [Coverage](coverage.md)
- [Doxygen](doxygen.md)
