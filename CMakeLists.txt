cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)

project(raycaster)

add_subdirectory("raylib")

add_executable(raycaster "main.cpp")

target_link_libraries(raycaster "raylib")
