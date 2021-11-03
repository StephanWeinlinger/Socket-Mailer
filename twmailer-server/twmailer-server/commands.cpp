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

	if (!std::filesystem::is_directory(path))
	{
		bool dir = std::filesystem::create_directory(path); // Check if Ordner with this name already exist
		if (dir) {
			std::cout << "directory sucsessfull created! " << std::endl;
			;
		}
		else {
			std::cout << "directory creation failed!" << std::endl;
		}
	}
	
	//Creating txt Data and writing into the file
	std::fstream output_fstream;

	output_fstream.open(path + "/" + output[0], std::fstream::out);
	if (!output_fstream.is_open()) {
		std::cerr << "Failed to open " << path << '\n';
	}
	else {
		for(std::string i : output)
		{
			output_fstream << i << std::endl;

		}
		std::cout << "done" << std::endl;
	}
	output_fstream.close();
}

void Commands::list(int fd, bool& isAlive) {}

void Commands::read(int fd, bool& isAlive) {}

void Commands::del(int fd, bool& isAlive) {}