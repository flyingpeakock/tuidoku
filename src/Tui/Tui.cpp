#include "Tui.h"
#include <ncurses.h>
#include <locale.h>
#include <string>
#include <iostream>
#include "../Config.h"

static const std::string topRowString = "╔═══════╤═══════╤═══════╦═══════╤═══════╤═══════╦═══════╤═══════╤═══════╗";
static const std::string row1String   = "║       │       │       ║       │       │       ║       │       │       ║";
static const std::string row2String   = "╟───────┼───────┼───────╫───────┼───────┼───────╫───────┼───────┼───────╢";
static const std::string row3String   = "╠═══════╪═══════╪═══════╬═══════╪═══════╪═══════╬═══════╪═══════╪═══════╣";
static const std::string botRowString = "╚═══════╧═══════╧═══════╩═══════╧═══════╧═══════╩═══════╧═══════╧═══════╝";

SCREEN* Tui::init_curses(Colors col) {
    setlocale(LC_ALL, "");
    //initscr();
    char termname[] = "xterm-256color";
    SCREEN *my_terminal;
    my_terminal = newterm(termname, stderr, stdin);

    if (!has_colors()) {
        return NULL;
    }
    cbreak(); // Get input directly without needing to press enter
    noecho(); // Dont show key presses
    curs_set(0);

    // Setting up colors
    use_default_colors();
    start_color();
    init_pair(1, col.menu[0], col.menu[1]);
    init_pair(2, col.menu_selected[0], col.menu_selected[1]);
    init_pair(3, col.highlight_pencil[0], col.highlight_pencil[1]);
    init_pair(4, col.highlight_filled[0], col.highlight_filled[1]);
    init_pair(5, col.error[0], col.error[1]);
    init_pair(6, col.grid[0], col.grid[1]);
    init_pair(7, col.numbers[0], col.numbers[1]);
    init_pair(8, col.pencil[0], col.pencil[1]);
    return my_terminal;
}

void Tui::end_curses(SCREEN *screen, WINDOW *window) {
    delwin(stdscr);
    delwin(window);
    endwin();
    delscreen(screen);
}

WINDOW *Tui::createMainWindow() {
    int cols = 73;
    int rows = 38;
    int starty = (LINES - rows) / 2;
    int startx = (COLS - cols) / 2;
    if (startx < 0) startx = 0;
    if (starty < 0) starty = 0;
    WINDOW *win = newwin(rows, cols, starty, startx);
    keypad(win, true);
    wrefresh(stdscr);
    wrefresh(win);
    return win;
}

void Tui::printOutline(WINDOW * win) {
    int startHeight = 0;
    int startCol = 0;
    werase(win);
    wattron(win, A_BOLD);
    wattron(win, COLOR_GRID);
    mvwprintw(win, startHeight++, startCol, topRowString.c_str());
    for (auto i = 0; i < 3; i++) {
        for (auto j = 0; j < 3; j++) {
            mvwprintw(win, startHeight++, startCol, row1String.c_str());
        }
        mvwprintw(win, startHeight++, startCol, row2String.c_str());
        for (auto j = 0; j < 3; j++) {
            mvwprintw(win, startHeight++, startCol, row1String.c_str());
        }
        mvwprintw(win, startHeight++, startCol, row2String.c_str());
        for (auto j = 0; j < 3; j++) {
            mvwprintw(win, startHeight++, startCol, row1String.c_str());
        }
        if (i != 2) {
            mvwprintw(win, startHeight++, startCol, row3String.c_str());
        }
    }
    mvwprintw(win, startHeight, startCol, botRowString.c_str());
    wattroff(win, COLOR_GRID);
    wattroff(win, A_BOLD);
}

void Tui::addMessage(WINDOW *win, std::string message) {
    int maxY = getmaxy(win);
    int maxX = getmaxx(win);

    // clearing any previous message
    wmove(win, maxY - 1, 1);
    for (auto i = 0; i < maxX - 1; i++) {
        waddch(win, ' ');
    }
    mvwprintw(win, maxY - 1, 1, message.c_str());
}

void Tui::printPuzzle(WINDOW *win, Sudoku::puzzle grid, int attr) {
    int start_row = 2;
    wattron(win, attr);
    wattron(win, COLOR_NUM);
    for (auto i = 0; i < Sudoku::SIZE; i++) {
        int start_col = 4;
        for (auto j = 0; j < Sudoku::SIZE; j++) {
            if (grid[i][j] != 0) {
                mvwaddch(win, start_row, start_col, (char)grid[i][j] + '0');
            }
            start_col += 8;
        }
        start_row += 4;
    }
    wattroff(win, COLOR_NUM);
    wattroff(win, attr);
}

void Tui::printPencilMark(WINDOW *win, int row, int col, std::uint16_t marks) {
    row = row * 4 + 1;
    col = col * 8 + 2;
    int startCol = col;

    wattron(win, A_DIM);
    wattron(win, COLOR_PENCIL);
    for (auto i = 0; i < 9; i++) {
        if ((marks & (1 << i)) != 0) {
            mvwaddch(win, row, col, i + '1');
        }
        col += 2;
        if (col > startCol + 4) {
            row++;
            col = startCol;
        }
    }
    wattroff(win, COLOR_PENCIL);
    wattroff(win, A_DIM);
}

void Tui::highlightCell(WINDOW *win, int current_row, int current_col) {
    static int prev_row = 0;
    static int prev_col = 0;

    if (prev_row != current_row || prev_col != current_col) {
        printOutline(win); // Drawing over the previously highlighted box
    }

    int row = current_row * 4;
    int col = current_col * 8;
    std::string tl = "╆";
    std::string tr = "╅";
    std::string bl = "╄";
    std::string br = "╃";
    std::string t  = "━";
    std::string b  = "━";
    std::string l  = "┃";
    std::string r =  "┃";

    if (current_row == 0 && current_col == 0) {
        tl = "┏";
        tr = "┱";
        bl = "┣";
    }
    else if (current_row == Sudoku::SIZE - 1 && current_col == Sudoku::SIZE - 1) {
        br = "┛";
        bl = "┺";
        tr = "┫";
    }
    else if (current_row == 0 && current_col == Sudoku::SIZE - 1) {
        tr = "┓";
        tl = "┲";
        br = "┫";
    }
    else if (current_row == Sudoku::SIZE - 1 && current_col == 0) {
        bl = "┗";
        br = "┹";
        tl = "┣";
    }
    else if (current_row == 0) {
        tl = "┳";
        tr = "┳";
    }
    else if (current_row == Sudoku::SIZE - 1) {
        bl = "┻";
        br = "┻";
    }
    else if (current_col == 0) {
        tl = "┣";
        bl = "┣";
    }
    else if (current_col == Sudoku::SIZE - 1) {
        tr = "┫";
        br = "┫";
    }

    wattron(win, A_BOLD);
    wattron(win, COLOR_GRID);
    mvwprintw(win, row, col, tl.c_str());
    for (auto i = 0; i < 7; i++) {
        wprintw(win, t.c_str());
    }
    wprintw(win, tr.c_str());
    for (auto i = 0; i < 3; i++) {
        mvwprintw(win, ++row, col, l.c_str());
        mvwprintw(win, row, col + 8, r.c_str());
    }
    mvwprintw(win, row+1, col, bl.c_str());
    for (auto i = 0; i < 7; i++) {
        wprintw(win, b.c_str());
    }
    wprintw(win, br.c_str());
    wattroff(win, COLOR_GRID);
    wattroff(win, A_BOLD);

    prev_row = current_row;
    prev_col = current_col;
}

int Tui::userInputBox(WINDOW *win, std::string text) {
    int height = 3;
    int width = text.size() + 10;
    
    WINDOW *inputBox = derwin(win, height, width, 7, 12);
    wattron(inputBox, COLOR_MENU);
    wmove(inputBox, 0, 0);
    for (auto i = 0; i < height * width; i++) {
        waddch(inputBox, ' '); // clearing like this to fill in colors
    }
    wmove(inputBox, 0, 0);
    waddstr(inputBox, "╔");
    for (auto i = 0; i < width - 2; i++) {
        waddstr(inputBox, "═");
    }
    waddstr(inputBox, "╗");
    for (auto i = 1; i < height - 1; i++) {
        mvwprintw(inputBox, i, 0, "║");
        mvwprintw(inputBox, i, width - 1, "║");
    }
    wmove(inputBox, height - 1, 0);
    waddstr(inputBox, "╚");
    for (auto i = 0; i < width - 2; i++) {
        waddstr(inputBox, "═");
    }
    waddstr(inputBox, "╝");

    mvwprintw(inputBox, 1, 1, " %s ", text.c_str());
    wchgat(inputBox, width - text.size() - 4, A_BOLD, 2, NULL);
    overlay(inputBox, win);
    
    std::string ret = "";
    wattron(inputBox, COLOR_MENU_SELECT);
    while (true) {
        wrefresh(inputBox);
        int c = getch();
        if (c == '\n' || c == KEY_ENTER) {
            break;
        }
        if (c >= '0' && c <= '9') {
            ret += c;
            waddch(inputBox, c);
        }
        if (c == 'q' || c == 'Q' || c == 27) {
            ret = "0";
            break;
        }
    }
    wattroff(inputBox, COLOR_MENU_SELECT);
    delwin(inputBox);
    try {
        return std::stoi(ret);
    }
    catch (std::invalid_argument const &e) {
        return 0;
    }
}

void Tui::highlightNum(WINDOW *win, int row, int col, int num, int attr, int color) {
    if (num == 0)
        return;
    char val = (char) num + '0';

    int cur_row = row * 4 + 1;
    int cur_col = col * 8 + 1;

    for (auto i = cur_row; i < cur_row + 3; i++) {
        for (auto j = cur_col; j < cur_col + 7; j++) {
            auto here = mvwinch(win, i, j);
            if ((here & A_CHARTEXT) == val) {
                mvwchgat(win, i, j - 1, 3, attr, color, NULL);
            }
        }
    }
}
