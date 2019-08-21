#pragma once

//#ifdef SEQ_AUDIO
#include <exception>
#include <portaudio.h>
#include <string>

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

        PaDeviceIndex get_default_input_device() const
        {
            const auto device = Pa_GetDefaultInputDevice();
            if ( device == paNoDevice )
            {
                throw exception{"No default input device."};
            }
            return device;
        }

        PaDeviceIndex get_default_output_device() const
        {
            const auto device = Pa_GetDefaultOutputDevice();
            if ( device == paNoDevice )
            {
                throw exception{"No default output device"};
            }
            return device;
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
} // namespace sequencer::portaudio

//#endif
