#include <iostream>
#include <filesystem>
#include <thread>
#include "../../shared/socket.h"
#include "commands.h"
#include <sys/wait.h>

namespace fs = std::filesystem;

void printUsage();
void startCommunication();
void signalHandler(int sig);
void shutdownServer();

int welcome_socket = -1;
int client_socket = -1;
sig_atomic_t abortRequested = 0;

int main(int argc, char* argv[]) {
	if (argc != 3) {
		printUsage();
		return EXIT_FAILURE;
	}
	if (!fs::exists(argv[2])) {
		std::cerr << "mail-spool-directory path invalid" << std::endl;
		return EXIT_FAILURE;
	}

	// set spool directory and add slash if needed
	Commands::_spool = std::string(argv[2]);
	if (Commands::_spool[Commands::_spool.length()] != '/') {
		Commands::_spool = Commands::_spool + "/";
	}

	struct sigaction sa;
	sa.sa_handler = signalHandler;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	if (sigaction(SIGINT, &sa, NULL) == -1) {
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
		while (!abortRequested) {
			std::cout << "Waiting for connections..." << std::endl;
			struct sockaddr_in client_address;
			client_socket = Socket::accept(welcome_socket, client_address);
			std::cout << "Client connected from " << inet_ntoa(client_address.sin_addr) << " on port " << ntohs(client_address.sin_port) << std::endl;
			if (!abortRequested) {
				startCommunication();
			}
		}
	}
	// TODO: use actually exceptions and not just strings
	catch (const char* msg) {
		std::cout << msg << std::endl;
	}
	// if exception occurs client has to get shut down too
	// on normal termination only the server gets shutdown right here
	shutdownServer();
	return EXIT_SUCCESS;
}

void printUsage() {
	std::cerr << "Usage: twmailer-server <port> <mail-spool-directoryname>" << std::endl;
	exit(EXIT_FAILURE);
}

void startCommunication() {
	std::string output;
	bool isAlive = true;
	// send welcome message
	std::string input = "Welcome to the mail server!\nHave fun!\n";
	Socket::send(client_socket, input, false, isAlive);
	do {
		Socket::recv(client_socket, output, true, isAlive);
		if (output.compare("SEND") == 0) {
			Commands::send(client_socket, isAlive);
		} else if (output.compare("LIST") == 0) {
			Commands::list(client_socket, isAlive);
		} else if (output.compare("READ") == 0) {
			Commands::read(client_socket, isAlive);
		} else if (output.compare("DEL") == 0) {
			Commands::del(client_socket, isAlive);
		} else if (output.compare("QUIT") == 0) {
			break;
		}
	} while(!abortRequested && isAlive);

	if (!abortRequested) {
		Socket::shutdown(client_socket); // shutdown client
		client_socket = -1;
	}
}

void signalHandler(int sig) {
	abortRequested = 1;
	std::cout << "\nShutting server down..." << std::endl;
	shutdownServer();
}

void shutdownServer() {
	if (welcome_socket != -1) {
		Socket::shutdown(welcome_socket);
		welcome_socket = -1;

	}
	if (client_socket != -1) {
		Socket::shutdown(client_socket);
		client_socket = -1;
	}
}