#include <behavior.hh>

#include <level.hh>
#include <entity.hh>

#include <list>
#include <map>

#include <stdlib.h>

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

class LevelBehavior : public IBehavior
{
public:
    LevelBehavior(std::shared_ptr<ILevel> level);

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
        auto isEmpty = [](const std::optional<TileType> tile)
        {
            if (tile)
            {
                return *tile == TileType::EMPTY;
            }

            return false;
        };

        auto entityShouldBeDestroyed = [](const std::shared_ptr<IEntity> ent)
        {
            if (!ent)
            {
                return false;
            }

            switch (ent->getType())
            {
            case EntityType::PLAYER:
            case EntityType::GHOST:
            case EntityType::BOMB:
                return true;
            default:
                break;
            }

            return false;
        };

        auto cur = m_entity->getPosition();
        auto below = m_level->tileAt(cur + Direction::DOWN);

        auto entityBelow = IEntityStore::getInstance()->getEntityByPoint(cur + Direction::DOWN);

        // Falling on an entity?
        if (isFalling() && entityShouldBeDestroyed(entityBelow))
        {
            m_entity->remove();
            entityBelow->remove();

            m_level->explode(cur + Direction::DOWN);

            return true;
        }
        // Standing on an entity
        else if (entityBelow)
        {
            // Fall is it's free to the side and down
            auto left = m_level->tileAt(cur + Direction::LEFT);
            auto downLeft = m_level->tileAt((cur + Direction::LEFT) + Direction::DOWN);
            auto right = m_level->tileAt(cur + Direction::RIGHT);
            auto downRight = m_level->tileAt((cur + Direction::RIGHT) + Direction::DOWN);

            if (isEmpty(left) && isEmpty(downLeft))
            {
                m_entity->setPosition(cur + Direction::LEFT);
                return fall();
            }
            else if (isEmpty(right) && isEmpty(downRight))
            {
                m_entity->setPosition(cur + Direction::RIGHT);
                return fall();
            }
        }
        else if (!entityBelow && isEmpty(below))
        {
            m_entity->setPosition(cur + Direction::DOWN);

            return fall();
        }

        return dontFall();
    }

private:
    bool isFalling() const
    {
        return m_falling;
    }

    bool fall()
    {
        m_falling = true;
        return true;
    }

    bool dontFall()
    {
        m_falling = false;
        return false;
    }

    bool m_falling{false};
    std::shared_ptr<ILevel> m_level;
    std::shared_ptr<IEntity> m_entity;
};

class ExplodeAfterTrait : public ITrait
{
public:
    ExplodeAfterTrait(int ms, std::shared_ptr<ILevel> level, std::shared_ptr<IEntity> entity) :
        m_timeLeft(ms),
        m_level(level),
        m_entity(entity)
    {
    }

    bool run(unsigned ms) override
    {
        m_timeLeft -= ms;

        // The time has expired, so explode!
        if (m_timeLeft <= 0)
        {
            auto where = m_entity->getPosition();

            m_entity->remove();
            m_level->explode(where);
        }

        return false;
    }

private:
    int m_timeLeft;
    std::shared_ptr<ILevel> m_level;
    std::shared_ptr<IEntity> m_entity;
};


class DisappearAfterTrait : public ITrait
{
public:
    DisappearAfterTrait(int ms, std::shared_ptr<IEntity> entity) :
        m_timeLeft(ms),
        m_entity(entity)
    {
    }

    bool run(unsigned ms) override
    {
        m_timeLeft -= ms;

        // The time has expired, so remove
        if (m_timeLeft <= 0)
        {
            m_entity->remove();
        }

        return false;
    }

private:
    int m_timeLeft;
    std::shared_ptr<IEntity> m_entity;
};

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
            if (m_timeout < 0)
            {
                auto dst = m_teleporterLocations[random() % m_teleporterLocations.size()];
                toTeleport->setPosition(dst);
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

class CollisionTrait : public ITrait
{
public:
    CollisionTrait()
    {
        auto store = IEntityStore::getInstance();

        m_cookie = store->onCollision([this](std::shared_ptr<IEntity> one, std::shared_ptr<IEntity> other){onCollision(one, other);});
    }

    bool run(unsigned ms) override
    {
        // Handled in the callback
        return false;
    }

    void onCollision(std::shared_ptr<IEntity> one, std::shared_ptr<IEntity> other)
    {
        if (one->getType() == EntityType::FIREBALL)
        {
            other->remove();
        }
        else if (other->getType() == EntityType::FIREBALL)
        {
            one->remove();
        }

        // More to come
    }

private:
    std::unique_ptr<ObserverCookie> m_cookie;
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
