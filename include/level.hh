#pragma once

#include "tile.hh"
#include "point.hh"

#include <string>
#include <memory>
#include <vector>
#include <optional>

class IEntity;

class ILevel
{
public:
	virtual ~ILevel()
	{
	}

	virtual const struct extents &getSize() const = 0;

	virtual std::vector<std::shared_ptr<IEntity>> getEntities() = 0;

	virtual bool pointIsPassable(const point &where) const = 0;

	virtual bool pointIsSolid(const point &where) const = 0;

	virtual std::optional<TileType> tileAt(const point &where) const = 0;

	virtual void explode(const point &where) = 0;

	virtual std::string toString() const = 0;

	static std::unique_ptr<ILevel> fromString(const std::string &levelString);
	static std::unique_ptr<ILevel> fromFile(const std::string &levelFile);
};
