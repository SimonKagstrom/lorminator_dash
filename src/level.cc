#include <level.hh>
#include <utils.hh>

#include <algorithm>

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
	auto words = split_string(levelString, " ");
	if (words.size() < 2)
	{
		return nullptr;
	}
	if (!string_is_integer(words[0]) || !string_is_integer(words[1]))
	{
		return nullptr;
	}

	extents size = {(unsigned)string_to_integer(words[0]), (unsigned)string_to_integer(words[1])};

	// Empty
	if (size == extents{0,0})
	{
		return std::unique_ptr<Level>(new Level(size));
	}

	// width. We know the words exist because of the split above
	auto tmp = levelString.find(' ');
	auto dataIdx = levelString.find(' ', tmp) + 1; // Data starts afterwards

	// The data portion, without newlines
	auto data = levelString.substr(dataIdx);
	data.erase(std::remove(data.begin(), data.end(), '\n'), data.end());

	// The size is wrong
	if (data.size() != size.height * size.width)
	{
		return nullptr;
	}

	return std::unique_ptr<Level>(new Level(size));
}
