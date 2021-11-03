#include "commands.h"
#include <filesystem>

std::string Commands::_spool;

void Commands::send(int fd, bool& isAlive) {
	char buffer[1024];
	std::vector<std::string> output;
	for (int i = 0; i < 3; ++i) {
		Socket::recv(fd, buffer, true, isAlive);
		if (!isAlive) {
			return;
		}
		output.push_back(std::string(buffer));
	}
	while (true) {
		Socket::recv(fd, buffer, true, isAlive);
		if (!isAlive) {
			return;
		}
		if (strcmp(buffer, ".") == 0) {
			break;
		}
		output.push_back(std::string(buffer));
	}
	// output contains everything

	auto dir = std::filesystem::create_directory(Commands::_spool + output[1]); 
	if (dir) {
		std::cout << "directory sucsessfull created! " << std::endl;
		;
	}
	else {
		std::cout << "directory creation failed!" << std::endl;
	}


}

void Commands::list(int fd, bool& isAlive) {}

void Commands::read(int fd, bool& isAlive) {}

void Commands::del(int fd, bool& isAlive) {}