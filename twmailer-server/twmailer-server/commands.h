#pragma once

#include "../../shared/socket.h"
#include "../../shared/validation.h"

#include <filesystem>
#include <fstream>
#include <uuid/uuid.h>
#include <ldap.h>
#include <thread>

class Commands {
public:
	static std::string _spool;
	static std::mutex _mutexDirectory;
	static std::mutex _mutexFile;

	// helper function
	static std::vector<std::string> getDirectoryEntries(std::string path);
	static void chooseMessage(int fd, bool& error, std::string &fullpath);

	static void send(int fd);
	static void list(int fd);
	static void read(int fd);
	static void del(int fd);
};