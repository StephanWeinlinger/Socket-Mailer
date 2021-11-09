#include "commands.h"

// helper function for read and delete
// reads in username, displays messages, reads in index, sends index to server
void Commands::chooseMessage(int fd, bool& isAlive, bool& error) {
	std::string input;
	while (true) {
		std::cout << "Username: ";
		std::getline(std::cin, input);
		if (std::cin.fail()) {
			error = true;
			return;
		}
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

	std::string output;
	// receive filename count
	Socket::recv(fd, output, true, isAlive);
	if (!isAlive) {
		return;
	}
	int count = std::stoi(output);
	if (count == 0) {
		error = true;
		std::cout << "No messages where found" << std::endl;
		return;
	}
	// receive filenames
	for (int i = 0; i < count; ++i) {
		Socket::recv(fd, output, true, isAlive);
		if (!isAlive) {
			return;
		}
		std::cout << "[" << i << "] " << output << std::endl;
	}
	int index;
	while (true) {
		std::cout << "Index of message to read: ";
		std::getline(std::cin, input);
		if (std::cin.fail()) {
			error = true;
			return;
		}
		if (Validation::validateIndex(input, count)) {
			std::cout << "Index out of range or invalid format!" << std::endl;
			continue;
		}
		index = std::stoi(input); // should be safe, since it gets checked before
		break;
	}
	Socket::send(fd, std::to_string(index), true, isAlive);
	if (!isAlive) {
		return;
	}
}

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
		if (std::cin.fail()) {
			return;
		}
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
		if (std::cin.fail()) {
			return;
		}
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
		if (std::cin.fail()) {
			return;
		}
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
	Socket::send(fd, "READ", true, isAlive);
	if (!isAlive) {
		return;
	}
	// error flag if no message is found
	bool error = false;
	Commands::chooseMessage(fd, isAlive, error);
	if (!isAlive || error) {
		return;
	}
	// receive message data
	std::string output;
	std::vector<std::string> text = {
		"Sender: ",
		"Receiver: ",
		"Subject: "
	};
	Socket::recv(fd, output, true, isAlive);
	if (!isAlive) {
		return;
	}
	int count = std::stoi(output);
	if (count == 0) {
		std::cout << "Messages could not be found or was deleted" << std::endl;
	} else {
		for (int i = 0; i < count; ++i) {
			Socket::recv(fd, output, true, isAlive);
			if (!isAlive) {
				return;
			}
			if (i < 3) {
				std::cout << text[i] << output << std::endl;
				continue;
			}
			std::cout << output << std::endl;
		}
	}
}

void Commands::del(int fd, bool& isAlive) {
	Socket::send(fd, "DEL", true, isAlive);
	if (!isAlive) {
		return;
	}
	// error flag if no message is found
	bool error;
	Commands::chooseMessage(fd, isAlive, error);
	if (!isAlive || error) {
		return;
	}
	// receive answer
	std::string output;
	Socket::recv(fd, output, true, isAlive);
	if (!isAlive) {
		return;
	}
	if (output.compare("PASS") == 0) {
		std::cout << "Message was successfully deleted" << std::endl;
	} else if (output.compare("ERROR") == 0) {
		std::cout << "Message does not exist (already deleted)" << std::endl;
	}
}

void Commands::quit(int fd, bool& isAlive) {
	Socket::send(fd, "QUIT", true, isAlive);
	if (!isAlive) {
		return;
	}
}