#include "commands.h"

std::string Commands::_spool;

std::vector<std::string> Commands::getDirectoryEntries(std::string path) {
	std::vector<std::string> entries;
	if (std::filesystem::is_directory(path)) {
		for (auto const& entry : std::filesystem::directory_iterator(path)) {
			// just to make sure nobody created junk folders inside the spool directory
			if (std::filesystem::is_regular_file(entry)) {
				std::string filename = entry.path().filename();
				entries.push_back(filename);
			}
		}
	}
	return entries;
}

void Commands::send(int fd, bool& isAlive) {
	std::string output;
	std::vector<std::string> outputAll;
	for (int i = 0; i < 3; ++i) {
		Socket::recv(fd, output, true, isAlive);
		if (i < 2) {
			if (Validation::validateUsername(output)) {
				isAlive = false;
			}
		} else if (Validation::validateSubject(output)) {
			// subject is too long, shut client down
			isAlive = false;
		}
		if (!isAlive) {
			return;
		}
		outputAll.push_back(output);
	}
	while (true) {
		Socket::recv(fd, output, true, isAlive);
		if (!isAlive) {
			return;
		}
		if (output.compare(".") == 0) {
			break;
		}
		outputAll.push_back(output);
	}

	std::string path = Commands::_spool + outputAll[1];
	if (!std::filesystem::is_directory(path)) {
		std::filesystem::create_directory(path);
	}
	
	std::fstream output_fstream;
	// TODO: filename gets replaced with uuid in the future
	output_fstream.open(path + "/" + outputAll[0], std::fstream::out);
	if (!output_fstream.is_open()) {
		std::cerr << "Failed to open " << path << '\n';
	}
	else {
		for(std::string i : outputAll) {
			output_fstream << i << std::endl;
		}
	}
	output_fstream.close();
}

void Commands::list(int fd, bool& isAlive) {
	std::string output;
	Socket::recv(fd, output, true, isAlive);
	if (!isAlive || Validation::validateUsername(output)) {
		// if client trys to send invalid data shut it down
		isAlive = false;
		return;
	}

	std::string path = Commands::_spool + output;
	std::vector<std::string> subjects;
	std::vector<std::string> entries = Commands::getDirectoryEntries(path);

	for (auto entry : entries) {
		std::fstream input_fstream;
		input_fstream.open(path + "/" + entry, std::fstream::in);
		if (!input_fstream.is_open()) {
			std::cerr << "Failed to open " << path << "/" << entry << '\n';
		} else {
			std::string line;
			int counter = 0;
			while (std::getline(input_fstream, line)) {
				if (counter == 2) {
					break;
				}
				counter++;
			}
			subjects.push_back(line);
		}
		input_fstream.close();
	}
	
	// send subject count
	Socket::send(fd, std::to_string(subjects.size()), true, isAlive);
	if (!isAlive) {
		return;
	}
	// send subjects
	for (auto subject : subjects) {
		Socket::send(fd, subject, true, isAlive);
		if (!isAlive) {
			return;
		}
	}
	// send filenames (not necessary, but for read and del nice to have)
	for (auto entry : entries) {
		Socket::send(fd, entry, true, isAlive);
		if (!isAlive) {
			return;
		}
	}
}

void Commands::read(int fd, bool& isAlive) {}

void Commands::del(int fd, bool& isAlive) {}