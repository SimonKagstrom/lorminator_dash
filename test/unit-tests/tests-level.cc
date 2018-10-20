#include <catch.hpp>
#include <trompeloeil.hpp>

#include <level.hh>
#include <entity.hh>

#include <set>

TEST_CASE("An empty level can be created from a string", "[level]")
{
	auto lvl = ILevel::fromString("0 0");

    REQUIRE(lvl);
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

SCENARIO("A non-empty level can be created from a string", "[level]")
{
	WHEN("the level is square")
	{
		auto store = IEntityStore::getInstance();

		auto lvl = ILevel::fromString("2 2 ooop"); // 3 boulders and the player
		REQUIRE(lvl);

		THEN("entities can be created")
		{
			auto entities = store->getEntities();

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

	auto tm10 = lvl->tileAt({-1, 0});
	REQUIRE(!tm10); // Out of bounds

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

static void require_level_equals_to(std::unique_ptr<ILevel> one, const char *levelStr)
{
	auto other = ILevel::fromString(levelStr);
	REQUIRE(other);

	REQUIRE(one->getSize() == other->getSize());

	printf("ONE:\n%sOTHER:\n%s\n", one->toString().c_str(), other->toString().c_str());

	auto size = one->getSize();
	for (auto x = 0; x < size.width; x++)
	{
		for (auto y = 0; y < size.height; y++)
		{
			REQUIRE (one->tileAt({x,y}) == other->tileAt({x, y}));
		}
	}
}

SCENARIO("Explosions can cause havoc on levels", "[level]")
{
	GIVEN("that there is an explosion")
	{
		WHEN("there is only dirt around the explosion")
		{
			auto store = IEntityStore::getInstance();

			auto lvl = ILevel::fromString("9 9 "
					"........."
					"........."
					"........."
					"........."
					"........."
					"........."
					"........."
					"........."
					"........p");
			REQUIRE(lvl);

			lvl->explode({4, 2});

			THEN("the dirt is cleared")
			{
				require_level_equals_to(std::move(lvl),
						"9 9 "
						".... ...."
						"...   ..."
						"...   ..."
						"...   ..."
						".... ...."
						"........."
						"........."
						"........."
						"........p");
			}

			AND_THEN("fireballs show up")
			{
				auto ents = store->getEntities();
				unsigned fireballs = 0;

				std::set<point> ballPoints;
				for (auto &it : ents)
				{
					fireballs += it->getType() == EntityType::FIREBALL;
					if (it->getType() == EntityType::FIREBALL)
					{
						ballPoints.insert(it->getPosition());
					}
				}

				REQUIRE(fireballs == 11);

				std::set<point> expectedPoints =
				{
					         {4, 0},
					{ 3, 1}, {4, 1}, {5, 1},
					{ 3, 2}, {4, 2}, {5, 2},
					{ 3, 3}, {4, 3}, {5, 3},
					         {4, 4}
				};

				REQUIRE(ballPoints == expectedPoints);
			}
		}

		WHEN("there are stone walls around the explosion")
		{
			auto store = IEntityStore::getInstance();

			auto lvl = ILevel::fromString("9 9 "
					"........."
					".....#..."
					".....#..."
					".....#..."
					"....#...."
					"........."
					"........."
					"........."
					"........p");
			REQUIRE(lvl);

			lvl->explode({4, 2});

			THEN("they withstand the explosive force")
			{
			require_level_equals_to(std::move(lvl),
					"9 9 "
					".... ...."
					"...  #..."
					"...  #..."
					"...  #..."
					"....#...."
					"........."
					"........."
					"........."
					"........p");
			}

			AND_THEN("fireballs only show up on empty spots")
			{
				auto ents = store->getEntities();

				unsigned fireballs = 0;

				std::set<point> ballPoints;
				for (auto &it : ents)
				{
					fireballs += it->getType() == EntityType::FIREBALL;
					if (it->getType() == EntityType::FIREBALL)
					{
						ballPoints.insert(it->getPosition());
					}
				}

				REQUIRE(fireballs == 7);

				std::set<point> expectedPoints =
				{
					         {4, 0},
					{ 3, 1}, {4, 1},
					{ 3, 2}, {4, 2},
					{ 3, 3}, {4, 3}
				};

				REQUIRE(ballPoints == expectedPoints);
			}
		}

		WHEN("a stone wall blocks the explosion")
		{
			auto lvl = ILevel::fromString("9 9 "
					"........."
					"....##..."
					".....#..."
					".....#..."
					"....#...."
					"........."
					"........."
					"........."
					"........p");
			REQUIRE(lvl);

			lvl->explode({4, 2});


			THEN("tiles on the other side of the wall is unaffected by the explosion")
			{
				require_level_equals_to(std::move(lvl),
						"9 9 "
						"........."
						"... ##..."
						"...  #..."
						"...  #..."
						"....#...."
						"........."
						"........."
						"........."
						"........p");
			}
		}

		WHEN("the explosion is in the corner")
		{
			auto lvl = ILevel::fromString("9 9 "
					"........."
					"........."
					"........."
					"........."
					"........."
					"........."
					"........."
					"........."
					"........p");
			REQUIRE(lvl);

			lvl->explode({0, 0});
			THEN("only affected areas are cleared")
			{
				require_level_equals_to(std::move(lvl),
						"9 9 "
						"  ......."
						"  ......."
						" ........"
						"........."
						"........."
						"........."
						"........."
						"........."
						"........p");
			}
		}

		WHEN("weak walls are in the way of the explosion")
		{
			auto lvl = ILevel::fromString("9 9 "
					"........."
					".....#..."
					".....#..."
					".....w..."
					"....#...."
					"........."
					"........."
					"........."
					"........p");
			REQUIRE(lvl);

			lvl->explode({4, 2});
			THEN("they are obliterated by the explosion")
			{
				require_level_equals_to(std::move(lvl),
						"9 9 "
						".... ...."
						"...  #..."
						"...  #..."
						"...   ..."
						"....#...."
						"........."
						"........."
						"........."
						"........p");
			}
		}

		WHEN("doors are in the way of the explosion")
		{
			auto lvl = ILevel::fromString("9 9 "
					"........."
					".....d..."
					".....#..."
					".....#..."
					"....#...."
					"........."
					"........."
					"........."
					"........p");
			REQUIRE(lvl);

			lvl->explode({4, 2});
			THEN("wooden doors are obliterated by the explosion")
			{
				require_level_equals_to(std::move(lvl),
						"9 9 "
						".... ...."
						"...   ..."
						"...  #..."
						"...  #..."
						"....#...."
						"........."
						"........."
						"........."
						"........p");
			}
			AND_THEN("iron doors withstand the explosion without problems")
			{
				REQUIRE(false);
			}
		}
	}
}

SCENARIO("The light can show your way", "[level]")
{
	WHEN("there is open space around")
	{
		THEN("the light illuminates a cone")
		{
		}
	}

	WHEN("in a narrow corridor")
	{
		THEN("the light only shows the corridor")
		{
		}
	}

	WHEN("there are blocking stuff around")
	{
		THEN("the light is also blocked")
		{
		}
	}
}
