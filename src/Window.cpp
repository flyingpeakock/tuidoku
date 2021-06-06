#include "Window.h"
#include <locale.h>
#include <string.h>
#include "config.h"

Window::Window(Board *g){
    game = g;

    mode = "Insert mode";

    checkColors = false;
    cursorRow = 0;
    cursorCol = 0;
    highlightNum = 0;

    auto keyidx = 0;
    setlocale(LC_ALL, "");
    
    initscr();

    cbreak(); // Get input before enter is pressed;
    noecho(); // Don't show keypresses
    keypad(stdscr, true); // Use arrow keys to move

    if (has_colors()) {
        use_default_colors();
        start_color();
        init_pair(1, ERROR_COLOR, -1);
        init_pair(2, CORRECT_COLOR, -1);
        init_pair(3, HIGHLIGHT_COLOR, -1);
        init_pair(4, LOWLIGHT_COLOR, -1);
    }

    printBoard();

}

Window::~Window() {
    endwin();
}

void Window::printBoard() {
    int oldRows = windowRows;
    int oldCols = windowCols;
    getmaxyx(stdscr, windowRows, windowCols);
    bool resize = (oldRows != windowRows || oldCols != windowCols);

    boardTop = (windowRows - BoardRows) / 2;
    boardLeft = (windowCols - BoardCols) / 2;
    int gridTop = boardTop + 1;
    int gridLeft = boardLeft + 2;

    if (resize) {
        clear();
        if (windowRows < BoardRows || windowCols < BoardCols) {
            char error[] = "Not enough space to draw board";
            mvprintw(windowRows / 2, (windowCols - strlen(error)) / 2, "%s", error);
            refresh();
            return;
        }
        if (windowRows - BoardRows > 3 && PRINT_TITLE) {
            attron(A_BOLD | A_UNDERLINE);
            mvprintw(boardTop - 3, (windowCols - strlen(TITLE)) / 2, "%s", TITLE);
            attroff(A_BOLD | A_UNDERLINE);
        }
        printBoxes();
        printInstructions();
        printCoords();
    }

    printNumbs();
    printPencil();
    printMode();
    printCursor();
    refresh();
}

void Window::printBoxes() {

    const char topRow[] = "\u2554\u2550\u2550\u2550\u2564\u2550\u2550\u2550\u2564"
                          "\u2550\u2550\u2550\u2566\u2550\u2550\u2550\u2564\u2550"
                          "\u2550\u2550\u2564\u2550\u2550\u2550\u2566\u2550\u2550"
                          "\u2550\u2564\u2550\u2550\u2550\u2564\u2550\u2550\u2550\u2557";
    const char middleRow[] = "\u2551   \u2502   \u2502   \u2551   \u2502"
                             "   \u2502   \u2551   \u2502   \u2502   \u2551";
    const char middleRow2[] = "\u255f\u2500\u2500\u2500\u253c\u2500\u2500\u2500\u253c"
                              "\u2500\u2500\u2500\u256b\u2500\u2500\u2500\u253c"
                              "\u2500\u2500\u2500\u253c\u2500\u2500\u2500\u256b"
                              "\u2500\u2500\u2500\u253c\u2500\u2500\u2500\u253c"
                              "\u2500\u2500\u2500\u2562";
    const char middleRow3[] = "\u2560\u2550\u2550\u2550\u256a\u2550\u2550\u2550\u256a"
                              "\u2550\u2550\u2550\u256c\u2550\u2550\u2550\u256a"
                              "\u2550\u2550\u2550\u256a\u2550\u2550\u2550\u256c"
                              "\u2550\u2550\u2550\u256a\u2550\u2550\u2550\u256a"
                              "\u2550\u2550\u2550\u2563";
    const char botRow[] = "\u255a\u2550\u2550\u2550\u2567\u2550\u2550\u2550\u2567"
                          "\u2550\u2550\u2550\u2569\u2550\u2550\u2550\u2567"
                          "\u2550\u2550\u2550\u2567\u2550\u2550\u2550\u2569"
                          "\u2550\u2550\u2550\u2567\u2550\u2550\u2550\u2567"
                          "\u2550\u2550\u2550\u255d";
    
    int startHeight = boardTop;
    attron(A_BOLD);
    mvprintw(startHeight, boardLeft, "%s", topRow);
    for (auto i = 0; i < 3; i++) {
        mvprintw(++startHeight, boardLeft, "%s", middleRow);
        mvprintw(++startHeight, boardLeft, "%s", middleRow2);
        mvprintw(++startHeight, boardLeft, "%s", middleRow);
        mvprintw(++startHeight, boardLeft, "%s", middleRow2);
        mvprintw(++startHeight, boardLeft, "%s", middleRow);
        if (i != 2)
            mvprintw(++startHeight, boardLeft, "%s", middleRow3);
    }
    mvprintw(++startHeight, boardLeft, "%s", botRow);
    attroff(A_BOLD);
}

void Window::printPencil() {
    attron(A_DIM);
    auto marks = game->getPencilMarks();
    auto grid = game->getPlayGrid();
    int row = boardTop + 1;
    for (auto i = 0; i < 9; i++) {
        int col = boardLeft + 1;
        for (auto j = 0; j < 9; j++) {
           if (grid[i][j] < 1) {
               int idx[] = {0, 2, 1};
               move(row, col);
               for (auto k : idx) {
                   char c = marks[i][j][k];
                   if (!checkColors && c - '0' == highlightNum && HIGHLIGHT_SELECTED){
                       attron(COLOR_PAIR(3));
                   }
                   addch(c);
                   attroff(COLOR_PAIR(3));
               }
           }
           col += 4;
        }
        row += 2;
    }
    attroff(A_DIM);
}

void Window::printNumbs() {
    auto grid = game->getPlayGrid();
    auto start = game->getStartGrid();
    auto solution = game ->getSolution();

    int row = boardTop + 1;
    for (auto i = 0; i < 9; i++) {
        int col = boardLeft + 2;
        for (auto j = 0; j < 9; j++) {
            const char ch = grid[i][j] + '0';
            if (ch < '1') {
                col += 4;
                continue;
            }

            attron(A_BOLD);

            if (!game->isRemaining(ch - '0') && DIM_COMPLETED) {
                attron(COLOR_PAIR(4));
            }
            // Draw over potential pencilmarks
            mvprintw(row, col - 1, "   ");

            if (!checkColors && ch - '0' == highlightNum && HIGHLIGHT_SELECTED) {
                attron(COLOR_PAIR(3));
            }

            if (ch - '0' == start[i][j]) {
                attron(A_UNDERLINE);
            }
            else if (checkColors && grid[i][j] == solution[i][j]) {
                attron(COLOR_PAIR(2));
            }
            else if (checkColors && grid[i][j] != solution[i][j]) {
                attron(COLOR_PAIR(1));
            }
            mvaddch(row, col, ch);
            attroff(A_UNDERLINE);
            attroff(COLOR_PAIR(4));
            attroff(COLOR_PAIR(3));
            attroff(COLOR_PAIR(2));
            attroff(COLOR_PAIR(1));
            col += 4;
        }
        row += 2;
    }
    attroff(A_BOLD);
}

void Window::printInstructions() {
    if (windowCols - BoardCols < 24 || !PRINT_HELP) {
        return;
    }

    int row = boardTop + 3;
    int col = boardLeft + BoardCols + 5;

    mvaddch(row, col + 3, UP_KEY);
    mvaddch(row + 2, col, LEFT_KEY);
    mvaddch(row + 2, col + 6, RIGHT_KEY);
    mvaddch(row + 4, col + 3, DOWN_KEY);
    attron(A_UNDERLINE);
    mvaddch(row + 8, col, INSERT_KEY);
    attroff(A_UNDERLINE);
    if (INSERT_KEY == 'i' || INSERT_KEY == 'I') {
        printw("nsert");
    }
    else {
        printw(" insert");
    }
    attron(A_UNDERLINE);
    mvaddch(row + 9, col, PENCIL_KEY);
    attroff(A_UNDERLINE);
    if (PENCIL_KEY == 'p' || PENCIL_KEY == 'P') {
        printw("encil");
    }
    else {
        printw(" pencil");
    }
    attron(A_UNDERLINE);
    mvaddch(row + 10, col, GO_KEY);
    attroff(A_UNDERLINE);
    if (GO_KEY == 'g' || GO_KEY == 'G') {
        printw("o");
    }
    else {
        printw(" go");
    }
    attron(A_UNDERLINE);
    mvaddch(row+11, col, CHECK_KEY);
    attroff(A_UNDERLINE);
    if (CHECK_KEY == 'c' || CHECK_KEY == 'C') {
        printw("heck");
    }
    else {
        printw(" check");
    }
    attron(A_UNDERLINE);
    mvaddch(row + 12, col, QUIT_KEY);
    attroff(A_UNDERLINE);
    if (QUIT_KEY == 'q' || QUIT_KEY == 'Q') {
        printw("uit");
    }
    else {
        printw(" quit");
    }
}

void Window::printCoords() {
    if (windowCols - BoardCols < 4 || !PRINT_COORDS) {
        return;
    }

    int col = boardLeft + 2;

    char colCoord;
    char rowCoord;
    if (ALPHABETICAL_COL) {
        colCoord = 'a';
    }
    else {
        colCoord = '1';
    }
    if (ALPHABETICAL_ROW) {
        rowCoord = 'a';
    }
    else {
        rowCoord = '1';
    }
    for (auto i = '1'; i <= '9'; i++) {
        mvaddch(boardTop - 1, col, colCoord++);
        col += 4;
    }

    col = boardLeft - 2;

    int row = boardTop + 1;
    for (auto i = '1'; i <= '9'; i++) {
        mvaddch(row, col, rowCoord++);
        row += 2;
    }
}

void Window::printMode() {
    if (windowRows <= BoardRows || !PRINT_STATUS) {
        return;
    }
    move(boardTop + BoardRows, boardLeft);
    clrtoeol();
    printw("%s", mode.c_str());
}

void Window::printCursor() {
    int row = cursorRow * 2 + boardTop + 1;
    int col = cursorCol * 4 + boardLeft + 2;
    move(row, col);
}

void Window::moveCursor(int row, int col) {
    cursorRow = row;
    cursorCol = col;
}

void Window::changeMode(std::string s) {
    mode = s;
}

void Window::check() {
    checkColors = !checkColors;
}

void Window::select(int val) {
    if (val == ' ') {
        int initValue = game->getStartGrid()[cursorRow][cursorCol];
        if (initValue == 0) {
            highlightNum = 0;
            return;
        }
        highlightNum = initValue;
    }
    else {
        highlightNum = val - '0';
    }
}