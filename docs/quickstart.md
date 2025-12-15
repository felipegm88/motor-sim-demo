# Quickstart

## Prerequisites

You need the usual Zephyr host tools (Python, CMake, Ninja/Make, a compiler).

This demo targets **`native_sim`**, so it builds and runs on the host.

## Build and run

From the repository root:

```bash
west build -b native_sim -p always .
west build -t run
```

## Workspace creation (using this repo's manifest)

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
