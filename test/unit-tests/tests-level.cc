#include <catch.hpp>
#include <trompeloeil.hpp>

#include <level.hh>

TEST_CASE("An empty level can be created from a string", "[level]")
{
	auto lvl = Level::fromString("");

    REQUIRE(lvl);
    REQUIRE(lvl->getEntities().empty());
    REQUIRE(lvl->getSize() == extents{0,0});
}

TEST_CASE("A non-empty level can be created from a string", "[level]")
{
	REQUIRE(false);
}

TEST_CASE("A level can't be created from an invalid string", "[level]")
{
	REQUIRE(false);
}
