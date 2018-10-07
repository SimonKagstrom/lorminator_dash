#include <utils.hh>

#include <sstream>

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
