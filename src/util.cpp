#include "util.h"
#include <algorithm>

bool isNumeric(const std::string& s)
{
	if (s.empty())
		return false;
	std::string::const_iterator it = s.begin();

	while (it != s.end() && isdigit(*it))
		++it;

	return it == s.end();
}

void replaceAll(std::string& str, const std::string& from, const std::string& to)
{
	if (from.empty())
		return;
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos)
	{
		str.replace(start_pos, from.length(), to);
		start_pos += to.length();
	}
}

std::string toLowerCase(const std::string& s)
{
	std::string ret = s;
	std::transform(ret.begin(), ret.end(), ret.begin(),
		[](unsigned char c) { return (unsigned char)std::tolower(c); }
	);
	return ret;
}

std::string trimSpaces(const std::string& str)
{
	if (str.empty())
	{
		return str;
	}

	std::string result = str;
	while (!result.empty() && std::isspace(result.front())) {
		result.erase(result.begin());
	}
	while (!result.empty() && std::isspace(result.back())) {
		result.pop_back();
	}
	return result;
}
