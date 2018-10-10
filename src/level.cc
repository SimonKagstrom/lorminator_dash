#include <level.hh>
#include <entity.hh>

#include <utils.hh>

#include <algorithm>

class Level : public ILevel
{
public:
	Level(extents size, const std::string &data);

	virtual ~Level();

	virtual const struct extents &getSize() const override;

	virtual std::vector<std::shared_ptr<IEntity>> getEntities() override;

	virtual bool pointIsPassable(const point &where) const override;

	virtual bool pointIsSolid(const point &where) const override;

	virtual std::optional<TileType> tileAt(const point &where) const override;

	virtual void explode(const point &where) override;


	static bool verify(const std::string &data);

private:
	static TileType tileFromChar(char c);

	extents m_size;
	std::vector<std::shared_ptr<IEntity>> m_entities;
	std::vector<TileType> m_tiles;
};


Level::Level(extents size, const std::string &data)  :
	m_size(size)
{
	m_tiles.resize(size.height * size.width);

	// Try to create entities for all data
	unsigned cur = 0;
	for (auto &c : data)
	{
		std::shared_ptr<IEntity> ent = IEntity::fromChar(c);

		if (ent)
		{
			m_entities.push_back(ent);
			m_tiles[cur] = TileType::EMPTY;
		}
		else
		{
			m_tiles[cur] = Level::tileFromChar(c);
		}

		cur++;
	}
}

Level::~Level()
{
}

const extents &Level::getSize() const
{
	return m_size;
}

std::vector<std::shared_ptr<IEntity>> Level::getEntities()
{
	return m_entities;
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

	auto idx = where.y * m_size.width + where.x;
	if (idx < 0 || idx > m_size.height * m_size.width)
	{
		return out;
	}
	out = m_tiles[idx];

	return out;
}

void Level::explode(const point &where)
{

}

// Assumes the level has been verified
TileType Level::tileFromChar(char c)
{
	switch (c)
	{
	case '.':
		return TileType::DIRT;
	case '#':
		return TileType::STONE_WALL;
	case 'w':
		return TileType::WEAK_STONE_WALL;
	case 't':
		return TileType::TELEPORTER;
	case ' ':
	default:
		break;
	}

	return TileType::EMPTY;
}

bool Level::verify(const std::string &data)
{
	std::string validChars = " .#xwt";

	for (auto &c : data)
	{
		if (!IEntity::fromChar(c))
		{
			if (validChars.find(c) == std::string::npos)
			{
				// Not an entity and not a map character
				return false;
			}
		}
	}

	if (std::find_if(data.begin(), data.end(), [](char c) { return c == 'p';} ) == data.end())
	{
		// No player
		return false;
	}

	auto teleporterCount = std::count_if(data.begin(), data.end(), [](char c) { return c == 't'; });
	if (teleporterCount == 1)
	{
		// Need 0 or > 1 teleporters, for obvious reasons
		return false;
	}

	return true;
}

std::unique_ptr<ILevel> ILevel::fromString(const std::string &levelString)
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
		return std::unique_ptr<ILevel>(new Level(size, ""));
	}

	// width. We know the words exist because of the split above
	auto tmp = levelString.find(' ');
	auto dataIdx = levelString.find(' ', tmp + 1) + 1; // Data starts afterwards

	// The data portion, without newlines
	auto data = levelString.substr(dataIdx);
	data.erase(std::remove(data.begin(), data.end(), '\n'), data.end());

	if (data.size() != size.height * size.width)
	{
		// The size is wrong
		return nullptr;
	}

	if (!Level::verify(data))
	{
		// Invalid characters in the data
		return nullptr;
	}

	return std::unique_ptr<ILevel>(new Level(size, data));
}
