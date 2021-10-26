cmake_minimum_required(VERSION 3.20)
project(plant_game)

set(CMAKE_CXX_STANDARD 20)

include_directories(../lib)

add_definitions(-DSOKOL_METAL)
set_source_files_properties(../plant_game.cpp PROPERTIES COMPILE_FLAGS "-x objective-c++")

add_executable(plant_game
        ../lib/imgui.cpp
        ../lib/imgui_draw.cpp
        ../lib/imgui_demo.cpp
        ../lib/imgui_tables.cpp
        ../lib/imgui_widgets.cpp
        ../plant_game.cpp)
target_link_libraries(plant_game "-framework Metal")
target_link_libraries(plant_game "-framework Cocoa")
target_link_libraries(plant_game "-framework MetalKit")
target_link_libraries(plant_game "-framework Quartz")