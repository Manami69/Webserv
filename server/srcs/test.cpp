#include <time.h>
#include <iostream>
#include <string>
#include <sys/time.h>
#include <map>
#include <sstream>


std::string	_error_response(const std::map<int, std::string> err) {
	std::string ret;
	size_t f = 0;
	std::stringstream ss;
	// si pas de fichier personalisé
	ret.reserve(200);
	ret += "<html>\n<head><title>xx yy</title></head>\n<body>\n\
<center><h1>xx yy</h1></center>\n<hr><center>Webserv/1.0</center>\n\
</body>\n</html>";
	ss << 204;
	while ((f = ret.find("xx")) != std::string::npos)
		ret.replace(f, 2, ss.str());
	while ((f = ret.find("yy")) != std::string::npos)
		ret.replace(f, 2, err.find(204)->second);
	return ret;
}

std::map<int, std::string> error_code(void) {
	std::map<int, std::string> _err;
	_err[200] = "OK";
	_err[204] = "No Content";
	_err[400] = "Bad Request";
	_err[404] = "Not Found";
	_err[505] = "HTTP Version Not Supported";
	return _err;
}

int main ()
{
	std::map<int, std::string> err = error_code();

	std::cout << _error_response(err) << std::endl;
	return 0;
}

