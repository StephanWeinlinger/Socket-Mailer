#pragma once

#include <exception>
#include <string>

class isAliveException : public std::exception {
public:
	isAliveException(const std::string& error_message)
		: _error_message(error_message) {}
	const char* what() const throw() {
		return _error_message.c_str();
	}
private:
	std::string _error_message;
};