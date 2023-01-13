#include "Config.h"
#include <string>
#include <vector>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <ncurses.h>
#include <libconfig.h++>

libconfig::Config cfg;
static std::string location;

static std::map<std::string, int> color_map = {
    {"black", COLOR_BLACK},
    {"red", COLOR_RED},
    {"green", COLOR_GREEN},
    {"yellow", COLOR_YELLOW},
    {"blue", COLOR_BLUE},
    {"magenta", COLOR_MAGENTA},
    {"cyan", COLOR_CYAN},
    {"white", COLOR_WHITE},
    {"default", -1},
};

struct setting_t {
    std::string name;
    std::string value;
};


struct Colors_t {
    std::string name;
    setting_t foreground;
    setting_t background;
};

static setting_t allBinds[] = {
    {"up", "k"},
    {"down", "j"},
    {"left", "h"},
    {"right", "l"},

    // Mode binds
    {"pencil", "p"},
    {"insert", "i"},
    {"erase", std::string(1, (char)127)},
    {"fillPencils", "a"},
    {"hint", "?"},

    // Other
    {"exit", "q"},
};

static Colors_t allColors[] = {
    {
        "menu",
        {"foreground", "black"},
        {"background", "green"}
    },
    {
        "menuSelected",
        {"foreground", "black"},
        {"background", "yellow"}
    },
    {
        "highlightPencil",
        {"foreground", "black"},
        {"background", "blue"}
    },
    {
        "highlightFilled",
        {"foreground", "black"},
        {"background", "green"}
    },
    {
        "error",
        {"foreground", "red"},
        {"background", "default"}
    },
    {
        "grid",
        {"foreground", "default"},
        {"background", "default"}
    },
    {
        "numbers",
        {"foreground", "default"},
        {"background", "default"}
    },
    {
        "marks",
        {"foreground", "default"},
        {"background", "default"}
    }
};

static std::string getEnvVar(const std::string &key);

static std::string getDefaultLocation();

static bool exists(const std::string &file);

void Conf::setLocation(std::string locate){
    location = locate;
}

bool Conf::init() {
    // Attempting to read a config file
    if (location == "") {
        std::vector<std::string> potentialLocations;
        std::string xdg_conf = getEnvVar("XDG_CONFIG_HOME");
        std::string home = getEnvVar("HOME");
        if (xdg_conf != "") {
            potentialLocations.push_back(xdg_conf + "/tuidoku/tuidoku.conf");
            potentialLocations.push_back(xdg_conf + "/tuidoku.conf");
        }
        if (xdg_conf != home + "/.config" && home != "") {
            potentialLocations.push_back(home + "/.config/tuidoku/tuidoku.conf");
            potentialLocations.push_back(home + "/.config/tuidoku.conf");
        }
        if (home != "") {
            potentialLocations.push_back(home + "/.tuidoku.conf");
        }
        potentialLocations.push_back("/etc/tuidoku/tuidoku.conf");
        for (const auto &locate : potentialLocations) {
            if (!exists(locate)) {
                continue;
            }
            try {
                cfg.readFile(locate);
                location = locate;
                verifyValues();
                return true;
            }
            catch (const libconfig::FileIOException &fioex) {
                std::cerr << "I/O error while reading file." << std::endl;
                return false;
            }
            catch (const libconfig::ParseException &pex) {
                std::cerr << "Parse error at " << pex.getFile() << ':' << pex.getLine()
                          << " - " << pex.getError() << std::endl;
                return false;
            }
        }
    }

    // Creating default in memory instead
    libconfig::Setting &root = cfg.getRoot();
    libconfig::Setting &binds = root.add("keybinds", libconfig::Setting::TypeGroup);
    libconfig::Setting &cols = root.add("colors", libconfig::Setting::TypeGroup);

    for (const auto &k : allBinds) {
        binds.add(k.name, libconfig::Setting::TypeString) = k.value;
    }

    for (const auto &c : allColors) {
        cols.add(c.name, libconfig::Setting::TypeGroup);
        cols.lookup(c.name).add(c.foreground.name, libconfig::Setting::TypeString) = c.foreground.value;
        cols.lookup(c.name).add(c.background.name, libconfig::Setting::TypeString) = c.background.value;
    }

    location = getDefaultLocation();
    return true;
}

bool Conf::verifyValues() {
    bool ret = true;
    try {
        libconfig::Setting &binds = cfg.lookup("keybinds");
    }
    catch (const libconfig::SettingNotFoundException &nfex) {
        std::cerr << "No 'keybinds' settings found in " << location << std::endl;
        return false;
    }
    try {
        libconfig::Setting &colors = cfg.lookup("colors");
    }
    catch (const libconfig::SettingNameException &nfex) {
        std::cerr << "No 'colors' settings found in " << location << std::endl;
        return false;
    }

    libconfig::Setting &binds = cfg.lookup("keybinds");
    for (auto &b : allBinds) {
        try {
            libconfig::Setting &setting = binds.lookup(b.name);
            b = {b.name, setting.c_str()};
        }
        catch (const libconfig::SettingNotFoundException &nfex) {
            std::cerr << "No keybind for " << b.name << " found in " << location << std::endl;
            binds.add(b.name, libconfig::Setting::TypeString) = b.value;
            ret = false;
        }
    }
    libconfig::Setting &colors = cfg.lookup("colors");
    for (const auto &c : allColors) {
        try {
            libconfig::Setting &col = colors.lookup(c.name);
            try {
                std::string fg = col.lookup("foreground").c_str();
                std::string bg = col.lookup("background").c_str();
                if (color_map.count(fg) == 0) {
                    std::cerr << "Color " << fg << " is not known" << std::endl;
                    col.lookup("foreground") = c.foreground.value;
                    ret = false;
                }
                if (color_map.count(bg) == 0) {
                    std::cerr << "Color " << bg << " is not known" << std::endl;
                    col.lookup("background") = c.background.value;
                    ret = false;
                }
            }
            catch (const libconfig::SettingNotFoundException &nfex) {
                std::cerr << "Color " << c.name << " is missing background or foreground" << std::endl;
                if (!col.exists("foreground")) {
                    col.add("foreground", libconfig::Setting::TypeString) = c.foreground.value;
                }
                if (!col.exists("background")) {
                    col.add("background", libconfig::Setting::TypeString) = c.background.value;
                }
                ret = false;
            }
        }
        catch (const libconfig::SettingNotFoundException &nfex) {
            std::cerr << "No colors for " << c.name << " found in " << location << std::endl;
            colors.add(c.name, libconfig::Setting::TypeGroup);
            colors.lookup(c.name).add("foreground", libconfig::Setting::TypeString) = c.foreground.value;
            colors.lookup(c.name).add("background", libconfig::Setting::TypeString) = c.background.value;
        }
    }

    return ret;
}

int Conf::keyBind(std::string key) {
    libconfig::Setting &keybinds = cfg.lookup("keybinds");
    std::string key_string = keybinds.lookup(key);
    return (int)key_string[0];
}

int Conf::getColor(std::string col, std::string fgOrBg) {
    libconfig::Setting &colors = cfg.lookup("colors");
    std::string color_string = colors.lookup(col).lookup(fgOrBg).c_str();
    return color_map[color_string];
}

void Conf::write() {
    try {
        cfg.writeFile(location);
        std::cerr << "Configuration file successfully written to " << location << std::endl;
    }
    catch (const libconfig::FileIOException &fioex) {
        std::cerr << "I/O error while writing file: " << location << std::endl;
    }
}

static std::string getEnvVar(const std::string &key) {
    char *val = std::getenv(key.c_str());
    return val == NULL ? "" : val;
}

static std::string getDefaultLocation() {
    std::string l = getEnvVar("XDG_CONFIG_HOME");
    if (l != "" && exists(l)) {
        return l+ "/tuidoku/tuidoku.conf";
    }

    l = getEnvVar("HOME");
    if (l != "") {
        if (exists(l + "/.config")) {
            return l + "/.config/tuidoku/tuidoku.conf";
        }
        if (exists(l)) {
            return l + ".tuidoku.conf";
        }
    }

    l = "/etc/";
    if (exists(l)) {
        return l + "/tuidoku/tuidoku.conf";
    }

    return "";
}

static bool exists(const std::string &file) {
    std::filesystem::path f{file};
    return std::filesystem::exists(f);
}