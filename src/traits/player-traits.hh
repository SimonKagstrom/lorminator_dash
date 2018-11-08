#pragma once

#include <level.hh>
#include <entity.hh>
#include <point.hh>
#include <input.hh>

#include "itrait.hh"

namespace player
{
    class PlayerTraitBase : public ITrait
    {
    protected:
        PlayerTraitBase(std::shared_ptr<ILevel> level, std::shared_ptr<IEntity> entity) :
            m_level(level), m_entity(entity)
        {
            m_input = IInput::fromEntity(entity);
            if (!m_input)
            {
                // Something is horribly wrong it we can't control the player
                throw std::invalid_argument("Can't control the player???");
            }
        }

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

        std::shared_ptr<ILevel> m_level;
        std::shared_ptr<IEntity> m_entity;
        std::shared_ptr<IInput> m_input;
    };
}
