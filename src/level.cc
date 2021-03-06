#include <level.hh>
#include <entity.hh>

#include <utils.hh>

#include <algorithm>
#include <set>
#include <unordered_map>

static const std::unordered_map<TileType, char> tileToChar =
{
    {TileType::EMPTY, ' '},
    {TileType::DIRT, '.'},
    {TileType::STONE_WALL, '#'},
    {TileType::WEAK_STONE_WALL, 'w'},
    {TileType::TELEPORTER, 't'},
    {TileType::LEFT_TRANSPORT, '<'},
    {TileType::RIGHT_TRANSPORT, '>'},
};
static const std::unordered_map<char, TileType> charToTile =
{
    {' ', TileType::EMPTY},
    {'.', TileType::DIRT},
    {'#', TileType::STONE_WALL},
    {'w', TileType::WEAK_STONE_WALL},
    {'t', TileType::TELEPORTER},
    {'<', TileType::LEFT_TRANSPORT},
    {'>', TileType::RIGHT_TRANSPORT},
};

class Level : public ILevel
{
public:
    Level(extents size, const std::string &data);

    virtual ~Level();

    virtual const struct extents &getSize() const override;

    virtual std::optional<TileType> tileAt(const point &where) const override;

	virtual void setTile(const point &where, TileType what) override;

    virtual void explode(const point &where) override;

    virtual std::set<point> getIllumination(const point &where, Direction dir) override;

    virtual std::string toString() const override;


    static bool verify(const std::string &data);

private:
    TileType *rawTile(const point &where);
    int pointToIndex(const point &where) const;

    static TileType tileFromChar(char c);

    extents m_size;
    std::vector<TileType> m_tiles;
    std::vector<point> m_explosionScanOrder;
};


Level::Level(extents size, const std::string &data)  :
    m_size(size)
{
    m_tiles.resize(size.height * size.width);

    // Try to create entities for all data
    int cur = 0;
    for (auto &c : data)
    {
        std::shared_ptr<IEntity> ent = IEntity::createFromChar(c, {cur % (int)size.width, cur / (int)size.width});

        if (ent)
        {
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

int Level::pointToIndex(const point &where) const
{
    // Dont allow wrapping to the last line
    if (where.x < 0 || where.y < 0 || where.x >= m_size.width || where.y >= m_size.height)
    {
        return -1;
    }

    auto idx = where.y * m_size.width + where.x;
    if (idx < 0 || idx >= m_size.height * m_size.width)
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

void Level::setTile(const point &where, TileType what)
{
    auto idx = pointToIndex(where);
    if (idx < 0)
    {
        return;
    }

    m_tiles[idx] = what;
}

void Level::explode(const point &where)
{
    const std::vector<point> radius =
    {
                     {0,-3},
            {-2,-2},         {2,-2},
            {-2, 0}, {0, 0}, {2, 0}, // this is the center
            {-2, 2},         {2, 2},
                     {0, 3}
    };
    const auto &center = radius[4];

    std::set<point> wreckedPositions;

    auto src = where + center;
    for (auto &it : radius)
    {
        auto dst = where + it;

        bresenham(src, dst, [this, &wreckedPositions](const point &cur)
        {
            // Skip out-of-bounds stuff
            auto idx = pointToIndex(cur);
            if (idx < 0)
            {
                return BresenhamCallbackRv::STOP_SCANNING;
            }

            auto tile = rawTile(cur);

            if (tile && *tile == TileType::STONE_WALL)
            {
                return BresenhamCallbackRv::STOP_SCANNING;
            }

            // OK, mark this as a wrecked position
            wreckedPositions.insert(cur);

            return BresenhamCallbackRv::CONTINUE_SCANNING;
        });
    }

    for (auto &cur : wreckedPositions)
    {
        auto tile = rawTile(cur);

        // destroy this point and create a fireball
        *tile = TileType::EMPTY;
        IEntity::createFromType(EntityType::FIREBALL, cur);
    }
}

std::set<point> Level::getIllumination(const point &where, Direction dir)
{
    std::set<point> out;

    // This means down
    std::vector<point> radius =
    {
             {-1,-2},  {0,-2}, { 1,-2},
             {-2,-1},          { 2,-1},
             {-3, 0},          { 3, 0},
             {-2, 1},          { 1, 2},
    {-2, 3}, {-1, 3}, { 0, 3}, { 1, 3}, {2, 3},
                      { 0, 4}
    };
    const point center = {0,0};

    if (dir == Direction::UP)
    {
        std::for_each(radius.begin(), radius.end(), [](point &cur){ cur.y *= -1; });
    }
    else if (dir == Direction::RIGHT)
    {
        std::for_each(radius.begin(), radius.end(), [](point &cur){ std::swap(cur.x, cur.y); });
    }
    else if (dir == Direction::LEFT)
    {
        std::for_each(radius.begin(), radius.end(), [](point &cur){ std::swap(cur.x, cur.y); cur.x *= -1; });
    }

    auto src = where + center;
    for (auto &it : radius)
    {
        auto dst = where + it;

        bresenham(src, dst, [this, &out](const point &cur)
        {
            // Skip out-of-bounds stuff
            auto idx = pointToIndex(cur);
            if (idx < 0)
            {
                return BresenhamCallbackRv::STOP_SCANNING;
            }

            auto tile = rawTile(cur);

            // We can light that!
            out.insert(cur);

            if (*tile != TileType::DIRT && *tile != TileType::EMPTY)
            {
                return BresenhamCallbackRv::STOP_SCANNING;
            }

            return BresenhamCallbackRv::CONTINUE_SCANNING;
        });
    }

    return out;
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
        if (!IEntity::isValid(c))
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
            const auto tile = m_tiles[y * m_size.width + x];
            auto it = tileToChar.find(tile);

            if (it != tileToChar.end())
            {
                out += it->second;
            }
            else
            {
                out += '?';
            }
        }
        out += '\n';
    }

    return out;
}

bool ILevel::tileIsPassable(TileType what)
{
    if (what == TileType::DIRT || what == TileType::EMPTY || what == TileType::TELEPORTER)
    {
        return true;
    }

    return false;
}
