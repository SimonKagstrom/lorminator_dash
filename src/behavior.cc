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
        auto cur = m_entity->getPosition();
        auto below = m_level->tileAt(cur + Direction::DOWN);

        if (below && *below == TileType::EMPTY)
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


std::shared_ptr<IBehavior> IBehavior::fromEntity(std::shared_ptr<ILevel> level, std::shared_ptr<IEntity> entity)
{
    return std::shared_ptr<IBehavior>(new Behavior(level, entity));
}
