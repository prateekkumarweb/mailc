#ifndef IMAP_H
#define IMAP_H

class IMAPConnection {
public:
	IMAPConnection(std::string &hostname, int port);
	bool login(std::string &username, std::string &password); // OK, NO, BAD

	bool noop(); // OK, BAD
	bool logout();

	

	~IMAPConnection();
private:
	Socket socket;
	std::string username;
	std::string password;
};

#endif

/**

SELECT MAILBOX
sends exists, unseen, recent, etc.

EXAMINE similar to SELECT (read-only)

CREATE mailbox

DELETE mailbox

RENAME mailbox

LIST (lists messages)

STATUS mailbox ...

APPEND ?

CHECK ?

CLOSE (Delets msgs flagged)

(CLOSE vs EXPUNGE) ?

SEARCH

FETCH 

STORE ?

COPY mails across mailbox

UID ?



*/