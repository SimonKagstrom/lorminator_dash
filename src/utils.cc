#include <utils.hh>

#include <sstream>
#include <algorithm>

// http://stackoverflow.com/questions/236129/how-to-split-a-string-in-c
static std::vector<std::string> &split(const std::string &s, char delim,
		std::vector<std::string> &elems)
{
    std::stringstream ss(s);
    std::string item;

    while (std::getline(ss, item, delim))
    {
        elems.push_back(item);
    }

    return elems;
}


std::vector<std::string> split_string(const std::string &s, const std::string &delims)
{
    std::vector<std::string> elems;

    split(s, delims[0], elems);

    return elems;
}

bool string_is_integer(const std::string &str, unsigned base)
{
	size_t pos;

	try
	{
		stoull(str, &pos, base);
	}
	catch(std::invalid_argument &e)
	{
		return false;
	}
	catch(std::out_of_range &e)
	{
		return false;
	}

	return pos == str.size();
}

int64_t string_to_integer(const std::string &str, unsigned base)
{
	size_t pos;

	return (int64_t)stoull(str, &pos, base);
}

/* Bresenhams algorithm, from http://www.fact-index.com/b/br/bresenham_s_line_algorithm_c_code.html */
void bresenham(const point &from, const point &to, std::function<bool(const point &where)> callback)
{
	auto x0 = from.x;
	auto y0 = from.y;
	auto x1 = to.x;
	auto y1 = to.y;
	int steep = 1;
	int sx, sy;  /* step positive or negative (1 or -1) */
	int dx, dy;  /* delta (difference in X and Y between points) */
	int e;
	int i;

	/*
	 * optimize for vertical and horizontal lines here
	 */
	dx = abs(x1 - x0);
	sx = ((x1 - x0) > 0) ? 1 : -1;
	dy = abs(y1 - y0);
	sy = ((y1 - y0) > 0) ? 1 : -1;
	if (dy > dx)
	{
		steep = 0;
		std::swap(x0, y0);
		std::swap(dx, dy);
		std::swap(sx, sy);
	}

	e = (dy << 1) - dx;
	for (i = 0; i < dx; i++)
	{
		if (steep)
		{
			if (callback({x0,y0}))
			{
				return;
			}
		}
		else
		{
			if (callback({y0,x0}))
			{
				return;
			}
		}
		while (e >= 0)
		{
			y0 += sy;
			e -= (dx << 1);
		}
		x0 += sx;
		e += (dy << 1);
	}
}
