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

        auto cur = m_entity->getPosition();
        auto below = m_level->tileAt(cur + Direction::DOWN);

        auto entityBelow = IEntityStore::getInstance()->getEntityByPoint(cur + Direction::DOWN);

        // Standing on an entity?
        if (entityBelow && entityBelow->getType() != EntityType::PLAYER)
        {
            // Fall is it's free to the side and down
            auto left = m_level->tileAt(cur + Direction::LEFT);
            auto downLeft = m_level->tileAt((cur + Direction::LEFT) + Direction::DOWN);
            auto right = m_level->tileAt(cur + Direction::RIGHT);
            auto downRight = m_level->tileAt((cur + Direction::RIGHT) + Direction::DOWN);

            if (isEmpty(left) && isEmpty(downLeft))
            {
                m_entity->setPosition(cur + Direction::LEFT);
                return true;
            }
            else if (isEmpty(right) && isEmpty(downRight))
            {
                m_entity->setPosition(cur + Direction::RIGHT);
                return true;
            }
        }
        else if (!entityBelow && isEmpty(below))
        {
            m_entity->setPosition(cur + Direction::DOWN);

            return true;
        }

        return false;
    }

private:
    std::shared_ptr<ILevel> m_level;
    std::shared_ptr<IEntity> m_entity;
};

class ExplosionTrait : public ITrait
{
public:
    ExplosionTrait(std::shared_ptr<ILevel> level) :
        m_level(level)
    {
    }

    bool run(unsigned ms) override
    {
        return false;
    }

private:
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
