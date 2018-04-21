#ifndef IMAP_H
#define IMAP_H

class Mail {
	// ?
};

class IMAPConnection {
public:
	IMAPConnection(std::string &hostname, int port);
	bool login(std::string &username, std::string &password); // OK, NO, BAD

	bool noop(); // OK, BAD
	bool logout();

	std::vector<Mail> getUnseenMails(const std::string &mailbox);

	std::vector<Mail> getTopMails(const std::string &mailbox, int k);

	bool deleteMail(Mail mail);

	int getUnseenCount(const std::string &mailbox);

	int getTotalCount(const std::string &mailbox);

	int getRecentCount(const std::string &mailbox);

	bool createMailbox(std::string &mailbox);

	bool renameMailbox(std::string &mailbox);

	bool deleteMailbox(std::string &mailbox);

	Mail getMail(... uid);

	std::vector<Mail> search(???);

	std::vector<??> listFolders(const std::string &mailbox, ?? (Wildcard));

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