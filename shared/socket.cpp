#include "socket.h"

// returns tcp socket
int Socket::socket() {
    int fd;
    if ((fd = ::socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        throw "Socket couldn't be created";
    }
    return fd;
}

// only needed for welcome port on server
void Socket::setsockoptServer(int fd) {
    int reuseValue = -1;
    // set options for welcome socket, dont know what 4 and 5 do
    if (setsockopt(fd,
        SOL_SOCKET,
        SO_REUSEADDR,
        &reuseValue,
        sizeof(reuseValue)) == -1) {
        throw "Welcome socket options [reuseValue] could not be set";
    }

    // set options for welcome socket, dont know what 4 and 5 do
    if (setsockopt(fd,
        SOL_SOCKET,
        SO_REUSEPORT,
        &reuseValue,
        sizeof(reuseValue)) == -1) {
        throw "Welcome socket options [reusePort] could not be set";
    }
}

// bind address to socket
// only used by server, since the client just needs to connect
void Socket::bind(int fd, const struct sockaddr_in &addr) {
    if (::bind(fd, (struct sockaddr*) &addr, sizeof(addr)) == -1) {
        throw "Socket couldn't be bound";
    }
}

// let socket listen for incoming connections
void Socket::listen(int fd, int queue) {
    if (::listen(fd, 5) == -1) {
        throw "Socket couldn't start to listen";
    }
}

// accept incoming connections and return file descriptor
// addr_new can also be used for logging ip of client
int Socket::accept(int fd, const struct sockaddr_in &addr_new) {
    int fd_new;
    socklen_t addrlen = sizeof(struct sockaddr_in);
    if ((fd_new = ::accept(fd, (struct sockaddr*) &addr_new, &addrlen)) == -1) {
        throw "Socket couldn't accept incoming connection";
    }
    return fd_new;
}

// connect file descriptor to specific address
void Socket::connect(int fd, const struct sockaddr_in &addr) {
    if (::connect(fd, (struct sockaddr*) &addr, sizeof(addr)) == -1) {
        throw "Socket couldn't connect - Connection Error";
    }
}

// shutdown and close socket
void Socket::shutdown(int fd) {
    // close listening socket
    if (::close(fd) == -1) {
        std::cerr << "Socket couldn't close" << std::endl;
    }
}

int Socket::_buffersize = 1024;

// buffer gets the data received
// if sendAck is true an acknowledgement gets sent
// isAlive gets set to false if the connected client or server disconnected
void Socket::recv(int fd, std::string &output, bool sendAck, bool &isAlive) {
    char buffer[_buffersize];
    int size = ::recv(fd, buffer, _buffersize - 1, 0);
    if (size == -1) {
        std::cerr << "Socket couldn't read data" << std::endl;
        isAlive = false;
        return;
    } else if (size == 0) {
        std::cerr << "Connection closed remote socket" << std::endl;
        isAlive = false;
        return;
    }
    buffer[size] = '\0'; // add null terminator on last index
    output = buffer;
    if (sendAck) {
        Socket::send(fd, "OK", false, isAlive);
    }
}

// input holds data that needs to be sent
// if awaitAck is true the client or server waits for an answer ("OK", but doesn't get checked)
// isAlive is used if an acknowledgement is waited for, sinced its used in Socket::recv()
void Socket::send(int fd, std::string input, bool awaitAck, bool &isAlive) {
    int total = 0;
    int bytesleft = input.length();
    int current;
    while (total < input.length()) {
        current = ::send(fd, input.c_str() + total, bytesleft, 0);
        if (current == -1) {
            //throw "Socket couldn't send data";
            std::cerr << "Socket couldn't send data" << std::endl;
        }
        total = total + current;
        bytesleft = bytesleft - current;
    }
    // length check, since receiver wouldn't answer
    if (input.length() > 0) {
        if (awaitAck) {
            std::string output;
            Socket::recv(fd, output, false, isAlive);
            // buffer could be checked if it contains OK, but not really needed
        }

    }
}