#pragma once

#include <cstdint>

namespace sequencer::midi
{
    enum class percussion_key : std::uint8_t
    {
        // clang-format off
        AcousticBassDrum    = 35,
        BassDrum_1          = 36,
        RimShot             = 37,
        AcousticSnare       = 38,
        HandClap            = 39,
        ElectricSnare       = 40,
        LowTom_A            = 41,
        ClosedHiHat         = 42,
        LowTom_B            = 43,
        PedalHiHat          = 44,
        MidTom_A            = 45,
        OpenHiHat           = 46,
        MidTom_B            = 47,
        HighTom_A           = 48,
        CrashCymbal_1       = 49,
        HighTom_B           = 50,
        RideCymbal_1        = 51,
        ChineseCymbal       = 52,
        RideBell            = 53,
        Tambourine          = 54,
        SplashCymbal        = 55,
        Cowbell             = 56,
        CrashCymbal_2       = 57,
        Vibraslap           = 58,
        RideCymbal          = 59,
        HiBongo             = 60,
        LowBongo            = 61,
        MuteHiConga         = 62,
        OpenHiConga         = 63,
        LowConga            = 64,
        HighTimbale         = 65,
        LowTimbale          = 66,
        HighAgogo           = 67,
        LowAgogo            = 68,
        Cabasa              = 69,
        Maracas             = 70,
        ShortWhistle        = 71,
        LongWhistle         = 72,
        ShortGuiro          = 73,
        LongGuiro           = 74,
        Claves              = 75,
        HiWoodBlock         = 76,
        LowWoodBlock        = 77,
        MuteCuica           = 78,
        OpenCuica           = 79,
        MuteTriangle        = 80,
        OpenTriangle        = 81
        //clang-format on
    };
}
