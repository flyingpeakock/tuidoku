#pragma once
#include <string>
#include <map>
#include <array>

namespace Conf {
    bool verifyValues();

    void setLocation(std::string locate);
    bool init();
    int keyBind(std::string key);
    int getColor(std::string col, std::string fgOrBg);
    void write();
}
