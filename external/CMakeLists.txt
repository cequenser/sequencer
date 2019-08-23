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

if(NOT TARGET RtMidi::rtmidi)
    message(WARNING "Target RtMidi::rtmidi not found")
endif()
if(NOT TARGET
        rtmidi)
    message(WARNING "Target rtmidi not found")
endif()
