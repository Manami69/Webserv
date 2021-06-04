#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <dirent.h>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <list>
#include <fcntl.h>           /* Definition of AT_* constants */
#include <sys/stat.h>
#include <ctime>

#define INDEX_BODY "<html>\n\
<head><title>Index of "
#define INDEX2 "</title></head>\n\
<body bgcolor=\"white\">\n\
<h1>Index of "
#define INDEX3 "</h1><hr><pre><a href=\"../\">../</a>\n"
#define INDEXEND "</pre><hr></body>\n\
</html>"
#define LOC "/"

#if defined (__APPLE__)
	# define st_mtim st_mtimespec
#endif

struct t_index_file
{
	std::string			name;
	int					spaceL;
	std::string			date;
	long unsigned int	size;
	unsigned char		type;
};


std::string _fill_index_body(std::list<t_index_file> files)
{
	std::string ret;
	ret.reserve(1024);
	ret.append(INDEX_BODY);
	ret.append(LOC);
	ret.append(INDEX2);
	ret.append(LOC);
	ret.append(INDEX3);
	for (std::list<t_index_file>::iterator it = files.begin(); it != files.end(); it++) {
		if ((*it).type == static_cast<unsigned char>(4) && (*it).name.compare(".") != 0 && (*it).name.compare("..") != 0)
		{
			std::stringstream ss;
			ret.append("<a href=\"");
			ret.append((*it).name);
			ret.append("/\">");
			ret.append((*it).name);
			ret.append("/</a>");
			ret.append((*it).spaceL, ' ');
			ret.append((*it).date);
			ss << std::setfill(' ') << std::setw(20);
			ss << "-\n";
			ret.append(ss.str());
		}
	}
	for (std::list<t_index_file>::iterator it = files.begin(); it != files.end(); it++) {
		if ((*it).type == static_cast<unsigned char>(8))
		{
			std::stringstream ss;
			ret.append("<a href=\"");
			ret.append((*it).name);
			ret.append("/\">");
			ret.append((*it).name);
			ret.append("/</a>");
			ret.append((*it).spaceL, ' ');
			ret.append((*it).date);
			ss << std::setfill(' ') << std::setw(20);
			ss << (*it).size << "\n";
			ret.append(ss.str());
		}
	}
	ret.append(INDEXEND);
	return ret;
}

int main () {
	DIR *dir;
	t_index_file cur;
	std::list<t_index_file> files;
	struct dirent	*ent;
	struct stat		stat;

	if ((dir = opendir (".")) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			lstat(ent->d_name, &stat);
			char strNow[ 19 ];
    		strftime(strNow, 19, "%d-%b-%Y %H:%M", gmtime( &stat.st_mtim.tv_sec ));
			cur.name = ent->d_name;
			cur.spaceL = 51 - cur.name.size();
			cur.date = strNow;
			std::cout << stat.st_size << std::endl;
			cur.size = stat.st_size;
			cur.type = ent->d_type;
			files.push_back(cur);
  		}
  		closedir (dir);
	}
	else {
		std::cout << "Errrr" << std::endl;
	}
	std::cout << _fill_index_body(files) << std::endl;
	return 0;
}