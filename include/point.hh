#pragma once

enum class Direction
{
	UP,
	DOWN,
	LEFT,
	RIGHT
};

struct point
{
	int x{0};
	int y{0};

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
