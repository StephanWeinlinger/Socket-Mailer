#pragma once

#include <string>
#include <mutex>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <iostream>
#include <sstream>

namespace fs = std::filesystem;
namespace ch = std::chrono;

class Blacklist {
public:
	static std::mutex _lock;
	static std::string _path;

	static void initialize(std::string path);
	static void addIP(std::string ip);
	static bool checkIP(std::string ip);
	static void cleanup();
	static bool compareTimestamp(std::string timestamp);
};