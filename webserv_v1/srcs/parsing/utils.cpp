#include "../../includes/parsing.hpp"

//	Search parameter
bool check_line(std::string const &line, std::string const &comp)
{
	unsigned int	start = 0;
	unsigned int	end = comp.size();

	if (line.size() >= start && line.substr(start, end) == comp)
		return true;
	return false;
}

bool read_config(int fd, std::string *line)
{
	char	*tmp;
	//	End of file
	if (get_next_line(fd, &tmp) == 0)
		return false;
	//	End of location block
	if (check_line(tmp, "}"))
		return false;
	*line = tmp;
	free(tmp);
	//	Delete white space
	(*line).erase(0, (*line).find_first_not_of(" \t\n\r\f\v"));
	return true;
}

bool read_location(int fd, std::string *line)
{
	char	*tmp;
	if (get_next_line(fd, &tmp) == 0)
		return false;
	*line = tmp;
	free(tmp);
	(*line).erase(0, (*line).find_first_not_of(" \t\n\r\f\v"));
	if (check_line(*line, "}"))
		return false;
	return true;
}

std::string get_config(std::string line, const std::string &after)
{
	// if no parameter
	if (line.size() == after.size())
		return ("None");
	// will look for the parameter
	unsigned int start = after.size() + 1;
	line = line.substr(start, line.size());
	//	Delete white space
	line.erase(0, line.find_first_not_of(" \t\n\r\f\v"));
	line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);
	return line;
}

std::list<std::string>	get_index(std::string line, const std::string &after)
{
	std::list<std::string>	ret;
	std::string				index;

	if (line.size() == after.size())
		return (ret);
	//	will look for the parameter
	unsigned int start = after.size();
	line = line.substr(start, line.size());
	//	while for all index
	while (line.size() != 0)
	{
		line = line.substr(1, line.size());
		index = line.substr(0, line.find(" "));
		line = line.substr(index.size(), line.size());
		ret.push_back(index);
	}
	return (ret);
}
