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

};

#endif