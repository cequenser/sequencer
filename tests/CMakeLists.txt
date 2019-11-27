cmake_minimum_required(VERSION 3.13)

project(sequencer-tests LANGUAGES CXX)

if(NOT TARGET Sequencer::Sequencer)
    enable_testing()
    find_package(RtMidi REQUIRED)
    find_package(Sequencer REQUIRED)

    list(APPEND CMAKE_MODULE_PATH
        ${CMAKE_CURRENT_SOURCE_DIR}/../cmake
    )
    set(SEQUENCER_DEVICE_SPEC_PATH ${PROJECT_SOURCE_DIR}/../lib/device_spec/)
else()
    set(SEQUENCER_DEVICE_SPEC_PATH ${PROJECT_SOURCE_DIR}/../device_spec/)
endif()
add_executable(tests
    main.cpp
    DummyTest.cpp
    sequencer/beat_duration_test.cpp
    sequencer/beat_time_point_test.cpp
    sequencer/fixed_point_type_test.cpp
    sequencer/type_traits_test.cpp
    sequencer/vector_view_test.cpp
    sequencer/audio/delay_test.cpp
    sequencer/audio/double_buffer_test.cpp
    sequencer/audio/envelope_test.cpp
    sequencer/audio/fft_test.cpp
    sequencer/audio/oscillator_test.cpp
    sequencer/audio/ring_buffer_test.cpp
    sequencer/audio/sample_test.cpp
    sequencer/audio/transfer_function_test.cpp
    sequencer/backend/digitakt_test.cpp
    sequencer/backend/digitakt_lfo_test.cpp
    sequencer/backend/digitakt_parameter_test.cpp
    sequencer/midi/clock_test.cpp
    sequencer/midi/clock_base_test.cpp
    sequencer/midi/device_spec_test.cpp
    sequencer/midi/lfo_test.cpp
    sequencer/midi/pattern_test.cpp
    sequencer/midi/step_test.cpp
    sequencer/midi/track_test.cpp
    sequencer/midi/message/channel_voice_test.cpp
    sequencer/midi/message/realtime_test.cpp
    sequencer/midi/message/system_common_test.cpp
    sequencer/midi/message/system_exclusive_test.cpp
    sequencer/midi/message/util_test.cpp
    sequencer/chrono/chrono_matchers.hpp
    sequencer/chrono/sequencer_clock_test.cpp
    sequencer/chrono/testing_clock_test.cpp
    sequencer/chrono/testing_clock.hpp
)
target_include_directories(tests PRIVATE
   .
  ${PROJECT_SOURCE_DIR}/../external/include)
target_link_libraries(tests PRIVATE Sequencer::Sequencer)
target_compile_definitions(tests PRIVATE CATCH_CONFIG_ENABLE_ALL_STRINGMAKERS)
add_test(NAME tests COMMAND tests WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
add_test(NAME tests-random COMMAND tests --order rand WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
add_test(NAME tests-lex COMMAND tests --order lex WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/device_spec)

add_custom_command(
        TARGET tests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${SEQUENCER_DEVICE_SPEC_PATH}
                ${CMAKE_BINARY_DIR}/device_spec/
)
