cmake_minimum_required(VERSION 3.7)
project(simulator)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -lSDL2 -lSDL2_image")
find_package(SDL2 REQUIRED)

include_directories(${SDL2_INCLUDE_DIR})
include_directories("canframes/EWM/")
include_directories("canframes/BS/")
include_directories("canframes/ART/")
include_directories("canframes/EZS/")
include_directories("canframes/GS/")
include_directories("canframes/MS/")
include_directories("canframes/PSM/")
include_directories("canframes/SBW/")

add_definitions(-DSIM_MODE)

add_executable(simulator
        simulator/main.cpp
        simulator/ecus/abstract_ecu.cpp
        simulator/ecus/abstract_ecu.h
        simulator/ecus/ms/engine.cpp
        simulator/ecus/ms/engine.h
        simulator/ecus/gs/nag52.cpp
        simulator/ecus/gs/nag52.h
        simulator/canbus/car_sim.h
        simulator/canbus/esp32_forwarder.cpp
        simulator/canbus/esp32_forwarder.h
        simulator/canbus/car_sim.cpp
        simulator/ecus/ewm/ewm.cpp
        simulator/ecus/ewm/ewm.h
        simulator/ecus/bs/abs_esp.cpp
        simulator/ecus/bs/abs_esp.h
        simulator/v_kombi/virtual_kombi.cpp
        simulator/v_kombi/virtual_kombi.h
        simulator/v_kombi/kombi_lcd.cpp
        simulator/v_kombi/kombi_lcd.h
        simulator/v_kombi/Needle.cpp
        simulator/v_kombi/lcd_symbols.h
        simulator/v_kombi/lcd_symbols.cpp

        # Include all common code from NAG
        can_frame.h
        gearbox_brain.h
        gearbox_brain.cpp
        profiles.h
        profiles.cpp
        simulator/ecus/ecus.h)
target_link_libraries(simulator ${SDL2_LIBRARY} )