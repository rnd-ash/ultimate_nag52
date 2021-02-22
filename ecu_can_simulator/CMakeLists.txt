cmake_minimum_required(VERSION 3.7)
project(ecu_can_simulator)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -lSDL2 -lSDL2_image")
find_package(SDL2 REQUIRED)

include_directories(${SDL2_INCLUDE_DIR})

include_directories("../canframes/ART")
include_directories("../canframes/BS")
include_directories("../canframes/EWM")
include_directories("../canframes/GS")
include_directories("../canframes/MS")
include_directories("../canframes/PSM")
include_directories("../canframes/SBW")
include_directories("../canframes/EZS")
include_directories("../canframes/GS")

add_executable(ecu_can_simulator
        main.cpp
        ecus/abstract_ecu.cpp
        ecus/abstract_ecu.h
        ecus/ms/engine.cpp
        ecus/ms/engine.h
        ecus/gs/nag52.cpp
        ecus/gs/nag52.h
        canbus/can_sim.h
        canbus/esp32_forwarder.cpp
        canbus/esp32_forwarder.h
        canbus/can_sim.cpp
        canbus/can_frame.h ecus/ewm/ewm.cpp ecus/ewm/ewm.h ecus/bs/abs_esp.cpp ecus/bs/abs_esp.h v_kombi/virtual_kombi.cpp v_kombi/virtual_kombi.h v_kombi/kombi_lcd.cpp v_kombi/kombi_lcd.h v_kombi/Needle.cpp)
target_link_libraries(ecu_can_simulator ${SDL2_LIBRARY} )