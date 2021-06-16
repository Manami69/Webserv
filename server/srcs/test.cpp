#include <time.h>
#include <iostream>
#include <string>
#include <sys/time.h>
#include <map>
#include <sstream>
#include <fstream>
#include <unistd.h>

int main ()
{
	std::fstream fs;
	int		num = 0;
	std::stringstream ss;
	bool t = true;
	while (t)
	{
		ss.str("");
		ss << num;
		std::cout << "body" + ss.str() << std::endl;
		fs.open("./body" + ss.str(), std::fstream::in);
		t = fs.is_open();
		fs.close();
		num++;
	}
	fs.open("body" + ss.str(), std::fstream::out);
	std::cout <<  "body" + ss.str() << std::endl;
	return 0;
}

