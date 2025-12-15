cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(motor_sim_demo_unit_telemetry)

target_sources(app PRIVATE
  src/test_telemetry.c
  ../../../src/app_state.c
  ../../../src/telemetry.c
)

target_include_directories(app PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/../../../src
)

target_compile_definitions(app PRIVATE MOTOR_SIM_DEMO_UNIT_TEST=1)

