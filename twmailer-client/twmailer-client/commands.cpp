#include "commands.h"

void Commands::send(int fd, bool &isAlive) {
	Socket::send(fd, "SEND", true, isAlive);
	if (!isAlive) {
		return;
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