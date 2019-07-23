add_executable(tests
    main.cpp
    DummyTest.cpp
    sequencer/beat_duration_test.cpp
    sequencer/chrono/chrono_matchers.hpp
    sequencer/chrono/sequencer_clock_test.cpp
    sequencer/chrono/testing_clock_test.cpp
    sequencer/chrono/testing_clock.hpp
    sequencer/midi/midi_message_test.cpp
)
target_include_directories(tests PRIVATE ${PROJECT_SOURCE_DIR}/external/include)
target_link_libraries(tests PRIVATE Sequencer::Sequencer)
target_compile_definitions(tests PRIVATE CATCH_CONFIG_ENABLE_ALL_STRINGMAKERS)
add_test(NAME tests COMMAND tests)
add_test(NAME tests-random COMMAND tests --order rand)
add_test(NAME tests-lex COMMAND tests --order lex)
