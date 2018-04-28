#ifndef CLIUTILS_H
#define CLIUTILS_H

#include "imap.h"
#include <dirent.h>
#include <fstream>
#include <string>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

void sync();

#endif