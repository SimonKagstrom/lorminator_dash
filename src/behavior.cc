#include <behavior.hh>

#include <level.hh>
#include <entity.hh>

#include <list>
#include <map>

#include <stdlib.h>

// Ok, not very nice perhaps
#include "traits/falling.cc"
#include "traits/exploding.cc"
#include "traits/disappear-after.cc"
#include "traits/ghost-exploring.cc"
#include "traits/colliding.cc"
#include "traits/player-walk.cc"

#include "traits/transporter-bands.cc"
#include "traits/teleporting.cc"

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
    case EntityType::FIREBALL:
        m_traits.push_back(std::unique_ptr<ITrait>(new FallTrait(level, entity)));
        m_traits.push_back(std::unique_ptr<ITrait>(new DisappearAfterTrait(1000, entity)));
        break;
    case EntityType::GHOST:
        m_traits.push_back(std::unique_ptr<ITrait>(new GhostWalkingTrait(level, entity)));
        break;
    case EntityType::PLAYER:
        m_traits.push_back(std::unique_ptr<ITrait>(new player::Walk(level, entity)));
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
    m_traits.push_back(std::unique_ptr<ITrait>(new CollisionTrait()));
    m_traits.push_back(std::unique_ptr<ITrait>(new TransporterTrait(level)));
    m_traits.push_back(std::unique_ptr<ITrait>(new TeleporterTrait(level, 1500)));
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
