#include "../../shared/socket.h"

class Commands {
public:
	static void send(int fd);
	static void list(int fd);
	static void read(int fd);
	static void del(int fd);
};