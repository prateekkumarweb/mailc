#ifndef CLIUTILS_H
#define CLIUTILS_H

#include "imap.h"
#include <filesystem>
#include <dirent.h>
#include <fstream>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

void sync();
