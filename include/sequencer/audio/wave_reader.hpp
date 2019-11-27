// Copied from https://github.com/adamstark/AudioFile

#pragma once

#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

namespace sequencer::audio::wave
{
    enum class endian_t
    {
        LittleEndian,
        BigEndian
    };

    inline std::int32_t four_bytes_to_int( const std::vector< uint8_t >& source,
                                           std::vector< uint8_t >::size_type index,
                                           endian_t endianness = endian_t::LittleEndian )
    {
        if ( endianness == endian_t::LittleEndian )
        {
            return ( source[ index + 3 ] << 24 ) | ( source[ index + 2 ] << 16 ) |
                   ( source[ index + 1 ] << 8 ) | source[ index ];
        }

        return ( source[ index ] << 24 ) | ( source[ index + 1 ] << 16 ) |
               ( source[ index + 2 ] << 8 ) | source[ index + 3 ];
    }

    inline std::int16_t two_bytes_to_int( const std::vector< uint8_t >& source,
                                          std::vector< uint8_t >::size_type index,
                                          endian_t endianness = endian_t::LittleEndian )
    {
        if ( endianness == endian_t::LittleEndian )
        {
            return std::int16_t( source[ index + 1 ] << 8 ) | source[ index ];
        }

        return std::int16_t( source[ index ] << 8 ) | source[ index + 1 ];
    }

    inline int audio_file_format( const std::vector< std::uint8_t >& data )
    {
        std::string header( data.begin(), data.begin() + 4 );

        if ( header == "RIFF" )
        {
            return 1;
        }
        if ( header == "FORM" )
        {
            return 2;
        }

        return 3;
    }

    inline std::vector< uint8_t >::difference_type index_of( const std::vector< uint8_t >& source,
                                                             const std::string& str )
    {
        const auto length = str.length();

        for ( decltype( source.size() - length ) i = 0; i < source.size() - length; i++ )
        {
            using diff_t = std::vector< uint8_t >::difference_type;
            std::string section( source.begin() + diff_t( i ),
                                 source.begin() + diff_t( i + length ) );

            if ( section == str )
            {
                return diff_t( i );
            }
        }

        return -1;
    }

    inline float byte_to_sample( uint8_t sample )
    {
        return float( sample - 128 ) / float( 128 );
    }

    inline float two_bytes_to_sample( int16_t sample )
    {
        return float( sample ) / 32768.f;
    }

    inline std::vector< std::vector< float > > read( const std::vector< std::uint8_t >& data )
    {
        const std::string header_chunk_id( data.begin(), data.begin() + 4 );
        const std::string format( data.begin() + 8, data.begin() + 12 );

        const auto index_of_data_chunk = index_of( data, "data" );
        const auto index_of_format_chunk = index_of( data, "fmt" );

        if ( index_of_data_chunk == -1 || index_of_format_chunk == -1 ||
             header_chunk_id != "RIFF" || format != "WAVE" )
        {
            std::cerr << "ERROR: this doesn't seem to be a valid .WAV file" << std::endl;
            return {};
        }

        const auto f = index_of_format_chunk;
        using size_t = std::vector< std::uint8_t >::size_type;
        const std::string format_chunk_id( data.begin() + f, data.begin() + f + 4 );
        const auto audio_format = two_bytes_to_int( data, size_t( f + 8 ) );
        const auto channel_count = two_bytes_to_int( data, size_t( f + 10 ) );
        const auto sample_rate = four_bytes_to_int( data, size_t( f + 12 ) );
        const auto bytes_per_second = four_bytes_to_int( data, size_t( f + 16 ) );
        const auto bytes_per_block = two_bytes_to_int( data, size_t( f + 20 ) );
        const auto bit_depth = two_bytes_to_int( data, size_t( f + 22 ) );

        const int bytes_per_sample = bit_depth / 8;

        // check that the audio format is PCM
        if ( audio_format != 1 )
        {
            std::cout << "ERROR: this is a compressed .WAV file and this library does not support "
                         "decoding them at present"
                      << std::endl;
            return {};
        }

        if ( channel_count < 1 || channel_count > 2 )
        {
            std::cout << "ERROR: this WAV file seems to be neither mono nor stereo (perhaps "
                         "multi-track, or corrupted?)"
                      << std::endl;
            return {};
        }

        if ( ( bytes_per_second != ( channel_count * sample_rate * bit_depth ) / 8 ) ||
             ( bytes_per_block != ( channel_count * bytes_per_sample ) ) )
        {
            std::cout << "ERROR: the header data in this WAV file seems to be inconsistent"
                      << std::endl;
            return {};
        }

        if ( bit_depth != 8 && bit_depth != 16 && bit_depth != 24 )
        {
            std::cout << "ERROR: this file has a bit depth that is not 8, 16 or 24 bits"
                      << std::endl;
            return {};
        }

        const auto d = index_of_data_chunk;
        const std::string data_chunk_id( data.begin() + d, data.begin() + d + 4 );
        const auto data_chunk_size =
            four_bytes_to_int( data, std::vector< std::uint8_t >::size_type( d + 4 ) );

        const auto num_samples = data_chunk_size / ( channel_count * bit_depth / 8 );
        const auto sample_start_index = index_of_data_chunk + 8;

        std::vector< std::vector< float > > samples;
        samples.resize( channel_count );

        for ( int i = 0; i < num_samples; i++ )
        {
            for ( int channel = 0; channel < channel_count; channel++ )
            {
                const auto sample_index =
                    sample_start_index + ( bytes_per_block * i ) + channel * bytes_per_sample;

                if ( bit_depth == 8 )
                {
                    const auto sample = byte_to_sample( data[ sample_index ] );
                    samples[ channel ].push_back( sample );
                }
                else if ( bit_depth == 16 )
                {
                    const auto sample_as_int = two_bytes_to_int( data, sample_index );
                    const auto sample = two_bytes_to_sample( sample_as_int );
                    samples[ channel ].push_back( sample );
                }
                else if ( bit_depth == 24 )
                {
                    auto sample_as_int = 0;
                    sample_as_int = ( data[ sample_index + 2 ] << 16 ) |
                                    ( data[ sample_index + 1 ] << 8 ) | data[ sample_index ];

                    if ( ( sample_as_int & 0x800000 ) >
                         0 ) //  if the 24th bit is set, this is a negative
                    {        //  number in 24-bit world
                        sample_as_int =
                            sample_as_int |
                            ~0xFFFFFF; // so make sure sign is extended to the 32 bit float
                    }

                    const auto sample = sample_as_int / 8388608.f;
                    samples[ channel ].push_back( sample );
                }
            }
        }

        return samples;
    }

    inline std::vector< std::vector< float > > read( const std::string& filename )
    {
        std::ifstream file( filename, std::ios::binary );

        if ( !file.good() )
        {
            std::cerr << "ERROR: File doesn't exist or otherwise can't load file: " << filename
                      << std::endl;
            return {};
        }

        file.unsetf( std::ios::skipws );
        std::istream_iterator< std::uint8_t > begin( file );
        std::istream_iterator< std::uint8_t > end;
        std::vector< std::uint8_t > data( begin, end );

        // get audio file format
        const auto format = audio_file_format( data );
        if ( format != 1 )
        {
            std::cerr << "wrong format: " << format << std::endl;
            return {};
        }

        return read( data );
    }
} // namespace sequencer::audio::wave
