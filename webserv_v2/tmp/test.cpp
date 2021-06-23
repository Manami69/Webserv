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
	char c;
	fs.open("./popo.txt", std::fstream::in);
	while (fs.get(c))          // loop getting single characters
    	std::cout << c << "=" << (int)c << " ";
	fs.close();
	return 0;
}

