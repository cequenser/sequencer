#include <sequencer/audio/sample.hpp>

#include <catch2/catch.hpp>

#include <cstdint>

SCENARIO( "reset frame index", "[sample]" )
{
    using namespace sequencer::audio;

    GIVEN( "an empty audio sample with max 10 frames and 2 channels" )
    {
        const auto max_frames = 10u;
        const auto n_channels = 2u;
        auto sample = sample_t{max_frames, n_channels};

        THEN( "there are 10 frames left" )
        {
            CHECK( sample.frames_left() == 10u );
        }

        WHEN( "5 bytes are read" )
        {
            std::vector< float > data{4};
            sample.read( data.data(), 2 );

            THEN( "there are 8 frames left" )
            {
                CHECK( sample.frames_left() == 8u );

                WHEN( "frame index is reset" )
                {
                    sample.reset_frame_index();

                    THEN( "there are 10 frames left" )
                    {
                        CHECK( sample.frames_left() == 10u );
                    }
                }
            }
        }
    }
}

SCENARIO( "write and read", "[sample]" )
{
    using namespace sequencer::audio;

    GIVEN( "an empty audio sample with max 10 frames and 1 channel" )
    {
        const auto max_frames = 10u;
        const auto n_channels = 1u;
        auto sample = sample_t{max_frames, n_channels};

        WHEN( "1.0f 2.0f 3.0f is read" )
        {
            std::vector< float > v = {1, 2, 3};
            sample.read( v.data(), 3 );

            AND_WHEN( "frame index is reset" )
            {
                sample.reset_frame_index();

                THEN( "write with 1 frames per buffer returns 1.0f" )
                {
                    std::vector< float > w( 10, -1.0f );
                    sample.write( w.data(), 1 );

                    CHECK( w[ 0 ] == 1.0f );
                    CHECK( w[ 1 ] == -1.0f );
                }

                THEN( "write with 4 frames per buffer returns 1.0f 2.0f 3.0f 0.0f" )
                {
                    std::vector< float > w( 10, -1.0f );
                    sample.write( w.data(), 4 );

                    CHECK( w[ 0 ] == 1.0f );
                    CHECK( w[ 1 ] == 2.0f );
                    CHECK( w[ 2 ] == 3.0f );
                    CHECK( w[ 3 ] == 0.0f );
                }
            }

            THEN( "write with 1 frames per buffer returns 0.0f" )
            {
                std::vector< float > w( 10, -1.0f );
                sample.write( w.data(), 1 );

                CHECK( w[ 0 ] == 0.0f );
            }

            AND_WHEN( "4.0f 5.0f 6.0f 7.0f 8.0f 9.0f 10.0f 11.0f is read" )
            {
                std::vector< float > v = {4, 5, 6, 7, 8, 9, 10, 11};
                sample.read( v.data(), 8 );

                AND_WHEN( "frame index is reset" )
                {
                    sample.reset_frame_index();

                    THEN( "write returns 1.0f 2.0f ... 10.0f" )
                    {
                        std::vector< float > w( 11, -1.0f );
                        sample.write( w.data(), 11 );

                        CHECK( w[ 0 ] == 1.0f );
                        CHECK( w[ 1 ] == 2.0f );
                        CHECK( w[ 9 ] == 10.0f );
                        CHECK( w[ 10 ] == -1.0f );
                    }
                }
            }

            AND_WHEN( "nullptr is read with 3 frames per buffer" )
            {
                std::vector< float > v( 3, 0.0f );
                sample.read( v.data(), 3 );

                AND_WHEN( "4.0f is read" )
                {
                    std::vector< float > v( 1, 4.0f );
                    sample.read( v.data(), 1 );

                    AND_WHEN( "frame index is reset" )
                    {
                        sample.reset_frame_index();

                        THEN( "write return 1.0f 2.0f 3.0f 0.0f 0.0f 0.0f 4.0f" )
                        {
                            std::vector< float > w( 7, -1.0f );
                            sample.write( w.data(), 7 );

                            CHECK( w[ 0 ] == 1.0f );
                            CHECK( w[ 2 ] == 3.0f );
                            CHECK( w[ 3 ] == 0.0f );
                            CHECK( w[ 5 ] == 0.0f );
                            CHECK( w[ 6 ] == 4.0f );
                        }
                    }
                }
            }
        }
    }
}
