#pragma once

#include "exception.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <iostream>
#include <string>

class Socket {
public:
	static int _buffersize;

	static int socket();
	static void setsockoptServer(int fd);
	static void bind(int fd, const struct sockaddr_in &addr);
	static void listen(int fd, int queue);
	static int accept(int fd, const struct sockaddr_in &addr_new);
	static void connect(int fd, const struct sockaddr_in &addr);
	static void shutdown(int fd);

	// could be replaced with read and write
	static void recv(int fd, std::string &output, bool sendAck, bool &isAlive);
	static void send(int fd, std::string input, bool awaitAck, bool &isAlive);
};