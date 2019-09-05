#pragma once

#include <sequencer/audio/double_buffer.hpp>
#include <sequencer/midi/track.hpp>
#include <sequencer/portaudio/portaudio.hpp>
#include <sequencer/rtmidi/util.hpp>

#include <QObject>
#include <QString>
#include <RtMidi.h>
#include <chrono>
#include <future>
#include <mutex>
#include <vector>

namespace qml
{
    class backend : public QObject
    {
        Q_OBJECT

        static constexpr auto number_of_steps = 16;
        static constexpr auto number_of_tracks = 8;
        using tracks_t = std::vector< sequencer::midi::sequencer_track_t >;

    public:
        backend();

        ~backend();

        Q_INVOKABLE void start_clock();

        Q_INVOKABLE void stop_clock();

        Q_INVOKABLE void reset_clock();

        Q_INVOKABLE void set_clock_tempo( int bpm );

        Q_INVOKABLE void set_song_position_pointer( int song_position_in_16th_notes );

        Q_INVOKABLE QString available_midi_ports();

        Q_INVOKABLE QString available_audio_devices();

        Q_INVOKABLE void set_audio_device( int id );

        Q_INVOKABLE bool open_port( unsigned id );

        Q_INVOKABLE void set_step( int step, bool checked );

        Q_INVOKABLE bool is_step_set( int step ) const;

        Q_INVOKABLE void set_current_track( int track );

        Q_INVOKABLE int min_note() const;

        Q_INVOKABLE int max_note() const;

        Q_INVOKABLE QString note_to_string( int note ) const;

        Q_INVOKABLE QString notes_to_string() const;

        Q_INVOKABLE void set_note_of_current_track( int note );

        Q_INVOKABLE void start_recording();

        Q_INVOKABLE void stop_recording();

        Q_INVOKABLE void playback();

        Q_INVOKABLE bool is_recording() const noexcept;

        Q_INVOKABLE void set_eq_freq_1( int value );

        Q_INVOKABLE void set_eq_gain_1( int value );

        Q_INVOKABLE void set_eq_q_factor_1( int value );

    private:
        constexpr bool no_device_selected() const noexcept
        {
            return audio_device_id_ == 0;
        }

        void update_sample();

        RtMidiOut midiout_;
        tracks_t tracks_;
        decltype( sequencer::rtmidi::make_clock() ) clock_;
        std::future< void > clock_done_;
        std::uint8_t current_track_{0};
        std::array< sequencer::midi::note_t, number_of_tracks > track_notes_;
        sequencer::portaudio::portaudio portaudio_{};
        int audio_device_id_{0};
        sequencer::portaudio::stream_t stream_;
        std::atomic_bool stop_recording_{false};
        std::future< void > recording_done_;
        std::chrono::seconds max_recording_time_{3};
        int sample_rate_ = 44100;
        std::size_t frames_per_buffer_ = 512;
        sequencer::audio::read_write_lockable< sequencer::audio::sample_t > sample_{
            static_cast< std::size_t >( max_recording_time_.count() ) * sample_rate_};
        int eq_freq_1_{10};
        int eq_gain_1_{0};
        int eq_q_factor_1_{100};
        sequencer::audio::double_buffer_t double_buffer_;
    };
} // namespace qml
