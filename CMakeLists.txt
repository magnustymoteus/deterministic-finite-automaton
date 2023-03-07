cmake_minimum_required(VERSION 3.22.1)
project(TA_opdracht1)

set(CMAKE_CXX_STANDARD 17)

add_executable(TA_opdracht1 main.cpp DFA.cpp DFA.h State.cpp State.h json.hpp)
