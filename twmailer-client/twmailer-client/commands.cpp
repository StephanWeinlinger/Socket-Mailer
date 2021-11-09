#include "commands.h"

void Commands::send(int fd, bool &isAlive) {
	Socket::send(fd, "SEND", true, isAlive);
	if (!isAlive) {
		return;
	}
	std::vector<std::string> text = {
		"Sender: ",
		"Receiver: ",
		"Subject: "
	};
	std::string input;
	for (int i = 0; i < 3; ++i) {
		std::cout << text[i];
		std::getline(std::cin, input);
		if (i < 2) {
			if (Validation::validateUsername(input)) {
				std::cout << "Input may not be longer than 8 characters or consist of something else than [a-z][0-9]" << std::endl;
				i--;
				continue;
			}
		} else if (Validation::validateSubject(input)) { // check subject
			std::cout << "Subject may not be longer than 80 characters" << std::endl;
			i--;
			continue;
		}
		Socket::send(fd, input, true, isAlive);
		if (!isAlive) {
			return;
		}
	}
	while (true) {
		std::getline(std::cin, input);
		if (input.compare(".") == 0) {
			Socket::send(fd, ".", true, isAlive);
			if (!isAlive) {
				return;
			}
			break;
		}
		Socket::send(fd, input, true, isAlive);
		if (!isAlive) {
			return;
		}
	}
}

void Commands::list(int fd, bool& isAlive) {
	Socket::send(fd, "LIST", true, isAlive);
	if (!isAlive) {
		return;
	}
	std::string input;
	while (true) {
		std::cout << "Username: ";
		std::getline(std::cin, input);
		if (Validation::validateUsername(input)) {
			std::cout << "Input may not be longer than 8 characters or consist of something else than [a-z][0-9]" << std::endl;
			continue;
		}
		break;
	}
	Socket::send(fd, input, true, isAlive);
	if (!isAlive) {
		return;
	}
	// receive subject count and subjects
	std::vector<std::string> outputAll;
	std::string output;
	// receive subject count
	Socket::recv(fd, output, true, isAlive);
	if (!isAlive) {
		return;
	}
	outputAll.push_back(output);
	int count = std::stoi(output);
	// receive subjects and filenames
	for (int i = 0; i < count * 2; ++i) {
		Socket::recv(fd, output, true, isAlive);
		if (!isAlive) {
			return;
		}
		outputAll.push_back(output);
	}
	// output in form of [index] filename: subject
	std::cout << "User " << input << " has " << count << " messages!" << std::endl;
	for (int i = 0; i < count; ++i) {
		std::cout << "[" << i << "] " << outputAll[i + 1 + count] << ": " << outputAll[i + 1] << std::endl;
	}
}

void Commands::read(int fd, bool& isAlive) {
	//Socket::send(fd, "READ");
}

void Commands::del(int fd, bool& isAlive) {
	//Socket::send(fd, "DEL");
}

void Commands::quit(int fd, bool& isAlive) {
	//Socket::send(fd, "QUIT");
}