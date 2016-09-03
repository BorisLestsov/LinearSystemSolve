cmake_minimum_required(VERSION 3.0)
project(LinearSystemSolve)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp Exception.cpp Exception.hpp)
add_executable(LinearSystemSolve ${SOURCE_FILES})
