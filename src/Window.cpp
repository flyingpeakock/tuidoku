#include "Window.h"
#include <locale.h>
#include <string.h>
#include "config.h"

BasicWindow::BasicWindow(Board *g) {
    game = g;
    cursorRow = 0;
    cursorCol = 0;
    setlocale(LC_ALL, "");

    initscr();

    cbreak(); // Get input before enter is pressed
    noecho(); // Don't show keypresses
    keypad(stdscr, true); // Use arrow keys to move

    if (has_colors()) {
        use_default_colors();
        start_color();
        init_pair(5, BOARD_COLOR, BACKGROUND_COLOR);
        init_pair(7, PLACED_COLOR, BACKGROUND_COLOR);
        init_pair(10, FOREGROUND_COLOR, BACKGROUND_COLOR);
    }
    printInstructions();
    printBoard();
    printCoords();
    printBoard();
}

BasicWindow::~BasicWindow() {
    endwin();
}

Board *BasicWindow::getBoard() {
    return game;
}

void BasicWindow::printInstructions() {
    return;
}

void BasicWindow::clear() {
    move(0,0);
    attron(COLOR_PAIR(10));
    for (auto i = 0; i < windowRows; i++) {
        for (auto j = 0; j < windowCols; j++) {
            addch(' ');
        }
    }
    attroff(COLOR_PAIR(10));
}

void BasicWindow::printBoxes() {
    int startHeight = boardTop;
    attron(A_BOLD);
    attron(COLOR_PAIR(5));
    mvprintw(startHeight, boardLeft, TOPROW);
    for (auto i = 0; i < 3; i++) {
        mvprintw(++startHeight, boardLeft, ROW1);
        mvprintw(++startHeight, boardLeft, ROW2);
        mvprintw(++startHeight, boardLeft, ROW1);
        mvprintw(++startHeight, boardLeft, ROW2);
        mvprintw(++startHeight, boardLeft, ROW1);
        if (i != 2)
            mvprintw(++startHeight, boardLeft, ROW3);
    }
    mvprintw(++startHeight, boardLeft, BOTROW);
    attroff(A_BOLD);
    attroff(COLOR_PAIR(5));
}

void BasicWindow::printNumbs() {
    auto grid = game->getPlayGrid();
    int row = boardTop + 1;
    attron(A_BOLD);
    attron(COLOR_PAIR(7));

    for (auto i = 0; i < 9; i++) {
        int col = boardLeft + 2;
        for (auto j = 0; j < 9; j++) {
            char ch = grid[i][j] + '0';
            if (ch == '0') {
                ch = ' ';
            }

            mvaddch(row, col, ch);
            col += 4;
        }
        row += 2;
    }

    attroff(A_BOLD);
    attroff(COLOR_PAIR(7));
}

void BasicWindow::printCoords() {
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

    attron(COLOR_PAIR(10));

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

    attroff(COLOR_PAIR(10));
}

void BasicWindow::printCursor() {
    int row = cursorRow * 2 + boardTop + 1;
    int col = cursorCol * 4 + boardLeft + 2;
    move(row, col);
}

void BasicWindow::printBoard() {
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
            attron(COLOR_PAIR(10));
            mvprintw(windowRows / 2, (windowCols - strlen(error)) / 2, "%s", error);
            attroff(COLOR_PAIR(10));
            refresh();
            return;
        }
        if (windowRows - BoardRows > 3 && PRINT_TITLE) {
            attron(A_BOLD | A_UNDERLINE);
            attron(COLOR_PAIR(10));
            mvprintw(boardTop - 3, (windowCols - strlen(TITLE)) / 2, "%s", TITLE);
            attroff(COLOR_PAIR(10));
            attroff(A_BOLD | A_UNDERLINE);
        }
        printBoxes();
        printInstructions();
        printCoords();
    }

    printNumbs();
    printCursor();
    refresh();
}

void BasicWindow::moveCursor(int row, int col) {
    cursorRow = row;
    cursorCol = col;
}

SolveWindow::SolveWindow(Board *g) : BasicWindow(g) {
    printInstructions();
}

void SolveWindow::printInstructions() {
    if (windowCols - BoardCols < 36 || !PRINT_HELP) {
        return;
    }
    int row = boardTop + 3;
    int col = boardLeft + BoardCols + 5;

    attron(COLOR_PAIR(10));
    mvaddch(row, col + 3, UP_KEY);
    mvaddch(row + 2, col, LEFT_KEY);
    mvaddch(row + 2, col + 6, RIGHT_KEY);
    mvaddch(row + 4, col + 3, DOWN_KEY);
    row += 8;
    col -= 2;
    mvprintw(row, col, "Insert digits");
    mvprintw(++row, col, "until the");
    mvprintw(++row, col, "solution is");
    mvprintw(++row, col, "unique.");
}

Window::Window(Board *g): BasicWindow(g){
    mode = "Insert mode";
    checkColors = false;
    highlightNum = 0;

    if (has_colors()) {
        init_pair(1, ERROR_COLOR, BACKGROUND_COLOR);
        init_pair(2, CORRECT_COLOR, BACKGROUND_COLOR);
        init_pair(3, HIGHLIGHT_COLOR, BACKGROUND_COLOR);
        init_pair(4, LOWLIGHT_COLOR, BACKGROUND_COLOR);
        init_pair(6, GIVEN_COLOR, BACKGROUND_COLOR);
    }
    printBoxes();
    printInstructions();
    printCoords();
    printBoard();
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
            attron(COLOR_PAIR(10));
            mvprintw(windowRows / 2, (windowCols - strlen(error)) / 2, "%s", error);
            attroff(COLOR_PAIR(10));
            refresh();
            return;
        }
        if (windowRows - BoardRows > 3 && PRINT_TITLE) {
            attron(A_BOLD | A_UNDERLINE);
            attron(COLOR_PAIR(10));
            mvprintw(boardTop - 3, (windowCols - strlen(TITLE)) / 2, "%s", TITLE);
            attroff(COLOR_PAIR(10));
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
                   else {
                       attron(COLOR_PAIR(10));
                   }
                   addch(c);
                   attroff(COLOR_PAIR(3));
                   attroff(COLOR_PAIR(10));
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
    attron(A_BOLD);
    for (auto i = 0; i < 9; i++) {
        int col = boardLeft + 2;
        for (auto j = 0; j < 9; j++) {
            const char ch = grid[i][j] + '0';
            if (ch < '1') {
                col += 4;
                continue;
            }

            // Draw over potential pencilmarks
            attron(COLOR_PAIR(10));
            mvprintw(row, col - 1, "   ");
            attroff(COLOR_PAIR(10));

            attron(getColor(ch, i, j));
            mvaddch(row, col, ch);
            attroff(getColor(ch, i, j));
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

    attron(COLOR_PAIR(10));
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
    attroff(COLOR_PAIR(0));
}


void Window::printMode() {
    if (windowRows <= BoardRows || !PRINT_STATUS) {
        return;
    }
    attron(COLOR_PAIR(10));
    move(boardTop + BoardRows, boardLeft);

    int length;
    std::string longMode = mode;
    length = windowCols - boardLeft;
    while (longMode.length() < length) {
        longMode += ' ';
    }
    clrtoeol();
    printw("%s", longMode.c_str());
    attroff(COLOR_PAIR(10));
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

int Window::getColor(char c, int row, int col) {
    c = c - '0';
    if (!c) {
        // Empty square, return default color pair
        return COLOR_PAIR(10);
    }

    auto start = game->getStartGrid();
    int ret = 0;
    if (c == start[row][col] && c) {
        // Given square, underline it
        ret = A_UNDERLINE;
    }

    if (!game->isRemaining(c) && DIM_COMPLETED) {
        // Dimming numbers that appear 9 times
        return COLOR_PAIR(4) | ret;
    }

    if (!checkColors && c == highlightNum && HIGHLIGHT_SELECTED) {
        // This number is currently selected
        return COLOR_PAIR(3) | ret;
    }

    if (c == start[row][col] && c) {
        // Given number that isn't selected or dimmed
        return COLOR_PAIR(6) | ret;
    }

    auto grid = game->getPlayGrid();
    auto solution = game->getSolution();
    if (checkColors && grid[row][col] == solution[row][col]) {
        // Check colors is on and this is correct
        return COLOR_PAIR(2) | ret;
    }
    if (checkColors && grid[row][col] != solution[row][col]) {
        // Check colors is on and this is incorrect
        return COLOR_PAIR(1) | ret;
    }

    // Only remaining is placed numbers that aren't highlighted or checked
    return COLOR_PAIR(7) | ret;
}
