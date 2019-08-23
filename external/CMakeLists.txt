option(BuildExternalDeps "build external dependencies" ON)

if(BuildExternalDeps)
    message(STATUS "Configure rtmidi")
    set(RTMIDI_BUILD_TESTING OFF)
    add_subdirectory(external/repo/rtmidi)
    add_library(RtMidi::rtmidi ALIAS rtmidi)

    message(STATUS "Configure portaudio")
    add_subdirectory(external/repo/portaudio)
else()
    find_package(RtMidi REQUIRED)
    find_package(portaudio REQUIRED)
endif()
