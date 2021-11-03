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
	for (int i = 0; i < 3; ++i) {
		std::string input;
		std::cout << text[i];
		std::cin >> input;
		// also restricts sender and receiver length
		if (input.length() > 80) {
			std::cout << "Input may not be longer than 80 characters" << std::endl;
			i--;
			continue;
		}
		Socket::send(fd, input, true, isAlive);
		if (!isAlive) {
			return;
		}
	}
	fgetc(stdin); // remove \n from std::cin
	char buffer[1024];
	while (true) {
		if (fgets(buffer, 1024, stdin) != NULL) {
			// remove new-line signs from string at the end
			buffer[strcspn(buffer, "\r\n")] = 0;
			if (strcmp(buffer, ".") == 0) {
				Socket::send(fd, ".", true, isAlive);
				if (!isAlive) {
					return;
				}
				break;
			}
			Socket::send(fd, std::string(buffer), true, isAlive);
			if (!isAlive) {
				return;
			}
		}
	}
}

void Commands::list(int fd, bool& isAlive) {
	//Socket::send(fd, "LIST");
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