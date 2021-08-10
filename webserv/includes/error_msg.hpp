#ifndef ERROR_MSG_HPP
#  define ERROR_MSG_HPP

# include "header.hpp"

class ErrorMsg : public std::exception
{
private:
	std::string error_message;

public:
	explicit ErrorMsg(const std::string& msg) :error_message(msg) {}
	virtual ~ErrorMsg() throw  () {}
	virtual const char *what() const throw() {
		return (error_message.c_str());
	}
};

#endif