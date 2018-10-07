#pragma once

#include <memory>

enum class EntityType
{
	BOULDER,
	BLOCK,
	GHOST,
	PLAYER,
	DIAMOND,
	BOMBS,
	IRON_KEY,
	GOLD_KEY,
	RED_KEY,
};

class Entity
{
public:
	virtual ~Entity();

	EntityType getType() const;

	static std::unique_ptr<Entity> fromChar(char c);

private:
	Entity(EntityType type);

	const EntityType m_type;
};
