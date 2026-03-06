cmake_minimum_required(VERSION 3.22)
project(spacesim)

set(CMAKE_CXX_STANDARD 20)

find_package(raylib CONFIG REQUIRED)

add_executable(sim_visualizer
    src/sim_visualizer.cpp
    src/physics/solver.cpp
    src/physics/mission.cpp
    src/renderer/body.cpp
    src/renderer/camera.cpp
    src/renderer/grid.cpp
    src/renderer/hud.cpp
    src/renderer/renderer.cpp
    src/renderer/skysphere.cpp
)

target_link_libraries(sim_visualizer PRIVATE raylib)

add_executable(sim_headless
    src/sim_headless.cpp
    src/physics/solver.cpp
    src/physics/mission.cpp
)
