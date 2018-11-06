#include <behavior.hh>
#include <point.hh>
#include <entity.hh>
#include <level.hh>

#include "itrait.hh"

#include <memory>
#include <vector>

class TransporterTrait : public ITrait
{
public:
    TransporterTrait(std::shared_ptr<ILevel> level)
    {
        locateBands(level);
    }

    bool run(unsigned ms) override
    {
        for (auto &band : m_bands)
        {
            band.runBand();
        }

        return true;
    }

private:
    class Band
    {
    public:
        Band(std::shared_ptr<ILevel> level, point &start, const point &end, Direction dir) :
            m_level(level),
            m_start(start),
            m_end(end),
            m_dir(dir)
        {
        }

        void runBand()
        {
            auto store = IEntityStore::getInstance();

            point start = {m_start.x, m_start.y - 1};
            point end = {m_end.x, m_end.y - 1};

            std::vector<std::shared_ptr<IEntity>> toMove;

            for (auto cur = start; cur != end; cur = cur + Direction::RIGHT)
            {
                auto ent = store->getEntityByPoint(cur);

                if (ent)
                {
                    toMove.push_back(ent);
                }
            }

            for (auto &ent : toMove)
            {
                auto dst = ent->getPosition() + m_dir;
                auto dstTile = m_level->tileAt(dst);

                if (!dstTile)
                {
                    continue;
                }
                if (*dstTile != TileType::EMPTY)
                {
                    // Can't transport through walls etc
                    continue;
                }

                if (store->getEntityByPoint(dst))
                {
                    // Don't collide with other entities
                    continue;
                }

                ent->setPosition(dst);
            }
        }

    private:
        std::shared_ptr<ILevel> m_level;
        const point m_start;
        const point m_end;
        Direction m_dir;
    };

    void locateBands(std::shared_ptr<ILevel> level)
    {
        for (int y = 0; y < level->getSize().height; y++)
        {
            // We only support horizontal bands
            point start = {-1, y};
            bool left = true;

            for (int x = 0; x < level->getSize().width; x++)
            {
                auto cur = level->tileAt({x,y});

                if (!cur)
                {
                    // Should never happen, but anyway
                    continue;
                }
                if (*cur != TileType::LEFT_TRANSPORT && *cur != TileType::RIGHT_TRANSPORT)
                {
                    // End of band
                    if (start.x != -1)
                    {
                        m_bands.push_back(Band(level, start, {x,y}, left ? Direction::LEFT : Direction::RIGHT));
                        start.x = -1;
                    }

                    continue;
                }

                // Start of band?
                if (start.x == -1)
                {
                    start.x = x;
                    left = *cur == TileType::LEFT_TRANSPORT;
                }
            }
        }
    }

    std::vector<Band> m_bands;
};
