#include <iostream>
#include <string>

#include <ldap.h>

class Ldap {
public:
	static LDAP* ldapHandle;

	static void startConnection(std::string ldapUri);
	static bool checkCredentials(std::string username, std::string password);
	static void closeConnection();
};