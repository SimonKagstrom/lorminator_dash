#include <catch.hpp>
#include <trompeloeil.hpp>

#include <level.hh>
#include <entity.hh>

TEST_CASE("An empty level can be created from a string", "[level]")
{
	auto lvl = Level::fromString("0 0");

    REQUIRE(lvl);
    REQUIRE(lvl->getEntities().empty());
    REQUIRE(lvl->getSize() == extents{0,0});
}

TEST_CASE("A level can't be created from an invalid string", "[level]")
{
	WHEN("the string is empty")
	{
		auto lvl = Level::fromString("");
		REQUIRE(!lvl);
	}
	WHEN("the size is invalid")
	{
		// 1x2 size, but only 3 entries
		auto lvl = Level::fromString("1 2 ...");
		REQUIRE(!lvl);
	}
	WHEN("the string contains invalid characters")
	{
		auto lvl = Level::fromString("2 2 ...M");
		REQUIRE(!lvl);
	}
}

TEST_CASE("A non-empty level can be created from a string", "[level]")
{
	WHEN("the level is square")
	{
		auto lvl = Level::fromString("2 2 oooo"); // 4 boulders
		REQUIRE(lvl);

		WHEN("entities can be created")
		{
			auto entities = lvl->getEntities();

			REQUIRE(entities.size() == 4);

			for (auto &it : entities)
			{
				REQUIRE(it->getType() == EntityType::BOULDER);
			}
		}
	}

	WHEN("the level is not square")
	{
		auto lvl = Level::fromString("2 3 ooooop");
		REQUIRE(lvl);
	}

	WHEN("the level data contains newlines")
	{
		auto lvl = Level::fromString("2 3 ooo\noop");
		REQUIRE(lvl);
	}
}
