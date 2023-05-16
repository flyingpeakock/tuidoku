#include "Tui/Tui.h"                    // for Tui
#include "sudoku/DancingLinkObjects.h"  // for eSize, DancingLinkContainer

#include <libconfig.h++>

int main(int argc, char *argv[]) {

    libconfig::Config cfg;

    tui::Tui tui;
    tui.runLoop();
    return 0;
}