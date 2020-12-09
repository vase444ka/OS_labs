cmake_minimum_required(VERSION 3.17)
project(lab_1)

set(CMAKE_CXX_STANDARD 20)

add_executable(lab_1 main.cpp src/manager.cpp)

add_executable(f_g_funcs src/worker_executable.cpp src/worker.cpp src/worker.hpp)