#pragma once
#include <ncurses.h>
#include <string>
#include <vector>
#include "../Sudoku/Sudoku.h"

#define COLOR_MENU           COLOR_PAIR(1)
#define COLOR_MENU_SELECT    COLOR_PAIR(2)
#define COLOR_HIGHLIGHT      COLOR_PAIR(3)
#define COLOR_HIGHLIGH_NUM   3
#define COLOR_FILLED         COLOR_PAIR(4)
#define COLOR_FILLED_NUM     4
#define COLOR_ERROR          COLOR_PAIR(5)
#define COLOR_ERROR_NUM      5
#define COLOR_GRID           COLOR_PAIR(6)
#define COLOR_GRID_NUM       6
#define COLOR_NUM            COLOR_PAIR(7)
#define COLOR_NUM_NUM        7
#define COLOR_PENCIL         COLOR_PAIR(8)
#define COLOR_PENCIL_NUM     8

namespace Tui {
    const std::string title = "Tuidoku";

    template <typename T>
    struct MenuItem {
        std::string text;
        char keyBind;
        T choice;
    };

    enum main_menu_choices {
        PLAY,
        SOLVE,
        GENERATE,
        EXIT
    };

    SCREEN* init_curses();
    void end_curses(SCREEN *screen, WINDOW *window);

    WINDOW *createMainWindow();

    void printOutline(WINDOW *win);

    void addMessage(WINDOW *win, std::string message);

    void printPuzzle(WINDOW *win, Sudoku::puzzle grid, int attr);

    void printPencilMark(WINDOW *win, int row, int col, std::uint16_t marks);

    void highlightCell(WINDOW *win, int row, int col);

    void highlightNum(WINDOW *win, int row, int col, int num, int attr, int color);

    int userInputBox(WINDOW *win, std::string text);

    template <typename T>
    WINDOW* printMenu(WINDOW *parentWin, std::vector<MenuItem<T>> items, std::string title);

    template <typename T>
    T handleMenu(WINDOW *menuWin, std::vector<MenuItem<T>> items);
    #include "Menu.hpp"
} // namespace Tui