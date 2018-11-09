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
        auto isEmpty = [](const std::optional<TileType> tile)
        {
            if (tile)
            {
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

        auto cur = m_entity->getPosition();
        auto below = m_level->tileAt(cur + Direction::DOWN);

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
        else if (entityBelow)
        {
            // Fall is it's free to the side and down
            auto left = m_level->tileAt(cur + Direction::LEFT);
            auto downLeft = m_level->tileAt((cur + Direction::LEFT) + Direction::DOWN);
            auto right = m_level->tileAt(cur + Direction::RIGHT);
            auto downRight = m_level->tileAt((cur + Direction::RIGHT) + Direction::DOWN);

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
