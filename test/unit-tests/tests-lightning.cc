#include <lightning.hh>

#include <catch.hpp>
#include <trompeloeil.hpp>

SCENARIO("Darkness is complete, but can be lighted")
{
    GIVEN("a valid level with dirt")
    {
        auto store = IEntityStore::getInstance();

        std::shared_ptr<ILevel> lvl = ILevel::fromString("9 3 "
                                        "........."
                                        "........."
                                        ".oo.....p");
        REQUIRE(lvl);

        auto lightning = ILightning::create(lvl);
        REQUIRE(lvl);

        WHEN("there is no light")
        {
            THEN("the entire level will be unlighted in the light map")
            {
                auto size = lvl->getSize();

                for (auto y = 0; y < size.height - 1; y++) // Skip the player line
                {
                    for (auto x = 0; x < size.width; x++)
                    {
                        auto real = lvl->tileAt({x,y});
                        auto lighted = lightning->tileAt({x,y});

                        REQUIRE(real);
                        REQUIRE(lighted);

                        REQUIRE(*real == TileType::DIRT);
                        REQUIRE(*lighted == TileType::UNKNOWN);
                    }
                }
            }

            AND_THEN("all entities will be undiscovered")
            {
                auto visible = lightning->getVisibleEntities();
                auto shadows = lightning->getShadowEntities();

                REQUIRE(visible.empty());
                REQUIRE(shadows.empty());
            }
        }

        WHEN("there is light over the dirt")
        {
            lightning->updateLightning({ {1,1}, {2, 1} });

            THEN("the dirt will become known")
            {
                auto t10 = lightning->tileAt({1,0});
                auto t11 = lightning->tileAt({1,1});
                auto t21 = lightning->tileAt({2,1});

                REQUIRE(t10);
                REQUIRE(t11);
                REQUIRE(t21);

                REQUIRE(*t10 == TileType::UNKNOWN);
                REQUIRE(*t11 == TileType::DIRT);
                REQUIRE(*t21 == TileType::DIRT);
            }
        }

        WHEN("a seen tile in the shadows is changed")
        {
            lightning->updateLightning({ {1,1}, {2, 1} });

            // change in darkness
            lvl->setTile({1,1}, TileType::STONE_WALL);

            THEN("it will not be known")
            {
                auto t11 = lightning->tileAt({1,1});

                REQUIRE(t11);
                REQUIRE(*t11 == TileType::DIRT);
            }

            WHEN("it is lighted")
            {
                lightning->updateLightning({ {1,1}, {2, 1} });

                THEN("it will be known again")
                {
                    auto t11 = lightning->tileAt({1,1});

                    REQUIRE(t11);
                    REQUIRE(*t11 == TileType::STONE_WALL);
                }
            }
        }

        WHEN("there is light over entities")
        {
            lightning->updateLightning({ {1,2} });

            THEN("they will be seen")
            {
                auto visible = lightning->getVisibleEntities();
                auto shadow = lightning->getShadowEntities();

                REQUIRE(shadow.empty());
                REQUIRE(visible.size() == 1);

                auto ent = store->getEntityById(*visible.begin());
                REQUIRE(ent);
                REQUIRE(ent->getPosition() == (point){1,2});
                REQUIRE(ent->getType() == EntityType::BOULDER);

                WHEN("the light changes and the entity is in the darkness")
                {
                    lightning->updateLightning({ {0,0} });

                    THEN("it will be in the shadow entities")
                    {
                        auto visible = lightning->getVisibleEntities();
                        auto shadow = lightning->getShadowEntities();

                        REQUIRE(visible.empty());
                        REQUIRE(shadow.size() == 1);

                        auto shadowEnt = shadow.front();
                        REQUIRE(shadowEnt.pt == (point){1,2});
                        REQUIRE(shadowEnt.type == EntityType::BOULDER);
                    }
                }
            }

            WHEN("a known entity in the shadows is moved")
            {
                lightning->updateLightning({});

                auto visible = lightning->getVisibleEntities();
                auto shadow = lightning->getShadowEntities();

                REQUIRE(shadow.size() == 1);
                REQUIRE(visible.empty());

                auto shadowEnt = shadow.front();
                REQUIRE(shadowEnt.pt == (point){1,2});
                REQUIRE(shadowEnt.type == EntityType::BOULDER);

                auto ent = store->getEntityByPoint({1,2});
                REQUIRE(ent);

                ent->setPosition({0,0});

                THEN("it will stay in the previous position in the shadows")
                {
                    lightning->updateLightning({{0,1}});

                    auto visible = lightning->getVisibleEntities();
                    auto shadow = lightning->getShadowEntities();

                    REQUIRE(shadow.size() == 1);
                    REQUIRE(visible.empty());

                    auto shadowEnt = shadow.front();
                    REQUIRE(shadowEnt.pt == (point){1,2});
                    REQUIRE(shadowEnt.type == EntityType::BOULDER);
                }

                AND_WHEN("the new position is lighted")
                {
                    lightning->updateLightning({{0,0}});

                    THEN("the entity will be seen at the new position")
                    {
                        auto visible = lightning->getVisibleEntities();
                        auto shadow = lightning->getShadowEntities();

                        REQUIRE(shadow.size() == 1);
                        REQUIRE(visible.size() == 1);
                        REQUIRE(visible.front() == ent->getId());

                        AND_THEN("since the old position is still in darkness, it will also be seen")
                        {
                            auto shadowEnt = shadow.front();
                            REQUIRE(shadowEnt.pt == (point){1,2});
                            REQUIRE(shadowEnt.type == EntityType::BOULDER);
                        }
                    }
                }
            }
       }
    }
}
