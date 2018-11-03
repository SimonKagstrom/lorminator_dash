#include <catch.hpp>
#include <trompeloeil.hpp>

#include <level.hh>
#include <entity.hh>
#include <behavior.hh>

// In ms
static const unsigned FALL_TIME = 100;
static const unsigned BOMB_TIMEOUT = 2000;
static const unsigned FIREBALL_BURNOUT_TIME = 1000;
static const unsigned GHOST_MOVEMENT_TIME = 100;
static const unsigned TRANSPORT_BAND_MOVEMENT_TIME = 500;
static const unsigned TELEPORTER_DELAY = 1500;

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
                        auto ents = store->getEntities();
                        REQUIRE(std::find_if(ents.begin(), ents.end(),
                                [](std::shared_ptr<IEntity> cur) {return cur->getType() == EntityType::PLAYER;}) == ents.end());
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
            WHEN("it hits the player")
            {
                THEN("there will be an explosion")
                {

                    AND_THEN("the player will die")
                    {
                    }
                }
            }
        }
    }
}

SCENARIO("bombs will explode")
{
    WHEN("a bomb is placed on an empty spot")
    {
        auto store = IEntityStore::getInstance();

        std::shared_ptr<ILevel> lvl = ILevel::fromString("2 4 "
                "b."
                " ."
                ".."
                ".p"
        );
        REQUIRE(lvl);

        auto bomb = store->getEntityByPoint({0,0});
        REQUIRE(bomb);

        auto behavior = IBehavior::fromEntity(lvl, bomb);
        REQUIRE(bomb);

        THEN("it will fall until it hits solid ground")
        {
            behavior->run(FALL_TIME);
            REQUIRE(bomb->getPosition() == (point){0,1});

            AND_THEN("it will explode after a while")
            {
                behavior->run(BOMB_TIMEOUT - FALL_TIME - 1);
                REQUIRE(store->getEntityByPoint({0,1})->getType() == EntityType::BOMB);

                // Should now explode
                behavior->run(2);
                REQUIRE(store->getEntityByPoint({0,1})->getType() == EntityType::FIREBALL);
            }
        }
    }
}

SCENARIO("fireballs fall and disappears")
{
    WHEN("a fireball appears above an empty spot")
    {
        auto store = IEntityStore::getInstance();

        std::shared_ptr<ILevel> lvl = ILevel::fromString("2 4 "
                                                         "f."
                                                         " ."
                                                         ".."
                                                         ".p");
        REQUIRE(lvl);

        auto fireball = store->getEntityByPoint({0, 0});
        REQUIRE(fireball);
        auto behavior = IBehavior::fromEntity(lvl, fireball);

        THEN("it will fall until it hits solid ground")
        {
            behavior->run(FALL_TIME);
            REQUIRE(store->getEntityByPoint({0, 1})->getType() == EntityType::FIREBALL);

            AND_THEN("disappear when it has burned up")
            {
                behavior->run(FIREBALL_BURNOUT_TIME);

                REQUIRE(!store->getEntityByPoint({0, 1}));
            }
        }
    }
}

SCENARIO("Ghosts appear!")
{
    WHEN("a ghost appears")
    {
        auto store = IEntityStore::getInstance();

        std::shared_ptr<ILevel> lvl = ILevel::fromString("9 9 "
                                      ".... ...."
                                      ".... ...."
                                      "....   .."
                                      ".... ...."
                                      ".... ...."
                                      "....g...." // 4, 5
                                      "........."
                                      "........."
                                      "........p");
        REQUIRE(lvl);

        auto ghost = store->getEntityByPoint({4, 5});
        REQUIRE(ghost);
        auto behavior = IBehavior::fromEntity(lvl, ghost);

        THEN("it will explore non-visited places along corridors")
        {
            std::vector<point> visitOrder;
            auto onMovement = [&visitOrder](std::shared_ptr<IEntity> ent,
                const point &from, const point &to)
            {
                visitOrder.push_back(to);
            };
            auto cookie = ghost->onMovement(onMovement);

            for (auto i = 0; i < 5; i++)
            {
                behavior->run(GHOST_MOVEMENT_TIME);
            }
            REQUIRE(visitOrder.size() == 5U);

            // Expected to walk along a line upwards
            std::vector<point> expected = {{4,4}, {4,3}, {4,2}, {4,1}, {4,0}};
            // or visit the bend
            std::vector<point> alternatively = {{4,4}, {4,3}, {4,2},  {5,2}, {6,2}};

            REQUIRE( (visitOrder == expected || visitOrder == alternatively) );

            WHEN("it encounters a dead end")
            {
                THEN("it will retrace its steps until a non-visited place appears")
                {
                    visitOrder.clear();

                    // Go back two steps until the bend and then up/to the side
                    for (auto i = 0; i < 4; i++)
                    {
                        behavior->run(GHOST_MOVEMENT_TIME);
                    }
                    expected = {{4,1}, {4,2},  {5,2}, {6,2}};
                    alternatively = {{5,2}, {4,2},  {4,1}, {4,0}};

                    REQUIRE((visitOrder == expected || visitOrder == alternatively));
                }
            }
        }
    }

    WHEN("a ghost can select between paths")
    {
        THEN("it will normally prefer continuing in the same direction as before")
        {
        }
    }

    WHEN("a ghost encounters the player")
    {
        THEN("an explosion will occur")
        {
        }
    }
}

SCENARIO("Transport bands can transport things")
{
    WHEN("objects are placed on a transport band")
    {
        auto store = IEntityStore::getInstance();

        std::shared_ptr<ILevel> lvl = ILevel::fromString("9 9 "
                                      "........."
                                      "........."
                                      "........."
                                      "........."
                                      "..   ob.." // {5,4}, {6,4}
                                      ".<<<<<<.."
                                      "........."
                                      "........."
                                      "........p");
        REQUIRE(lvl);

        auto boulder = store->getEntityByPoint({5,4});
        auto bomb = store->getEntityByPoint({6,4});

        REQUIRE(boulder);
        REQUIRE(bomb);

        auto behavior = IBehavior::fromLevel(lvl);
        REQUIRE(behavior);

        THEN("they will be transported along it until they hit a solid object")
        {
            behavior->run(TRANSPORT_BAND_MOVEMENT_TIME);
            REQUIRE(boulder->getPosition() == (point){4,4});
            REQUIRE(bomb->getPosition() == (point){5,4});

            // Until the wall is hit
            for (unsigned i = 0; i < 3; i++)
            {
                behavior->run(TRANSPORT_BAND_MOVEMENT_TIME);
            }
            REQUIRE(boulder->getPosition() == (point){2,4});
            REQUIRE(bomb->getPosition() == (point){3,4});
        }
    }

    WHEN("two transport bands with different directions touch each other")
    {
        THEN("objects will be transported back and forth")
        {

        }
    }
}

SCENARIO("teleporters transport entities between different points")
{
    WHEN("an entity stands on a teleporter")
    {
        auto store = IEntityStore::getInstance();

        std::shared_ptr<ILevel> lvl = ILevel::fromString("9 9 "
                                      "t.......t" // {0,0}, {8,0}
                                      "........."
                                      "........."
                                      "........."
                                      "........."
                                      "........."
                                      "........."
                                      "........."
                                      "........p");
        REQUIRE(lvl);

        auto ent = IEntity::createFromType(EntityType::BOULDER, {0,0});

        auto behavior = IBehavior::fromLevel(lvl);

        THEN("it will stay put for a while")
        {
            behavior->run(TELEPORTER_DELAY - 1);

            REQUIRE(ent->getPosition() == (point){0,0});
            AND_THEN("be moved to another teleporting location later")
            {
            behavior->run(2); // now after the teleporting delay;

            REQUIRE(ent->getPosition() == (point){8,0});
            }
        }
    }

    WHEN("an entity is teleported to a location where another entity stands")
    {
    }
}