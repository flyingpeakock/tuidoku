#include "Game.h"
#include "Stopwatch.h"
#include "config.h"

Controller::Controller(BasicWindow *win){
    window = win;
    board = window->getBoard();
    row = 0;
    col = 0;
}

int Controller::mainLoop() {
    board->startPlaying();
    while (board->isPlaying()) {
        window->printBoard();
        int ch = wgetch(stdscr);
        switch (ch) {
            case KEY_LEFT:
            case LEFT_KEY:
                left();
                break;
            case KEY_DOWN:
            case DOWN_KEY:
                down();
                break;
            case KEY_UP:
            case UP_KEY:
                up();
                break;
            case KEY_RIGHT:
            case RIGHT_KEY:
                right();
                break;
            case GO_KEY:
                go();
                break;
            case QUIT_KEY:
                board->stopPlaying();
                break;
            default:
                if ((ch >= START_CHAR - 1 && ch <= START_CHAR + 8) || ch == ERASE_KEY) {
                    board->insert(ch, row, col);
                }
        }
    }
    return 0;
}

void Controller::up() {
    if (row <= 0) {
        row = 9;
    }
    window->moveCursor(--row, col);
}

void Controller::down() {
    if (row >= 8) {
        row = -1;
    }
    window->moveCursor(++row, col);
}

void Controller::left() {
    if (col <= 0) {
        col = 9;
    }
    window->moveCursor(row, --col);
}

void Controller::right() {
    if (col >= 8) {
        col = -1;
    }
    window->moveCursor(row, ++col);
}

void Controller::go() {
    char r = 0;
    char c = 0;

    while (c < COL_CHAR || c > COL_CHAR + 8) {
        c = getch();
        if (c == 'q' || c == TOGGLE_KEY) {
            return;
        }
    }

    while (r < ROW_CHAR || r > ROW_CHAR + 8) {
        r = getch();
        if (r == 'q' || r == TOGGLE_KEY) {
            return;
        }
    }
    row = r - ROW_CHAR;
    col = c - COL_CHAR;
    window->moveCursor(row, col);
}

Selection::Selection(SelectionWindow *win) : Controller(win) {
    index = 0;
    window = win;
}

int Selection::mainLoop() {
    board->startPlaying();
    while(board->isPlaying()) {
        window->printBoard();
        int ch = wgetch(stdscr);
        switch (ch) {
            case KEY_LEFT:
            case LEFT_KEY:
                window->changeBoard(--index);
                break;
            case RIGHT_KEY:
            case KEY_RIGHT:
                window->changeBoard(++index);
                break;
            case GO_KEY:
            case QUIT_KEY:
                return index;
        }
    }
    return index;
}

InteractiveSolver::InteractiveSolver(SolveWindow *win): Controller(win) {
    window = win;
};

int InteractiveSolver::mainLoop() {
    board->startPlaying();
    while(board->isPlaying()) {
        window->printBoard();
        int ch = wgetch(stdscr);
        switch(ch) {
        case KEY_LEFT:
        case LEFT_KEY:
            left();
            break;
        case KEY_DOWN:
        case DOWN_KEY:
            down();
            break;
        case KEY_UP:
        case UP_KEY:
            up();
            break;
        case KEY_RIGHT:
        case RIGHT_KEY:
            right();
            break;
        case GO_KEY:
            go();
            break;
        case QUIT_KEY:
            board->stopPlaying();
            break;
        default:
            if ((ch >= START_CHAR - 1 && ch <= START_CHAR + 8) || ch == ERASE_KEY) {
                if (Sudoku::isSafe(board->getPlayGrid(), row, col, ch - '0')) {
                    board->insert(ch, row, col);
                    Sudoku::puzzle solveGrid = board->getPlayGrid();
                    bool isUnique = Sudoku::solve(solveGrid);
                    if (isUnique) {
                        board->swapStartGrid(board->getPlayGrid());
                        solve();
                        window->printBoard();
                        getch();
                        return 0;
                    }
                }
            }
        }
        board->isWon();
    }
    window->printBoard();
    getch();
    return 0;
}

void InteractiveSolver::solve() {
    auto solution = board->getPlayGrid();
    Sudoku::solve(solution);
    for (auto i = 0; i < solution.size(); i++) {
        for (auto j = 0; j < solution[i].size(); j++) {
            board->insert(solution[i][j] + '0', i, j);
        }
    }
}

Game::Game(Window *win, bool big): Controller(win) {
    window = win;
    mode = INSERT_KEY;
    isBig = big;
}

int Game::mainLoop() {
    board->startPlaying();
    Stopwatch timer;
    if (START_TIMER)
        timer.start();
    while (board->isPlaying()) {
        window->printBoard();
        wchar_t prevMode = mode;
        int ch = wgetch(stdscr);
        switch (ch) {
        case KEY_LEFT:
        case LEFT_KEY:
            left();
            break;
        case KEY_DOWN:
        case DOWN_KEY:
            down();
            break;
        case KEY_UP:
        case UP_KEY:
            up();
            break;
        case KEY_RIGHT:
        case RIGHT_KEY:
            right();
            break;
        case GO_KEY:
            changeMode(ch);
            window->printBoard();
            go();
            changeMode(prevMode);
            break;
        case INSERT_KEY:
        case PENCIL_KEY:
            changeMode(ch);
            break;
        case QUIT_KEY:
            board->stopPlaying();
            break;
        case CHECK_KEY:
            window->check();
            break;
        case AUTO_PENCIL_KEY:
            board->autoPencil();
            break;
        case TOGGLE_KEY:
            mode == INSERT_KEY ? changeMode(PENCIL_KEY) : changeMode(INSERT_KEY);
            break;
        case HINT_KEY:
            hints_since_move++;
            getHint();
            break;
        default:
            if ((ch >= START_CHAR - 1 && ch <= START_CHAR + 8) || ch == ERASE_KEY) {
                hints_since_move = 0;
                window->select(ch);
                if (mode == INSERT_KEY) {
                    board->insert(ch, row, col);
                }
                else if (mode == PENCIL_KEY) {
                    board->pencil(ch, row, col);
                }
            }
        }

        board->isWon();
    }
    if (START_TIMER)
        timer.stop();
    if (!board->isWon()) {
        return START_TIMER ? timer.totalSeconds() : 0;
    }

    if (START_TIMER) {
        window->changeMode(timer.timeTaken());
    }
    window->printBoard();

    getch();
    return START_TIMER ? timer.totalSeconds() : 0;
}

void Game::changeMode(char c) {
    std::string s;
    switch (c) {
    case INSERT_KEY:
        s = "Input mode";
        mode = c;
        break;
    case PENCIL_KEY:
        s = "Pencil mode";
        mode = c;
        break;
    case GO_KEY:
        s = "Go";
        mode = c;
        break;
    }
    Window *win = (Window *) window;
    win->changeMode(s);
}

void Game::getHint() {
    static Hint hint;
    if (hints_since_move == 1) {
        hint = solveHuman(*board);
        window->changeMode(hint.hint1);
    }
    else if (hints_since_move == 2) {
        window->changeMode(hint.hint2);
    }
    else if (hints_since_move > 2) {
        for (auto &move : hint.moves) {
            move(board);
        }
        hints_since_move = 0;
    }
}
