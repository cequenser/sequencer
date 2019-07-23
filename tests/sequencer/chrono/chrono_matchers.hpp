#pragma once

#include <catch2/catch.hpp>

#include <chrono>
#include <sstream>

namespace sequencer::chrono
{

    template < typename RepT, typename PeriodT >
    class within_abs : public Catch::MatcherBase< std::chrono::duration< RepT, PeriodT > >
    {
    public:
        using duration = std::chrono::duration< RepT, PeriodT >;

        within_abs( duration expected, duration margin )
            : expected_( expected ), margin_( abs( margin ) )
        {
        }

        bool match( duration const& actual ) const override
        {
            return abs( expected_ - actual ) < margin_;
        }

        std::string describe() const override
        {
            std::ostringstream ss;
            ss << "is " << to_seconds( expected_ ) << "s +/- " << to_seconds( margin_ ) << "s";
            return ss.str();
        }

    private:
        constexpr static double to_seconds( duration duration )
        {
            using seconds = std::chrono::duration< double >;
            return seconds( duration ).count();
        }

        duration expected_;
        duration margin_;
    };
} // namespace sequencer::chrono
