#include "../../shared/socket.h"
#include <vector>

class Commands {
public:
	static std::string _spool;

	static void send(int fd, bool& isAlive);
	static void list(int fd, bool& isAlive);
	static void read(int fd, bool& isAlive);
	static void del(int fd, bool& isAlive);
};