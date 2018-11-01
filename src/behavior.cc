#include <behavior.hh>

#include <level.hh>
#include <entity.hh>

class ITrait
{
public:
    virtual ~ITrait()
    {
    }

    virtual bool run(unsigned ms) = 0;
};

class Behavior : public IBehavior
{
public:
    Behavior(std::shared_ptr<ILevel> level, std::shared_ptr<IEntity> entity);

    void run(unsigned ms) override;

private:
    std::vector<std::unique_ptr<ITrait>> m_traits;
};

class LevelBehavior : public IBehavior
{
public:
    LevelBehavior(std::shared_ptr<ILevel> level);

    void run(unsigned ms) override;

private:
    std::vector<std::unique_ptr<ITrait>> m_traits;
};

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

class ExplodeAfterTrait : public ITrait
{
public:
    ExplodeAfterTrait(int ms, std::shared_ptr<ILevel> level, std::shared_ptr<IEntity> entity) :
        m_timeLeft(ms),
        m_level(level),
        m_entity(entity)
    {
    }

    bool run(unsigned ms) override
    {
        m_timeLeft -= ms;

        // The time has expired, so explode!
        if (m_timeLeft <= 0)
        {
            auto where = m_entity->getPosition();

            m_entity->remove();
            m_level->explode(where);
        }

        return false;
    }

private:
    int m_timeLeft;
    std::shared_ptr<ILevel> m_level;
    std::shared_ptr<IEntity> m_entity;
};

Behavior::Behavior(std::shared_ptr<ILevel> level, std::shared_ptr<IEntity> entity)
{
    switch (entity->getType())
    {
    case EntityType::BOULDER:
        m_traits.push_back(std::unique_ptr<ITrait>(new FallTrait(level, entity)));
        break;
    case EntityType::DIAMOND:
        m_traits.push_back(std::unique_ptr<ITrait>(new FallTrait(level, entity)));
        break;
    case EntityType::BOMB:
        m_traits.push_back(std::unique_ptr<ITrait>(new FallTrait(level, entity)));
        m_traits.push_back(std::unique_ptr<ITrait>(new ExplodeAfterTrait(2000, level, entity)));
        break;
    default:
        break;
    }
}

void Behavior::run(unsigned ms)
{
    for (auto &trait : m_traits)
    {
        // FIXME! Maybe do something about the return value
        trait->run(ms);
    }
}

LevelBehavior::LevelBehavior(std::shared_ptr<ILevel> level)
{
}

void LevelBehavior::run(unsigned ms)
{
    for (auto &trait : m_traits)
    {
        trait->run(ms);
    }
}

std::shared_ptr<IBehavior> IBehavior::fromEntity(std::shared_ptr<ILevel> level, std::shared_ptr<IEntity> entity)
{
    return std::shared_ptr<IBehavior>(new Behavior(level, entity));
}

std::shared_ptr<IBehavior> IBehavior::fromLevel(std::shared_ptr<ILevel> level)
{
    return std::shared_ptr<IBehavior>(new LevelBehavior(level));
}
