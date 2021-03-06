#include <catch.hpp>
#include <trompeloeil.hpp>

#include <entity.hh>
#include <point.hh>

TEST_CASE("An entity can't be created from an invalid character", "[entity]")
{
    auto ent = IEntity::createFromChar('.', {5,5}); // Not an entity in the map
    REQUIRE(!ent);
}

TEST_CASE("An entity can be created from a character", "[entity]")
{
    WHEN("the entity is a boulder")
    {
        auto ent = IEntity::createFromChar('o', {90,1});
        REQUIRE(ent);
    }
    WHEN("the entity is a player")
    {
        auto ent = IEntity::createFromChar('p', {0,0});
        REQUIRE(ent);
    }
    WHEN("the entity is a fireball")
    {
        auto ent = IEntity::createFromChar('f', {0,0});
        REQUIRE(ent);
        REQUIRE(ent->getType() == EntityType::FIREBALL);
    }
}

TEST_CASE("The position of the entity can be read and modified")
{
    auto ent = IEntity::createFromChar('o', {10, 11});
    REQUIRE(ent);

    REQUIRE(ent->getPosition() == (point){10, 11});

    SECTION("the position can be set")
    {
        ent->setPosition({99,56});
        REQUIRE(ent->getPosition() == (point){99, 56});
    }
}

SCENARIO("Entities can be placed and retrieved from the entity store")
{
    WHEN("the store singleton has not been created before")
    {
        THEN("entries are lost after creation")
        {
            auto ent = IEntity::createFromChar('o', {90,1});
            REQUIRE(ent);

            REQUIRE(IEntityStore::getInstance()->getEntities().size() == 0);
        }
    }

    WHEN("the store singleton is present")
    {
        auto inst = IEntityStore::getInstance();

        THEN("entries can be retrieved from the store")
        {
            auto ent = IEntity::createFromChar('o', {90,1});
            REQUIRE(ent);
            ent = IEntity::createFromChar('o', {92,1});
            REQUIRE(ent);

            auto store = IEntityStore::getInstance();
            auto all = store->getEntities();
            REQUIRE(all.size() == 2);

            REQUIRE(all[0]->getType() == EntityType::BOULDER);
            REQUIRE(all[1]->getType() == EntityType::BOULDER);

            // Get by point
            auto e90_1 = store->getEntityByPoint({90,1});
            auto e91_1 = store->getEntityByPoint({91,1});
            auto e92_1 = store->getEntityByPoint({92,1});

            REQUIRE(!e91_1);
            REQUIRE(e90_1);
            REQUIRE(e92_1);

            auto byId = store->getEntityById(e90_1->getId());
            REQUIRE(byId);
            REQUIRE(byId->getId() == e90_1->getId());
        }
    }
}
