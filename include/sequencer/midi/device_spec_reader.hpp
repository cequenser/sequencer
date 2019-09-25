#pragma once

#include <sequencer/midi/device_spec.hpp>

#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <vector>

namespace sequencer::midi
{
    const auto key_sep = std::string( ":" );
    const auto string_identifier_regex_str = std::string{"([a-zA-Z]+[\\sa-zA-Z0-9_]*)"};

    inline std::regex get_key_int_regex( const std::string& key )
    {
        return std::regex( key + "\\s*" + key_sep + "\\s*([\\-0-9]+)" );
    }

#define X( type, name, is_optional )                                                               \
    if ( std::regex_search( buffer, match, get_key_int_regex( #name ) ) )                          \
    {                                                                                              \
        entry.name = decltype( entry.name )( std::stoi( match[ 1 ] ) );                            \
    }                                                                                              \
    else                                                                                           \
    {                                                                                              \
        if constexpr ( !is_optional )                                                              \
        {                                                                                          \
            throw std::runtime_error( "Could not parse '" #name "' in line: " + buffer );          \
        }                                                                                          \
    }
    inline void read_entry( device_entry_t& entry, const std::string& buffer )
    {
        const auto name_regex =
            std::regex( "name\\s*" + key_sep + "\\s*" + string_identifier_regex_str );

        std::smatch match;
        if ( std::regex_search( buffer, match, name_regex ) )
        {
            entry.name = match[ 1 ];
        }
        else
        {
            throw std::runtime_error( "Could not parse 'name' in line: " + buffer );
        }

        SEQUENCER_DEVICE_CC_VALUES

        const auto sub_sep = ",";
        const auto str_map_regex = std::regex( "str_map\\s*" + key_sep + "\\s*\\{.*\\}" );
        if ( std::regex_search( buffer, str_map_regex ) )
        {
            entry.str_map.resize( entry.max - entry.min + 1 );
            auto pos = buffer.find( "str_map" );
            pos = 1 + buffer.find( key_sep, pos + 1 );
            const auto entry_regex =
                std::regex( "([\\-0-9]+)\\s*" + key_sep + "\\s*" + string_identifier_regex_str );
            auto sub = buffer.substr( pos );
            while ( std::regex_search( sub, match, entry_regex ) )
            {
                entry.str_map[ std::size_t( std::stoi( match[ 1 ] ) - entry.min ) ] = match[ 2 ];
                pos = sub.find( sub_sep );
                if ( pos == std::string::npos )
                {
                    break;
                }
                sub = sub.substr( pos + 1 );
            }
            return;
        }

        const auto map_regex = std::regex( "map\\s*" + key_sep + "\\s*\\{.*\\}" );
        if ( std::regex_search( buffer, map_regex ) )
        {
            entry.map.resize( entry.max - entry.min + 1 );
            auto pos = buffer.find( "map" );
            pos = 1 + buffer.find( key_sep, pos + 1 );
            const auto entry_regex = std::regex( "([\\-0-9]+)\\s*" + key_sep + "\\s*([\\-0-9]+)" );
            auto sub = buffer.substr( pos );
            while ( std::regex_search( sub, match, entry_regex ) )
            {
                entry.map[ std::size_t( std::stoi( match[ 1 ] ) - entry.min ) ] =
                    std::stoi( match[ 2 ] );
                pos = sub.find( sub_sep );
                if ( pos == std::string::npos )
                {
                    break;
                }
                sub = sub.substr( pos + 1 );
            }
        }
    }
#undef X

    inline std::vector< section_t > read_file( std::istream& file )
    {
        std::vector< section_t > sections;

        while ( !file.eof() )
        {
            std::string buffer;
            std::getline( file, buffer );
            if ( buffer.empty() )
            {
                continue;
            }

            // new section
            std::smatch match;
            const auto section_regex = std::regex( "section: " + string_identifier_regex_str );
            if ( std::regex_match( buffer, match, section_regex ) )
            {
                sections.emplace_back( match[ 1 ] );
                continue;
            }

            // entry in section
            sections.back().entries.emplace_back();
            try
            {
                read_entry( sections.back().entries.back(), buffer );
            }
            catch ( std::runtime_error& error )
            {
                std::cerr << error.what() << std::endl;
                sections.back().entries.pop_back();
            }
        }

        return sections;
    }
} // namespace sequencer::midi
