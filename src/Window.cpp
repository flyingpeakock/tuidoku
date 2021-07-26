#include "Window.h"
#include <locale.h>
#include <string.h>
#include <sstream>
#include <map>
#include "config.h"

BasicWindow::BasicWindow(SimpleBoard *g, WINDOW *w) {
    window = w;
    game = g;

    setlocale(LC_ALL, "");
    initscr();
    init();
}

BasicWindow::BasicWindow(SimpleBoard *g) {
    int maxY, maxX;
    setlocale(LC_ALL, "");
    initscr();

    getmaxyx(stdscr, maxY, maxX);
    window = newwin(maxY, maxX, 0, 0);
    refresh();

    game = g;
    cursorRow = 0;
    cursorCol = 0;

    init();
}

BasicWindow::~BasicWindow() {
    delwin(window);
}

void BasicWindow::init() {
    BoardRows = 19;
    BoardCols = 37;
    cbreak(); // Get input before enter is pressed
    noecho(); // Don't show keypresses
    keypad(window, true); // Use arrow keys to move

    if (has_colors()) {
        use_default_colors();
        start_color();
        init_pair(1, ERROR_COLOR, BACKGROUND_COLOR);
        init_pair(2, CORRECT_COLOR, BACKGROUND_COLOR);
        init_pair(3, HIGHLIGHT_COLOR, BACKGROUND_COLOR);
        init_pair(4, LOWLIGHT_COLOR, BACKGROUND_COLOR);
        init_pair(5, BOARD_COLOR, BACKGROUND_COLOR);
        init_pair(6, GIVEN_COLOR, BACKGROUND_COLOR);
        init_pair(7, PLACED_COLOR, BACKGROUND_COLOR);
        init_pair(10, FOREGROUND_COLOR, BACKGROUND_COLOR);
    }
    printBoxes();
    printCoords();
    printBoard();
}

Board *BasicWindow::getBoard() {
    return (Board *) game;
}

void BasicWindow::clear() {
    wmove(window, 0,0);
    wattron(window, COLOR_PAIR(10));
    for (auto i = 0; i < windowRows; i++) {
        for (auto j = 0; j < windowCols; j++) {
            waddch(window, ' ');
        }
    }
    wattroff(window, COLOR_PAIR(10));
}

void BasicWindow::printBoxes() {
    int startHeight = boardTop;
    wattron(window, A_BOLD);
    wattron(window, COLOR_PAIR(5));
    mvwprintw(window, startHeight, boardLeft, TOPROW);
    for (auto i = 0; i < 3; i++) {
        mvwprintw(window, ++startHeight, boardLeft, ROW1);
        mvwprintw(window, ++startHeight, boardLeft, ROW2);
        mvwprintw(window, ++startHeight, boardLeft, ROW1);
        mvwprintw(window, ++startHeight, boardLeft, ROW2);
        mvwprintw(window, ++startHeight, boardLeft, ROW1);
        if (i != 2)
            mvwprintw(window, ++startHeight, boardLeft, ROW3);
    }
    mvwprintw(window, ++startHeight, boardLeft, BOTROW);
    wattroff(window, A_BOLD);
    wattroff(window, COLOR_PAIR(5));
}

void BasicWindow::printNumbs() {
    auto grid = game->getPlayGrid();
    int row = boardTop + 1;
    wattron(window, A_BOLD);

    for (auto i = 0; i < 9; i++) {
        int col = boardLeft + 2;
        for (auto j = 0; j < 9; j++) {
            char ch = grid[i][j] + START_CHAR - 1;
            if (ch < START_CHAR) {
                ch = ' ';
            }

            // Draw over potential pencilmarks
            wattron(window, COLOR_PAIR(10));
            mvwprintw(window, row, col - 1, "   ");
            wattroff(window, COLOR_PAIR(10));

            int color = getColor(ch, i, j);
            wattron(window, color);
            mvwaddch(window, row, col, ch);
            wattroff(window, color);
            col += 4;
        }

        row += 2;
    }

    wattroff(window, A_BOLD);
    wattroff(window, COLOR_PAIR(7));
}

int BasicWindow::getColor(char i, int j, int k) {
    return COLOR_PAIR(7);
}

void BasicWindow::printCoords() {
    if (windowCols - BoardCols < 4 || !PRINT_COORDS) {
        return;
    }

    int col = boardLeft + 2;

    char colCoord = COL_CHAR;
    char rowCoord = ROW_CHAR;

    wattron(window, COLOR_PAIR(10));

    for (auto i = '1'; i <= '9'; i++) {
        mvwaddch(window, boardTop - 1, col, colCoord++);
        col += 4;
    }

    col = boardLeft - 2;

    int row = boardTop + 1;
    for (auto i = '1'; i <= '9'; i++) {
        mvwaddch(window, row, col, rowCoord++);
        row += 2;
    }

    wattroff(window, COLOR_PAIR(10));
}

void BasicWindow::printCursor() {
    int row = cursorRow * 2 + boardTop + 1;
    int col = cursorCol * 4 + boardLeft + 2;
    wmove(window, row, col);
}

bool BasicWindow::resize() {
    int oldRows = windowRows;
    int oldCols = windowCols;
    getmaxyx(window, windowRows, windowCols);

    boardTop = (windowRows - BoardRows) / 2;
    boardLeft = (windowCols - BoardCols) / 2;
    int gridTop = boardTop + 1;
    int gridLeft = boardLeft + 2;
    clear();
    if (windowRows < BoardRows || windowCols < BoardCols) {
        char error[] = "Not enough space to draw board";
        wattron(window, COLOR_PAIR(10));
        mvwprintw(window, windowRows / 2, (windowCols - strlen(error)) / 2, "%s", error);
        wattroff(window, COLOR_PAIR(10));
        wrefresh(window);
        return false;
    }
    if (windowRows - BoardRows > 3 && PRINT_TITLE) {
        wattron(window, A_BOLD | A_UNDERLINE);
        wattron(window, COLOR_PAIR(10));
        mvwprintw(window, boardTop - 3, (windowCols - strlen(TITLE)) / 2, "%s", TITLE);
        wattroff(window, COLOR_PAIR(10));
        wattroff(window, A_BOLD | A_UNDERLINE);
    }
        printBoxes();
        printCoords();
        return true;
}

void BasicWindow::printBoard() {
    if (!resize())
        return;
    printNumbs();
    printCursor();
    wrefresh(window);
}

void BasicWindow::moveCursor(int row, int col) {
    cursorRow = row;
    cursorCol = col;
}

SelectionWindow::SelectionWindow(std::vector<SimpleBoard> &g) : BasicWindow(&g[0]) {
    boardVector = g;
    printInstructions();
}

SelectionWindow::SelectionWindow(std::vector<SimpleBoard> &g, WINDOW *w) : BasicWindow(&g[0], w) {
    boardVector = g;
    printInstructions();
}

void SelectionWindow::printBoard() {
    if (!resize())
        return;
    printNumbs();
    printInstructions();
    printCursor();
    wrefresh(window);
}

void SelectionWindow::printInstructions() {
    if (windowCols - BoardCols < 36 || !PRINT_HELP) {
        return;
    }
    int row = boardTop + 5;
    int col = boardLeft + BoardCols + 5;

    wattron(window, COLOR_PAIR(10));
    mvwaddch(window, row, col, LEFT_KEY);
    mvwaddch(window, row, col + 6, RIGHT_KEY);

    wattron(window, A_UNDERLINE);
    mvwaddch(window, row+2, col, GO_KEY);
    wattroff(window, A_UNDERLINE);
    wprintw(window, " Select");
    wattroff(window, COLOR_PAIR(10));
}

void SelectionWindow::changeBoard(int idx) {
    int max = boardVector.size() - 1;
    if (idx > max) {
        idx = max;
    }
    else if (idx < 0) {
        idx = 0;
    }
    game = &boardVector[idx];
}

SolveWindow::SolveWindow(Board *g) : BasicWindow(g) {
    game = g;
    printInstructions();
}

SolveWindow::SolveWindow(Board *g, WINDOW * w) : BasicWindow(g, w) {
    game = g;
    printInstructions();
}

void SolveWindow::printInstructions() {
    if (windowCols - BoardCols < 36 || !PRINT_HELP) {
        return;
    }
    int row = boardTop + 3;
    int col = boardLeft + BoardCols + 5;

    wattron(window, COLOR_PAIR(10));
    mvwaddch(window, row, col + 3, UP_KEY);
    mvwaddch(window, row + 2, col, LEFT_KEY);
    mvwaddch(window, row + 2, col + 6, RIGHT_KEY);
    mvwaddch(window, row + 4, col + 3, DOWN_KEY);
    row += 8;
    col -= 2;
    mvwprintw(window, row, col, "Insert digits");
    mvwprintw(window, ++row, col, "until the");
    mvwprintw(window, ++row, col, "solution is");
    mvwprintw(window, ++row, col, "unique.");
}

Window::Window(Board *g): BasicWindow(g){
    game = g;
    mode = "Insert mode";
    checkColors = false;
    highlightNum = 0;
}

Window::Window(Board *g, WINDOW *w) : BasicWindow(g, w) {
    game = g;
    mode = "Insert mode";
    checkColors = false;
    highlightNum = 0;
}

void Window::printBoard() {
    if (!resize())
        return;
    printInstructions();
    printNumbs();
    printPencil();
    printMode();
    printCursor();
    wrefresh(window);
}


void Window::printPencil() {
    auto marks = game->getPencilMarks();
    auto grid = game->getPlayGrid();
    int row = boardTop + 1;
    for (auto i = 0; i < 9; i++) {
        int col = boardLeft + 1;
        for (auto j = 0; j < 9; j++) {
           if (grid[i][j] < 1) {
               int idx[] = {0, 2, 1};
               wmove(window, row, col);
               for (auto k : idx) {
                   char c = marks[i][j][k];
                   if (!checkColors && c - '0' == highlightNum && HIGHLIGHT_SELECTED){
                       wattron(window, COLOR_PAIR(3));
                   }
                   else {
                       wattron(window, COLOR_PAIR(10) | A_DIM);
                   }
                   waddch(window, c);
                   wattroff(window, COLOR_PAIR(3));
                   wattroff(window, COLOR_PAIR(10) | A_DIM);
               }
           }
           col += 4;
        }
        row += 2;
    }
}


void Window::printInstructions() {
    if (windowCols - BoardCols < 24 || !PRINT_HELP) {
        return;
    }

    int row = boardTop + 3;
    int col = boardLeft + BoardCols + 5;

    wattron(window, COLOR_PAIR(10));
    mvwaddch(window, row, col + 3, UP_KEY);
    mvwaddch(window, row + 2, col, LEFT_KEY);
    mvwaddch(window, row + 2, col + 6, RIGHT_KEY);
    mvwaddch(window, row + 4, col + 3, DOWN_KEY);
    wattron(window, A_UNDERLINE);
    mvwaddch(window, row + 8, col, INSERT_KEY);
    wattroff(window, A_UNDERLINE);
    if (INSERT_KEY == 'i' || INSERT_KEY == 'I') {
        wprintw(window, "nsert");
    }
    else {
        wprintw(window, " insert");
    }
    wattron(window, A_UNDERLINE);
    mvwaddch(window, row + 9, col, PENCIL_KEY);
    wattroff(window, A_UNDERLINE);
    if (PENCIL_KEY == 'p' || PENCIL_KEY == 'P') {
        wprintw(window, "encil");
    }
    else {
        wprintw(window, " pencil");
    }
    wattron(window, A_UNDERLINE);
    mvwaddch(window, row + 10, col, GO_KEY);
    wattroff(window, A_UNDERLINE);
    if (GO_KEY == 'g' || GO_KEY == 'G') {
        wprintw(window, "o");
    }
    else {
        wprintw(window, " go");
    }
    wattron(window, A_UNDERLINE);
    mvwaddch(window, row+11, col, CHECK_KEY);
    wattroff(window, A_UNDERLINE);
    if (CHECK_KEY == 'c' || CHECK_KEY == 'C') {
        wprintw(window, "heck");
    }
    else {
        wprintw(window, " check");
    }
    wattron(window, A_UNDERLINE);
    mvwaddch(window, row+12, col, AUTO_PENCIL_KEY);
    wattroff(window, A_UNDERLINE);
    if (AUTO_PENCIL_KEY == 'a' || AUTO_PENCIL_KEY == 'A') {
        wprintw(window, "uto pencil");
    }
    else {
        wprintw(window, " auto pencil");
    }
    wattron(window, A_UNDERLINE);
    mvwaddch(window, row + 13, col, QUIT_KEY);
    wattroff(window, A_UNDERLINE);
    if (QUIT_KEY == 'q' || QUIT_KEY == 'Q') {
        wprintw(window, "uit");
    }
    else {
        wprintw(window, " quit");
    }
    wattroff(window, COLOR_PAIR(0));
}


void Window::printMode() {
    if (windowRows <= BoardRows || !PRINT_STATUS) {
        return;
    }
    wattron(window, COLOR_PAIR(10));
    wmove(window, boardTop + BoardRows, boardLeft);

    int length;
    std::string longMode = mode;
    length = windowCols - boardLeft;
    while (longMode.length() < length) {
        longMode += ' ';
    }
    wclrtoeol(window);
    wprintw(window, "%s", longMode.c_str());
    wattroff(window, COLOR_PAIR(10));
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

    if (!game->isRemaining(c) && DIM_COMPLETED && !checkColors) {
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

BigWindow::BigWindow(Board *g) : Window(g) {
    BoardRows = 37;
    BoardCols = 73;
}

BigWindow::BigWindow(Board *g, WINDOW *w) : Window(g, w) {
    BoardRows = 37;
    BoardCols = 73;
}

void BigWindow::printBoard() {
    if (!resize())
        return;
    printInstructions();
    printNumbs();
    printPencil();
    printMode();
    printCursor();
    wrefresh(window);
}

void BigWindow::printBoxes() {
    int startHeight = boardTop;
    wattron(window, A_BOLD);
    wattron(window, COLOR_PAIR(5));
    mvwprintw(window, startHeight, boardLeft, BIGTOP);
    for (auto i = 0; i < 3; i++) {
        for (auto j = 0; j < 3; j++)
            mvwprintw(window, ++startHeight, boardLeft, BIGROW1);
        mvwprintw(window, ++startHeight, boardLeft, BIGROW2);
        for (auto j = 0; j < 3; j++)
            mvwprintw(window, ++startHeight, boardLeft, BIGROW1);
        mvwprintw(window, ++startHeight, boardLeft, BIGROW2);
        for (auto j = 0; j < 3; j++)
            mvwprintw(window, ++startHeight, boardLeft, BIGROW1);
        if (i != 2)
            mvwprintw(window, ++startHeight, boardLeft, BIGROW3);
    }
    mvwprintw(window, ++startHeight, boardLeft, BIGBOTTOM);
    wattroff(window, A_BOLD);
    wattroff(window, COLOR_PAIR(5));
}

void BigWindow::clearPencil(int row, int col) {
    // row and col is where the main number is in the cell
    wattron(window, COLOR_PAIR(10));
    mvwprintw(window, row - 1, col - 2, "     ");
    mvwprintw(window, row, col - 2, "     ");
    mvwprintw(window, row + 1, col - 2, "     ");
    wattroff(window, COLOR_PAIR(10));
}

void BigWindow::printNumbs() {
    auto grid = game->getPlayGrid();
    int row = boardTop + 2;
    wattron(window, A_BOLD);

    for (auto i = 0; i < 9; i++) {
        int col = boardLeft + 4;
        for (auto j = 0; j < 9; j++) {
            char ch = grid[i][j] + START_CHAR - 1;
            if (ch < START_CHAR) {
                ch = ' ';
            }
            clearPencil(row, col);
            int color = getColor(ch, i, j);
            wattron(window, color);
            mvwaddch(window, row, col, ch);
            wattroff(window, color);
            col += 8;
        }
        row += 4;
    }

    wattroff(window, A_BOLD);
    wattroff(window, COLOR_PAIR(7));
}

void BigWindow::printPencil(char c, int row, int col, std::map<char, bool> marks) {
    if (!marks[c])
        return;
    if (c <= '3') {
        row--;
    }
    else if (c >= '7') {
        row++;
    }
    char c_normalized = c - '1';
    if (c_normalized % 3 == 0) {
        col -= 2;
    }
    else if (c_normalized % 3 == 2) {
        col += 2;
    }
    if (!checkColors && c - '0' == highlightNum && HIGHLIGHT_SELECTED) {
        wattron(window, COLOR_PAIR(3));
    }
    else {
        wattron(window, COLOR_PAIR(10) | A_DIM);
    }
    mvwaddch(window, row, col, c);
    wattroff(window, COLOR_PAIR(3));
    wattroff(window, COLOR_PAIR(10) | A_DIM);
}

void BigWindow::printPencil() {
    auto marks = game->getPencilMarks();
    auto grid = game->getPlayGrid();

    //  row and col will be relative to the middle of the board
    int row = boardTop + 2;
    for (auto i = 0; i < 9; i++) {
        int col = boardLeft + 4;
        for (auto j = 0; j < 9; j++) {
            if (grid[i][j] < 1) {
                std::map<char, bool> markmap;
                for (auto &m : marks[i][j]) {
                    if (m != ' ')
                        markmap[m] = true;
                }
                for (char c = '1'; c <= '9'; c++) {
                    printPencil(c, row, col, markmap);
                }
            }
            col += 8;
        }
        row += 4;
    }
}

void BigWindow::printCursor() {
    int row = cursorRow * 4 + boardTop + 2;
    int col = cursorCol * 8 + boardLeft + 4;
    wmove(window, row, col);
}

void BigWindow::printCoords() {
    if (windowCols - BoardCols < 4 || !PRINT_COORDS) {
        return;
    }

    int col = boardLeft + 4;

    char colCoord = COL_CHAR;
    char rowCoord = ROW_CHAR;

    wattron(window, COLOR_PAIR(10));
    for (auto i = 0; i < 9; i++) {
        mvwaddch(window, boardTop - 1, col, colCoord++);
        col += 8;
    }

    col = boardLeft - 2;

    int row = boardTop + 2;
    for (auto i = 0; i < 9; i++) {
        mvwaddch(window, row, col, rowCoord++);
        row += 4;
    }
    wattroff(window, COLOR_PAIR(10));
}