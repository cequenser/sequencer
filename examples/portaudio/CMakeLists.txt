function(add_example name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} PRIVATE
        Sequencer::Sequencer portaudio)
endfunction()

add_example(record_and_playback)
