#include <game.hh>
#include <level.hh>
#include <entity.hh>
#include <entity-properties.hh>

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
        return false;
    }

private:
    class CurrentLevel
    {
    public:
        CurrentLevel(std::shared_ptr<ILevel> level)
        {
        }

    private:
        std::shared_ptr<ILevel> m_level;
        std::shared_ptr<IEntityStore> m_entityStore;
        std::shared_ptr<IEntityProperties> m_entityProperties;
    };

    std::unique_ptr<CurrentLevel> m_currentLevel;
};
