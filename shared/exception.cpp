#include "exception.h"

const char* isAliveException::what() const throw() {
	return "Socket was closed";
}