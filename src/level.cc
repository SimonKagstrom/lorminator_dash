#include <level.hh>
#include <entity.hh>

#include <utils.hh>

#include <algorithm>
#include <unordered_map>

static const std::unordered_map<TileType, char> tileToChar =
{
	{TileType::EMPTY, ' '},
	{TileType::DIRT, '.'},
	{TileType::STONE_WALL, '#'},
	{TileType::WEAK_STONE_WALL, 'w'},
	{TileType::TELEPORTER, 't'},
};
static const std::unordered_map<char, TileType> charToTile =
{
	{' ', TileType::EMPTY},
	{'.', TileType::DIRT},
	{'#', TileType::STONE_WALL},
	{'w', TileType::WEAK_STONE_WALL},
	{'t', TileType::TELEPORTER},
};

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

	virtual std::string toString() const override;


	static bool verify(const std::string &data);

private:
	TileType *rawTile(const point &where);
	int pointToIndex(const point &where) const;

	static TileType tileFromChar(char c);

	extents m_size;
	std::vector<std::shared_ptr<IEntity>> m_entities;
	std::vector<TileType> m_tiles;
	std::vector<point> m_explosionScanOrder;
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

int Level::pointToIndex(const point &where) const
{
	auto idx = where.y * m_size.width + where.x;
	if (idx < 0 || idx > m_size.height * m_size.width)
	{
		return -1;
	}

	return idx;
}

TileType *Level::rawTile(const point &where)
{
	auto idx = pointToIndex(where);
	if (idx < 0)
	{
		return nullptr;
	}

	return &m_tiles[idx];
}

std::optional<TileType> Level::tileAt(const point &where) const
{
	std::optional<TileType> out;

	auto idx = pointToIndex(where);
	if (idx < 0)
	{
		return out;
	}
	out = m_tiles[idx];

	return out;
}

void Level::explode(const point &where)
{
	const std::vector<point> radius =
	{
			         {0,-3},
			{-2,-2}, {0,-1}, {2,-2},
			{-2, 0}, {0, 0}, {2, 0}, // 5 is the center
			{-2, 2}, {0, 1}, {2, 2},
			         {0, 3}
	};
	const auto &center = radius[5];

	auto src = where + center;
	for (auto &it : radius)
	{
		auto dst = where + it;

		bresenham(src, dst, [this](const point &cur)
		{
			auto tile = rawTile(cur);

			if (tile && *tile == TileType::STONE_WALL)
			{
				return true;
			}
			*tile = TileType::EMPTY;

			return false;
		});
	}
}

// Assumes the level has been verified
TileType Level::tileFromChar(char c)
{
	auto it = charToTile.find(c);

	if (it == charToTile.end())
	{
		return TileType::EMPTY;
	}

	return it->second;
}

bool Level::verify(const std::string &data)
{
	for (auto &c : data)
	{
		if (!IEntity::fromChar(c))
		{
			if (charToTile.find(c) == charToTile.end())
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

std::string Level::toString() const
{
	std::string out;

	for (auto y = 0; y < m_size.height; y++)
	{
		for (auto x = 0; x < m_size.width; x++)
		{
			const auto tile = tileAt({x,y});

			if (tile)
			{
				auto it = tileToChar.find(*tile);

				if (it != tileToChar.end())
				{
					out += it->second;
				}
				else
				{
					out += '?';
				}
			}
			else
			{
				out += "?";
			}
		}
		out += '\n';
	}

	return out;
}
