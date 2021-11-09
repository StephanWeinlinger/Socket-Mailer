#pragma once

#include <exception>

class isAliveException : public std::exception {
public:
	const char* what() const throw();
};