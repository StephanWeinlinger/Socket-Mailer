#include "validation.h"

bool Validation::validateUsername(std::string username) {
	if (username.length() > 0 && username.length() <= 8 && std::regex_match(username, std::regex("([a-z]*[0-9]*)*"))) {
		return false;
	}
	return true;
}

bool Validation::validateSubject(std::string subject) {
	if (subject.length() > 0 && subject.length() <= 80) {
		return false;
	}
	return true;
}

bool Validation::validateIndex(std::string input, int count) {
	int index;
	try {
		index = std::stoi(input);
	} catch (...) {
		return true;
	}
	if (index >= 0 && index < count) {
		return false;
	}
	return true;
}

bool Validation::validatePassword(std::string password) {
	if (password.length() > 0 && password.length() <= 200) {
		return false;
	}
	return true;
}