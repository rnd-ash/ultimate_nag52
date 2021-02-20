# cmake_minimum_required(VERSION <specify CMake version here>)
project(ecu_can_simulator)

set(CMAKE_CXX_STANDARD 14)

add_executable(ecu_can_simulator main.cpp ecus/abstract_ecu.cpp ecus/abstract_ecu.h ecus/ms/engine.cpp ecus/ms/engine.h canbus/can_sim.h canbus/esp32_forwarder.cpp canbus/esp32_forwarder.h canbus/can_sim.cpp ecus/gs/nag52_emulator.cpp ecus/gs/nag52_emulator.h)