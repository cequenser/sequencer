#pragma once

#include <any>
#include <cstdint>
#include <ostream>
#include <string>
#include <vector>

#define SEQUENCER_DEVICE_CC_VALUES                                                                 \
    X( std::int16_t, min, false )                                                                  \
    X( std::int16_t, max, false )                                                                  \
    X( std::int16_t, value, false )                                                                \
    X( std::uint8_t, cc_msb, false )                                                               \
    X( std::uint8_t, cc_lsb, true )                                                                \
    X( std::uint8_t, decimals, true )

#define SEQUENCER_DEVICE_CC_ENTRIES                                                                \
    X( std::string, name, false )                                                                  \
    SEQUENCER_DEVICE_CC_VALUES                                                                     \
    X( std::vector< std::string >, str_map, true )                                                 \
    X( std::vector< int >, map, true )

namespace sequencer::midi
{
#define X( type, name, is_optional ) type name{};
    struct device_entry_t
    {
        SEQUENCER_DEVICE_CC_ENTRIES
    };
#undef X

#define X( type, name, is_optional )                                                               \
    os << ( is_optional ? sub_sep : sep ) << " " << #name ": " << int( entry.name );
    inline std::ostream& operator<<( std::ostream& os, const device_entry_t& entry )
    {
        constexpr auto sep = ";";
        constexpr auto sub_sep = ",";
        os << "name: " << entry.name;
        SEQUENCER_DEVICE_CC_VALUES
        if ( !entry.str_map.empty() )
        {
            os << sep << "str_map: " << entry.str_map.front();
            for ( auto i = 1u; i < entry.str_map.size(); ++i )
            {
                os << sub_sep << " " << entry.str_map[ i ];
            }
        }
        if ( !entry.map.empty() )
        {
            os << sep << "map: " << int( entry.map.front() );
            for ( auto i = 1u; i < entry.map.size(); ++i )
            {
                os << sub_sep << " " << int( entry.map[ i ] );
            }
        }
        return os;
    }
#undef X

    struct section_t
    {
        explicit section_t( const std::string& name ) : name( name )
        {
        }

        explicit section_t( std::string&& name ) : name( std::move( name ) )
        {
        }

        std::string name;
        std::vector< device_entry_t > entries{};
    };

    inline std::ostream& operator<<( std::ostream& os, const section_t& section )
    {
        os << "section: " << section.name << "\n";
        for ( const auto& entry : section.entries )
        {
            os << entry << "\n";
        }
        return os;
    }

} // namespace sequencer::midi
