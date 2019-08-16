add_executable(tests
    main.cpp
    DummyTest.cpp
    sequencer/beat_duration_test.cpp
    sequencer/beat_time_point_test.cpp
    sequencer/fixed_point_type_test.cpp
    sequencer/rtmidi_sender_test.cpp
    sequencer/midi/clock_test.cpp
    sequencer/midi/clock_base_test.cpp
    sequencer/midi/step_sequencer_test.cpp
    sequencer/midi/track_test.cpp
    sequencer/midi/message/channel_mode_test.cpp
    sequencer/midi/message/channel_voice_test.cpp
    sequencer/midi/message/control_change_test.cpp
    sequencer/midi/message/message_type_test.cpp
    sequencer/midi/message/realtime_test.cpp
    sequencer/midi/message/system_common_test.cpp
    sequencer/midi/message/to_string_test.cpp
    sequencer/chrono/chrono_matchers.hpp
    sequencer/chrono/sequencer_clock_test.cpp
    sequencer/chrono/testing_clock_test.cpp
    sequencer/chrono/testing_clock.hpp
)
target_include_directories(tests PRIVATE
  ${PROJECT_SOURCE_DIR}/tests
  ${PROJECT_SOURCE_DIR}/external/include
  ${PROJECT_SOURCE_DIR}/external/repo/install/rtmidi/include)
target_link_libraries(tests PRIVATE Sequencer::Sequencer RtMidi::rtmidi)
target_compile_definitions(tests PRIVATE CATCH_CONFIG_ENABLE_ALL_STRINGMAKERS)
add_test(NAME tests COMMAND tests)
add_test(NAME tests-random COMMAND tests --order rand)
add_test(NAME tests-lex COMMAND tests --order lex)