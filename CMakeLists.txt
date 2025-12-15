cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED)
project(motor_sim_demo)

target_sources(app PRIVATE
    src/main.c
    src/app_state.c
    src/motor_control.c
    src/telemetry.c
    src/fault_monitor.c
    src/console_shell.c
)
