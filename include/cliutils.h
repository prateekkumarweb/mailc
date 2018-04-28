#ifndef CLIUTILS_H
#define CLIUTILS_H

#include "imap.h"
#include "smtp.h"
#include <dirent.h>
#include <fstream>
#include <string>
#include <sstream>
#include <experimental/filesystem>

struct config {
    std::string imap_server;
    int imap_port;
    std::string smtp_server;
    int smtp_port;
    std::string username;
    std::string password;
};

namespace fs = std::experimental::filesystem;

namespace cliutils {
	bool sync(IMAPConnection &imap);
};

#endif