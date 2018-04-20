#ifndef SMTP_H
#define SMTP_H

class SMTPConnection {
public:
	SMTPConnection(std::string &hostname, int port);
	bool auth(std::string &username, std::string &password);
	~SMTPConnection();
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