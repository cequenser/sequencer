#pragma once

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

struct value_map
{
    std::int16_t value;
    std::string name;
};

#define SEQUENCER_DEVICE_CC_ENTRIES                                                                \
    X( std::string, name, false )                                                                  \
    SEQUENCER_DEVICE_CC_VALUES                                                                     \
    X( std::vector< value_map >, map, true )

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
