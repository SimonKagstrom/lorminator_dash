#include <catch.hpp>
#include <trompeloeil.hpp>

#include <input.hh>
#include <io.hh>
#include <game.hh>
#include <entity.hh>

#include "mock-input.hh"
#include "mock-io.hh"

using trompeloeil::_;

SCENARIO("the game can be played")
{
    auto game = IGame::create();

    WHEN("an invalid level is loaded")
    {
        auto rv = game->setLevel("9 2 "
            "........."
            ".........");

        THEN("the loader returns false")
        {
            REQUIRE(rv == false);
        }
    }

    WHEN("a valid level is loaded")
    {
        g_mockInput = std::make_shared<MockInput>();
        g_mockIo = std::make_shared<MockIo>();

        auto rv = game->setLevel("9 9 "
            "...o....d" // b1 Fall
            "... ....d"
            "... ....d"
            "... .. .."
            ".#. t.o  " // b2 Fall
            ".##dp.o ." // The player takes the diamond and then gets hit by the boulder
            "........."
            "........."
            "........t");

        THEN("the loader returns true")
        {
            REQUIRE(rv == true);
        }

        auto store =  IEntityStore::getInstance();
        auto b1 = store->getEntityByPoint({6, 5});
        auto b2 = store->getEntityByPoint({6, 4});
        REQUIRE(b2);

        AND_THEN("the game can be played")
        {
            // The player walks to the left to collect the diamond and then hit the wall
            REQUIRE_CALL(*g_mockInput, getInput())
                .TIMES(AT_LEAST(1))
                .RETURN(InputTypes::LEFT);

            REQUIRE_CALL(*g_mockIo, display(_,_,_))
                .TIMES(AT_LEAST(1));
            REQUIRE_CALL(*g_mockIo, delay(_))
                .TIMES(AT_LEAST(1));

            rv = game->play();

            AND_WHEN("the player is removed, the play method returns")
            {
                REQUIRE(b1->getPosition() == (point){6,5});
                REQUIRE(b2->getPosition() == (point){7,5});
                auto fb = store->getEntityByPoint({3,5}); // Now a fireball
                REQUIRE(fb);
                REQUIRE(fb->getType() == EntityType::FIREBALL);
                REQUIRE(rv == false); // Didn't finish
            }
        }
        g_mockInput = nullptr;
        g_mockIo = nullptr;
    }

}
