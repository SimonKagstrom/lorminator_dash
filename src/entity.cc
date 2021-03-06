#include <entity.hh>

#include <point.hh>

#include <unordered_map>

static const std::unordered_map<char, EntityType> charToEntity =
{
    {'o', EntityType::BOULDER},
    {'d', EntityType::DIAMOND},
    {'b', EntityType::BOMB},
    {'p', EntityType::PLAYER},
    {'f', EntityType::FIREBALL},
    {'g', EntityType::GHOST},
};

class Entity : public IEntity, public std::enable_shared_from_this<Entity>
{
public:
    Entity(EntityType type, const point &where);
    ~Entity();

    EntityType getType() const override;

    point getPosition() const override;

    void setPosition(const point &dst) override;

	Direction getDirection() const override;

	void setDirection(Direction dir) override;

    uint32_t getId() const override;

    void remove() override;

    std::unique_ptr<ObserverCookie> onRemoval(std::function<void(std::shared_ptr<IEntity>)> cb) override;

    std::unique_ptr<ObserverCookie> onMovement(std::function<void(std::shared_ptr<IEntity>,
            const point &from, const point &to)> cb) override;

private:
    const EntityType m_type;
    point m_position;
    uint32_t m_id;
    Direction m_direction{Direction::NONE};

    Notifier1<std::shared_ptr<IEntity>> m_onRemoval;
    Notifier3<std::shared_ptr<IEntity>, const point &, const point &> m_onMovement;
};

class EntityStore : public IEntityStore
{
public:
    EntityStore();

    std::vector<std::shared_ptr<IEntity>> getEntities() override;

    std::shared_ptr<IEntity> getEntityByPoint(const point &where) override;

    std::shared_ptr<IEntity> getEntityById(uint32_t id) override;

    void add(std::shared_ptr<IEntity> entity);

    std::unique_ptr<ObserverCookie> onCreation(std::function<void(std::shared_ptr<IEntity> entity)> cb) override;

    std::unique_ptr<ObserverCookie> onCollision(std::function<void(std::shared_ptr<IEntity> one, std::shared_ptr<IEntity> other)> cb) override;

private:
    std::unordered_map<uint32_t, std::shared_ptr<IEntity>> m_entities;
    std::unordered_map<point, std::shared_ptr<IEntity>> m_entitiesByPoint;

    std::unordered_map<uint32_t, std::unique_ptr<ObserverCookie>> m_movementCookies;
    std::unordered_map<uint32_t, std::unique_ptr<ObserverCookie>> m_removalCookies;

    Notifier2<std::shared_ptr<IEntity>, std::shared_ptr<IEntity>> m_onCollision;
    Notifier1<std::shared_ptr<IEntity>> m_onCreation;
};



Entity::Entity(EntityType type, const point &where) :
    m_type(type),
    m_position(where)
{
    static uint32_t g_entityId = 1;

    m_id = g_entityId++;
}

Entity::~Entity()
{
}

EntityType Entity::getType() const
{
    return m_type;
}

point Entity::getPosition() const
{
    return m_position;
}

uint32_t Entity::getId() const
{
    return m_id;
}

void Entity::setPosition(const point &dst)
{
    m_onMovement.invoke(shared_from_this(), m_position, dst);
    m_position = dst;
}

Direction Entity::getDirection() const
{
    return m_direction;
}

void Entity::setDirection(Direction dir)
{
    m_direction = dir;
}

void Entity::remove()
{
    m_onRemoval.invoke(shared_from_this());
}

std::unique_ptr<ObserverCookie> Entity::onRemoval(std::function<void(std::shared_ptr<IEntity>)> cb)
{
    return m_onRemoval.listen(cb);
}

std::unique_ptr<ObserverCookie> Entity::onMovement(std::function<void(std::shared_ptr<IEntity>,
        const point &from, const point &to)> cb)
{
    return m_onMovement.listen(cb);
}



bool IEntity::isValid(char c)
{
    if (charToEntity.find(c) == charToEntity.end())
    {
        return false;
    }

    return true;
}

std::shared_ptr<IEntity> IEntity::createFromChar(char c, const point &where)
{
    auto it = charToEntity.find(c);

    if (it == charToEntity.end())
    {
        return nullptr;
    }

    return createFromType(it->second, where);
}

std::shared_ptr<IEntity> IEntity::createFromType(EntityType type, const point &where)
{
    std::shared_ptr<IEntity> out;

    out = std::shared_ptr<IEntity>(new Entity(type, where));

    auto store = std::dynamic_pointer_cast<EntityStore>(IEntityStore::getInstance());

    store->add(out);

    return out;
}

EntityStore::EntityStore()
{
}

std::vector<std::shared_ptr<IEntity>> EntityStore::getEntities()
{
    std::vector<std::shared_ptr<IEntity>> out;

    for (auto &it : m_entities)
    {
        out.push_back(it.second);
    }

    return out;
}

std::shared_ptr<IEntity> EntityStore::getEntityByPoint(const point &where)
{
    auto it = m_entitiesByPoint.find(where);

    if (it == m_entitiesByPoint.end())
    {
        return nullptr;
    }

    return it->second;
}

std::shared_ptr<IEntity> EntityStore::getEntityById(uint32_t id)
{
    auto it = m_entities.find(id);

    if (it == m_entities.end())
    {
        return nullptr;
    }

    return it->second;
}

void EntityStore::add(std::shared_ptr<IEntity> entity)
{
    m_removalCookies[entity->getId()] = entity->onRemoval([this](std::shared_ptr<IEntity> entity)
    {
        m_removalCookies.erase(entity->getId());
        m_entities.erase(entity->getId());
        m_entitiesByPoint.erase(entity->getPosition());
    });

    // Check for collisions on movement
    m_movementCookies[entity->getId()] = entity->onMovement([this](std::shared_ptr<IEntity> ent, const point &from, const point &to)
    {
        auto other = getEntityByPoint(to);

        if (other)
        {
            m_onCollision.invoke(ent, other);
        }
        m_entitiesByPoint.erase(from);
        m_entitiesByPoint[to] = ent;
    });

    // Another already here?
    auto other = getEntityByPoint(entity->getPosition());
    if (other)
    {
        m_onCollision.invoke(entity, other);
    }

    m_entities[entity->getId()] = entity;
    m_entitiesByPoint[entity->getPosition()] = entity;
    m_onCreation.invoke(entity);
}

std::unique_ptr<ObserverCookie> EntityStore::onCreation(std::function<void(std::shared_ptr<IEntity> entity)> cb)
{
    return m_onCreation.listen(cb);
}

std::unique_ptr<ObserverCookie> EntityStore::onCollision(std::function<void(std::shared_ptr<IEntity> one,
        std::shared_ptr<IEntity> other)> cb)
{
    return m_onCollision.listen(cb);
}

std::shared_ptr<IEntityStore> IEntityStore::getInstance()
{
    // See https://dakerfp.github.io/post/weak_ptr_singleton/
    static std::weak_ptr<IEntityStore> g_instance;

    if (auto p = g_instance.lock())
    {
        return p;
    }

    // Create a new one
    auto p = std::shared_ptr<IEntityStore>(new EntityStore());
    g_instance = p;

    return p;
}
