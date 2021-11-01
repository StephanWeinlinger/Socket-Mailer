#include <iostream>
#include <filesystem>
#include <thread>
#include "../../shared/socket.h"
#include <sys/wait.h>

namespace fs = std::filesystem;

void printUsage();
void startCommunication(int fd);
void signalHandler(int sig);
void shutdownServer();

// only global because shutdown function needs it
int welcome_socket;

int main(int argc, char* argv[]) {
	if (argc != 3) {
		printUsage();
		return EXIT_FAILURE;
	}
	if (!fs::exists(argv[2])) {
		std::cerr << "mail-spool-directory path invalid" << std::endl;
		return EXIT_FAILURE;
	}

	if (signal(SIGINT, signalHandler) == SIG_ERR) {
		std::cerr << "signal could not be registered" << std::endl;
		return EXIT_FAILURE;
	}

	try {
		// create welcome socket
		welcome_socket = Socket::socket();
		// set socket options for welcome socket
		Socket::setsockoptServer(welcome_socket);
		// configure address which will be bound to the welcome socket
		struct sockaddr_in welcome_address;
		memset(&welcome_address, 0, sizeof(sockaddr_in));
		welcome_address.sin_family = AF_INET;
		welcome_address.sin_addr.s_addr = INADDR_ANY;
		welcome_address.sin_port = htons(atoi(argv[1]));
		// bind address to welcome socket
		Socket::bind(welcome_socket, welcome_address);
		// listen for incoming connections on welcome socket
		// allow 5 queued connections max
		Socket::listen(welcome_socket, 5);
		// loop for accepting connections
		// TODO: implement quit functionality
		while (true) {
			struct sockaddr_in client_address;
			int client_fd = Socket::accept(welcome_socket, client_address);
			std::cout << "Client connected from " << inet_ntoa(client_address.sin_addr) << " on port " << ntohs(client_address.sin_port) << std::endl;
			// TODO add client counter and shared variable to notify threads that they have to shut down and count the variable down maybe
			// and only really end the programm if the counter reaches 0
			// but start by closing the welcome socket, so that no new clients can connect
			std::thread(startCommunication, client_fd).detach();
			// no join, since welcome socket waits anyway
		}
	}
	// shit solution TODO: make it better
	catch (const char* msg) {
		std::cout << msg << std::endl;
		shutdownServer();
	}
	return EXIT_SUCCESS;
}

void printUsage() {
	std::cerr << "Usage: twmailer-server <port> <mail-spool-directoryname>" << std::endl;
	exit(EXIT_FAILURE);
}

void startCommunication(int fd) {
	while (1) {
		std::cout << "heartbeat" << std::endl;
		sleep(5);
	}
	// start communication
}

void signalHandler(int sig) {
	// do you really need to check this?
	if (sig == SIGINT) {
		std::cout << "Shutting server down..." << std::endl;
		shutdownServer();
	}
}

void shutdownServer() {
	Socket::shutdown(welcome_socket);
}