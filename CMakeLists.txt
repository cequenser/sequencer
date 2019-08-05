cmake_minimum_required(VERSION 3.13)

list(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_LIST_DIR}/external/repo/install)
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

option(BuildTests "build tests" OFF)
option(BuildExamples "build examples" OFF)
option(Coverage "gather coverage data" OFF)

project(sequencer VERSION 0.0.1 LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake
)

find_package(RtMidi REQUIRED)

if(Coverage)
    set(CMAKE_BUILD_TYPE Debug)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
endif()

add_subdirectory(include)

if(BuildTests)
    enable_testing()
    add_subdirectory(tests)
endif()

if(BuildExamples)
    add_subdirectory(examples)
endif()

