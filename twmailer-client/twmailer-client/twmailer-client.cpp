#include <iostream>
#include "../../shared/socket.h"

void printUsage();
void startCommunication(int fd);
void signalHandler(int sig);
void shutdownClient();

int client_socket;

int main(int argc, char* argv[]) {
	if (argc != 3) {
		printUsage();
	}

	if (signal(SIGINT, signalHandler) == SIG_ERR) {
		std::cerr << "signal could not be registered" << std::endl;
		return EXIT_FAILURE;
	}

	try {
		// create client socket
		client_socket = Socket::socket();
		// configure address to connect to
		struct sockaddr_in server_address;
		memset(&server_address, 0, sizeof(server_address));
		server_address.sin_family = AF_INET;
		server_address.sin_port = htons(atoi(argv[2]));
		inet_aton(argv[1], &server_address.sin_addr);
		// connect to configured address
		Socket::connect(client_socket, server_address);
		std::cout << "Connection established" << std::endl;
		startCommunication(client_socket);
	}
	catch (const char* msg) {
		std::cout << msg << std::endl;
		shutdownClient();
	}

	return EXIT_SUCCESS;
}

void printUsage() {
	std::cerr << "Usage: twmailer-client <ip> <port>" << std::endl;
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
	if (sig == SIGINT) {
		std::cout << "Shutting client down..." << std::endl;
		shutdownClient();
	}
}

void shutdownClient() {
	Socket::shutdown(client_socket);
}