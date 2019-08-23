#pragma once

#include <sequencer/audio/sample.hpp>

#include <exception>
#include <portaudio.h>
#include <string>
#include <vector>

namespace sequencer::portaudio
{
    class exception : public std::exception
    {
    public:
        explicit exception( std::string&& message ) noexcept : message_( std::move( message ) )
        {
        }

        const char* what() const noexcept override
        {
            return message_.c_str();
        }

    private:
        std::string message_;
    };

    class portaudio
    {
    public:
        portaudio()
        {
            if ( Pa_Initialize() != paNoError )
            {
                throw exception{"Failed to initialize portaudio."};
            }
        }

        ~portaudio()
        {
            Pa_Terminate();
        }

        int get_default_input_device() const
        {
            const auto device = Pa_GetDefaultInputDevice();
            if ( device == paNoDevice )
            {
                throw exception{"No default input device."};
            }
            return device;
        }

        int get_default_output_device() const
        {
            const auto device = Pa_GetDefaultOutputDevice();
            if ( device == paNoDevice )
            {
                throw exception{"No default output device"};
            }
            return device;
        }

        int number_of_devices() const noexcept
        {
            return Pa_GetDeviceCount();
        }

        std::vector< std::string > get_device_names() const
        {
            std::vector< std::string > device_names;
            for ( auto i = 0; i < number_of_devices(); ++i )
            {
                device_names.push_back( Pa_GetDeviceInfo( i )->name );
            }
            return device_names;
        }

        bool is_supported( int input_device_id, audio::mode_t input_mode, int output_device_id,
                           audio::mode_t output_mode, int desired_sample_rate )
        {
            const auto input_parameters = get_parameters( input_device_id, input_mode );
            const auto output_parameters = get_parameters( output_device_id, output_mode );
            const auto error =
                Pa_IsFormatSupported( &input_parameters, &output_parameters, desired_sample_rate );
            return error == paFormatIsSupported;
        }

        PaStreamParameters get_parameters( int device_id,
                                           audio::mode_t mode = audio::mode_t::stereo ) const
        {
            PaStreamParameters parameters;
            parameters.channelCount = static_cast< audio::underlying_t::mode_t >( mode );
            parameters.device = device_id;
            parameters.hostApiSpecificStreamInfo = nullptr;
            parameters.sampleFormat = paFloat32;
            parameters.suggestedLatency = Pa_GetDeviceInfo( device_id )->defaultLowInputLatency;
            parameters.hostApiSpecificStreamInfo =
                nullptr; // See you specific host's API docs for info on using this field
            return parameters;
        }
    };

    class stream_t
    {
    public:
        ~stream_t()
        {
            Pa_CloseStream( stream_ );
        }

        void open_input_stream( const PaStreamParameters& input_parameters, double sample_rate,
                                unsigned long frames_per_buffer, PaStreamCallback* stream_callback,
                                void* data )
        {
            const auto error = Pa_OpenStream( &stream_, &input_parameters, nullptr, sample_rate,
                                              frames_per_buffer, paClipOff, stream_callback, data );
            if ( error != paNoError )
            {
                throw exception{"Could not open input stream, error code: " +
                                std::to_string( error )};
            }
        }

        void open_output_stream( const PaStreamParameters& output_parameters, double sample_rate,
                                 unsigned long frames_per_buffer, PaStreamCallback* stream_callback,
                                 void* data )
        {
            const auto error = Pa_OpenStream( &stream_, nullptr, &output_parameters, sample_rate,
                                              frames_per_buffer, paClipOff, stream_callback, data );
            if ( error != paNoError )
            {
                throw exception{"Could not open output stream, error code: " +
                                std::to_string( error )};
            }
        }

        void start()
        {
            const auto error = Pa_StartStream( stream_ );
            if ( error != paNoError )
            {
                throw exception{"Could not start stream, error code: " + std::to_string( error )};
            }
        }

        bool is_active() const
        {
            return stream_ != nullptr && Pa_IsStreamActive( stream_ ) == 1;
        }

        void close()
        {
            const auto error = Pa_CloseStream( stream_ );
            if ( error != paNoError )
            {
                throw exception{"Could not close stream, error code: " + std::to_string( error )};
            }
        }

    private:
        PaStream* stream_;
    };

    inline int record_callback( const void* input_buffer, void*, unsigned long frames_per_buffer,
                                const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags,
                                void* user_data )
    {
        using sequencer::audio::sample_t;
        auto* data = static_cast< sample_t* >( user_data );
        data->read( static_cast< const sample_t::frame_rep* >( input_buffer ), frames_per_buffer );
        return data->has_frames_left() ? paContinue : paComplete;
    }

    inline int play_callback( const void*, void* output_buffer, unsigned long frames_per_buffer,
                              const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags,
                              void* user_data )
    {
        using sequencer::audio::sample_t;
        auto* data = static_cast< sample_t* >( user_data );
        data->write( static_cast< sample_t::frame_rep* >( output_buffer ), frames_per_buffer );
        return data->has_frames_left() ? paContinue : paComplete;
    }
} // namespace sequencer::portaudio
