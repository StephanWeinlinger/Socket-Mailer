﻿#include "commands.h"
#include "ldap.h"
#include "../../shared/socket.h"

#include <thread>
#include <filesystem>
#include <atomic>
namespace fs = std::filesystem;

void printUsage();
void startCommunication(int client_socket);
void signalHandler(int sig);

int welcome_socket = -1;
std::atomic<int> abortRequested = 0;
std::atomic<int> threadCount = 0;

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
		// start ldap connection
		Ldap::startConnection("ldap://ldap.technikum-wien.at:389");
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
			int client_socket = Socket::accept(welcome_socket, client_address);
			std::cout << "Client connected from " << inet_ntoa(client_address.sin_addr) << " on port " << ntohs(client_address.sin_port) << std::endl;
			if (!abortRequested) {
				std::thread(startCommunication, client_socket).detach();
			}
		}
	} catch (const char* msg) {
		std::cout << msg << std::endl;
	}
	if (welcome_socket = !- 1) {
		Socket::shutdown(welcome_socket);
	}
	// wait for all threads to finish
	// TODO might be good to force finishing
	if (threadCount > 0) {
		std::cout << "Waiting for all threads to finish..." << std::endl;
		while (threadCount > 0) {
			sleep(1); // so it doesn't get spammed
		}
	}
	return EXIT_SUCCESS;
}

void printUsage() {
	std::cerr << "Usage: twmailer-server <port> <mail-spool-directoryname>" << std::endl;
	exit(EXIT_FAILURE);
}

void startCommunication(int client_socket) {
	try {
		threadCount++; // increase thread count
		std::string output;
		// send welcome message
		std::string input = "Welcome to the mail server!\nHave fun!\n";
		Socket::send(client_socket, input, true);
		do {
			Socket::recv(client_socket, output, true);
			if (output.compare("SEND") == 0) {
				Commands::send(client_socket);
			} else if (output.compare("LIST") == 0) {
				Commands::list(client_socket);
			} else if (output.compare("READ") == 0) {
				Commands::read(client_socket);
			} else if (output.compare("DEL") == 0) {
				Commands::del(client_socket);
			} else if (output.compare("QUIT") == 0) {
				break;
			}
		} while (!abortRequested);
	} catch (isAliveException& e) {
		std::cout << e.what() << std::endl;
	}
	Socket::shutdown(client_socket); // shutdown client
	threadCount--; // decrease thread count
}

void signalHandler(int sig) {
	abortRequested = 1;
	std::cout << "\nShutting server down..." << std::endl;
	Socket::shutdown(welcome_socket);
	welcome_socket = -1;
}