#include "commands.h"

void Commands::send(int fd, bool& isAlive) {
	char buffer[1024];
	std::vector<std::string> output; // or just put it in the file instantly
	for (int i = 0; i < 3; ++i) {
		Socket::recv(fd, buffer, true, isAlive);
		if (!isAlive) {
			return;
		}
		std::cout << buffer << std::endl;
		output.push_back(std::string(buffer));
	}
	/*while (true) {
		Socket::recv(fd, buffer, true, isAlive);
		if (!isAlive) {
			return;
		}
		std::cout << std::string(buffer) << std::endl;
	}*/

	// vector will include the rest of the text, but that doesn't work right now

}

void Commands::list(int fd, bool& isAlive) {}

void Commands::read(int fd, bool& isAlive) {}

void Commands::del(int fd, bool& isAlive) {}