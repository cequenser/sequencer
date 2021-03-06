cmake_minimum_required(VERSION 3.13)

project(sequencer VERSION 0.0.1 LANGUAGES CXX)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

option(SEQUENCER_BuildTests "build tests" OFF)
option(SEQUENCER_BuildExamples "build examples" OFF)
option(SEQUENCER_Coverage "gather coverage data" OFF)
option(SEQUENCER_DisableAssert "disable asserts" OFF)


include(external/CMakeLists.txt)

list(APPEND CMAKE_MODULE_PATH
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake
)

if(SEQUENCER_Coverage)
    set(CMAKE_BUILD_TYPE Debug)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
endif()

if(SEQUENCER_DisableAssert)
    add_definitions(-DSEQUENCER_DISABLE_ASSERT)
endif()

add_subdirectory(include)

if(SEQUENCER_BuildTests)
    enable_testing()
    add_subdirectory(tests)
endif()

if(SEQUENCER_BuildExamples)
    add_subdirectory(examples)
endif()

