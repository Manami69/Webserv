#include <time.h>
#include <iostream>
#include <string>
#include <sys/time.h>
#include <map>
#include <sstream>
#include <fstream>
#include <vector>
#include <unistd.h>
int popo[] = {1, 1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 1, 1, 2, 3, 4,5, 5, 6, 7, 8, 9};
std::vector<int>p(popo, popo + sizeof(popo) /sizeof(int));

size_t	stupid(size_t i) {
	while (p.at(++i))
		std::cout << "pouet";
	return i;
}

int main ()
{
	size_t i =  -1;
	try {
	while (p.at(++i) != 12)
	{
		if (p.at(i) == 1)
			i = stupid(i);
	}
	}
	catch (const std::exception& e) {
		std::cout << "hello " << e.what() <<  std::endl;
	}
	return 0;
}
