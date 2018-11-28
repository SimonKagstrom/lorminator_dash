#include <game.hh>
#include <level.hh>
#include <entity.hh>
#include <entity-properties.hh>
#include <behavior.hh>
#include <animator.hh>
#include <io.hh>
#include <resource-store.hh>
#include <lightning.hh>
#include <level-animator.hh>

#include <memory>

class Game : public IGame
{
public:
    Game()
    {
    }

    bool setLevel(const std::string &levelData) override
    {
        m_currentLevel.reset();
        auto cur = std::make_unique<CurrentLevel>();

        if (!cur->setLevel(levelData))
        {
            return false;
        }
        m_currentLevel = std::move(cur);

        return true;
    }

    bool play() override
    {
        auto io = IIo::getInstance();

        if (!m_currentLevel)
        {
            // Cannot play in that case
            return false;
        }

        auto player = m_currentLevel->getPlayer();
        bool playerAlive = true;

        // Check for player aliveness
        unsigned timeSinceDead = 0;
        auto cookie = player->onRemoval([&timeSinceDead, &playerAlive, io](std::shared_ptr<IEntity> entity)
        {
            playerAlive = false;
            timeSinceDead = io->msSince(0);
        });

        auto lightning = m_currentLevel->getLightning();
        auto level = m_currentLevel->getLevel();
        auto levelAnimator = m_currentLevel->getLevelAnimator();

        while (1)
        {
            if (!playerAlive && io->msSince(timeSinceDead) > 2500)
            {
                //  Didn't pass this level
                return false;
            }

            m_currentLevel->run(160);
            auto lighted = level->getIllumination(player->getPosition(), player->getDirection());
            lightning->updateLightning(lighted);

            for (unsigned i = 0; i < 8; i++)
            {
                animate(i);
                io->display(player, m_currentLevel->getLevel(), lightning, levelAnimator, m_currentLevel->getAnimators());
                io->delay(160/8);
            }
        }

        return true;
    }

private:
    class CurrentLevel
    {
    public:
        CurrentLevel() :
            m_entityStore(IEntityStore::getInstance()),
            m_entityProperties(IEntityProperties::getInstance()),
            m_resourceStore(IResourceStore::getInstance())
        {
        }

        bool setLevel(const std::string &str)
        {
            m_level = ILevel::fromString(str);

            if (!m_level)
            {
                return false;
            }

            auto entities = m_entityStore->getEntities();

            // Create behavior
            m_levelBehavior = IBehavior::fromLevel(m_level);
            for (auto &it : entities)
            {
                if (it->getType() == EntityType::PLAYER)
                {
                    m_player = it;
                }

                addEntity(it);
            }

            // And listen for new creations
            m_cookie = m_entityStore->onCreation([this](std::shared_ptr<IEntity> entity)
            {
                addEntity(entity);
            });

            m_lightning = ILightning::create(m_level);
            m_levelAnimator = ILevelAnimator::fromLightning(m_lightning);

            return m_player != nullptr;
        }

        void run(unsigned ms)
        {
            for (auto &it : m_behavior)
            {
                it.second->run(ms);
            }
            m_levelBehavior->run(ms);

            // Remove all now invalid behaviors
            for (auto &it : m_toErase)
            {
                m_behavior.erase(it);
            }
        }

        std::shared_ptr<IEntity> getPlayer() const
        {
            return m_player;
        }

        std::shared_ptr<IEntityStore> getEntityStore() const
        {
            return m_entityStore;
        }

        std::shared_ptr<ILevel> getLevel() const
        {
            return m_level;
        }

        std::shared_ptr<ILightning> getLightning() const
        {
            return m_lightning;
        }

        std::shared_ptr<ILevelAnimator> getLevelAnimator() const
        {
            return m_levelAnimator;
        }

        std::unordered_map<uint32_t, std::shared_ptr<IAnimator>> &getAnimators()
        {
            return m_animators;
        }

private:
        void addEntity(std::shared_ptr<IEntity> entity)
        {
            auto id = entity->getId();

            m_behavior[id] = IBehavior::fromEntity(m_level, entity);
            m_animators[id] = IAnimator::fromEntity(entity, m_resourceStore->getFrameExtents(), 8);

            m_removalCookies[id] = entity->onRemoval([this](std::shared_ptr<IEntity> toRemove)
            {
                auto id = toRemove->getId();

                auto it = m_behavior.find(id);
                if (it != m_behavior.end())
                {
                    m_toErase.push_back(it->first);
                }
                m_removalCookies.erase(id);
                m_animators.erase(id);
            });
        }

        std::shared_ptr<ILevel> m_level;
        std::shared_ptr<ILightning> m_lightning;
        std::shared_ptr<ILevelAnimator> m_levelAnimator;
        std::shared_ptr<IEntityStore> m_entityStore;
        std::shared_ptr<IEntityProperties> m_entityProperties;
        std::shared_ptr<IResourceStore> m_resourceStore;
        std::shared_ptr<IEntity> m_player;

        std::unordered_map<uint32_t, std::unique_ptr<IBehavior>> m_behavior;
        std::unordered_map<uint32_t, std::unique_ptr<ObserverCookie>> m_removalCookies;

        std::unordered_map<uint32_t, std::shared_ptr<IAnimator>> m_animators;

        std::vector<uint32_t> m_toErase;

        std::unique_ptr<IBehavior> m_levelBehavior;
        std::unique_ptr<ObserverCookie> m_cookie;
    };

    void animate(unsigned round)
    {
        for (auto &it : m_currentLevel->getAnimators())
        {
            it.second->animate(round);
        }
        m_currentLevel->getLevelAnimator()->animate(round);
    }

    std::unique_ptr<CurrentLevel> m_currentLevel;
};



std::shared_ptr<IGame> IGame::create()
{
    auto out = std::make_shared<Game>();

    return out;
}
