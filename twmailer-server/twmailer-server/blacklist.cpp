#include "blacklist.h"

std::mutex Blacklist::_lock;
std::string Blacklist::_path;

// inititalize path and create file if it doesn't exist
void Blacklist::initialize(std::string path) {
	Blacklist::_path = path + ".blacklist";
	if (!fs::exists(Blacklist::_path)) {
		std::ofstream(Blacklist::_path);
	}
}

// add IP to blacklist
// call checkIP beforehand, otherwise it will cause duplicate entries
void Blacklist::addIP(std::string ip) {
	std::fstream output_fstream;
	Blacklist::_lock.lock();
	output_fstream.open(Blacklist::_path, std::ios::out);
	if (!output_fstream.is_open()) {
		std::cerr << "Failed to open " << Blacklist::_path << std::endl;
		return;
	} else {
		int timestamp = ch::duration_cast<ch::milliseconds>(ch::system_clock::now().time_since_epoch()).count();
		timestamp = timestamp + 60000;
		output_fstream << ip << "," << timestamp << std::endl;
		output_fstream.close();
	}
	Blacklist::_lock.unlock();
}

bool Blacklist::checkIP(std::string ip) {
	std::fstream input_fstream;
	Blacklist::_lock.lock();
	input_fstream.open(Blacklist::_path, std::fstream::in);
	if (!input_fstream.is_open()) {
		std::cerr << "Failed to open " << Blacklist::_path << std::endl;
	} else {
		std::string line;
		while (std::getline(input_fstream, line)) {
			std::stringstream linestream(line);
			std::string ipBlacklisted;
			std::getline(linestream, ipBlacklisted, ',');
			// check if ip matches, else continue
			if (ipBlacklisted.compare(ip) == 0) {
				std::string timestamp;
				std::getline(linestream, timestamp);
				if (Blacklist::compareTimestamp(timestamp)) {
					return true;
				}
				break;
			}
		}
		input_fstream.close();
	}
	Blacklist::_lock.unlock();
	return false;
}

void Blacklist::cleanup() {

}

// returns true if timestamp is in the future
bool Blacklist::compareTimestamp(std::string timestamp) {
	int currentTimestamp = ch::duration_cast<ch::milliseconds>(ch::system_clock::now().time_since_epoch()).count();
	if (currentTimestamp < std::stoi(timestamp)) {
		return true;
	}
	return false;
}