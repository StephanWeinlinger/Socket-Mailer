#pragma once

#include "../../shared/socket.h"
#include "../../shared/validation.h"

#include <filesystem>
#include <fstream>
#include <uuid/uuid.h>
#include <ldap.h>

class Commands {
public:
	static std::string _spool;

	// helper function
	static std::vector<std::string> getDirectoryEntries(std::string path);
	static void chooseMessage(int fd, bool& error, std::string &fullpath);

	static void send(int fd);
	static void list(int fd);
	static void read(int fd);
	static void del(int fd);
};