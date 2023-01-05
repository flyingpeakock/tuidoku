#pragma once
#include "Tui.h"
#include <ncurses.h>

extern int up_key;
extern int down_key;
extern int select_key;

template <typename T>
WINDOW *Tui::printMenu(WINDOW *parentWin, std::vector<MenuItem<T>> items, std::string title) {

    int height = items.size() * 2 + 3;
    int width = title.size();
    for (const auto &item : items) {
        if (item.text.size() > width) {
            width = item.text.size();
        }
    }
    width += 7;

    WINDOW *win = derwin(parentWin, height, width, 7, 12);

    wattron(win, COLOR_MENU);

    wmove(win, 0, 0);
    for (auto i = 0; i < height * width; i++) {
        // Clearing like this to fill in the colors
        waddch(win, ' ');
    }
    //box(win, 0, 0);
    // Drawing my own box
    wmove(win, 0, 0);
    waddstr(win, "╔");
    for (auto i = 0; i < width - 2; i++) {
        waddstr(win, "═");
    }
    waddstr(win, "╗");
    for (auto i = 1; i < height - 1; i++) {
        mvwprintw(win, i, 0, "║");
        mvwprintw(win, i, width - 1, "║");
    }
    wmove(win, height - 1, 0);
    waddstr(win, "╚");
    for (auto i = 0; i < width - 2; i++) {
        waddstr(win, "═");
    }
    waddstr(win, "╝");

    wattron(win, A_UNDERLINE);
    mvwprintw(win, 1, (width - title.size()) / 2, title.c_str());
    wattroff(win, A_UNDERLINE);

    int printRow = 3;
    wmove(win, printRow - 1, 0);

    // Adding menu items
    waddstr(win, "╟");
    for (auto i = 0; i < width - 2; i++) {
        waddstr(win, "─");
    }
    waddstr(win, "╢");
    for (auto i = 0; i < items.size() - 1; i++) {
        MenuItem item = items[i];
        wmove(win, printRow++, 2);
        waddch(win, item.keyBind);
        waddstr(win, ". ");
        waddstr(win, item.text.c_str());
        wmove(win, printRow++, 0);
        waddstr(win, "╟");
        for (auto i = 1; i < width - 1; i++) {
            waddstr(win, "─");
        }
        waddstr(win, "╢");
    }
    mvwprintw(win, printRow, 2, "%c. %s", (items.end() - 1)->keyBind, (items.end() - 1)->text.c_str());

    wattroff(win, COLOR_MENU);

    overlay(win, parentWin);
    return win;
}

template <typename T>
T Tui::handleMenu(WINDOW *menuWin, std::vector<MenuItem<T>> items) {
    int currently_selected = 0;
    int past_selected = 0;
    int maxX = getmaxx(menuWin);
    keypad(menuWin, true);
    while (true) {
        // Highlight the currently selected
        int row_to_highlight = currently_selected * 2 + 3;
        mvwchgat(menuWin, row_to_highlight, 1, maxX - 2, A_BOLD | A_BLINK, 2, NULL);
        wrefresh(menuWin);
        int c = wgetch(menuWin);
        if (c == KEY_UP || c == up_key) {
            past_selected = currently_selected;
            currently_selected--;
        }
        else if (c == KEY_DOWN || c == down_key) {
            past_selected = currently_selected;
            currently_selected++;
        }
        else if (c == KEY_ENTER || c == select_key) {
            return items[currently_selected].choice;
        }
        else if (c >= '1' && c <= '9') {
            for (auto i = 0; i < items.size(); i++) {
                if (items[i].keyBind == c) {
                    return items[i].choice;
                }
            }
        }
        if (currently_selected < 0) {
            currently_selected = items.size() - 1;
        }
        if (currently_selected >= items.size()) {
            currently_selected = 0;
        }

        // Unhighlight the past selected row
        if (past_selected == currently_selected) continue;
        int row_to_unhighlight = past_selected * 2 + 3;
        mvwchgat(menuWin, row_to_unhighlight, 1, maxX - 2, A_NORMAL, 1, NULL);
    }
    return T(); // Should never get here
}
