#pragma once
#include <map>
#include <string>

namespace arguments {
    std::map<std::string, bool> parse(int argc, char *argv[]);
    std::string getFileName(int argc, char *argv[]);
    int getInt(int argc, char *argv[]);
    void printHelp();
    bool incompatible(std::map<std::string, bool> args);
}