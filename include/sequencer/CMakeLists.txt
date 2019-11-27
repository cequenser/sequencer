find_package(Threads REQUIRED)

add_library(sequencer
    Test.hpp
    Test.cpp
    assert.hpp
    beat_duration.hpp
    beat_time_point.hpp
    beats_per_minute.hpp
    copyable_atomic.hpp
    fixed_point_type.hpp
    type_traits.hpp
    vector.hpp
    vector_view.hpp
    wave_form.hpp
    audio/delay.hpp
    audio/double_buffer.hpp
    audio/dry_wet.hpp
    audio/envelope.hpp
    audio/fft.hpp
    audio/note.hpp
    audio/oscillator.hpp
    audio/ring_buffer.hpp
    audio/sample.hpp
    audio/transfer_function.hpp
    audio/wave_reader.hpp
    backend/clock.hpp
    backend/digitakt.hpp
    backend/digitakt_parameter.hpp
    backend/rtmidi.hpp
    backend/synth.hpp
    midi/clock.hpp
    midi/clock_base.hpp
    midi/constants.hpp
    midi/device_spec.hpp
    midi/device_spec_reader.hpp
    midi/note.hpp
    midi/lfo.hpp
    midi/pattern.hpp
    midi/percussion_key.hpp
    midi/step.hpp
    midi/track.hpp
    midi/track_parameter.hpp
    midi/trig_condition.hpp
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
    util/callable.hpp
    util/thread_pool.hpp
    util/thread_safe_queue.hpp
    util/type_erased_storage.hpp
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
install(DIRECTORY ../../device_spec
        DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(EXPORT sequencer-targets
  FILE
    SequencerTargets.cmake
  NAMESPACE
    Sequencer::
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
