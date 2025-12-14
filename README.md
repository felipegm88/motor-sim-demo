# motor-sim-demo

Small Zephyr RTOS project using the `native_sim` board.

The goal of this repository is to be a compact, out-of-tree Zephyr application that can evolve into a small real-time “control/motor simulation” demo, showcasing RTOS and embedded firmware patterns (threads, synchronization, etc.).

> This project is developed and tested in a Zephyr 4.3 workspace.

---

## 1. Prerequisites

You need the basic host tools required by Zephyr (Python, CMake, build tools, compiler, etc.).

Install the host dependencies for your operating system by following the **“Installing prerequisites”** section of the official Zephyr Getting Started guide (Linux, macOS, or Windows):

- Zephyr Getting Started (host dependencies):  
  https://docs.zephyrproject.org/latest/develop/getting_started/index.html#install-dependencies

You do **not** need to create a Zephyr workspace using that guide; this repository provides its own manifest and workspace layout. For this demo we only build for `native_sim`, so installing the Zephyr SDK is optional.

---

## 2. Create a workspace and install west

Pick a directory to use as your Zephyr workspace, for example:

```
    mkdir -p ~/motor-sim-workspace
    cd ~/motor-sim-workspace
```

Create and activate a Python virtual environment (recommended):

```
    python3 -m venv .venv
    source .venv/bin/activate

    pip install --upgrade pip
    pip install west
```

---

## 3. Clone this repository and initialize west

Clone the application repository inside the workspace:

```
    cd ~/motor-sim-workspace
    git clone git@github.com:felipegm88/motor-sim-demo.git
```

Initialize the west workspace using this repo as the manifest:

```
    west init -l motor-sim-demo
```

This creates a `.west/` directory in the workspace root and registers `motor-sim-demo` as the manifest repository.

Fetch Zephyr and any additional projects defined in `west.yml`:

```
    west update
```

Export a Zephyr CMake package:

```
    west zephyr-export
```

Install Python dependencies:

```
    west packages pip --install
```

---

## 4. Build the demo (`native_sim`)

From inside the application directory:

```
    cd ~/motor-sim-workspace/motor-sim-demo
    west build -b native_sim -s .
```

Alternatively, from the workspace root:

```
    cd ~/motor-sim-workspace
    west build -b native_sim motor-sim-demo
```

This configures and builds the project for the `native_sim` board, placing build artifacts under `motor-sim-demo/build/`.

---

## 5. Run the demo

After a successful build, run the `native_sim` executable:

```
    cd ~/motor-sim-workspace/motor-sim-demo
    ./build/zephyr/zephyr.exe
```

You should see output similar to:

```
    Hello from Zephyr motor-sim-demo (native_sim)!
    Tick: 1000 ms
    Tick: 2000 ms
    Tick: 3000 ms
    ...
```

---

## 6. Project structure

Current layout:

    motor-sim-demo/
    ├── west.yml          # Zephyr manifest (pulls in zephyr/ at a fixed revision)
    ├── CMakeLists.txt    # Application CMake entry point
    ├── prj.conf          # Application configuration for native_sim
    └── src/
        └── main.c        # Application entry point

The repository is structured as an out-of-tree Zephyr application.
Future iterations will extend `main.c` into a multi-threaded “motor control simulation” using RTOS primitives (threads, semaphores, workqueues, etc.).

