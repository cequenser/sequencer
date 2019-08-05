add_executable(send_midi_message send_midi_message.cpp)
target_link_libraries(send_midi_message PRIVATE
    Sequencer::Sequencer RtMidi::rtmidi)
target_include_directories(send_midi_message PRIVATE
  ${PROJECT_SOURCE_DIR}/external/repo/install/rtmidi/include)
