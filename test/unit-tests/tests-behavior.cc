#include <catch.hpp>
#include <trompeloeil.hpp>

SCENARIO("a boulder falls")
{
	WHEN("an empty space is created under a boulder")
	{
		THEN("it will fall")
		{
		}

		AND_THEN("stop when it encounters firm ground")
		{
		}
	}

	WHEN("a boulder sits on top of another boulder with empty space on the side and below")
	{
		THEN("the boulder will fall")
		{
		}

		AND_THEN("it will stop to the right or left of the other boulder")
		{
		}
	}
}

SCENARIO("a boulder falls on the player")
{
	WHEN("the player stands under a boulder")
	{
		THEN("the boulder will stay put")
		{
		}
	}

	WHEN("the player stands under the boulder, but moves downwards")
	{
		THEN("the boulder will fall")
		{
		}

		AND_THEN("the boulder will cause an explosion when hitting the player")
		{
			// Because the player carries explosive material
		}

		AND_THEN("the player will die")
		{
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
		}

		WHEN("it hits the player")
		{
			THEN("there will be an explosion")
			{
			}

			AND_THEN("the player will die")
			{
			}
		}
	}
}
