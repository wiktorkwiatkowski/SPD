cmake_minimum_required(VERSION 3.10)
project(LAB2)

add_executable(main src/main.cpp
                src/scheduler.cpp
                inc/scheduler.h
                src/test.cpp)

target_compile_options(main PRIVATE -Wall -Wextra -Wpedantic -O2 -std=c++17)
