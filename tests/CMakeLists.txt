set(TEST_FILES
    main.cpp
    DummyTest.cpp
)

add_executable(tests ${TEST_FILES})
target_include_directories(tests PRIVATE
    ${PROJECT_SOURCE_DIR}/external/include
)
target_compile_features(tests PRIVATE cxx_std_17)
