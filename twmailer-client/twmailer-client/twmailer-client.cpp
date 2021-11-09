#include <iostream>
#include "../../shared/socket.h"
#include "commands.h"
#include <sstream>

void printUsage();
void startCommunication();
void signalHandler(int sig);
void shutdownClient();

int client_socket = -1;
sig_atomic_t abortRequested = 0;

int main(int argc, char* argv[]) {
	if (argc != 3) {
		printUsage();
	}

	struct sigaction sa;
	sa.sa_handler = signalHandler;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	if (sigaction(SIGINT, &sa, NULL) == -1) {
		std::cerr << "signal could not be registered" << std::endl;
		return EXIT_FAILURE;
	}

	// TODO maybe add isAlive exception
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
		startCommunication();
	}
	catch (isAliveException& e) {
		std::cout << e.what();
	}
	catch (const char* msg) {
		std::cout << msg;
	}

	shutdownClient();
	return EXIT_SUCCESS;
}

void printUsage() {
	std::cerr << "Usage: twmailer-client <ip> <port>" << std::endl;
	exit(EXIT_FAILURE);
}

void startCommunication() {
	// get welcome message
	std::string output;
	Socket::recv(client_socket, output, true);
	std::cout << output;
	while (!abortRequested) {
		std::cout << ">> ";
		std::getline(std::cin, output);
		if (output.compare("SEND") == 0) {
			Commands::send(client_socket);
		} else if (output.compare("LIST") == 0) {
			Commands::list(client_socket);
		} else if (output.compare("READ") == 0) {
			Commands::read(client_socket);
		} else if (output.compare("DEL") == 0) {
			Commands::del(client_socket);
		} else if (output.compare("QUIT") == 0) {
			Commands::quit(client_socket);
			break;
		} else {
			// probably only happens on CTRL + C, so no need to clear std::cin
			if (std::cin.fail()) {
				break;
			}
			std::cout << "Valid commands: [SEND] [LIST] [READ] [DEL] [QUIT]" << std::endl;
		}
	}
}

void signalHandler(int sig) {
	abortRequested = 1;
	shutdownClient();
}

void shutdownClient() {
	if (client_socket != -1) {
		std::cout << "\nShutting client down..." << std::endl;
		Socket::shutdown(client_socket);
		client_socket = -1;
	}
}
