#pragma once

#include <catch2/catch.hpp>

#include <chrono>
#include <sstream>

namespace sequencer::chrono
{

    inline std::string to_string( const std::chrono::duration< double >& value )
    {
        std::ostringstream ss;
        ss << value.count() << "s";
        return ss.str();
    };

    inline std::string to_string( const std::chrono::seconds& value )
    {
        std::ostringstream ss;
        ss << value.count() << "s";
        return ss.str();
    };

    inline std::string to_string( const std::chrono::milliseconds& value )
    {
        std::ostringstream ss;
        ss << value.count() << "ms";
        return ss.str();
    };

    inline std::string to_string( const std::chrono::microseconds& value )
    {
        std::ostringstream ss;
        ss << value.count() << "us";
        return ss.str();
    };

    inline std::string to_string( const std::chrono::nanoseconds& value )
    {
        std::ostringstream ss;
        ss << value.count() << "ns";
        return ss.str();
    };

} // namespace sequencer::chrono

namespace Catch
{

    template <>
    struct StringMaker< std::chrono::seconds >
    {
        static std::string convert( const std::chrono::seconds& value )
        {
            return sequencer::chrono::to_string( value );
        }
    };

    template <>
    struct StringMaker< std::chrono::milliseconds >
    {
        static std::string convert( const std::chrono::milliseconds& value )
        {
            return sequencer::chrono::to_string( value );
        }
    };

    template <>
    struct StringMaker< std::chrono::microseconds >
    {
        static std::string convert( const std::chrono::microseconds& value )
        {
            return sequencer::chrono::to_string( value );
        }
    };

    template <>
    struct StringMaker< std::chrono::nanoseconds >
    {
        static std::string convert( const std::chrono::nanoseconds& value )
        {
            return sequencer::chrono::to_string( value );
        }
    };

    template <>
    struct StringMaker< std::chrono::duration< double > >
    {
        static std::string convert( const std::chrono::duration< double >& value )
        {
            return sequencer::chrono::to_string( value );
        }
    };

} // namespace Catch
