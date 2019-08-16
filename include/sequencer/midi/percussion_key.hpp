#pragma once

#include <cstdint>
#include <string_view>

namespace sequencer::midi
{
// clang-format off
#define PERCUSSION_KEYS \
    X(AcousticBassDrum    , 35) \
    X(BassDrum_1          , 36) \
    X(RimShot             , 37) \
    X(AcousticSnare       , 38) \
    X(HandClap            , 39) \
    X(ElectricSnare       , 40) \
    X(LowTom_A            , 41) \
    X(ClosedHiHat         , 42) \
    X(LowTom_B            , 43) \
    X(PedalHiHat          , 44) \
    X(MidTom_A            , 45) \
    X(OpenHiHat           , 46) \
    X(MidTom_B            , 47) \
    X(HighTom_A           , 48) \
    X(CrashCymbal_1       , 49) \
    X(HighTom_B           , 50) \
    X(RideCymbal_1        , 51) \
    X(ChineseCymbal       , 52) \
    X(RideBell            , 53) \
    X(Tambourine          , 54) \
    X(SplashCymbal        , 55) \
    X(Cowbell             , 56) \
    X(CrashCymbal_2       , 57) \
    X(Vibraslap           , 58) \
    X(RideCymbal          , 59) \
    X(HiBongo             , 60) \
    X(LowBongo            , 61) \
    X(MuteHiConga         , 62) \
    X(OpenHiConga         , 63) \
    X(LowConga            , 64) \
    X(HighTimbale         , 65) \
    X(LowTimbale          , 66) \
    X(HighAgogo           , 67) \
    X(LowAgogo            , 68) \
    X(Cabasa              , 69) \
    X(Maracas             , 70) \
    X(ShortWhistle        , 71) \
    X(LongWhistle         , 72) \
    X(ShortGuiro          , 73) \
    X(LongGuiro           , 74) \
    X(Claves              , 75) \
    X(HiWoodBlock         , 76) \
    X(LowWoodBlock        , 77) \
    X(MuteCuica           , 78) \
    X(OpenCuica           , 79) \
    X(MuteTriangle        , 80) \
    X(OpenTriangle        , 81)
    // clang-format on

#define X( name, key ) name = key,
    enum class percussion_key : std::uint8_t
    {
        PERCUSSION_KEYS
    };
#undef X

#define X( name, key )                                                                             \
    case percussion_key::name:                                                                     \
        return #name;
    std::string_view to_string( percussion_key key )
    {
        switch ( key )
        {
            PERCUSSION_KEYS
        }

        return "";
    }
#undef X
} // namespace sequencer::midi
