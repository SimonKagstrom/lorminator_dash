#include <catch.hpp>
#include <trompeloeil.hpp>

#include <level.hh>

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
}

TEST_CASE("A non-empty level can be created from a string", "[level]")
{
	REQUIRE(false);
}
