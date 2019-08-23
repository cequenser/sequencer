function(add_example name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} PRIVATE
        Sequencer::Sequencer RtMidi::rtmidi)
endfunction()

add_example(midi_clock)
add_example(send_midi_message)
add_example(set_song_position_pointer)
