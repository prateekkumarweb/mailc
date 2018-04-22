#ifndef IMAP_H
#define IMAP_H

#include <tuple>
#include <string>
#include <vector>
#include <regex>
#include <iostream>

#include "socket.h"

struct Mail {
    std::string mailbox;
    int uid;
    
    std::string from;
    std::string to;
    std::string subject;
    std::string date;
    std::string text;
    std::string html;
};

class IMAPConnection {
public:
	IMAPConnection(std::string hostname, int port);
	bool login(std::string username, std::string password); // OK, NO, BAD

	bool noop(); // OK, BAD
	bool logout();

	std::vector<Mail> getUnseenMails(const std::string &mailbox);

	std::vector<Mail> getTopMails(const std::string &mailbox, int k);
	
	Mail getMail(const std::string mailbox, const int uid);

	bool deleteMail(Mail mail);

	std::tuple<int, int, int> getCount(const std::string &mailbox);

	bool createMailbox(std::string mailbox);

	bool renameMailbox(std::string &oldmailbox, std::string &newmailbox);

	bool deleteMailbox(std::string &mailbox);

	// std::vector<Mail> search(???);

	// std::vector<??> listFolders(const std::string &mailbox, ?? (Wildcard));

	// ~IMAPConnection();
private:
	Socket socket;
	std::string username;
	std::string password;
	std::regex rgx;
	bool check_response(std::string &response, std::string &id_string);
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
