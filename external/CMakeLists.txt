option(BuildExternalDeps "build external dependencies" ON)

if(BuildExternalDeps)
    message(STATUS "Configure rtmidi")
    set(RTMIDI_BUILD_TESTING OFF)
    add_subdirectory(external/repo/rtmidi)
    target_include_directories(rtmidi PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/external/repo/rtmidi>
    )
    add_library(RtMidi::rtmidi ALIAS rtmidi)

    message(STATUS "Configure portaudio")
    add_subdirectory(external/repo/portaudio)
    target_include_directories(portaudio PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/external/repo/portaudio/include>
    )
    add_library(PortAudio::portaudio ALIAS portaudio)

    message(STATUS "Configure fftw")
    add_subdirectory(external/repo/fftw-3.3.8)
else()
    find_package(RtMidi REQUIRED)
    find_package(portaudio REQUIRED)
endif()
