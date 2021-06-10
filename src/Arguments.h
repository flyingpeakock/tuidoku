#pragma once
#include <map>
#include <string>

enum feature {
    Generate,
    Solve,
    Play,
};

class arguments {
    private:
    int argInt;
    std::string argStr;

    std::map<std::string, bool> args;
    std::map<std::string, bool> parse(int argc, char *argv[]);
    std::string getFileName(int argc, char *argv[]);
    int getInt(int argc, char *argv[]);
    bool incompatible(std::map<std::string, bool> args);
    public:
    bool printHelp();
    arguments(int argc, char *argv[]);
    bool shouldExit();
    feature getFeature();
    int getArgInt();
    std::string getFileName();
    bool fileArgSet();
};