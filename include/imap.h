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
};

class IMAPConnection {
public:
	IMAPConnection(const std::string &hostname, int port);
	bool login(const std::string &username, const std::string &password); // OK, NO, BAD

	bool noop(); // OK, BAD
	bool logout();

	std::vector<Mail> getUnseenMails(const std::string &mailbox);

	std::vector<Mail> getTopMails(const std::string &mailbox, int k);
	
	std::vector<int> search(const std::string &mailbox, const std::string &from = "",
                const std::string &to = "", const std::string &subject = "", const std::string &text = "" ,
                const std::string &nottext = "", const std::string &since = "", const std::string &before = "");

	Mail getMail(const std::string &mailbox, const int uid);
	
	std::vector<Mail> getMails(const std::string &mailbox, std::vector<int> uids);

	bool deleteMail(Mail mail);

	std::tuple<int, int, int> getCount(const std::string &mailbox);

	bool createMailbox(const std::string &mailbox);

	bool renameMailbox(const std::string &oldmailbox, const std::string &newmailbox);

	bool deleteMailbox(const std::string &mailbox);
	
	std::vector<std::string> getmailboxes();
	
	bool select(const std::string &mailbox);
	
	std::vector<int> getAllmails(const std::string &mailbox);
	
	bool moveMail(const std::string &oldmailbox, int uid, const std::string &newmailbox);

	// ~IMAPConnection();
private:
	Socket socket;
	std::string username;
	std::string password;
	std::regex rgx;
	std::string id_string;
	bool check_response(const std::string &response, const std::string &id_string);
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
