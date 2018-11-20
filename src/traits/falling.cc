#include <behavior.hh>
#include <point.hh>
#include <entity.hh>
#include <level.hh>

#include "itrait.hh"

#include <memory>
#include <vector>

class FallTrait : public ITrait
{
public:
    FallTrait(std::shared_ptr<ILevel> level, std::shared_ptr<IEntity> entity) :
        m_level(level),
        m_entity(entity)
    {
    }

    bool run(unsigned ms) override
    {
        auto isEmpty = [this](const point &pt)
        {
            auto tile = m_level->tileAt(pt);
            if (tile)
            {
                auto entityAt = IEntityStore::getInstance()->getEntityByPoint(pt);

                if (entityAt)
                {
                    return false;
                }

                return *tile == TileType::EMPTY;
            }

            return false;
        };

        auto entityShouldBeDestroyed = [](const std::shared_ptr<IEntity> ent)
        {
            if (!ent)
            {
                return false;
            }

            switch (ent->getType())
            {
            case EntityType::PLAYER:
            case EntityType::GHOST:
            case EntityType::BOMB:
                return true;
            default:
                break;
            }

            return false;
        };

        auto shouldFallOnEntityBelow = [](const std::shared_ptr<IEntity> ent)
        {
            if (!ent)
            {
                return false;
            }

            switch (ent->getType())
            {
            case EntityType::BOMB:
            case EntityType::BOULDER:
            case EntityType::DIAMOND:
                return true;
            default:
                break;
            }

            return false;
        };

        auto cur = m_entity->getPosition();
        auto below = cur + Direction::DOWN;

        auto entityBelow = IEntityStore::getInstance()->getEntityByPoint(cur + Direction::DOWN);

        // Falling on an entity?
        if (isFalling() && entityShouldBeDestroyed(entityBelow))
        {
            m_entity->remove();
            entityBelow->remove();

            m_level->explode(cur + Direction::DOWN);

            return true;
        }
        // Standing on an entity
        else if (shouldFallOnEntityBelow(entityBelow))
        {
            // Fall if it's free to the side and down
            auto left = cur + Direction::LEFT;
            auto downLeft = (cur + Direction::LEFT) + Direction::DOWN;
            auto right = cur + Direction::RIGHT;
            auto downRight = (cur + Direction::RIGHT) + Direction::DOWN;

            if (isEmpty(left) && isEmpty(downLeft))
            {
                m_entity->setPosition(cur + Direction::LEFT);
                return fall();
            }
            else if (isEmpty(right) && isEmpty(downRight))
            {
                m_entity->setPosition(cur + Direction::RIGHT);
                return fall();
            }
        }
        else if (!entityBelow && isEmpty(below))
        {
            m_entity->setPosition(cur + Direction::DOWN);

            return fall();
        }

        return dontFall();
    }

private:
    bool isFalling() const
    {
        return m_falling;
    }

    bool fall()
    {
        m_falling = true;
        return true;
    }

    bool dontFall()
    {
        m_falling = false;
        return false;
    }

    bool m_falling{false};
    std::shared_ptr<ILevel> m_level;
    std::shared_ptr<IEntity> m_entity;
};
