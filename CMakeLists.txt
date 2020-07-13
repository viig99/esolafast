cmake_minimum_required(VERSION 3.16)
project(esolafast)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_VISIBILITY_PRESET "hidden")
set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_EXTENSIONS OFF)

# Include KFR subdirectory
add_subdirectory(kfr)
add_subdirectory(spdlog)
add_executable(esolafast main.cpp)

target_sources(esolafast INTERFACE src/*.cpp)
target_link_libraries(esolafast kfr_io spdlog)