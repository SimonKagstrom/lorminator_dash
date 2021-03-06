#include <behavior.hh>
#include <point.hh>
#include <entity.hh>
#include <level.hh>

#include "itrait.hh"

#include <memory>
#include <vector>
#include <assert.h>

class GhostWalkingTrait : public ITrait
{
public:
    GhostWalkingTrait(std::shared_ptr<ILevel> level, std::shared_ptr<IEntity> entity) :
        m_level(level),
        m_entity(entity)
    {
    }

    bool run(unsigned ms) override
    {
        auto walkable = [this](std::vector<point> &dst, const point &where)
        {
            auto ent = IEntityStore::getInstance()->getEntityByPoint(where);

            if (ent)
            {
                if (ent->getType() == EntityType::PLAYER)
                {
                    // OK, a bit ugly to detect here, but anyways
                    m_level->explode(where);
                }

                // Not a possible point
                return;
            }

            auto tile = m_level->tileAt(where);
            if (tile)
            {
                // Ghosts can only walk to some positions
                if (*tile == TileType::EMPTY ||
                    *tile == TileType::TELEPORTER)
                {
                    dst.push_back(where);
                }
            }
        };

        auto lookup = [this](const point &where)
        {
           return std::find_if(m_visitedPositions.begin(), m_visitedPositions.end(),
                [where](const visit &cur)
                {
                    return cur.where == where;
                });
        };

        auto intersection = [this, lookup](const std::vector<point> &positions)
        {
            std::vector<point> out;

            for (auto &it : positions)
            {
                auto cur = lookup(it);
                if (cur == m_visitedPositions.end())
                {
                    out.push_back(it);
                }
            }

            return out;
        };

        auto select = [this, lookup, intersection](const std::vector<point> &positions)
        {
            auto unique = intersection(positions);

            if (!unique.empty())
            {
                auto &which = unique[random() % unique.size()];

                // Unvisited position
                if (m_visitedPositions.size() >= m_visitLimit)
                {
                    m_visitedPositions.pop_back();
                }
                m_visitedPositions.push_back({which, 1});

                return which;
            }

            // All has been visited before
            unsigned smallest = 0xffffffff;
            auto which = positions[0];

            auto selected = m_visitedPositions.end();
            for (auto &cur : positions)
            {
                auto it = lookup(cur);
                assert(it != m_visitedPositions.end());

                if (it->count < smallest)
                {
                    which = it->where;
                    selected = it;
                    smallest = it->count;
                }
            }
            assert(selected != m_visitedPositions.end());
            selected->count++;

            // Order by count
            std::sort(m_visitedPositions.begin(), m_visitedPositions.end(),
                [](const visit &one, const visit &other)
                {
                     return one.count >= other.count;
                });

            return which;
        };

        auto pos = m_entity->getPosition();
        std::vector<point> examine;

        walkable(examine, pos + Direction::UP);
        walkable(examine, pos + Direction::DOWN);
        walkable(examine, pos + Direction::LEFT);
        walkable(examine, pos + Direction::RIGHT);

        if (examine.size() == 0)
        {
            // I'm stuck!
            return false;
        }

        auto where = select(examine);
        m_entity->setPosition(where);

        return false;
    }

private:
    struct visit
    {
        point    where;
        uint32_t count {0};
    };

    std::vector<visit> m_visitedPositions;

    const uint32_t m_visitLimit{20}; // The maximum number of positions to remember
    Direction m_dir{Direction::UP};
    std::shared_ptr<ILevel> m_level;
    std::shared_ptr<IEntity> m_entity;
};
