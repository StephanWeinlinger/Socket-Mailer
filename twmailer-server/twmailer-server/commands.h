#pragma once

#include "../../shared/socket.h"
#include "../../shared/validation.h"
#include "ldap.h"

#include <filesystem>
#include <fstream>
#include <uuid/uuid.h>
#include <thread>

class Commands {
public:
	static std::string _spool;
	static std::mutex _mutexDirectory;
	static std::mutex _mutexFile;

	// helper function
	static std::vector<std::string> getDirectoryEntries(std::string path);
	static void chooseMessage(int fd, bool &error, std::string &fullpath);

	static void send(int fd, std::string username);
	static void list(int fd, std::string username);
	static void read(int fd, std::string username);
	static void del(int fd, std::string username);
	static bool login(int fd, std::string &username);
};