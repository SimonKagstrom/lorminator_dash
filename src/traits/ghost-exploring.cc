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
                // Not a possible point
                return;
            }

            auto tile = m_level->tileAt(where);
            if (tile && *tile == TileType::EMPTY)
            {
                // Ghosts can only walk to empty positions
                dst.push_back(where);
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

        auto select = [this, intersection](const std::vector<point> &positions)
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
            auto &which = positions[random() % positions.size()];

            for (auto &it : m_visitedPositions)
            {
                if (it.where == which)
                {
                    it.count++;
                    break;
                }
            }

            // Order by count
            std::sort(m_visitedPositions.begin(), m_visitedPositions.end(),
                [](const visit &one, const visit &other)
                {
                     return one.count > other.count;
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
