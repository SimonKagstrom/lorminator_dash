#include <catch.hpp>
#include <trompeloeil.hpp>

#include <entity.hh>

TEST_CASE("An entity can't be created from an invalid character", "[entity]")
{
	auto ent = Entity::fromChar('.'); // Not an entity in the map
	REQUIRE(!ent);
}

TEST_CASE("An entity can be created from a character", "[entity]")
{
	WHEN("the entity is a boulder")
	{
		auto ent = Entity::fromChar('o');
		REQUIRE(ent);
	}
	WHEN("the entity is a player")
	{
		auto ent = Entity::fromChar('p');
		REQUIRE(ent);
	}
}
