@page quickstart_page Quickstart

@section prereq Prerequisites

You need the usual Zephyr host tools (Python, CMake, Ninja/Make, a compiler).

This demo targets **`native_sim`**, so it builds and runs on the host.

@section build Build & run

From the repository root:

```bash
west build -b native_sim -p always .
west build -t run
```

@section workspace Workspace creation (using this repo's manifest)

This repository provides a `west.yml` pinned to Zephyr v4.3.0.

```bash
mkdir -p ~/git/motor-sim-workspace
cd ~/git/motor-sim-workspace

git clone https://github.com/felipegm88/motor-sim-demo.git
west init -l motor-sim-demo
west update
west zephyr-export
```
