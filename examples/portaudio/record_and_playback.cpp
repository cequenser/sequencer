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

using sample_t = std::uint32_t;

constexpr auto n_seconds = 5;
constexpr auto sample_rate = 44100;
constexpr auto n_channels = 2; // stereo?
const auto n_samples = n_seconds * sample_rate * n_channels;
constexpr auto frames_per_buffer = 512;

struct pa_data
{
    unsigned long frame_index{0};
    unsigned long max_frame_index{n_seconds * sample_rate};
    std::vector< sample_t > recorded_samples = std::vector< sample_t >( n_samples, 0 );
};

int record_callback( const void* input_buffer, void*, unsigned long frames_per_buffer,
                     const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags, void* user_data )
{
    auto* data = static_cast< pa_data* >( user_data );
    const auto frames_left = data->max_frame_index - data->frame_index;
    const auto frames_to_calc = std::min( frames_left, frames_per_buffer );

    sample_t* wptr = &data->recorded_samples[ data->frame_index * n_channels ];
    ( input_buffer == nullptr ) ? std::memset( wptr, 0, frames_to_calc * n_channels * 4 )
                                : std::memcpy( wptr, static_cast< const sample_t* >( input_buffer ),
                                               frames_to_calc * n_channels * 4 );

    data->frame_index += frames_to_calc;
    return frames_left < frames_per_buffer ? paComplete : paContinue;
}

int play_callback( const void*, void* output_buffer, unsigned long frames_per_buffer,
                   const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags, void* user_data )
{
    auto* data = static_cast< pa_data* >( user_data );
    auto* rptr = &data->recorded_samples[ data->frame_index * n_channels ];
    auto wptr = static_cast< sample_t* >( output_buffer );
    const auto frames_left = data->max_frame_index - data->frame_index;

    if ( frames_left < frames_per_buffer )
    {
        /* final buffer... */
        const auto size = frames_left * n_channels * 4;
        std::memcpy( wptr, rptr, size );
        std::memset( wptr + size, 0, ( frames_per_buffer - frames_left ) * n_channels * 4 );
        data->frame_index += frames_left;
        return paComplete;
    }

    std::memcpy( wptr, rptr, frames_per_buffer * n_channels * 4 );
    data->frame_index += frames_per_buffer;
    return paContinue;
}

int main()
{
    pa_data data;
    sequencer::portaudio::portaudio pa;

    PaStreamParameters parameters{};
    parameters.device = pa.get_default_input_device();
    parameters.channelCount = 2;
    parameters.sampleFormat = paFloat32;
    parameters.suggestedLatency = Pa_GetDeviceInfo( parameters.device )->defaultLowInputLatency;
    parameters.hostApiSpecificStreamInfo = nullptr;

    // record
    sequencer::portaudio::stream_t stream;
    stream.open_input_stream( parameters, sample_rate, frames_per_buffer, record_callback, &data );
    stream.start();

    cout << " === Now recording! === " << endl;
    while ( stream.is_active() )
    {
        std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
        cout << "frame index: " << data.frame_index << endl;
    }
    std::cout << " Done recording." << endl;
    stream.close();

    // playback recorded data
    data.frame_index = 0;
    parameters.device = pa.get_default_output_device();
    parameters.suggestedLatency = Pa_GetDeviceInfo( parameters.device )->defaultLowOutputLatency;

    cout << " === Playing back. === " << endl;
    stream.open_output_stream( parameters, sample_rate, frames_per_buffer, play_callback, &data );
    stream.start();
    cout << "Waiting for playback to finish." << endl;
    while ( stream.is_active() )
        std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
    stream.close();
    cout << "Done." << endl;
    return 0;
}
