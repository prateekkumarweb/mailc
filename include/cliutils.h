#ifndef CLIUTILS_H
#define CLIUTILS_H

#include "imap.h"
#include "smtp.h"
#include <dirent.h>
#include <fstream>
#include <string>
#include <sstream>
#include <random>
#include <experimental/filesystem>

struct config {
    int imap_port;
    int smtp_port;
    std::string imap_server;
    std::string smtp_server;
    std::string username;
    std::string password;
    std::string name;
};

namespace fs = std::experimental::filesystem;

namespace cliutils {
	bool sync(IMAPConnection &imap);
	bool sendMail(config &config, const std::string &to,
		const std::string &subject, const std::string &msg);
    bool deleteMail(IMAPConnection &imap, const std::string& mailbox, int uid);
    Mail readMail(const std::string& mailbox, int uid);
    std::vector<Mail> getMails(IMAPConnection &imap, const std::string &mailbox, std::vector<int> uids);
    std::vector<int> searchMails(IMAPConnection &imap, const std::string &mailbox, const std::string &from,
                const std::string &to, const std::string &subject, const std::string &text,
                const std::string &nottext, const std::string &since, const std::string &before);
    bool createMailbox(IMAPConnection &imap, const std::string &mailbox);
    bool deleteMailbox(IMAPConnection &imap, const std::string &mailbox);
    bool renameMailbox(IMAPConnection &imap, const std::string &oldmailbox, const std::string &newmailbox);
    bool moveMail(IMAPConnection &imap, const std::string &oldmailbox, int uid, const std::string &newmailbox);
    std::vector<std::string> getMailboxes();
};

#endif
