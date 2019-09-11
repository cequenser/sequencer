#include <sequencer/backend/digitakt.hpp>

#include <catch2/catch.hpp>

SCENARIO( "set step in play mode", "[digitakt]" )
{
    using namespace sequencer;
    using namespace sequencer::backend;

    GIVEN( "a digitakt backend" )
    {
        auto backend = digitakt{};
        REQUIRE( backend.mode() == digitakt_mode::play );

        WHEN( "set_step(2, true) is called" )
        {
            backend.set_step( 2, true );

            THEN( "the 3rd step of the current track gets a note" )
            {
                CHECK( backend.current_track()[ 2 ] != midi::no_note() );
            }

            AND_WHEN( "set_step(2, false) is called" )
            {
                backend.set_step( 2, false );

                THEN( "the 3rd step of the current track is set to no_note" )
                {
                    CHECK( backend.current_track()[ 2 ] == midi::no_note() );
                }
            }
        }
    }
}

SCENARIO( "set step in mute mode", "[digitakt]" )
{
    using namespace sequencer;
    using namespace sequencer::backend;

    GIVEN( "a digitakt backend" )
    {
        auto backend = digitakt{};
        backend.set_mode( digitakt_mode::track_select );
        backend.set_step( 2 );

        WHEN( "mode is set to mute" )
        {
            backend.set_mode( digitakt_mode::mute );
            CHECK( backend.mode() == digitakt_mode::mute );

            AND_WHEN( "mode is set to mute again" )
            {
                backend.set_mode( digitakt_mode::mute );

                THEN( "mode is back to play" )
                CHECK( backend.mode() == digitakt_mode::play );
            }

            AND_WHEN( "set_step(2, true) is called" )
            {
                backend.set_step( 2, true );

                THEN( "the 3rd step of the current track has no note" )
                {
                    CHECK( backend.current_track()[ 2 ] == midi::no_note() );
                }

                THEN( "the current track is muted" )
                {
                    CHECK( backend.current_track().is_muted() );
                }

                AND_WHEN( "set_step(2, false) is called" )
                {
                    backend.set_step( 2, false );

                    THEN( "the current track is unmuted" )
                    {
                        CHECK_FALSE( backend.current_track().is_muted() );
                    }
                }
            }
        }
    }
}

SCENARIO( "track select", "[digitakt]" )
{
    using namespace sequencer;
    using namespace sequencer::backend;

    GIVEN( "a digitakt backend" )
    {
        auto backend = digitakt{};

        WHEN( "set_step(2, true) is called" )
        {
            backend.set_step( 2, true );

            AND_WHEN( "mode is set to track_select" )
            {
                backend.set_mode( digitakt_mode::track_select );
                CHECK( backend.mode() == digitakt_mode::track_select );

                AND_WHEN( "set_step(2) is called" )
                {
                    backend.set_step( 2 );

                    THEN( "the 3rd step of the current track is set to no_note" )
                    {
                        CHECK( backend.current_track()[ 2 ] == midi::no_note() );
                    }

                    THEN( "mode is play" )
                    {
                        CHECK( backend.mode() == digitakt_mode::play );
                    }

                    AND_WHEN( "mode is set to track_select" )
                    {
                        backend.set_mode( digitakt_mode::track_select );
                        CHECK( backend.mode() == digitakt_mode::track_select );

                        AND_WHEN( "set_step(0) is called" )
                        {
                            backend.set_step( 0 );

                            THEN( "the 3rd step of the current track has a note" )
                            {
                                CHECK( backend.current_track()[ 2 ] != midi::no_note() );
                            }
                        }
                    }
                }
            }
        }
    }
}

SCENARIO( "pattern select", "[digitakt]" )
{
    using namespace sequencer;
    using namespace sequencer::backend;

    GIVEN( "a digitakt backend" )
    {
        auto backend = digitakt{};

        WHEN( "set_step(2, true) is called" )
        {
            backend.set_step( 2, true );

            AND_WHEN( "mode is set to pattern_select" )
            {
                backend.set_mode( digitakt_mode::pattern_select );
                CHECK( backend.mode() == digitakt_mode::pattern_select );

                AND_WHEN( "set_step(2) is called" )
                {
                    backend.set_step( 2 );

                    THEN( "the 3rd step of the current track is set to no_note" )
                    {
                        CHECK( backend.current_track()[ 2 ] == midi::no_note() );
                    }

                    THEN( "mode is play" )
                    {
                        CHECK( backend.mode() == digitakt_mode::play );
                    }

                    AND_WHEN( "mode is set to pattern_select" )
                    {
                        backend.set_mode( digitakt_mode::pattern_select );
                        CHECK( backend.mode() == digitakt_mode::pattern_select );

                        AND_WHEN( "set_step(0) is called" )
                        {
                            backend.set_step( 0 );

                            THEN( "the 3rd step of the current track has a note" )
                            {
                                CHECK( backend.current_track()[ 2 ] != midi::no_note() );
                            }
                        }
                    }
                }
            }
        }
    }
}

SCENARIO( "bank select", "[digitakt]" )
{
    using namespace sequencer;
    using namespace sequencer::backend;

    GIVEN( "a digitakt backend" )
    {
        auto backend = digitakt{};
        backend.set_mode( digitakt_mode::pattern_select );
        backend.set_step( 2 );
        backend.set_mode( digitakt_mode::track_select );
        backend.set_step( 1 );

        WHEN( "set_step(2, true) is called" )
        {
            backend.set_step( 2, true );

            AND_WHEN( "mode is set to bank_select" )
            {
                backend.set_mode( digitakt_mode::bank_select );
                CHECK( backend.mode() == digitakt_mode::bank_select );

                AND_WHEN( "set_step(3) is called" )
                {
                    backend.set_step( 3 );

                    THEN( "mode is pattern_select" )
                    {
                        CHECK( backend.mode() == digitakt_mode::pattern_select );
                    }

                    AND_WHEN( "set_step(2) is called" )
                    {
                        backend.set_step( 2 );

                        THEN( "the 3rd step of the current track is set to no_note" )
                        {
                            CHECK( backend.current_track()[ 2 ] == midi::no_note() );
                        }

                        THEN( "mode is play" )
                        {
                            CHECK( backend.mode() == digitakt_mode::play );
                        }

                        AND_WHEN( "mode is set to bank_select" )
                        {
                            backend.set_mode( digitakt_mode::bank_select );
                            CHECK( backend.mode() == digitakt_mode::bank_select );

                            AND_WHEN( "set_step(0) is called" )
                            {
                                backend.set_step( 0 );

                                THEN( "mode is pattern_select" )
                                {
                                    CHECK( backend.mode() == digitakt_mode::pattern_select );
                                }

                                AND_WHEN( "set_step(2) is called" )
                                {
                                    backend.set_step( 2 );

                                    THEN( "the 3rd step of the current track has a note" )
                                    {
                                        CHECK( backend.current_track()[ 2 ] != midi::no_note() );
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
