#include <sequencer/audio/sample.hpp>
#include <sequencer/portaudio/portaudio.hpp>

#include <chrono>
#include <iostream>
#include <thread>

using std::cout;
using std::endl;

constexpr auto n_seconds = 5;
constexpr auto sample_rate = 44100;

int main()
{
    try
    {

        auto sample = sequencer::audio::read_write_lockable< sequencer::audio::sample_t >{
            n_seconds * sample_rate};
        sequencer::portaudio::portaudio pa;

        auto parameters = pa.get_parameters( pa.get_default_input_device() );

        // record
        constexpr auto frames_per_buffer = 512;
        sequencer::portaudio::stream_t stream;
        auto writer = sequencer::audio::sample_writer_t{sample};
        stream.open_input_stream( parameters, sample_rate, frames_per_buffer,
                                  sequencer::portaudio::record_callback, &writer );
        stream.start();

        cout << " === Now recording! === " << endl;
        while ( stream.is_active() )
        {
            std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
        }
        std::cout << " Done recording." << endl;
        stream.close();

        // playback recorded data
        parameters = pa.get_parameters( pa.get_default_output_device() );
        auto reader = sequencer::audio::sample_reader_t{sample};

        cout << " === Playing back. === " << endl;
        stream.open_output_stream(
            parameters, sample_rate, frames_per_buffer,
            sequencer::portaudio::play_callback< sequencer::audio::sample_reader_t >, &reader );
        stream.start();
        cout << "Waiting for playback to finish." << endl;
        while ( stream.is_active() )
            std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
        stream.close();
        cout << "Done." << endl;
    }
    catch ( const std::exception& exception )
    {
        cout << "Failure: " << exception.what() << endl;
    }

    return 0;
}
