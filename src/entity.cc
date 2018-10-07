#include <entity.hh>

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

std::unique_ptr<Entity> Entity::fromChar(char c)
{
	switch (c)
	{
	case 'o':
		return std::unique_ptr<Entity>(new Entity(EntityType::BOULDER));
	case 'p':
		return std::unique_ptr<Entity>(new Entity(EntityType::PLAYER));
	}

	return nullptr;
}

