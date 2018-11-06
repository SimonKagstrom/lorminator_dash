#include <catch.hpp>
#include <trompeloeil.hpp>

#include <input.hh>
#include <level.hh>
#include <entity.hh>
#include <behavior.hh>

class MockInput : public IInput
{
public:
    MAKE_MOCK0(getInput, uint32_t());
};

static std::shared_ptr<MockInput> g_mockInput;
std::shared_ptr<IInput> IInput::fromEntity(std::shared_ptr<IEntity> entity)
{
    return g_mockInput;
}


SCENARIO("The player takes diamonds")
{
    g_mockInput = std::make_shared<MockInput>();

    auto store = IEntityStore::getInstance();

    std::shared_ptr<ILevel> lvl = ILevel::fromString("9 9 "
                                    "........."
                                    "........."
                                    "........."
                                    "........."
                                    "........."
                                    "...dpd..." // 4, 5
                                    "........."
                                    "........."
                                    ".........");
    REQUIRE(lvl);

    auto player = store->getEntityByPoint({4,5});
    auto d1 = store->getEntityByPoint({3,5});
    auto d2 = store->getEntityByPoint({5,5});
    REQUIRE(player);
    REQUIRE(d1);
    REQUIRE(d2);

    auto behavior = IBehavior::fromEntity(lvl, player);

    WHEN("the player walks over a diamond")
    {
        REQUIRE_CALL(*g_mockInput, getInput())
            .RETURN(InputTypes::LEFT);
        behavior->run(100);

        // Back again
        REQUIRE_CALL(*g_mockInput, getInput())
            .RETURN(InputTypes::RIGHT);
        behavior->run(100);

        THEN("the diamond will disappear")
        {
            d1 = store->getEntityByPoint({3,5});
            REQUIRE(!d1);

            AND_THEN("the player diamond count will increase by one")
            {
            }
        }
    }

    WHEN("the player stands beside the diamond and operates towards the diamond")
    {
        THEN("the diamond will disappear")
        {
            AND_THEN("the player diamond count will increase by one")
            {
            }
        }
    }
}

SCENARIO("The player changes its environment")
{
    WHEN("the player walks around a region of dirt")
    {
        THEN("the dirt will turn to empty tiles")
        {
        }
    }

    WHEN("the player walks over a teleporter")
    {
        THEN("the teleporter will stay the same")
        {
        }
    }

    WHEN("the player encounters a wall")
    {
        THEN("it cannot move further")
        {
        }
    }
}

SCENARIO("The player can place bombs")
{
    WHEN("the player places a bomb")
    {
        THEN("it will explode after a while")
        {
        }

        THEN("the player has one bomb less in it's store")
        {
        }
    }
}