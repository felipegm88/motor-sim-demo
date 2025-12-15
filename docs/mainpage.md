# motor-sim-demo

This repository is a small Zephyr-based demo that simulates a motor control loop.

## Modules

- **app_state**: Owns the global motor state (setpoint, measured speed, output, temperature). Provides a snapshot API, a sample-ready synchronization primitive, and a Zbus channel for setpoint updates.
- **motor_control**: Periodic control loop thread. Reads the state, updates output to follow the setpoint, simulates motor dynamics and temperature, and publishes feedback.
- **telemetry**: Thread that waits for new samples and periodically logs the current snapshot.
- **fault_monitor**: Delayable work item that periodically checks speed/temperature conditions and logs fault flags.
- **console_shell**: Shell commands `motor_set <rpm>` and `motor_info` to interact with the demo.

## Tests and coverage

The project includes:
- **Unit tests** for each module using Zephyr's `ztest`.
- A **system integration test** that starts the threads/work items and drives the system through typical scenarios.
- **Line coverage gated at 100% for `src/`** (excluding `src/main.c` and test code).

## Building and running (native_sim)

> Exact steps may depend on your local Zephyr workspace setup.

- Build the application for `native_sim` and run it.
- Use the shell commands to change the setpoint and inspect the current state.

## Generating documentation

This project uses Doxygen. From the repository root:

```sh
doxygen Doxyfile
```

Then open:

- `doxygen-out/html/index.html`

