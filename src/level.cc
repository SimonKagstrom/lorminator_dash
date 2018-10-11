#include <level.hh>
#include <entity.hh>

#include <utils.hh>

#include <algorithm>
#include <list>

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
	std::vector<point> m_explosionScanOrder;
};

static std::vector<point> getScanOrder(const std::vector<point> &radius, unsigned centreIdx)
{
	const auto &center = radius[centreIdx];
	std::list<point> order;

	// Find the order of which to visit entries
	for (const auto &cur : radius)
	{
		bresenham(center, cur,
				[&order](const point &where)
				{
					order.push_back(where);
					return true;
				});
	}
	// Remove duplicate entries
	order.unique();

	std::vector<point> out;
	for (auto &cur : order)
	{
		out.push_back(cur);
	}

	return out;
}


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

	const std::vector<point> radius =
	{
			         {0,-2},
			{-1,-1}, {0,-1}, {1,-1},
			{-1, 0}, {0, 0}, {1, 0}, // 5 is the center
			{-1, 1}, {0, 1}, {1, 1},
			         {0, 2}
	};

	m_explosionScanOrder = getScanOrder(radius, 5);
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
	for (const auto &it : m_explosionScanOrder)
	{
		const auto cur = where + it;

		auto idx = cur.y * m_size.width + cur.x;
		if (idx < 0 || idx > m_size.height * m_size.width)
		{
			continue;
		}

		if (m_tiles[idx] == TileType::STONE_WALL)
		{
			continue;
		}

		m_tiles[idx] = TileType::EMPTY;
	}
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
	case ' ': // Empty
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
