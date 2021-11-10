#pragma once

#include <exception>
#include <string>

// used for recv, since the function can indicate if the client/server disconnected
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