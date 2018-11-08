#pragma once

#include <point.hh>
#include <input.hh>

#include "itrait.hh"

namespace player
{
    class PlayerTraitBase : public ITrait
    {
    protected:
        Direction keysToDir(uint32_t keys) const
        {
            auto dir = Direction::NONE;

            if (keys & InputTypes::LEFT)
            {
                dir = Direction::LEFT;
            }
            else if (keys & InputTypes::RIGHT)
            {
                dir = Direction::RIGHT;
            }
            else if (keys & InputTypes::UP)
            {
                dir = Direction::UP;
            }
            else if (keys & InputTypes::DOWN)
            {
                dir = Direction::DOWN;
            }

            return dir;
        }
    };
}
