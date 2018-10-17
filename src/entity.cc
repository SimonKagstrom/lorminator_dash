#include <entity.hh>

#include <point.hh>

class Entity : public IEntity
{
public:
	Entity(EntityType type, const point &where);
	~Entity();

	EntityType getType() const override;

private:
	const EntityType m_type;
	point m_position;
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

