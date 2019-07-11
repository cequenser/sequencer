cmake_minimum_required(VERSION 3.13)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

option(BuildTests "build tests" OFF)
option(Coverage "gather coverage data" OFF)

project(sequencer VERSION 0.0.1 LANGUAGES CXX)

if(Coverage)
    set(CMAKE_BUILD_TYPE Debug)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -coverage")
endif()

if(BuildTests)
    add_subdirectory(tests)
endif()

