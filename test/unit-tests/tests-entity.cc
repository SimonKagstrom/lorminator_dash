#include <catch.hpp>
#include <trompeloeil.hpp>

#include <entity.hh>
#include <point.hh>

TEST_CASE("An entity can't be created from an invalid character", "[entity]")
{
	auto ent = IEntity::fromChar('.', {5,5}); // Not an entity in the map
	REQUIRE(!ent);
}

TEST_CASE("An entity can be created from a character", "[entity]")
{
	WHEN("the entity is a boulder")
	{
		auto ent = IEntity::fromChar('o', {90,1});
		REQUIRE(ent);
	}
	WHEN("the entity is a player")
	{
		auto ent = IEntity::fromChar('p', {0,0});
		REQUIRE(ent);
	}
}

TEST_CASE("The position of the entity can be read and modified")
{
	auto ent = IEntity::fromChar('o', {10, 11});
	REQUIRE(ent);

	REQUIRE(ent->getPosition() == (point){10, 11});

	SECTION("the position can be set")
	{
		ent->setPosition({99,56});
		REQUIRE(ent->getPosition() == (point){99, 56});
	}
}
