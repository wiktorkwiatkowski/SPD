cmake_minimum_required(VERSION 3.10)
project(LAB3)

add_executable(main 
src/main.cpp
src/generator.cpp
src/instance_loader.cpp
src/scheduler.cpp
src/tester.cpp
)

target_compile_options(main PRIVATE -Wall -Wextra -Wpedantic -O2 -std=c++17)
