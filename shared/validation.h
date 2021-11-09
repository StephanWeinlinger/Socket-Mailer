#pragma once

#include <string>
#include <regex>

namespace Validation {
	bool validateUsername(std::string username);
	bool validateSubject(std::string subject);
	bool validateIndex(std::string input, int count);
}