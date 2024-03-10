cmake_minimum_required(VERSION 3.12)
project(cpu_emulator C CXX)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(${CMAKE_SOURCE_DIR}/3rd_party/gtest ${CMAKE_BINARY_DIR}/3rd_party/gtest EXCLUDE_FROM_ALL)

include_directories(src/)
add_executable(${PROJECT_NAME} tests/main.cpp tests/instructions_tests.cpp src/base.cpp src/instructions.cpp src/cpu.cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC gtest)
