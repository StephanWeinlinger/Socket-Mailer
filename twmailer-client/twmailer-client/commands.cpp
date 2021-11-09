#include "commands.h"

// helper function for read and delete
// reads in username, displays messages, reads in index, sends index to server
void Commands::chooseMessage(int fd, bool& error) {
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
	Socket::send(fd, input, true);

	std::string output;
	// receive filename count
	Socket::recv(fd, output, true);
	int count = std::stoi(output);
	if (count == 0) {
		error = true;
		std::cout << "No messages where found" << std::endl;
		return;
	}
	// receive filenames
	for (int i = 0; i < count; ++i) {
		Socket::recv(fd, output, true);
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
	Socket::send(fd, std::to_string(index), true);
}

void Commands::send(int fd) {
	Socket::send(fd, "SEND", true);
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
		Socket::send(fd, input, true);
	}
	while (true) {
		std::getline(std::cin, input);
		if (std::cin.fail()) {
			return;
		}
		if (input.compare(".") == 0) {
			Socket::send(fd, ".", true);
			break;
		}
		Socket::send(fd, input, true);
	}
}

void Commands::list(int fd) {
	Socket::send(fd, "LIST", true);
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
	Socket::send(fd, input, true);
	// receive subject count and subjects
	std::vector<std::string> outputAll;
	std::string output;
	// receive subject count
	Socket::recv(fd, output, true);
	outputAll.push_back(output);
	int count = std::stoi(output);
	// receive subjects and filenames
	for (int i = 0; i < count * 2; ++i) {
		Socket::recv(fd, output, true);
		outputAll.push_back(output);
	}
	// output in form of [index] filename: subject
	std::cout << "User " << input << " has " << count << " message(s)!" << std::endl;
	for (int i = 0; i < count; ++i) {
		std::cout << "[" << i << "] " << outputAll[i + 1 + count] << ": " << outputAll[i + 1] << std::endl;
	}
}

void Commands::read(int fd) {
	Socket::send(fd, "READ", true);
	// error flag if no message is found
	bool error = false;
	Commands::chooseMessage(fd, error);
	if (error) {
		return;
	}
	// receive message data
	std::string output;
	std::vector<std::string> text = {
		"Sender: ",
		"Receiver: ",
		"Subject: "
	};
	Socket::recv(fd, output, true);
	int count = std::stoi(output);
	if (count == 0) {
		std::cout << "Messages could not be found or was deleted" << std::endl;
	} else {
		for (int i = 0; i < count; ++i) {
			Socket::recv(fd, output, true);
			if (i < 3) {
				std::cout << text[i] << output << std::endl;
				continue;
			}
			std::cout << output << std::endl;
		}
	}
}

void Commands::del(int fd) {
	Socket::send(fd, "DEL", true);
	// error flag if no message is found
	bool error;
	Commands::chooseMessage(fd, error);
	if (error) {
		return;
	}
	// receive answer
	std::string output;
	Socket::recv(fd, output, true);
	if (output.compare("PASS") == 0) {
		std::cout << "Message was successfully deleted" << std::endl;
	} else if (output.compare("ERROR") == 0) {
		std::cout << "Message does not exist (already deleted)" << std::endl;
	}
}

void Commands::quit(int fd) {
	Socket::send(fd, "QUIT", true);
}