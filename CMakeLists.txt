cmake_minimum_required(VERSION 3.0)
project(LinearSystemSolve)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -pg -O2 -lboost_program_options")

find_package(Boost 1.61.0 COMPONENTS program_options REQUIRED)

message(STATUS "Lib dir: " ${Boost_LIBRARIES})

set(INCLUDE_DIR "include")

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})
set(SOURCE_FILES
        include/Exception.hpp
        include/functions.hpp
        source/Exception.cpp
        source/functions.cpp
        source/LinSolve_app.cpp
        )

add_executable(LinearSystemSolve ${SOURCE_FILES})
target_link_libraries(LinearSystemSolve  ${Boost_LIBRARIES})