#include "commands.h"

std::string Commands::_spool;

std::mutex Commands::_mutexDirectory;

std::mutex Commands::_mutexFile;

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
void Commands::chooseMessage(int fd, bool &error, std::string &fullpath, std::string username) {
	std::string path = Commands::_spool + username;
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
	std::string output;
	Socket::recv(fd, output, true);
	if (Validation::validateIndex(output, count)) {
		// if client tries to send invalid data shut it down
		throw isAliveException("Socket received invalid input");
	}
	int index = std::stoi(output);
	fullpath = path + "/" + entries[index];
}

void Commands::send(int fd, std::string username) {
	std::string output;
	std::vector<std::string> outputAll;
	outputAll.push_back(username); // insert username into message
	for (int i = 0; i < 2; ++i) {
		Socket::recv(fd, output, true);
		if (i == 0) {
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
	// lock, since two clients could send to the same new receiver at the same time
	Commands::_mutexDirectory.lock();
	if (!std::filesystem::is_directory(path)) {
		std::filesystem::create_directory(path);
	}
	Commands::_mutexDirectory.unlock();

	// create uuid
	uuid_t bin;
	char filename[40];
	uuid_generate(bin);
	uuid_unparse(bin, filename);
	std::fstream output_fstream;
	std::unique_lock<std::mutex> lock(Commands::_mutexFile); // lock and unlock if out of scope, since Socket::send could throw an exception
	output_fstream.open(path + "/" + std::string(filename), std::fstream::out);
	if (!output_fstream.is_open()) {
		std::cerr << "Failed to open " << path << '\n';
		Socket::send(fd, "ERROR", true);
		return;
	} else {
		for (std::string i : outputAll) {
			output_fstream << i << std::endl;
		}
		output_fstream.close();
	}
	lock.unlock();
	Socket::send(fd, "PASS", true);
}

void Commands::list(int fd, std::string username) {
	std::string path = Commands::_spool + username;
	std::vector<std::string> subjects;
	std::vector<std::string> entries;

	// no need for unique_lock, since there shouldn't be any exceptions
	Commands::_mutexFile.lock();
	for (auto entry : Commands::getDirectoryEntries(path)) {
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
			// build entries vector here, otherwise there could be more entries than subjects
			// since some files might not exist anymore
			entries.push_back(entry);
			subjects.push_back(line);
			input_fstream.close();
		}
	}
	Commands::_mutexFile.unlock();

	// send subject count
	Socket::send(fd, std::to_string(subjects.size()), true);
	// send subjects
	for (auto subject : subjects) {
		Socket::send(fd, subject, true);
	}
	// send filenames (not necessary, but for read and delete nice to have)
	for (auto entry : entries) {
		Socket::send(fd, entry, true);
	}
}

void Commands::read(int fd, std::string username) {
	bool error = false;
	std::string fullpath;
	Commands::chooseMessage(fd, error, fullpath, username);
	if (error) {
		return;
	}
	std::vector<std::string> message;
	error = false; // reset error
	if (!std::filesystem::is_regular_file(fullpath)) {
		error = true;
	}
	std::fstream input_fstream;
	Commands::_mutexFile.lock();
	input_fstream.open(fullpath, std::fstream::in);
	if (!input_fstream.is_open()) {
		error = true;
		std::cerr << "Failed to open " << fullpath << '\n';
	} else {
		std::string line;
		while (std::getline(input_fstream, line)) {
			message.push_back(line);
		}
		input_fstream.close();
	}
	Commands::_mutexFile.unlock();
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

void Commands::del(int fd, std::string username) {
	bool error = false;
	std::string fullpath;
	Commands::chooseMessage(fd, error, fullpath, username);
	if (error) {
		return;
	}
	Commands::_mutexFile.lock();
	bool success = std::filesystem::remove(fullpath);
	Commands::_mutexFile.unlock();
	if (success) {
		Socket::send(fd, "PASS", true);
	} else {
		Socket::send(fd, "ERROR", true);
	}
}

bool Commands::login(int fd, std::string &username) {
	// receive username
	Socket::recv(fd, username, true);
	if (Validation::validateUsername(username)) {
		throw isAliveException("Socket received invalid input");
	}
	// receive password
	std::string password;
	Socket::recv(fd, password, true);
	if (Validation::validatePassword(password)) {
		throw isAliveException("Socket received invalid input");
	}
	// check credentials
	if (Ldap::checkCredentials(username, password)) {
		Socket::send(fd, "PASS", true);
		return true;
	} else {
		Socket::send(fd, "ERROR", true);
	}
	return false;
}