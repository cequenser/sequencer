#pragma once

#include <sequencer/midi/message/message_type.hpp>
#include <sequencer/midi/message/util.hpp>

#include <cstdint>

namespace sequencer::midi::system::exclusive
{
    static constexpr auto start_byte = std::byte{0xF0};
    static constexpr auto end_byte = std::byte{0xF7};

    constexpr message_t< 1 > end_system_exclusive() noexcept
    {
        return {end_byte};
    }

    template < class... Data >
    constexpr message_t< sizeof...( Data ) + 3u > system_exclusive( std::byte manufacturer_id,
                                                                    Data&&... data ) noexcept
    {
        return {start_byte, manufacturer_id, std::byte( std::forward< Data >( data ) )...,
                end_byte};
    }

    template < class... Data >
    constexpr message_t< sizeof...( Data ) + 3u > system_exclusive( std::uint8_t manufacturer_id,
                                                                    Data&&... data ) noexcept
    {
        return system_exclusive( std::byte{manufacturer_id}, std::forward< Data >( data )... );
    }

    namespace realtime
    {
        static constexpr auto id_byte = std::byte{0x7F};
        static constexpr auto device_control_byte = std::byte{0x04};

        template < class... Data >
        constexpr message_t< sizeof...( Data ) + 4u >
        system_exclusive( std::uint8_t manufacturer_id, Data&&... data ) noexcept
        {
            return exclusive::system_exclusive( id_byte, manufacturer_id,
                                                std::forward< Data >( data )... );
        }

        constexpr message_t< 8 > device_control( std::uint8_t manufacturer_id, std::uint8_t key,
                                                 std::uint16_t value ) noexcept
        {
            const auto hex_value = uint16_to_lsb_msb( value );
            return system_exclusive( manufacturer_id, device_control_byte, key, hex_value.first,
                                     hex_value.second );
        }

        constexpr message_t< 8 > master_volume( std::uint8_t manufacturer_id,
                                                std::uint16_t value ) noexcept
        {
            return device_control( manufacturer_id, 0x01, value );
        }

        constexpr message_t< 8 > master_balance( std::uint8_t manufacturer_id,
                                                 std::uint16_t value ) noexcept
        {
            return device_control( manufacturer_id, 0x02, value );
        }

        constexpr message_t< 8 > master_fine_tuning( std::uint8_t manufacturer_id,
                                                     std::uint16_t value ) noexcept
        {
            return device_control( manufacturer_id, 0x03, value );
        }

        constexpr message_t< 8 > master_coarse_tuning( std::uint8_t manufacturer_id,
                                                       std::uint16_t value ) noexcept
        {
            return device_control( manufacturer_id, 0x04, value );
        }

        template < std::size_t... index >
        constexpr message_t< 4 + sizeof...( index ) >
        make_sysex( std::uint8_t manufacturer_id, const message_t< sizeof...( index ) >& data,
                    std::index_sequence< index... > )
        {
            return system_exclusive( manufacturer_id, data[ index ]... );
        }

        template < class Id, class Value, std::size_t number_of_values,
                   std::size_t number_of_slots = 0 >
        constexpr message_t< 9 + ( sizeof( Id ) + sizeof( Value ) ) * number_of_values +
                             2 * number_of_slots >
        global_parameter_control(
            std::uint8_t manufacturer_id,
            const std::array< std::pair< Id, Value >, number_of_values >& id_value_pairs,
            const std::array< std::uint16_t, number_of_slots >& slots = {} ) noexcept
        {
            constexpr auto pair_size = sizeof( Id ) + sizeof( Value );
            constexpr auto user_data_size = pair_size * number_of_values + 2 * number_of_slots;
            message_t< 5 + user_data_size > data;
            data[ 0 ] = device_control_byte;
            data[ 1 ] = std::byte{0x05}; // global parameter control
            data[ 2 ] = std::byte{number_of_slots};
            data[ 3 ] = std::byte{sizeof( Id )};
            data[ 4 ] = std::byte{sizeof( Value )};

            for ( auto i = std::size_t{0}; i < number_of_slots; ++i )
            {
                const auto [ lsb, msb ] = uint16_to_lsb_msb( slots[ i ] );
                data[ 5 + 2 * i ] = std::byte{msb};
                data[ 5 + 2 * i + 1 ] = std::byte{lsb};
            }

            const auto offset = 5 + 2 * number_of_slots;
            for ( auto i = std::size_t{0}; i < number_of_values; ++i )
            {
                if constexpr ( sizeof( Id ) == 1 && sizeof( Value ) == 1 )
                {
                    data[ offset + pair_size * i ] = std::byte{id_value_pairs[ i ].first};
                    data[ offset + pair_size * i + 1 ] = std::byte{id_value_pairs[ i ].second};
                }

                if constexpr ( sizeof( Id ) == 2 && sizeof( Value ) == 1 )
                {
                    const auto [ id_lsb, id_msb ] = uint16_to_lsb_msb( id_value_pairs[ i ].first );
                    data[ offset + pair_size * i ] = std::byte{id_msb};
                    data[ offset + pair_size * i + 1 ] = std::byte{id_lsb};
                    data[ offset + pair_size * i + 2 ] = std::byte{id_value_pairs[ i ].second};
                }

                if constexpr ( sizeof( Id ) == 1 && sizeof( Value ) == 2 )
                {
                    const auto [ value_lsb, value_msb ] =
                        uint16_to_lsb_msb( id_value_pairs[ i ].second );
                    data[ offset + pair_size * i ] = std::byte{id_value_pairs[ i ].first};
                    data[ offset + pair_size * i + 1 ] = std::byte{value_lsb};
                    data[ offset + pair_size * i + 2 ] = std::byte{value_msb};
                }

                if constexpr ( sizeof( Id ) == 2 && sizeof( Value ) == 2 )
                {
                    const auto [ id_lsb, id_msb ] = uint16_to_lsb_msb( id_value_pairs[ i ].first );
                    data[ offset + pair_size * i ] = std::byte{id_msb};
                    data[ offset + pair_size * i + 1 ] = std::byte{id_lsb};
                    const auto [ value_lsb, value_msb ] =
                        uint16_to_lsb_msb( id_value_pairs[ i ].second );
                    data[ offset + pair_size * i + 2 ] = std::byte{value_lsb};
                    data[ offset + pair_size * i + 3 ] = std::byte{value_msb};
                }
            }

            return make_sysex( manufacturer_id, data,
                               std::make_index_sequence< 5 + user_data_size >() );
        }

        enum class key_based_instrument_control_type : std::uint8_t
        {
            note_volume = 0x07,
            pan = 0x0A,
            timbre_harmonic_intensity = 0x47,
            release_time = 0x48,
            attack_time = 0x49,
            brightness = 0x4A,
            decay_time = 0x4B,
            vibrato_rate = 0x4C,
            vibrato_depth = 0x4D,
            vibrato_delay = 0x4E,
            reverb_send = 0x5B,
            chorus_send = 0x5D,
            fine_tuning = 0x78,
            coarse_tuning = 0x79
        };

        constexpr std::uint8_t to_uint8( key_based_instrument_control_type type ) noexcept
        {
            return static_cast< std::uint8_t >( type );
        }

        template < std::size_t number_of_entries >
        constexpr message_t< 8 + 2 * number_of_entries > key_based_instrument_control(
            std::uint8_t manufacturer_id, std::uint8_t channel, std::uint8_t key,
            const std::array< std::pair< std::uint8_t, std::uint8_t >, number_of_entries >&
                number_value_pairs ) noexcept
        {
            assert( channel < 16 );
            constexpr auto fixed_entry_offset = 4u;
            constexpr auto data_size = fixed_entry_offset + 2 * number_of_entries;
            message_t< data_size > data;
            data[ 0 ] = std::byte{0x0A};
            data[ 1 ] = std::byte{0x01};
            data[ 2 ] = std::byte{channel};
            data[ 3 ] = std::byte{key};
            for ( auto i = std::size_t{0}; i < number_of_entries; ++i )
            {
                const auto id = number_value_pairs[ i ].first;
                const auto value = number_value_pairs[ i ].second;
                data[ fixed_entry_offset + 2 * i ] = std::byte{id};
                assert( !( id == to_uint8( key_based_instrument_control_type::note_volume ) ) ||
                        ( value <= 0x40 ) );
                assert(
                    !( id ==
                       to_uint8( key_based_instrument_control_type::timbre_harmonic_intensity ) ) ||
                    ( value <= 0x40 ) );
                assert( !( id == to_uint8( key_based_instrument_control_type::release_time ) ) ||
                        ( value <= 0x40 ) );
                assert( !( id == to_uint8( key_based_instrument_control_type::attack_time ) ) ||
                        ( value <= 0x40 ) );
                assert( !( id == to_uint8( key_based_instrument_control_type::brightness ) ) ||
                        ( value <= 0x40 ) );
                assert( !( id == to_uint8( key_based_instrument_control_type::decay_time ) ) ||
                        ( value <= 0x40 ) );
                assert( !( id == to_uint8( key_based_instrument_control_type::vibrato_rate ) ) ||
                        ( value <= 0x40 ) );
                assert( !( id == to_uint8( key_based_instrument_control_type::vibrato_depth ) ) ||
                        ( value <= 0x40 ) );
                assert( !( id == to_uint8( key_based_instrument_control_type::vibrato_delay ) ) ||
                        ( value <= 0x40 ) );
                assert( !( id == to_uint8( key_based_instrument_control_type::fine_tuning ) ) ||
                        ( value <= 0x40 ) );
                assert( !( id == to_uint8( key_based_instrument_control_type::coarse_tuning ) ) ||
                        ( value <= 0x40 ) );
                data[ fixed_entry_offset + 2 * i + 1 ] = std::byte{value};
            }

            return make_sysex( manufacturer_id, data, std::make_index_sequence< data_size >() );
        }
    } // namespace realtime

    namespace non_realtime
    {
        static constexpr auto id_byte = std::byte{0x7E};
        static constexpr auto gm_byte = std::byte{0x09};
        static constexpr auto dls_byte = std::byte{0x0A};

        template < class... Data >
        constexpr message_t< sizeof...( Data ) + 4u >
        system_exclusive( std::uint8_t manufacturer_id, Data&&... data ) noexcept
        {
            return exclusive::system_exclusive( id_byte, manufacturer_id,
                                                std::forward< Data >( data )... );
        }

        constexpr message_t< 6 > general_midi_system( std::uint8_t manufacturer_id,
                                                      std::uint8_t value ) noexcept
        {
            return system_exclusive( manufacturer_id, gm_byte, value );
        }

        constexpr message_t< 6 >
        general_midi_system_disable( std::uint8_t manufacturer_id ) noexcept
        {
            return general_midi_system( manufacturer_id, 0x02 );
        }

        constexpr message_t< 6 >
        general_midi_1_system_enable( std::uint8_t manufacturer_id ) noexcept
        {
            return general_midi_system( manufacturer_id, 0x01 );
        }

        constexpr message_t< 6 >
        general_midi_2_system_enable( std::uint8_t manufacturer_id ) noexcept
        {
            return general_midi_system( manufacturer_id, 0x03 );
        }

        constexpr message_t< 6 > downloadable_sounds( std::uint8_t manufacturer_id,
                                                      std::byte value ) noexcept
        {
            return system_exclusive( manufacturer_id, dls_byte, value );
        }

        constexpr message_t< 6 > downloadable_sounds( std::uint8_t manufacturer_id,
                                                      bool on ) noexcept
        {
            const std::uint8_t value = on ? 0x01 : 0x02;
            return downloadable_sounds( manufacturer_id, std::byte{value} );
        }

        constexpr message_t< 6 > downloadable_sounds_voice_allocation( std::uint8_t manufacturer_id,
                                                                       bool on ) noexcept
        {
            const std::uint8_t value = on ? 0x04 : 0x03;
            return downloadable_sounds( manufacturer_id, std::byte{value} );
        }
    } // namespace non_realtime
} // namespace sequencer::midi::system::exclusive
