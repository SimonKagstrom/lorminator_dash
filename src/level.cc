#include <level.hh>

Level::Level(extents size)  :
	m_size(size)
{
}

Level::~Level()
{
}

const extents &Level::getSize() const
{
	return m_size;
}

std::vector<std::shared_ptr<Entity>> Level::getEntities()
{
	std::vector<std::shared_ptr<Entity>> out;
	return out;
}

bool Level::pointIsPassable(const point &where) const
{
	return true;
}

bool Level::pointIsSolid(const point &where) const
{
	return false;
}

std::optional<TileType> Level::tileAt(const point &where) const
{
	std::optional<TileType> out;
	return out;
}

void Level::explode(const point &where)
{

}


std::unique_ptr<Level> Level::fromString(const std::string &levelString)
{
	std::unique_ptr<Level> out(new Level(extents{0,0}));

	return out;
}
