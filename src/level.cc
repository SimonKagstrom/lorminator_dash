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
	extents m_size;
	std::vector<std::shared_ptr<IEntity>> m_entities;
};


Level::Level(extents size, const std::string &data)  :
	m_size(size)
{
	// Try to create entities for all data
	for (auto &c : data)
	{
		std::shared_ptr<IEntity> ent = IEntity::fromChar(c);

		if (ent)
		{
			m_entities.push_back(ent);
		}
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
	return out;
}

void Level::explode(const point &where)
{

}


bool Level::verify(const std::string &data)
{
	std::string validChars = " .#xwotpgbIGR";

	for (auto &c : data)
	{
		if (validChars.find(c) == std::string::npos)
		{
			return false;
		}
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
