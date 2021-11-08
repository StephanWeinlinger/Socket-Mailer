#pragma once

#include "../../shared/socket.h"
#include "../../shared/validation.h"
#include <vector>
#include <sstream>

class Commands {
public:
	static void send(int fd, bool &isAlive);
	static void list(int fd, bool& isAlive);
	static void read(int fd, bool& isAlive);
	static void del(int fd, bool& isAlive);
	static void quit(int fd, bool& isAlive);
};