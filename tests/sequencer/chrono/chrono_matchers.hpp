#pragma once

#include "chrono_to_string.hpp"

template < typename RepT, typename PeriodT >
class WithinDuration : public Catch::MatcherBase< std::chrono::duration< RepT, PeriodT > >
{
    constexpr static double to_seconds( std::chrono::duration< RepT, PeriodT > duration )
    {
        using seconds = std::chrono::duration< double >;
        return seconds( duration ).count();
    }

public:
    using duration = std::chrono::duration< RepT, PeriodT >;

    WithinDuration( duration expected, duration margin ) : expected_( expected ), margin_( margin )
    {
    }

    bool match( duration const& actual ) const override
    {
        const auto difference = to_seconds( std::chrono::abs( expected_ - actual ) );
        return difference < to_seconds( margin_ );
    }

    virtual std::string describe() const override
    {
        std::ostringstream ss;
        ss << "is " << to_seconds( expected_ ) << "s with a margin of " << to_seconds( margin_ )
           << "s";
        return ss.str();
    }

private:
    duration expected_;
    duration margin_;
};
