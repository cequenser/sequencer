#pragma once

#include <sequencer/copyable_atomic.hpp>

#include <cassert>
#include <limits>

namespace sequencer::audio
{
    class envelope_t
    {
    public:
        double operator()( double t ) const noexcept
        {
            assert( t >= 0 );
            if ( t < attack() )
            {
                return t / attack();
            }
            const auto decay_start = attack() + sustain();
            if ( t < decay_start )
            {
                return 1;
            }
            if ( t < decay_start + decay() )
            {
                return 1 - ( t - decay_start ) / decay();
            }
            return 0;
        }

        void set_attack( double attack ) noexcept
        {
            attack_ = attack;
        }

        double attack() const noexcept
        {
            return attack_;
        }

        void set_sustain( double sustain ) noexcept
        {
            sustain_ = sustain;
        }

        double sustain() const noexcept
        {
            return sustain_;
        }

        void set_decay( double decay ) noexcept
        {
            decay_ = decay;
        }

        double decay() const noexcept
        {
            return decay_;
        }

    private:
        copyable_atomic< double > attack_{0};
        copyable_atomic< double > sustain_{std::numeric_limits< double >::max()};
        copyable_atomic< double > decay_{0};
    };
} // namespace sequencer::audio
