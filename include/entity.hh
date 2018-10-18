#pragma once

#include <memory>

struct point;

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

class IEntity
{
public:
	virtual ~IEntity()
	{
	}

	virtual EntityType getType() const = 0;

	virtual point getPosition() const = 0;

	virtual void setPosition(const point &dst) = 0;

	static std::shared_ptr<IEntity> fromChar(char c, const point &where);
	static bool isValid(char c);
};

// Singleton keeper of all entities
class IEntityStore
{
public:
	virtual ~IEntityStore()
	{
	}

	static std::shared_ptr<IEntityStore> getInstance();
};
