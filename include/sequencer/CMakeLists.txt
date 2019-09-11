find_package(Threads REQUIRED)

add_library(sequencer
    Test.h
    Test.cpp
    assert.hpp
    beat_duration.hpp
    beat_time_point.hpp
    beats_per_minute.hpp
    fixed_point_type.hpp
    type_traits.hpp
    vector_view.hpp
    audio/double_buffer.hpp
    audio/fft.hpp
    audio/sample.hpp
    audio/transfer_function.hpp
    backend/digitakt.hpp
    midi/clock.hpp
    midi/clock_base.hpp
    midi/note.hpp
    midi/percussion_key.hpp
    midi/track.hpp
    midi/message/channel_voice.hpp
    midi/message/message_type.hpp
    midi/message/realtime.hpp
    midi/message/system_common.hpp
    midi/message/system_exclusive.hpp
    midi/message/util.hpp
    chrono/chrono_adapter.hpp
    chrono/sequencer_clock.hpp
    chrono/units.hpp
    rtmidi/util.hpp
    portaudio/portaudio.hpp
)
target_compile_features(sequencer PUBLIC cxx_std_17)
set_target_properties(sequencer PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(sequencer
  PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/external/repo/portaudio/include>
)
target_link_libraries(sequencer PUBLIC RtMidi::rtmidi Threads::Threads)

# configure installation and registration
set_target_properties(sequencer PROPERTIES EXPORT_NAME Sequencer)
add_library(Sequencer::Sequencer ALIAS sequencer)

include(GenerateExportHeader)
generate_export_header(sequencer
    EXPORT_FILE_NAME sequencer_export.hpp
    )

include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/sequencer)

install(TARGETS sequencer
    EXPORT sequencer-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(DIRECTORY .
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/sequencer
    FILES_MATCHING PATTERN "*.hpp"
    )
install(FILES ${PROJECT_BINARY_DIR}/include/sequencer/sequencer_export.hpp
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/sequencer
)
install(EXPORT sequencer-targets
  FILE
    SequencerTargets.cmake
  NAMESPACE
    SequencerTargets::
  DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cmake/sequencer
)

#Create a ConfigVersion.cmake file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/SequencerConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(${PROJECT_SOURCE_DIR}/cmake/SequencerConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/SequencerConfig.cmake
    INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)

#Install the config, configversion and custom find modules
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/SequencerConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/SequencerConfigVersion.cmake
    DESTINATION ${INSTALL_CONFIGDIR}
)

##############################################
export(EXPORT sequencer-targets FILE ${CMAKE_CURRENT_BINARY_DIR}/SequencerTargets.cmake NAMESPACE Sequencer::)
#Register package in user's package registry
export(PACKAGE Sequencer)
