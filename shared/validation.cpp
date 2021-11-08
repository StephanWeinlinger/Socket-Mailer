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