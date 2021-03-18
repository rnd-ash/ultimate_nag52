cmake_minimum_required(VERSION 3.7)
project(nag52_simulator)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -lSDL2 -lSDL2_image -lSDL2_ttf")
find_package(SDL2 REQUIRED)

include_directories(simulator)
include_directories(can_frames)
include_directories(gearbox)

add_definitions(-DSIM_MODE)

add_executable(nag52_simulator
        simulator/simulator.cpp
        simulator/canbus/esp32_forwarder.cpp
        simulator/canbus/esp32_forwarder.h
        simulator/canbus/car_sim.cpp

        simulator/ecus/abstract_ecu.cpp
        simulator/ecus/abstract_ecu.h
        simulator/ecus/ms/engine.cpp
        simulator/ecus/ms/engine.h
        simulator/ecus/gs/nag52.cpp
        simulator/ecus/gs/nag52.h
        simulator/ecus/ewm/ewm.cpp
        simulator/ecus/ewm/ewm.h
        simulator/ecus/bs/abs_esp.cpp
        simulator/ecus/bs/abs_esp.h
        can_frames/can_c_enums.h
        can_frames/can_frames.h
        can_frames/can_frames.cpp

        simulator/v_kombi/virtual_kombi.cpp
        simulator/v_kombi/virtual_kombi.h
        simulator/v_kombi/kombi_lcd.cpp
        simulator/v_kombi/kombi_lcd.h
        simulator/v_kombi/Needle.cpp
        simulator/v_kombi/lcd_symbols.h
        simulator/v_kombi/lcd_symbols.cpp
        simulator/v_kombi/diag_overlay.cpp
        simulator/v_kombi/diag_overlay.h

        gearbox/profiles.h
        gearbox/profiles.cpp
        gearbox/nag_iface.h
        gearbox/gearbox_brain.cpp
        gearbox/gearbox_brain.h
        gearbox/shift_api.cpp
        gearbox/shift_api.h
)

target_link_libraries(nag52_simulator ${SDL2_LIBRARY} )