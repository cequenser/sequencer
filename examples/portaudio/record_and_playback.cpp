#include <sequencer/audio/sample.hpp>
#include <sequencer/portaudio/portaudio.hpp>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <numeric>
#include <thread>
#include <vector>

using std::cout;
using std::endl;

constexpr auto n_seconds = 5;
constexpr auto sample_rate = 44100;

int record_callback( const void* input_buffer, void*, unsigned long frames_per_buffer,
                     const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags, void* user_data )
{
    auto* data = static_cast< sequencer::audio::sample_t* >( user_data );
    data->read( static_cast< const float* >( input_buffer ), frames_per_buffer );
    return data->frames_left() == 0 ? paComplete : paContinue;
}

int play_callback( const void*, void* output_buffer, unsigned long frames_per_buffer,
                   const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags, void* user_data )
{
    auto* data = static_cast< sequencer::audio::sample_t* >( user_data );
    data->write( static_cast< float* >( output_buffer ), frames_per_buffer );
    return data->frames_left() == 0 ? paComplete : paContinue;
}

int main()
{
    try
    {
        sequencer::audio::sample_t data{n_seconds * sample_rate};
        sequencer::portaudio::portaudio pa;

        PaStreamParameters parameters{};
        parameters.device = pa.get_default_input_device();
        parameters.channelCount = 2;
        parameters.sampleFormat = paFloat32;
        parameters.suggestedLatency = Pa_GetDeviceInfo( parameters.device )->defaultLowInputLatency;
        parameters.hostApiSpecificStreamInfo = nullptr;

        // record
        constexpr auto frames_per_buffer = 512;
        sequencer::portaudio::stream_t stream;
        stream.open_input_stream( parameters, sample_rate, frames_per_buffer, record_callback,
                                  &data );
        stream.start();

        cout << " === Now recording! === " << endl;
        while ( stream.is_active() )
        {
            std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
        }
        std::cout << " Done recording." << endl;
        stream.close();

        // playback recorded data
        data.reset_frame_index();
        parameters.device = pa.get_default_output_device();
        parameters.suggestedLatency =
            Pa_GetDeviceInfo( parameters.device )->defaultLowOutputLatency;

        cout << " === Playing back. === " << endl;
        stream.open_output_stream( parameters, sample_rate, frames_per_buffer, play_callback,
                                   &data );
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
