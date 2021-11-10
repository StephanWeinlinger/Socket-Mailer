#pragma once

#include "../../shared/socket.h"
#include "../../shared/validation.h"

#include <sstream>

class Commands {
public:
	static void chooseMessage(int fd, bool& error);

	static void send(int fd);
	static void list(int fd);
	static void read(int fd);
	static void del(int fd);
	static void quit(int fd);
};