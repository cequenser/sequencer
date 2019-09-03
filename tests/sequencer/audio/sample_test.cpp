#include <sequencer/audio/sample.hpp>

#include <catch2/catch.hpp>

#include <cstdint>

SCENARIO( "sample number of frames", "[sample]" )
{
    using namespace sequencer::audio;

    GIVEN( "an empty audio stereo sample with 0 frames" )
    {
        auto sample = sample_t{0, sequencer::audio::mode_t::stereo};

        THEN( "number of frames is 0" )
        {
            CHECK( sample.number_of_frames() == 0 );
        }

        WHEN( "trim is called" )
        {
            sample.trim();

            THEN( "number of frames is 0" )
            {
                CHECK( sample.number_of_frames() == 0 );
            }
        }
    }

    GIVEN( "an empty audio mono sample with 4 frames" )
    {
        auto sample = sample_t( 4, sequencer::audio::mode_t::mono );

        THEN( "number of frames is 4" )
        {
            CHECK( sample.number_of_frames() == 4 );
        }

        WHEN( "trim is called" )
        {
            sample.trim();

            THEN( "number of frames is 0" )
            {
                CHECK( sample.number_of_frames() == 0 );
            }
        }

        WHEN( "third frame is changed to non-zero" )
        {
            sample.frames[ 2 ] = 1;

            AND_WHEN( "trim is called" )
            {
                sample.trim();

                THEN( "number of frames is 3" )
                {
                    CHECK( sample.number_of_frames() == 3 );
                }
            }

            AND_WHEN( "clear is called" )
            {
                sample.clear();

                THEN( "third frame is 0" )
                {
                    CHECK( sample.frames[ 2 ] == 0.0f );
                }
            }
        }
    }
}

SCENARIO( "reset frame index", "[sample]" )
{
    using namespace sequencer::audio;

    GIVEN( "an empty audio stereo sample with max 2 frames" )
    {
        const auto max_frames = 2u;
        auto sample = read_write_lockable< sample_t >{max_frames};
        auto writer = sample_writer_t{sample};

        THEN( "there are frames left" )
        {
            CHECK( writer.has_frames_left() );
        }

        WHEN( "trim is called" )
        {
            sample.trim();

            THEN( "no frames are left" )
            {
                CHECK_FALSE( writer.has_frames_left() );
            }
        }

        WHEN( "16 bytes are written" )
        {
            std::vector< float > data = {3.0f, 4.0f, 5.0f, 6.0f};
            writer.write( data.data(), 2 );

            THEN( "there are no frames left" )
            {
                CHECK_FALSE( writer.has_frames_left() );

                WHEN( "frame index is reset" )
                {
                    writer.reset_frame_index();

                    THEN( "there are frames left" )
                    {
                        CHECK( writer.has_frames_left() );
                    }

                    AND_WHEN( "8 bytes are written" )
                    {
                        std::vector< float > data = {1.0f, 2.0f};
                        writer.write( data.data(), 1 );

                        THEN( "read returns 16 bytes" )
                        {
                            std::vector< float > received_data( 4, 0 );
                            auto reader = sample_reader_t{sample};
                            reader.read( received_data.data(), 2 );

                            CHECK( received_data[ 0 ] == 1.0f );
                            CHECK( received_data[ 1 ] == 2.0f );
                            CHECK( received_data[ 2 ] == 5.0f );
                            CHECK( received_data[ 3 ] == 6.0f );
                        }
                    }
                }
            }
        }

        WHEN( "8 bytes are written" )
        {
            sample.clear();
            std::vector< float > data = {22.0f, 33.0f};
            writer.write( data.data(), 2 );

            AND_WHEN( "trim is called" )
            {
                sample.trim();

                THEN( "read returns 8 bytes" )
                {
                    std::vector< float > received_data( 4, 0 );
                    auto reader = sample_reader_t{sample};
                    reader.read( received_data.data(), 2 );

                    CHECK( received_data[ 0 ] == 22.0f );
                    CHECK( received_data[ 1 ] == 33.0f );
                    CHECK( received_data[ 2 ] == 0.0f );
                    CHECK( received_data[ 3 ] == 0.0f );
                }
            }
        }
    }
}

SCENARIO( "read-write lockable sample", "[sample]" )
{
    using namespace sequencer::audio;

    GIVEN( "an empty mono audio sample with max 10 frames" )
    {
        const auto max_frames = 10u;
        auto sample = read_write_lockable< sample_t >{max_frames, sequencer::audio::mode_t::mono};

        WHEN( "writing is blocked" )
        {
            sample.block_writing( true );

            THEN( "writing is blocked" )
            {
                CHECK( sample.writing_is_blocked() );
            }
        }

        WHEN( "reading is blocked" )
        {
            sample.block_reading( true );

            THEN( "reading is blocked" )
            {
                CHECK( sample.reading_is_blocked() );
            }
        }
    }
}

SCENARIO( "block read or write", "[sample]" )
{
    using namespace sequencer::audio;

    GIVEN( "an empty mono audio sample with max 10 frames" )
    {
        const auto max_frames = 10u;
        auto sample = read_write_lockable< sample_t >{max_frames, sequencer::audio::mode_t::mono};

        WHEN( "writing is blocked" )
        {
            sample.block_writing( true );

            AND_WHEN( "1.0f 2.0f 3.0f is written" )
            {
                std::vector< float > v = {1, 2, 3};
                auto writer = sample_writer_t{sample};
                writer.write( v.data(), 3 );

                THEN( "reading with 1 frames per buffer returns 0" )
                {
                    std::vector< float > w( 10, -1.0f );
                    auto reader = sample_reader_t{sample};
                    reader.read( w.data(), 1 );

                    CHECK( w[ 0 ] == 0.0f );
                    CHECK( w[ 1 ] == -1.0f );
                }

                AND_WHEN( "writing is unblocked" )
                {
                    sample.block_writing( false );

                    AND_WHEN( "reading is blocked" )
                    {
                        sample.block_reading( true );

                        THEN( "reading with 1 frames per buffer returns nothing" )
                        {
                            std::vector< float > w( 10, -1.0f );
                            auto reader = sample_reader_t{sample};
                            reader.read( w.data(), 1 );

                            CHECK( w[ 0 ] == -1.0f );
                            CHECK( w[ 1 ] == -1.0f );
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
        auto sample = read_write_lockable< sample_t >{max_frames, sequencer::audio::mode_t::mono};

        WHEN( "1.0f 2.0f 3.0f is written" )
        {
            std::vector< float > v = {1, 2, 3};
            auto writer = sample_writer_t{sample};
            writer.write( v.data(), 3 );

            THEN( "read with 1 frames per buffer returns 1.0f" )
            {
                std::vector< float > w( 10, -1.0f );
                auto reader = sample_reader_t{sample};
                reader.read( w.data(), 1 );

                CHECK( w[ 0 ] == 1.0f );
                CHECK( w[ 1 ] == -1.0f );
            }

            THEN( "read with 4 frames per buffer returns 1.0f 2.0f 3.0f 0.0f" )
            {
                std::vector< float > w( 10, -1.0f );
                auto reader = sample_reader_t{sample};
                reader.read( w.data(), 4 );

                CHECK( w[ 0 ] == 1.0f );
                CHECK( w[ 1 ] == 2.0f );
                CHECK( w[ 2 ] == 3.0f );
                CHECK( w[ 3 ] == 0.0f );
            }

            AND_WHEN( "4.0f 5.0f 6.0f 7.0f 8.0f 9.0f 10.0f 11.0f is written" )
            {
                std::vector< float > v = {4, 5, 6, 7, 8, 9, 10, 11};
                writer.write( v.data(), 8 );

                THEN( "read returns 1.0f 2.0f ... 10.0f" )
                {
                    std::vector< float > w( 11, -1.0f );
                    auto reader = sample_reader_t{sample};
                    reader.read( w.data(), 11 );

                    CHECK( w[ 0 ] == 1.0f );
                    CHECK( w[ 1 ] == 2.0f );
                    CHECK( w[ 9 ] == 10.0f );
                    CHECK( w[ 10 ] == -1.0f );
                }
            }

            AND_WHEN( "nullptr is written with 3 frames per buffer" )
            {
                std::vector< float > v( 3, 0.0f );
                writer.write( v.data(), 3 );

                AND_WHEN( "4.0f is written" )
                {
                    std::vector< float > v( 1, 4.0f );
                    writer.write( v.data(), 1 );

                    THEN( "read return 1.0f 2.0f 3.0f 0.0f 0.0f 0.0f 4.0f" )
                    {
                        std::vector< float > w( 7, -1.0f );
                        auto reader = sample_reader_t{sample};
                        reader.read( w.data(), 7 );

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
