#pragma once

#include "tile.hh"
#include "point.hh"

#include <string>
#include <memory>
#include <vector>
#include <set>
#include <optional>

class IEntity;

class ILevel
{
public:
	virtual ~ILevel()
	{
	}

	virtual const struct extents &getSize() const = 0;

	virtual bool pointIsPassable(const point &where) const = 0;

	virtual bool pointIsSolid(const point &where) const = 0;

	virtual std::optional<TileType> tileAt(const point &where) const = 0;

	virtual void explode(const point &where) = 0;

	/**
	 * Get the flashlight cone from a point
	 */
	virtual std::set<point> getIllumination(const point &where, Direction dir) = 0;

	virtual std::string toString() const = 0;

	static std::unique_ptr<ILevel> fromString(const std::string &levelString);
	static std::unique_ptr<ILevel> fromFile(const std::string &levelFile);
};
