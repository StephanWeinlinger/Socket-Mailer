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

// used for read and delete
// returns path to message
void Commands::chooseMessage(int fd, bool &error, std::string &fullpath) {
	std::string output;
	Socket::recv(fd, output, true);
	if (Validation::validateUsername(output)) {
		// if client tries to send invalid data shut it down
		throw isAliveException("Socket received invalid input");
	}

	std::string path = Commands::_spool + output;
	std::vector<std::string> entries = Commands::getDirectoryEntries(path);
	int count = entries.size();
	// send filename count
	Socket::send(fd, std::to_string(count), true);
	// count check is after sending, since client also needs the information
	if (count == 0) {
		error = true;
		return;
	}
	// send filenames
	for (auto entry : entries) {
		Socket::send(fd, entry, true);
	}
	// receive message index
	Socket::recv(fd, output, true);
	if (Validation::validateIndex(output, count)) {
		// if client tries to send invalid data shut it down
		throw isAliveException("Socket received invalid input");
	}
	int index = std::stoi(output);
	fullpath = path + "/" + entries[index];
}

void Commands::send(int fd) {
	std::string output;
	std::vector<std::string> outputAll;
	for (int i = 0; i < 3; ++i) {
		Socket::recv(fd, output, true);
		if (i < 2) {
			if (Validation::validateUsername(output)) {
				throw isAliveException("Socket received invalid input");
			}
		} else if (Validation::validateSubject(output)) {
			// subject is too long, shut client down
			throw isAliveException("Socket received invalid input");
		}
		outputAll.push_back(output);
	}
	while (true) {
		Socket::recv(fd, output, true);
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

void Commands::list(int fd) {
	std::string output;
	Socket::recv(fd, output, true);
	if (Validation::validateUsername(output)) {
		// if client trys to send invalid data shut it down
		throw isAliveException("Socket received invalid input");
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
	Socket::send(fd, std::to_string(subjects.size()), true);
	// send subjects
	for (auto subject : subjects) {
		Socket::send(fd, subject, true);
	}
	// send filenames (not necessary, but for read and del nice to have)
	for (auto entry : entries) {
		Socket::send(fd, entry, true);
	}
}

void Commands::read(int fd) {
	bool error = false;
	std::string fullpath;
	Commands::chooseMessage(fd, error, fullpath);
	if (error) {
		return;
	}
	std::vector<std::string> message;
	error = false; // reset error
	if (!std::filesystem::is_regular_file(fullpath)) {
		error = true;
	}
	std::fstream input_fstream;
	input_fstream.open(fullpath, std::fstream::in);
	if (!input_fstream.is_open()) {
		error = true;
		std::cerr << "Failed to open " << fullpath << '\n';
	} else {
		std::string line;
		while (std::getline(input_fstream, line)) {
			message.push_back(line);
		}
	}
	input_fstream.close();
	// check if error occured (file was probably deleted)
	if (error) {
		Socket::send(fd, "0", true);
	} else {
		// size needs to be a different variable, otherwise loop doesn't work
		int size = message.size();
		for (int i = -1; i < size; ++i) {
			if (i == -1) {
				// send line count
				Socket::send(fd, std::to_string(size), true);
			} else {
				// send actual message
				Socket::send(fd, message[i], true);
			}
		}
	}
}

void Commands::del(int fd) {
	bool error = false;
	std::string fullpath;
	Commands::chooseMessage(fd, error, fullpath);
	if (error) {
		return;
	}
	if (std::filesystem::remove(fullpath)) {
		Socket::send(fd, "PASS", true);
	} else {
		Socket::send(fd, "ERROR", true);
	}
}