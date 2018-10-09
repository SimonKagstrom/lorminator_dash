#include <entity.hh>

class Entity : public IEntity
{
public:
	Entity(EntityType type);
	~Entity();

	EntityType getType() const override;

private:
	const EntityType m_type;
};

Entity::Entity(EntityType type) :
	m_type(type)
{
}

Entity::~Entity()
{
}

EntityType Entity::getType() const
{
	return m_type;
}

std::unique_ptr<IEntity> IEntity::fromChar(char c)
{
	switch (c)
	{
	case 'o':
		return std::unique_ptr<IEntity>(new Entity(EntityType::BOULDER));
	case 'p':
		return std::unique_ptr<IEntity>(new Entity(EntityType::PLAYER));
	}

	return nullptr;
}

