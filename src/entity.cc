#include <entity.hh>

#include <point.hh>

class Entity : public IEntity
{
public:
	Entity(EntityType type, const point &where);
	~Entity();

	EntityType getType() const override;

	point getPosition() const override;

	void setPosition(const point &dst) override;

private:
	const EntityType m_type;
	point m_position;
};

class EntityStore : public IEntityStore
{
public:
	EntityStore();
};



Entity::Entity(EntityType type, const point &where) :
	m_type(type),
	m_position(where)
{
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

void Entity::setPosition(const point &dst)
{
	m_position = dst;
}

bool IEntity::isValid(char c)
{
	std::string validChars = "op";

	if (validChars.find(c) == std::string::npos)
	{
		return false;
	}

	return true;
}

std::shared_ptr<IEntity> IEntity::fromChar(char c, const point &where)
{
	if (!isValid(c))
	{
		return nullptr;
	}

	switch (c)
	{
	case 'o':
		return std::shared_ptr<IEntity>(new Entity(EntityType::BOULDER, where));
	case 'p':
		return std::shared_ptr<IEntity>(new Entity(EntityType::PLAYER, where));
	}

	return nullptr;
}


EntityStore::EntityStore()
{
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
