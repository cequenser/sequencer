add_executable(tests
    main.cpp
    DummyTest.cpp
)
target_include_directories(tests PRIVATE ${PROJECT_SOURCE_DIR}/external/include)
target_link_libraries(tests PRIVATE Sequencer::Sequencer)
