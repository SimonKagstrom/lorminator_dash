#include <behavior.hh>
#include <point.hh>
#include <entity.hh>
#include <level.hh>

#include "itrait.hh"

#include <memory>
#include <vector>

class TeleporterTrait : public ITrait
{
public:
    TeleporterTrait(std::shared_ptr<ILevel> level, int delay) :
        m_level(level),
        m_delay(delay),
        m_timeout(delay)
    {
        locateTeleporters();
    }

    bool run(unsigned ms) override
    {
        auto store = IEntityStore::getInstance();

        std::shared_ptr<IEntity> toTeleport;

        for (auto &where : m_teleporterLocations)
        {
            auto ent = store->getEntityByPoint(where);

            if (ent)
            {
                toTeleport = ent;
                break;
            }
        }

        if (toTeleport)
        {
            m_timeout -= ms;
            if (m_timeout <= 0)
            {
                point dst;

                do
                {
                    dst = m_teleporterLocations[random() % m_teleporterLocations.size()];
                } while (dst == toTeleport->getPosition());

                auto entAtDst = store->getEntityByPoint(dst);

                if (entAtDst)
                {
                    // An entity on the destination - explode and destroy the teleporters!
                    for (auto &cur : m_teleporterLocations)
                    {
                        m_level->explode(cur);
                        m_level->setTile(cur, TileType::EMPTY);
                    }

                    m_teleporterLocations.clear();
                }
                else
                {
                    toTeleport->setPosition(dst);
                }
                m_timeout = m_delay;
            }
        }
        else
        {
            m_timeout = m_delay;
        }

        return false;
    }

private:
    void locateTeleporters()
    {
        for (int y = 0; y < m_level->getSize().height; y++)
        {
            for (int x = 0; x < m_level->getSize().width; x++)
            {
                auto tile = m_level->tileAt({x,y});

                if (tile && *tile == TileType::TELEPORTER)
                {
                    m_teleporterLocations.push_back({x,y});
                }
            }
        }
    }

    std::shared_ptr<ILevel> m_level;
    const int m_delay;
    int m_timeout;

    std::vector<point> m_teleporterLocations;
};
    