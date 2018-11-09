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
        std::shared_ptr<ILevel> level = ILevel::fromString(levelData);
        
        if (!level)
        {
            return false;
        }
        m_currentLevel.reset();
        m_currentLevel = std::make_unique<CurrentLevel>(level);

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

        while (1)
        {
            m_currentLevel->run(100);

            io->display(m_currentLevel->getPlayer()->getPosition(),
                m_currentLevel->getLevel(), m_currentLevel->getEntityStore());
            io->delay(100);
        }

        return false;
    }

private:
    class CurrentLevel
    {
    public:
        CurrentLevel(std::shared_ptr<ILevel> level) :
            m_level(level),
            m_entityStore(IEntityStore::getInstance()),
            m_entityProperties(IEntityProperties::getInstance())
        {
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
        }

        void run(unsigned ms)
        {
            for (auto &it : m_behavior)
            {
                it.second->run(ms);
            }
        }

        std::shared_ptr<IEntity> getPlayer()
        {
            return m_player;
        }

        std::shared_ptr<IEntityStore> getEntityStore()
        {
            return m_entityStpre;
        }

        std::shared_ptr<ILevel> getLevel()
        {
            return m_level;
        }

private:
        std::shared_ptr<ILevel> m_level;
        std::shared_ptr<IEntityStore> m_entityStore;
        std::shared_ptr<IEntityProperties> m_entityProperties;
        std::shared_ptr<IEntity> m_player;

        std::unordered_map<uint32_t, std::unique_ptr<IBehavior>> m_behavior;
    };

    std::unique_ptr<CurrentLevel> m_currentLevel;
};
