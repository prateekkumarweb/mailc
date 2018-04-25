#ifndef SMTP_H
#define SMTP_H

#include "socket.h"
#include <iostream>

class SMTPConnection {
public:
	SMTPConnection(const std::string &hostname, int port);
	bool auth(const std::string &username, const std::string &password);
	bool send(const std::string &from, const std::string &to,
		const std::string &subject, const std::string &body);
	// ~SMTPConnection();
private:
	Socket socket;
	std::string username;
	std::string password;
};

#endif

/**

HELO

MAIL

RCPT

DATA

SEND

SOML

SAML

RSET

VRFY

EXPN

HELP

NOOP

QUIT

TURN


*/