#pragma once

#include "../../shared/socket.h"
#include "../../shared/validation.h"
#include <vector>
#include <filesystem>
#include <fstream>

class Commands {
public:
	static std::string _spool;

	// helper function
	static std::vector<std::string> getDirectoryEntries(std::string path);
	static void chooseMessage(int fd, bool& isAlive, bool& error, std::string &fullpath);

	static void send(int fd, bool& isAlive);
	static void list(int fd, bool& isAlive);
	static void read(int fd, bool& isAlive);
	static void del(int fd, bool& isAlive);
};