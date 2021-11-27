#include "ldap.h"

LDAP* Ldap::ldapHandle;

void Ldap::startConnection(std::string ldapUri) {
	// initialize connection
	if (ldap_initialize(&Ldap::ldapHandle, ldapUri.c_str()) != LDAP_SUCCESS) {
		throw "Failed to initialize LDAP connection";
	}
	// set options
	const int ldapVersion = LDAP_VERSION3;
	if (ldap_set_option(Ldap::ldapHandle, LDAP_OPT_PROTOCOL_VERSION, &ldapVersion) != LDAP_OPT_SUCCESS) {
		// unbind since options has failed, but connection was initialized
		ldap_unbind_ext_s(Ldap::ldapHandle, nullptr, nullptr);
		throw "Failed to set LDAP options";
	}
	// start secure connection
	if (ldap_start_tls_s(Ldap::ldapHandle, nullptr, nullptr)) {
		ldap_unbind_ext_s(Ldap::ldapHandle, nullptr, nullptr);
		throw "Failed to start LDAP connection";
	}
}

bool Ldap::checkCredentials(std::string username, std::string password) {
	char* passwordBind;
	strcpy(passwordBind, password.c_str());
	BerValue bindCredentials;
	bindCredentials.bv_val = passwordBind;
	bindCredentials.bv_len = strlen(passwordBind);
	BerValue* serverCredentials;
	if (ldap_sasl_bind_s(Ldap::ldapHandle, username.c_str(), LDAP_SASL_SIMPLE, &bindCredentials, nullptr, nullptr, &serverCredentials) != LDAP_SUCCESS) {
		return false;
	}
	return true;
}

void Ldap::closeConnection() {
	ldap_unbind_ext_s(Ldap::ldapHandle, nullptr, nullptr);
}