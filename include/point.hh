#pragma once

#include <string>
#include <ostream>

enum class Direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
    NONE,   // No direction at all
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

    point operator+(const point &other) const
    {
        return {x + other.x, y + other.y};
    }

    point operator-(const point &other) const
    {
        return {x - other.x, y - other.y};
    }

    bool operator==(const point &other) const
    {
        return x == other.x && y == other.y;
    }

    bool operator!=(const point &other) const
    {
        return !(*this == other);
    }

    bool operator<(const point &other) const
    {
        if (y < other.y)
        {
            return true;
        }

        if (y > other.y)
        {
            return false;
        }

        return x < other.x;
    }

    point operator*(int val) const
    {
        return {x * val, y * val};
    }
};

namespace std
{
template <>
struct hash<point>
{
    std::size_t operator()(const point& k) const
    {
        return std::hash<int>()(k.x) ^ std::hash<int>()(k.y);
    }
};
}

struct extents
{
    unsigned width{0};
    unsigned height{0};

    bool operator==(const extents &other) const
    {
        return width == other.width &&
                height == other.height;
    }

    bool operator!=(const extents &other) const
    {
        return !(*this == other);
    }
};

static std::ostream& operator << (std::ostream& os, point const& value)
{
    os << "{" << value.x << ", " << value.y << "}";

    return os;
}
