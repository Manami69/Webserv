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

size_t		Config::parse_location(size_t i) {
	if (!_tokens.at(i + 3).compare("}")) //  prefix modifier case
	{
		i++;
		std::string c[] = {"=", "~", "~*", "^~"};
		std::vector<std::string> cas(c, c + sizeof(c) / sizeof(std::string)); //  POURQUOI CA M;ENGUEULE? ca doit etre c++ 11 par defaut ?
		if (std::find(cas.begin(), cas.end(), _tokens.at(i)) == cas.end()) // faire une erreur speciale au lieu de Wrong Config
			throw WrongConfig(); 
		else
			_serv_config.back().locations.back().modifier = _tokens[i];
		_serv_config.back().locations.back().access = _tokens.at(++i);
	}
	else if (!_tokens.at(i + 2).compare("}"))
	{
		_serv_config.back().locations.back().access = _tokens.at(++i);
	}
	else
		throw WrongConfig();
	i++;
	while (!_tokens.at(++i).compare("}"))
	{
		if (!_tokens.at(i).compare("allow_methods"))
			;
		else if (!_tokens.at(i).compare("limit_methods")) // meme fonction mais pourquoi pas avec un int pour savoir lequel
			;
		else if (!_tokens.at(i).compare("root"))
			;
		else if (!_tokens.at(i).compare("return"))
			;
		else if (!_tokens.at(i).compare("autoindex"))
			;
		else if (!_tokens.at(i).compare("index"))
			;
		else if (!_tokens.at(i).compare("CGI_path"))
		
	}
	return i;
}