#pragma once
#include <libconfig.h++>
#include <string>

enum keys {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    INSERT,
    PENCIL,
    EXIT,
    CLEAR,
    FILLPENCIL,
    HINT,
};

enum Colors {
    MENU,
    MENU_SELECTED,
    HIGHLIGHT_PENCIL,
    HIGHLIGHT_FILLED,
    ERROR,
    GRID,
    NUMBERS,
    PENCIL_NUM,
};

class Conf {
    private:
    std::string location;
    libconfig::Config cfg;
    bool verifyValues();

    public:
    Conf();
    Conf(std::string locate);
    bool init();
    int keyBind(keys key);
    int getColor(Colors col, std::string fgOrBg);
    void write();
};

extern Conf config;