#include <sequencer/audio/fft.hpp>

#include <catch2/catch.hpp>

#include <algorithm>
#include <cmath>
#include <limits>

constexpr auto signal_size = 32*8;

SCENARIO("sine waves of 50 and 80 Hz", "fft")
{
    using namespace sequencer::audio;

    GIVEN("overlapping sine waves of 50 and 80 Hz")
    {
        double counter = 0;
        auto signal = std::vector<double>(signal_size);
        std::generate(begin(signal), end(signal), [&counter]
        {
            const auto t = counter++/signal_size;
            return 0.5*std::sin(80*2*M_PI*t) + std::sin(50*2*M_PI*t);
        });

        WHEN("fft is computed")
        {
            const auto result = fft(signal);

            THEN("frequencies 50 and 80 have values of signal_size/2 resp. 0.5*signal_size/2")
            {
                CHECK(std::abs(result[50]) == Approx(signal_size/2));
                CHECK(std::abs(result[80]) == Approx(0.5*signal_size/2));
            }

            THEN("all other frequencies are close to zero")
            {
                for(decltype(result)::size_type i=0; i<result.size(); ++i)
                {
                    if(i == 50 || i == 80)
                        continue;
                    CHECK(std::abs(result[i]) < 1e4*std::numeric_limits<double>::epsilon());
                }
            }
        }

        WHEN("fft_abs_scaled is computed")
        {
            const auto result = fft_abs_scaled(signal);

            THEN("frequencies 50 and 80 have values of 1 resp. 0.5")
            {
                CHECK(result[50] == Approx(1));
                CHECK(result[80] == Approx(0.5));
            }

            THEN("all other frequencies are close to zero")
            {
                for(decltype(result)::size_type i=0; i<result.size(); ++i)
                {
                    if(i == 50 || i == 80)
                        continue;
                    CHECK(result[i] < 1e4*std::numeric_limits<double>::epsilon());
                }
            }
        }
    }
}

SCENARIO("complex sine waves of 50 and 80 Hz", "fft")
{
    using namespace sequencer::audio;

    GIVEN("overlapping sine waves of 50 and 80 Hz")
    {
        double counter = 0;
        auto signal = std::vector<std::complex<double>>(signal_size);
        std::generate(begin(signal), end(signal), [&counter]
        {
            const auto t = counter++/signal_size;
            return std::complex{0.5*std::sin(80*2*M_PI*t) + std::sin(50*2*M_PI*t), 0.0};
        });

        WHEN("radix2 fft is computed")
        {
            const auto result = radix2(signal);

            THEN("frequencies 50 and 80 have values of signal_size/2 resp. 0.5*signal_size/2")
            {
                CHECK(std::abs(result[50]) == Approx(signal_size/2));
                CHECK(std::abs(result[80]) == Approx(0.5*signal_size/2));
            }
            THEN("frequencies (signal_size-50) and (signal_size-80) have values of signal_size/2 resp. 0.5*signal_size/2")
            {
                CHECK(std::abs(result[signal_size-50]) == Approx(signal_size/2));
                CHECK(std::abs(result[signal_size-80]) == Approx(0.5*signal_size/2));
            }

            THEN("all other frequencies are close to zero")
            {
                for(decltype(result)::size_type i=0; i<result.size(); ++i)
                {
                    if(i == 50 || i == 80 || i == signal_size-50 || i == signal_size-80)
                        continue;
                    CHECK(std::abs(result[i]) < 1e4*std::numeric_limits<double>::epsilon());
                }
            }

            AND_WHEN("inverse radix2 fft is computed")
            {
                const auto signal2 = inverse_radix2(result);

                THEN("computed signal coincides with input signal")
                {
                    CHECK(std::equal(begin(signal), end(signal), begin(signal2), end(signal2),
                                     [](auto lhs, auto rhs){ return std::abs(lhs - rhs) < 1e4*std::numeric_limits<double>::epsilon(); }));
                }
            }
        }
    }
}
