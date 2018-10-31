#include <catch.hpp>
#include <trompeloeil.hpp>

#include <level.hh>
#include <entity.hh>
#include <behavior.hh>

static const unsigned FALL_TIME = 100;

SCENARIO("a boulder can fall")
{
    WHEN("the boulder is standing on solid ground")
    {
        auto store = IEntityStore::getInstance();

        std::shared_ptr<ILevel> lvl = ILevel::fromString("2 4 "
                "o."
                ".."
                ".."
                ".p"
                );
        REQUIRE(lvl);

        auto boulder = store->getEntityByPoint({0,0});
        REQUIRE(boulder);

        auto behavior = IBehavior::fromEntity(lvl, boulder);
        REQUIRE(behavior);

        THEN("it will not fall")
        {
            behavior->run(FALL_TIME);
            REQUIRE(boulder->getPosition() == (point){0,0});
        }
    }

    WHEN("an empty space is created under a boulder")
    {
        auto store = IEntityStore::getInstance();

        std::shared_ptr<ILevel> lvl = ILevel::fromString("2 4 "
                "o."
                " ."
                ".."
                ".p"
                );
        REQUIRE(lvl);

        auto boulder = store->getEntityByPoint({0,0});
        REQUIRE(boulder);

        auto behavior = IBehavior::fromEntity(lvl, boulder);
        REQUIRE(behavior);

        THEN("it will fall")
        {
            behavior->run(FALL_TIME);
            REQUIRE(boulder->getPosition() == (point){0,1});
        }

        AND_THEN("stop when it encounters firm ground")
        {
            behavior->run(FALL_TIME);
            REQUIRE(boulder->getPosition() == (point){0,1});
        }
    }

    WHEN("a boulder sits on top of another boulder with empty space on the left and below")
    {
        auto store = IEntityStore::getInstance();

        std::shared_ptr<ILevel> lvl = ILevel::fromString("3 4 "
                " o."
                " o."
                "..."
                ".p."
                );
        REQUIRE(lvl);

        auto boulder = store->getEntityByPoint({1,0});
        REQUIRE(boulder);

        auto behavior = IBehavior::fromEntity(lvl, boulder);
        REQUIRE(behavior);

        THEN("the boulder will fall")
        {
            behavior->run(FALL_TIME);
            REQUIRE(boulder->getPosition() == (point){0,0});
            behavior->run(FALL_TIME);
            REQUIRE(boulder->getPosition() == (point){0,1});
        }
    }

    WHEN("a boulder sits on top of another boulder with empty space on the right and below")
    {
        auto store = IEntityStore::getInstance();

        std::shared_ptr<ILevel> lvl = ILevel::fromString("3 4 "
                ".o "
                ".o "
                "..."
                ".p."
                );
        REQUIRE(lvl);

        auto boulder = store->getEntityByPoint({1,0});
        REQUIRE(boulder);

        auto behavior = IBehavior::fromEntity(lvl, boulder);
        REQUIRE(behavior);

        THEN("the boulder will fall")
        {
            behavior->run(FALL_TIME);
            REQUIRE(boulder->getPosition() == (point){2,0});
            behavior->run(FALL_TIME);
            REQUIRE(boulder->getPosition() == (point){2,1});
        }
    }

    WHEN("only a short time has passed")
    {
        THEN("the boulder will not have moved. yet.")
        {
        }
    }
}

SCENARIO("a boulder falls on the player")
{
    WHEN("the player stands under a boulder")
    {
        auto store = IEntityStore::getInstance();

        std::shared_ptr<ILevel> lvl = ILevel::fromString("2 4 "
                "o."
                "p."
                ".."
                ".."
        );
        REQUIRE(lvl);

        auto boulder = store->getEntityByPoint({0,0});
        REQUIRE(boulder);

        auto behavior = IBehavior::fromEntity(lvl, boulder);
        REQUIRE(behavior);

        THEN("the boulder will stay put")
        {
            behavior->run(FALL_TIME);
            REQUIRE(boulder->getPosition() == (point){0,0});
        }

        WHEN("the player moves downwards")
        {
            auto player = store->getEntityByPoint({0,1});
            REQUIRE(player->getType() == EntityType::PLAYER);

            player->setPosition({0,2});

            THEN("the boulder will fall")
            {
                behavior->run(FALL_TIME);
                REQUIRE(boulder->getPosition() == (point){0,1});

                AND_THEN("the boulder will cause an explosion when hitting the player")
                {
                    // Because the player carries explosive material
                    behavior->run(FALL_TIME);

                    auto fire = store->getEntityByPoint({0,2});
                    REQUIRE(fire);
                    REQUIRE(fire->getType() == EntityType::FIREBALL);

                    AND_THEN("the player will die")
                    {
                    }
                }

            }
        }
    }
}

SCENARIO("diamonds are forever")
{
    WHEN("the player stands under a diamond")
    {
        THEN("it will stay put")
        {
        }

        WHEN("the player moves up to the diamond")
        {
            THEN("it will disappear")
            {
            }
        }
    }

    WHEN("the player stands under a diamond, but with empty space above")
    {
        THEN("the diamond will fall")
        {
        }

        WHEN("it hits the player")
        {
            THEN("there will be an explosion")
            {
            }

            AND_THEN("the player will die")
            {
            }
        }
    }
}
