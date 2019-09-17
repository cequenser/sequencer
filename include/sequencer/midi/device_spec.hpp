#pragma once

#include <cassert>
#include <cstdint>
#include <fstream>
#include <regex>
#include <string>

namespace sequencer::midi
{
    struct device_entry_t
    {
        std::string name;
        std::int16_t min{0};
        std::int16_t max{0};
        std::int16_t value{0};
        std::uint8_t cc_key{0};
        std::uint8_t cc_key_lsb{0};
        std::uint8_t decimals{0};
    };

    // clang-format off
    struct device_spec_cc_t
    {
        std::array<device_entry_t, 8> trig{};
        std::array<device_entry_t, 8> source{};
        std::array<device_entry_t, 8> filter{};
        std::array<device_entry_t, 8> amp{};
        std::array<device_entry_t, 8> lfo{};
        std::array<device_entry_t, 8> delay{};
        std::array<device_entry_t, 8> reverb{};
    };

    namespace reader
    {
    enum class device_spec_section
    {
        none,
        trig,
        source,
        filter,
        amp,
        lfo,
        delay,
        reverb
    };

    namespace
    {
        std::regex icase_regex(const std::string& str)
        {
            return std::regex(str, std::regex_constants::icase);
        }
    }

    inline device_spec_section get_section(std::string& line)
    {
        if (regex_match(line, icase_regex("trig")))
        {
            return device_spec_section::trig;
        }
        if (regex_match(line, icase_regex("source")))
        {
            return device_spec_section::source;
        }

        if (regex_match(line, icase_regex("filter")))
        {
            return device_spec_section::filter;
        }

        if (regex_match(line, icase_regex("amp")))
        {
            return device_spec_section::amp;
        }

        if (regex_match(line, icase_regex("lfo")))
        {
            return device_spec_section::lfo;
        }

        if (regex_match(line, icase_regex("delay")))
        {
            return device_spec_section::delay;
        }

        if (regex_match(line, icase_regex("reverb")))
        {
            return device_spec_section::reverb;
        }

        return device_spec_section::none;
    }

    inline std::uint8_t get_value(const std::string& line)
    {
        const auto sep_pos = line.find(':');
        assert(sep_pos != std::string::npos);
        return std::uint8_t(std::stoi(line.substr(sep_pos + 1)));
    }

    inline void read_entry(device_entry_t& entry, const std::string& line)
    {
        constexpr auto sep = ':';
        constexpr auto sub_sep = ',';

        auto start = 0ul;
        auto end = line.find(sep);
        entry.name = line.substr(start, end);
        start = end + 1;
        end = line.find(sub_sep, start);
        if(end != std::string::npos)
        {
            entry.cc_key = std::uint8_t(std::stoi(line.substr(start, end)));
            start = end + 1;
            end = line.find(sep, start);
            entry.cc_key_lsb = std::uint8_t(std::stoi(line.substr(start, end)));
        } else {
            end = line.find(sep, start);
            entry.cc_key = std::uint8_t(std::stoi(line.substr(start, end)));
        }
        start = end + 1;
        end = line.find(sep, start);
        entry.min = std::int16_t(std::stoi(line.substr(start, end)));
        start = end + 1;
        end = line.find(sep, start);
        entry.max = std::int16_t(std::stoi(line.substr(start, end)));
        start = end + 1;
        end = line.find(sep, start);
        entry.value = std::int16_t(std::stoi(line.substr(start, end)));
        if(end != std::string::npos)
        {
            start = end + 1;
            end = line.find(sep, start);
            entry.decimals = std::uint8_t(std::stoi(line.substr(start, end)));
        }
    }
    }

    inline device_spec_cc_t read_device_spec_cc(std::istream& stream)
    {
        using namespace reader;

        device_spec_cc_t device_spec;

        auto section = device_spec_section::none;
        auto counter = 0u;
        while(!stream.eof())
        {
            std::string line;
            std::getline(stream, line);
            if(line.empty())
            {
                continue;
            }

            const auto new_section = get_section(line);
            if(new_section != device_spec_section::none)
            {
                section = new_section;
                counter = 0;
                continue;
            }

            switch(section)
            {
            case device_spec_section::none:
                continue;
            case device_spec_section::trig:
                read_entry(device_spec.trig[counter++], line);
                continue;
            case device_spec_section::source:
                read_entry(device_spec.source[counter++], line);
                continue;
            case device_spec_section::filter:
                read_entry(device_spec.filter[counter++], line);
                continue;
            case device_spec_section::amp:
                read_entry(device_spec.amp[counter++], line);
                continue;
            case device_spec_section::lfo:
                read_entry(device_spec.lfo[counter++], line);
                continue;
            case device_spec_section::delay:
//                read_entry(device_spec.delay[counter++], line);
                continue;
            case device_spec_section::reverb:
//                read_entry(device_spec.reverb[counter++], line);
                continue;
            }
        }

        return device_spec;
    }
    // clang-format on
} // namespace sequencer::midi
