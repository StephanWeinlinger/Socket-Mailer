#include "commands.h"
#include <filesystem>
#include <fstream>

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

	std::string path = Commands::_spool + output[1];
	if (!std::filesystem::is_directory(path)) {
		std::filesystem::create_directory(path);
	}
	
	std::fstream output_fstream;
	// TODO: filename gets replaced with uuid in the future
	output_fstream.open(path + "/" + output[0], std::fstream::out);
	if (!output_fstream.is_open()) {
		std::cerr << "Failed to open " << path << '\n';
	}
	else {
		for(std::string i : output) {
			output_fstream << i << std::endl;
		}
	}
	output_fstream.close();
}

void Commands::list(int fd, bool& isAlive) {
	char buffer[1024];
	Socket::recv(fd, buffer, true, isAlive);
	if (!isAlive) {
		return;
	}
	std::string username = buffer;
	std::cout << username << std::endl;

	// not working yet
	/*std::vector<std::string> subjects;
	std::string path = Commands::_spool + username;

	if (std::filesystem::is_directory(path)) {
		for (auto const& entry : std::filesystem::directory_iterator(path)) {
			std::string filename = entry.path().filename();
			std::fstream input_fstream;
			input_fstream.open(path + "/" + filename, std::fstream::in);
			if (!input_fstream.is_open()) {
				std::cerr << "Failed to open " << path << "/" << filename << '\n';
			} else {
				std::string line;
				int counter = 1;
				while (std::getline(input_fstream, line)) {
					counter++;
					if (counter == 3) {
						break;
					}
				}
				subjects.push_back(line);
			}
			input_fstream.close();
		}
	} else {
		std::cout << "no such directory found" << std::endl;
	}
	std::cout << subjects[0] << std::endl;*/
}

void Commands::read(int fd, bool& isAlive) {}

void Commands::del(int fd, bool& isAlive) {}