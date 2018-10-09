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

class IEntity
{
public:
	virtual ~IEntity()
	{
	}

	virtual EntityType getType() const = 0;

	static std::unique_ptr<IEntity> fromChar(char c);
};
