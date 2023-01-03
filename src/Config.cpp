#include "Config.h"
#include <string>
#include <vector>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <map>
#include <ncurses.h>

Conf config;

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

static const setting_t allBinds[] = {
    {"up", "k"},
    {"down", "j"},
    {"left", "h"},
    {"right", "l"},

    // Mode binds
    {"pencil", "p"},
    {"insert", "i"},
    {"erase", std::string(1, (char)127)},
    {"fillPencils", "a"},

    // Other
    {"exit", "q"},
};

static const Colors_t allColors[] = {
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

Conf::Conf(){}

Conf::Conf(std::string locate){
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
                return verifyValues();
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
    for (const auto &b : allBinds) {
        try {
            libconfig::Setting &setting = binds.lookup(b.name);
        }
        catch (const libconfig::SettingNotFoundException &nfex) {
            std::cerr << "No keybind for " << b.name << " found in " << location << std::endl;
            binds.add(b.name, libconfig::Setting::TypeString) = b.value;
            return false;
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
                    return false;
                }
                if (color_map.count(bg) == 0) {
                    std::cerr << "Color " << bg << " is not known" << std::endl;
                    col.lookup("background") = c.background.value;
                    return false;
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
                return false;
            }
        }
        catch (const libconfig::SettingNotFoundException &nfex) {
            std::cerr << "No colors for " << c.name << " found in " << location << std::endl;
            colors.add(c.name, libconfig::Setting::TypeGroup);
            colors.lookup(c.name).add("foreground", libconfig::Setting::TypeString) = c.foreground.value;
            colors.lookup(c.name).add("background", libconfig::Setting::TypeString) = c.background.value;
        }
    }

    return true;
}

int Conf::keyBind(keys key) {
    std::string key_string;
    libconfig::Setting &keybinds = cfg.lookup("keybinds");
    switch (key) {
        case UP:
            key_string = keybinds.lookup("up").c_str();
            break;
        case DOWN:
            key_string = keybinds.lookup("down").c_str();
            break;
        case LEFT:
            key_string = keybinds.lookup("left").c_str();
            break;
        case RIGHT:
            key_string = keybinds.lookup("right").c_str();
            break;
        case INSERT:
            key_string = keybinds.lookup("insert").c_str();
            break;
        case PENCIL:
            key_string = keybinds.lookup("pencil").c_str();
            break;
        case EXIT:
            key_string = keybinds.lookup("exit").c_str();
            break;
        case CLEAR:
            key_string = keybinds.lookup("erase").c_str();
            break;
        case FILLPENCIL:
            key_string = keybinds.lookup("fillPencils").c_str();
            break;
        default:
            return 0;
    }
    return (int)key_string[0];
}

int Conf::getColor(Colors col, std::string fgOrBg) {
    std::string color_string;
    libconfig::Setting &colors = cfg.lookup("colors");

    switch(col) {
        case MENU:
            color_string = colors.lookup("menu").lookup(fgOrBg).c_str();
            break;
        case MENU_SELECTED:
            color_string = colors.lookup("menuSelected").lookup(fgOrBg).c_str();
            break;
        case HIGHLIGHT_PENCIL:
            color_string = colors.lookup("highlightPencil").lookup(fgOrBg).c_str();
            break;
        case HIGHLIGHT_FILLED:
            color_string = colors.lookup("highlightFilled").lookup(fgOrBg).c_str();
            break;
        case ERROR:
            color_string = colors.lookup("error").lookup(fgOrBg).c_str();
            break;
        case GRID:
            color_string = colors.lookup("grid").lookup(fgOrBg).c_str();
            break;
        case NUMBERS:
            color_string = colors.lookup("numbers").lookup(fgOrBg).c_str();
            break;
        case PENCIL_NUM:
            color_string = colors.lookup("marks").lookup(fgOrBg).c_str();
            break;
        default:
            return -1;
    }
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
    std::string location = getEnvVar("XDG_CONFIG_HOME");
    if (location != "" && exists(location)) {
        return location + "/tuidoku/tuidoku.conf";
    }

    location = getEnvVar("HOME");
    if (location != "") {
        if (exists(location + "/.config")) {
            return location + "/.config/tuidoku/tuidoku.conf";
        }
        if (exists(location)) {
            return location + ".tuidoku.conf";
        }
    }

    location = "/etc/";
    if (exists(location)) {
        return location + "/tuidoku/tuidoku.conf";
    }

    return "";
}

static bool exists(const std::string &file) {
    std::filesystem::path f{file};
    return std::filesystem::exists(f);
}