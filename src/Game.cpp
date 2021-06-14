#include "Game.h"
#include "Stopwatch.h"
#include "config.h"

Controller::Controller(BasicWindow *win){
    window = win;
    board = window->getBoard();
    row = 0;
    col = 0;
}

void Controller::mainLoop() {
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

InteractiveSolver::InteractiveSolver(SolveWindow *win): Controller(win) {
    window = win;
};

void InteractiveSolver::mainLoop() {
    board->startPlaying();
    while(board->isPlaying()) {
        solver.changeGrid(board->getPlayGrid());
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
                if (solver.isSafe(row, col, ch - '0')) {
                    board->insert(ch, row, col);
                    solver.changeGrid(board->getPlayGrid());
                    solver.solve();
                    if (solver.isUnique()) {
                        board->swapStartGrid(solver.getGrid());
                        solve();
                        window->printBoard();
                        getch();
                        return;
                    }
                }
            }
        }
        board->isWon();
    }
    window->printBoard();
    getch();
}

void InteractiveSolver::solve() {
    auto solution = solver.getGrid();
    for (auto i = 0; i < solution.size(); i++) {
        for (auto j = 0; j < solution[i].size(); j++) {
            board->insert(solution[i][j] + '0', i, j);
        }
    }
}

Game::Game(Window *win): Controller(win) {
    window = win;
    mode = INSERT_KEY;
}

void Game::mainLoop() {
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
        case TOGGLE_KEY:
            mode == INSERT_KEY ? changeMode(PENCIL_KEY) : changeMode(INSERT_KEY);
            break;
        default:
            if ((ch >= START_CHAR - 1 && ch <= START_CHAR + 8) || ch == ERASE_KEY) {
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
        return;
    }

    if (START_TIMER) {
        window->changeMode(timer.timeTaken());
    }
    window->printBoard();

    getch();
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