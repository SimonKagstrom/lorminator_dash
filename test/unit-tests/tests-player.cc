#include <catch.hpp>
#include <trompeloeil.hpp>

#include <input.hh>
#include <level.hh>
#include <entity.hh>
#include <behavior.hh>
#include <entity-properties.hh>

#include "mock-input.hh"

SCENARIO("The player takes diamonds")
{
    g_mockInput = std::make_shared<MockInput>();

    auto store = IEntityStore::getInstance();
    auto propStore = IEntityProperties::getInstance();

    std::shared_ptr<ILevel> lvl = ILevel::fromString("9 9 "
                                    "........."
                                    "........."
                                    "........."
                                    "........."
                                    "........."
                                    "...dpd..." // 4, 5
                                    "........."
                                    "........."
                                    ".........");
    REQUIRE(lvl);

    auto player = store->getEntityByPoint({4,5});
    auto d1 = store->getEntityByPoint({3,5});
    auto d2 = store->getEntityByPoint({5,5});
    REQUIRE(player);
    REQUIRE(d1);
    REQUIRE(d2);

    auto behavior = IBehavior::fromEntity(lvl, player);
    auto props = propStore->fromEntity(player);

    REQUIRE(props->asInt("diamonds") == 0);

    WHEN("the player walks over a diamond")
    {
        REQUIRE_CALL(*g_mockInput, getInput())
            .TIMES(AT_LEAST(1))
            .RETURN(InputTypes::LEFT);
        behavior->run(100);
        REQUIRE(player->getPosition() == (point){3,5});

        // Back again
        REQUIRE_CALL(*g_mockInput, getInput())
            .TIMES(AT_LEAST(1))
            .RETURN(InputTypes::RIGHT);
        behavior->run(100);
        REQUIRE(player->getPosition() == (point){4,5});

        THEN("the diamond will disappear")
        {
            d1 = store->getEntityByPoint({3,5});
            REQUIRE(!d1);

            AND_THEN("the player diamond count will increase by one")
            {
                REQUIRE(props->asInt("diamonds") == 1);
            }
        }
    }

    WHEN("the player stands beside the diamond and operates towards the diamond")
    {
        REQUIRE_CALL(*g_mockInput, getInput())
            .TIMES(AT_LEAST(1))
            .RETURN(InputTypes::LEFT | InputTypes::OPERATE);
        behavior->run(100);
        // No movement
        REQUIRE(player->getPosition() == (point){4,5});

        THEN("the diamond will disappear")
        {
            d1 = store->getEntityByPoint({3,5});
            REQUIRE(!d1);

            AND_THEN("the player diamond count will increase by one")
            {
                REQUIRE(props->asInt("diamonds") == 1);
            }
        }
    }

    g_mockInput = nullptr;
}

SCENARIO("The player changes its environment")
{
    g_mockInput = std::make_shared<MockInput>();

    auto store = IEntityStore::getInstance();

    std::shared_ptr<ILevel> lvl = ILevel::fromString("9 9 "
                                    "........."
                                    "........."
                                    "........."
                                    "...... .."
                                    ".#..t.o.."
                                    ".#..p.o ." // 4, 5
                                    "........."
                                    "........."
                                    "........t");
    REQUIRE(lvl);

    auto player = store->getEntityByPoint({4,5});
    auto b1 = store->getEntityByPoint({6,5});
    auto b2 = store->getEntityByPoint({6,4});

    REQUIRE(player);
    REQUIRE(b1);
    REQUIRE(b2);

    auto playerBehavior = IBehavior::fromEntity(lvl, player);
    auto b2Behavior = IBehavior::fromEntity(lvl, b2);

    WHEN("the player walks around a region of dirt")
    {
        REQUIRE_CALL(*g_mockInput, getInput())
            .TIMES(AT_LEAST(1))
            .RETURN(InputTypes::LEFT);

        // Two steps
        playerBehavior->run(100);
        playerBehavior->run(100);
        REQUIRE(player->getPosition() == (point){2,5});

        THEN("the dirt will turn to empty tiles")
        {
            auto t1 = lvl->tileAt({3,5});
            auto t2 = lvl->tileAt({2,5});

            REQUIRE(t1);
            REQUIRE(t2);
            REQUIRE(*t1 == TileType::EMPTY);
            REQUIRE(*t2 == TileType::EMPTY);
        }
    }

    WHEN("the player walks over a teleporter")
    {
        // Starts from the beginning again
        REQUIRE_CALL(*g_mockInput, getInput())
            .TIMES(AT_LEAST(1))
            .RETURN(InputTypes::UP);

        playerBehavior->run(100);
        REQUIRE(player->getPosition() == (point){4,4});

        THEN("the teleporter will stay the same")
        {
            auto t1 = lvl->tileAt({4,4});

            REQUIRE(t1);
            REQUIRE(*t1 == TileType::TELEPORTER);
        }
    }

    WHEN("the player encounters a wall")
    {
        REQUIRE_CALL(*g_mockInput, getInput())
            .TIMES(AT_LEAST(1))
            .RETURN(InputTypes::LEFT);

        // Three steps
        playerBehavior->run(100);
        playerBehavior->run(100);
        playerBehavior->run(100);

        THEN("it cannot move further")
        {
            REQUIRE(player->getPosition() == (point){2,5});
        }
    }

    WHEN("the player pushes a boulder")
    {
        REQUIRE_CALL(*g_mockInput, getInput())
            .TIMES(AT_LEAST(1))
            .RETURN(InputTypes::RIGHT);

        playerBehavior->run(100);
        playerBehavior->run(100);

        THEN("it will move if there is empty space beside it")
        {
            REQUIRE(player->getPosition() == (point){6,5});
            REQUIRE(b1->getPosition() == (point){7,5});

            WHEN("the boulder has dirt or other solid material beside it")
            {
                playerBehavior->run(100);
                THEN("it will not move")
                {
                    // Can't move thhe boulder anymore
                    REQUIRE(player->getPosition() == (point){6,5});
                    REQUIRE(b1->getPosition() == (point){7,5});
                }
            }
        }

        WHEN("the boulder is above the player")
        {
            REQUIRE_CALL(*g_mockInput, getInput())
                .TIMES(AT_LEAST(1))
                .RETURN(InputTypes::UP);

            playerBehavior->run(100);

            THEN("it will not move even with empty space above it")
            {
                    // Can't move thhe boulder anymore
                    REQUIRE(player->getPosition() == (point){6,5});
                    REQUIRE(b2->getPosition() == (point){6,4});
            }
        }
    }

    g_mockInput = nullptr;
}

SCENARIO("The player can place bombs")
{
    WHEN("the player places a bomb")
    {
        THEN("it will explode after a while")
        {
        }

        THEN("the player has one bomb less in it's store")
        {
        }
    }
}