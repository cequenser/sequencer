#pragma once

namespace sequencer::backend
{
    class digitakt;
}

namespace qt
{
    class poti_t;

    namespace note_control
    {
        constexpr auto index = 0u;

        void init( qt::poti_t& poti );

        void update( sequencer::backend::digitakt& backend, qt::poti_t& poti );
    } // namespace note_control

    namespace velocity_control
    {
        static constexpr auto index = 1u;

        void init( qt::poti_t& poti );

        void update( sequencer::backend::digitakt& backend, qt::poti_t& poti );
    } // namespace velocity_control
} // namespace qt
