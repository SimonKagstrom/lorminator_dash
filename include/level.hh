#pragma once

#include "tile.hh"

#include <string>
#include <memory>
#include <vector>
#include <optional>

enum class Direction
{
	UP,
	DOWN,
	LEFT,
	RIGHT
};

struct extents
{
	unsigned width{0};
	unsigned height{0};

	bool operator==(const extents &other) const
	{
		return width == other.width &&
				height == other.height;
	}
};

struct point
{
	unsigned x{0};
	unsigned y{0};

	point operator+(const Direction other) const
	{
		auto out = *this;

		if (other == Direction::UP)
		{
			out.y--;
		}
		else if (other == Direction::DOWN)
		{
			out.y++;
		}
		else if (other == Direction::LEFT)
		{
			out.x--;
		}
		else if (other == Direction::RIGHT)
		{
			out.x++;
		}

		return out;
	}
};

class Entity;

class Level
{
public:
	virtual ~Level();

	virtual const struct extents &getSize() const;

	virtual std::vector<std::shared_ptr<Entity>> getEntities();

	virtual bool pointIsPassable(const point &where) const;

	virtual bool pointIsSolid(const point &where) const;

	virtual std::optional<TileType> tileAt(const point &where) const;

	virtual void explode(const point &where);

	static std::unique_ptr<Level> fromString(const std::string &levelString);
	static std::unique_ptr<Level> fromFile(const std::string &levelFile);

private:
	Level(extents size);

	extents m_size;
};
