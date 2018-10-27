#pragma once

#include <vector>
#include <memory>

#include <observer.hh>

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
	FIREBALL,
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

	virtual uint32_t getId() const = 0;


	// Creation etc
	static std::shared_ptr<IEntity> createFromChar(char c, const point &where);
	static std::shared_ptr<IEntity> createFromType(EntityType type, const point &where);
	static bool isValid(char c);
};

// Singleton keeper of all entities
class IEntityStore
{
public:
	virtual ~IEntityStore()
	{
	}

    virtual std::vector<std::shared_ptr<IEntity>> getEntities() = 0;

    virtual std::shared_ptr<IEntity> getEntityByPoint(const point &where) = 0;


	static std::shared_ptr<IEntityStore> getInstance();
};
