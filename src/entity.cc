#include <entity.hh>

#include <point.hh>

#include <unordered_map>

static const std::unordered_map<char, EntityType> charToEntity =
{
    {'o', EntityType::BOULDER},
    {'p', EntityType::PLAYER},
    {'f', EntityType::FIREBALL},
};

class Entity : public IEntity
{
public:
    Entity(EntityType type, const point &where);
    ~Entity();

    EntityType getType() const override;

    point getPosition() const override;

    void setPosition(const point &dst) override;

    uint32_t getId() const;

private:
    const EntityType m_type;
    point m_position;
    uint32_t m_id;
};

class EntityStore : public IEntityStore
{
public:
    EntityStore();

    std::vector<std::shared_ptr<IEntity>> getEntities() override;

    void add(std::shared_ptr<IEntity> entity);

private:
    std::unordered_map<uint32_t, std::shared_ptr<IEntity>> m_entities;
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
    m_position = dst;
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

void EntityStore::add(std::shared_ptr<IEntity> entity)
{
    m_entities[entity->getId()] = entity;
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
