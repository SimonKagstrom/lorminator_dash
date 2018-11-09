#include <game.hh>
#include <level.hh>
#include <entity.hh>
#include <entity-properties.hh>
#include <behavior.hh>
#include <io.hh>

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
        auto cookie = player->onRemoval([&playerAlive](std::shared_ptr<IEntity> entity)
        {
            playerAlive = false;
        });

        while (1)
        {
            if (!playerAlive)
            {
                //  Didn't pass this level
                return false;
            }

            m_currentLevel->run(100);

            io->display(player->getPosition(), m_currentLevel->getLevel(), m_currentLevel->getEntityStore());
            io->delay(100);
        }

        return true;
    }

private:
    class CurrentLevel
    {
    public:
        CurrentLevel() :
            m_entityStore(IEntityStore::getInstance()),
            m_entityProperties(IEntityProperties::getInstance())
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
            for (auto &it : entities)
            {
                if (it->getType() == EntityType::PLAYER)
                {
                    m_player = it;
                }

                m_behavior[it->getId()] = IBehavior::fromEntity(m_level, it);
            }

            // And listen for new creations
            m_cookie = m_entityStore->onCreation([this](std::shared_ptr<IEntity> entity)
            {
                m_behavior[entity->getId()] = IBehavior::fromEntity(m_level, entity);
            });

            return m_player != nullptr;
        }

        void run(unsigned ms)
        {
            for (auto &it : m_behavior)
            {
                it.second->run(ms);
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

private:
        std::shared_ptr<ILevel> m_level;
        std::shared_ptr<IEntityStore> m_entityStore;
        std::shared_ptr<IEntityProperties> m_entityProperties;
        std::shared_ptr<IEntity> m_player;

        std::unordered_map<uint32_t, std::unique_ptr<IBehavior>> m_behavior;
        std::unique_ptr<ObserverCookie> m_cookie;
    };

    std::unique_ptr<CurrentLevel> m_currentLevel;
};



std::shared_ptr<IGame> IGame::create()
{
    auto out = std::make_shared<Game>();

    return out;
}
