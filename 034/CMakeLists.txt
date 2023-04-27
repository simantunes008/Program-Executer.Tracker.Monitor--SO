cmake_minimum_required(VERSION 3.25)
project(034 C)

set(CMAKE_C_STANDARD 11)

add_executable(034 includes/includes.h src/tracer.c src/monitor.c)
