cmake_minimum_required(VERSION 3.10)
project(LAB1)

add_executable(zad1 src/main.cpp src/problem.cpp src/test.cpp)

target_compile_options(zad1 PRIVATE -Wall -Wextra -Wpedantic -O2 -std=c++17)
