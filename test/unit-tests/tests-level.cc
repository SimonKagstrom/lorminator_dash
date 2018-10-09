#include <catch.hpp>
#include <trompeloeil.hpp>

#include <level.hh>
#include <entity.hh>

TEST_CASE("An empty level can be created from a string", "[level]")
{
	auto lvl = ILevel::fromString("0 0");

    REQUIRE(lvl);
    REQUIRE(lvl->getEntities().empty());
    REQUIRE(lvl->getSize() == extents{0,0});
}

TEST_CASE("A level can't be created from an invalid string", "[level]")
{
	WHEN("the string is empty")
	{
		auto lvl = ILevel::fromString("");
		REQUIRE(!lvl);
	}

	WHEN("the size is invalid")
	{
		// 1x2 size, but only 3 entries
		auto lvl = ILevel::fromString("1 2 ..p");
		REQUIRE(!lvl);
	}

	WHEN("the string contains invalid characters")
	{
		auto lvl = ILevel::fromString("2 2 ..pM");
		REQUIRE(!lvl);
	}

	WHEN("the level does not have a player")
	{
		auto lvl = ILevel::fromString("2 2 ....");
		REQUIRE(!lvl);
	}

	WHEN("the level has a single teleporter")
	{
		auto lvl = ILevel::fromString("2 2 ..tp");
		REQUIRE(!lvl);
	}
}

TEST_CASE("A non-empty level can be created from a string", "[level]")
{
	WHEN("the level is square")
	{
		auto lvl = ILevel::fromString("2 2 ooop"); // 3 boulders and the player
		REQUIRE(lvl);

		WHEN("entities can be created")
		{
			auto entities = lvl->getEntities();

			REQUIRE(entities.size() == 4);

			unsigned boulders = 0;
			unsigned players = 0;
			for (auto &it : entities)
			{
				if (it->getType() == EntityType::BOULDER)
				{
					boulders++;
				}
				else if (it->getType() == EntityType::PLAYER)
				{
					players++;
				}
			}
			REQUIRE(boulders == 3);
			REQUIRE(players == 1);
		}
	}

	WHEN("the level is not square")
	{
		auto lvl = ILevel::fromString("2 3 ooooop");
		REQUIRE(lvl);
	}

	WHEN("the level data contains newlines")
	{
		auto lvl = ILevel::fromString("2 3 ooo\noop");
		REQUIRE(lvl);
	}
}

TEST_CASE("A level can be queried for tiles", "[level]")
{
	auto lvl = ILevel::fromString("3 2 ...#.p");
	REQUIRE(lvl);

	auto t44 = lvl->tileAt({4,4});
	REQUIRE(!t44); // Out of bounds

	auto t00 = lvl->tileAt({0,0});
	REQUIRE(t00);
	REQUIRE(t00 == TileType::DIRT);

	auto t01 = lvl->tileAt({0, 1});
	REQUIRE(t01);
	REQUIRE(t01 == TileType::STONE_WALL);

	auto t11 = lvl->tileAt({2,1});
	REQUIRE(t11);
	REQUIRE(t11 == TileType::EMPTY); // The player
}

TEST_CASE("A level can be queried for passable positions", "[level]")
{
}

TEST_CASE("Entities on a level are created at the correct positions", "[level]")
{
}
