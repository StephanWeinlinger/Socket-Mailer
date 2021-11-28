#pragma once

#include "../../shared/socket.h"
#include "../../shared/validation.h"

#include <sstream>
#include <termios.h>

class Commands {
public:
	static void chooseMessage(int fd, bool& error);
	static std::string readPassword();

	static void send(int fd);
	static void list(int fd);
	static void read(int fd);
	static void del(int fd);
	static void quit(int fd);
	static bool login(int fd);
};