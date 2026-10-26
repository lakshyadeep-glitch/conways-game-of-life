cmake_minimum_required(VERSION 3.24)
project(GameOfLife VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
set(SFML_BUILD_AUDIO OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_TEST_SUITE OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(SFML)

add_executable(GameOfLife
    src/main.cpp
    src/Application.cpp
    src/GameOfLife.cpp
    src/Renderer.cpp
)

target_include_directories(GameOfLife PRIVATE include)
target_link_libraries(GameOfLife PRIVATE SFML::Graphics)

if(MSVC)
    target_compile_options(GameOfLife PRIVATE /W4)
else()
    target_compile_options(GameOfLife PRIVATE -Wall -Wextra -Wpedantic)
endif()
