cmake_minimum_required(VERSION 3.12)
project(cpu_emulator C CXX)
set(CMAKE_CXX_STANDARD 17)

include_directories(src/)
add_executable(${PROJECT_NAME} tests/main.cpp src/base.cpp src/instructions.cpp src/cpu.cpp)
