# motor-sim-demo

A small **Zephyr RTOS** demo application that runs on the **`native_sim`** board and simulates a basic motor control loop.

The goal is to keep a compact, readable project that shows common embedded patterns:
- module-style C design (small .c/.h units),
- state ownership and synchronization,
- threads + workqueue usage,
- Zephyr shell commands,
- unit + integration tests with `ztest`,
- **100% line coverage** for this repository's `src/` (excluding `src/main.c`).

> Developed and tested with **Zephyr v4.3.0**.

---

## Quickstart (build & run)

From a Zephyr workspace where this repo is the application root:

```bash
west build -b native_sim -p always .
west build -t run
```

You should see logs similar to:

```
Hello from Zephyr motor-sim-demo (native_sim)!
```

### Shell commands

In the console:

- `motor_set <rpm>` — set the target speed (0..3000)
- `motor_info` — print the current motor state snapshot

---

## Workspace setup (recommended)

This repository includes a `west.yml` manifest pinned to Zephyr v4.3.0. A clean setup looks like:

```bash
mkdir -p ~/git/motor-sim-workspace
cd ~/git/motor-sim-workspace

git clone https://github.com/felipegm88/motor-sim-demo.git
cd motor-sim-demo

python -m venv .venv
source .venv/bin/activate
pip install west

cd ..
west init -l motor-sim-demo
west update
west zephyr-export
west packages pip --install
```

Notes:
- `native_sim` is a host build, so the Zephyr SDK is optional.
- If you already have a workspace, you can also just add this repo and build it there.

---

## Project structure

Key folders:

```
motor-sim-demo/
├── src/                 # Application code (this is what we target for coverage)
├── tests/
│   ├── unit/            # Unit tests per module (ztest)
│   └── integration/     # System-level tests that exercise threads/work
├── docs/                # Doxygen markdown pages
├── west.yml             # Zephyr manifest (pins Zephyr version)
├── Doxyfile             # Doxygen configuration
└── prj.conf             # App config for native_sim
```

### Modules

- **app_state**: owns the global motor state and provides snapshot/update APIs
- **motor_control**: periodic control loop thread; simulates dynamics + temperature
- **telemetry**: thread that waits for samples and periodically logs snapshots
- **fault_monitor**: delayable work item; checks speed/temp and logs fault flags
- **console_shell**: `motor_set` and `motor_info` shell commands

---

## Tests

### Run all tests

```bash
west twister -T tests -p native_sim -v
```

### Run only unit tests

```bash
west twister -T tests/unit -p native_sim -v
```

### Run only integration tests

```bash
west twister -T tests/integration -p native_sim -v
```

Twister will also emit JUnit-style reports under `twister-out/`.

---

## Coverage (100% lines for `src/`)

### Generate coverage data + HTML report

```bash
west twister -T tests -p native_sim -v   --outdir twister-out-coverage   --coverage --coverage-tool gcovr --coverage-formats html
```

HTML report:

- `twister-out-coverage/coverage/index.html`

### Terminal summary + CI gate (this repo's code only)

This command filters to **only** the repository's `src/` and excludes `src/main.c` and all tests:

```bash
gcovr twister-out-coverage   --root .   --filter '^src/'   --exclude '^src/main\.c$'   --exclude '^tests/'   --print-summary   --fail-under-line 100
```

### What does “branch coverage” mean?

- **Line coverage**: was each line executed at least once?
- **Branch coverage**: were all outcomes of decisions executed? (e.g., both sides of `if/else`,
  all `switch` cases, short-circuit paths in `&&`/`||`, etc.)

In embedded-style code there are often defensive error branches that are hard to trigger deterministically
in CI without fault-injection. For this demo we enforce **100% line coverage** for `src/`.

---

## Documentation

- 📘 Doxygen (API docs): https://felipegm88.github.io/motor-sim-demo/
- 🛠️ Generate locally: `doxygen Doxyfile` → open `doxygen-out/html/index.html`
