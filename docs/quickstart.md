# Quickstart

This demo targets **native_sim**, so it builds and runs on your host (Linux/macOS/Windows via WSL).

## Create a Zephyr workspace

If you already have a Zephyr workspace, you can skip this and just add the repo inside it.

```bash
mkdir -p motor-sim-workspace
cd motor-sim-workspace

git clone https://github.com/felipegm88/motor-sim-demo.git motor-sim-demo

python -m venv .venv
source .venv/bin/activate
pip install --upgrade pip
pip install west
```

Initialize the workspace using this repo as the manifest:

```bash
west init -l motor-sim-demo
west update
west zephyr-export
python -m pip install -r zephyr/scripts/requirements.txt
```

## Build and run

From the repository root:

```bash
cd motor-sim-demo
west build -b native_sim -p always .
west build -t run
```

You should see logs similar to:

```text
Hello from Zephyr motor-sim-demo (native_sim)!
```

## Shell commands

In the console:

- `motor_set <rpm>` — set the target speed (0..3000)
- `motor_info` — print the current motor state snapshot
