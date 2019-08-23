#include <sequencer/audio/sample.hpp>

#include <catch2/catch.hpp>

#include <cstdint>

SCENARIO( "reset frame index", "[sample]" )
{
    using namespace sequencer::audio;

    GIVEN( "an empty audio stereo sample with max 2 frames" )
    {
        const auto max_frames = 2u;
        auto sample = sample_t{max_frames};

        THEN( "there are frames left" )
        {
            CHECK( sample.has_frames_left() );
        }

        WHEN( "trim is called" )
        {
            sample.trim();

            THEN( "no frames are left" )
            {
                CHECK_FALSE( sample.has_frames_left() );
            }
        }

        WHEN( "16 bytes are read" )
        {
            std::vector< float > data = {3.0f, 4.0f, 5.0f, 6.0f};
            sample.read( data.data(), 2 );

            THEN( "there are no frames left" )
            {
                CHECK_FALSE( sample.has_frames_left() );

                WHEN( "frame index is reset" )
                {
                    sample.reset_frame_index();

                    THEN( "there are frames left" )
                    {
                        CHECK( sample.has_frames_left() );
                    }

                    AND_WHEN( "8 bytes are read" )
                    {
                        std::vector< float > data = {1.0f, 2.0f};
                        sample.read( data.data(), 1 );

                        AND_WHEN( "frame index is reset" )
                        {
                            sample.reset_frame_index();

                            THEN( "write returns 16 bytes" )
                            {
                                std::vector< float > received_data( 4, 0 );
                                sample.write( received_data.data(), 2 );

                                CHECK( received_data[ 0 ] == 1.0f );
                                CHECK( received_data[ 1 ] == 2.0f );
                                CHECK( received_data[ 2 ] == 5.0f );
                                CHECK( received_data[ 3 ] == 6.0f );
                            }
                        }

                        AND_WHEN( "trim is called" )
                        {
                            sample.trim();

                            AND_WHEN( "frame index is reset" )
                            {
                                sample.reset_frame_index();

                                THEN( "write returns 8 bytes" )
                                {
                                    std::vector< float > received_data( 4, 0 );
                                    sample.write( received_data.data(), 2 );

                                    CHECK( received_data[ 0 ] == 1.0f );
                                    CHECK( received_data[ 1 ] == 2.0f );
                                    CHECK( received_data[ 2 ] == 0.0f );
                                    CHECK( received_data[ 3 ] == 0.0f );
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

SCENARIO( "write and read", "[sample]" )
{
    using namespace sequencer::audio;

    GIVEN( "an empty mono audio sample with max 10 frames" )
    {
        const auto max_frames = 10u;
        auto sample = sample_t{max_frames, sequencer::audio::mode_t::mono};

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
