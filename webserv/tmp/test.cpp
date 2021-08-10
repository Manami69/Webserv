#include <time.h>
#include <iostream>
#include <string>
#include <sys/time.h>
#include <map>
#include <sstream>
#include <fstream>
#include <vector>
#include <unistd.h>

int main ()
{
	std::string a= ".php$";
	std::string b = "/pouet.php";
	a.erase(a.size() - 1);
	std::cout << a << std::endl;
	int from = b.size() - a.size();
	if (b.find(a, from) != std::string::npos)
		std::cout << "yes" << std::endl;
	else
		std::cout << "nope" << std::endl;
	return 0;
}
