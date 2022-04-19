cmake_minimum_required(VERSION 3.21)
project(Project C)

set(CMAKE_C_STANDARD 99)

add_library(Project main.c)

add_executable(main main.c)