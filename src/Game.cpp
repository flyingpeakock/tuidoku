#include "Game.h"
#include "Stopwatch.h"
#include <ctime>
#include <sstream>
#include "config.h"

Game::Game(Board b): board(b), window(&board) {
    mode = INSERT_KEY;
    row = 0;
    col = 0;
}

void Game::mainLoop() {
    board.startPlaying();
    Stopwatch timer;
    if (START_TIMER)
        timer.start();
    while (board.isPlaying()) {
        window.printBoard();
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
            window.printBoard();
            go();
            changeMode(prevMode);
            break;
        case INSERT_KEY:
        case PENCIL_KEY:
            changeMode(ch);
            break;
        case QUIT_KEY:
            board.stopPlaying();
            break;
        case CHECK_KEY:
            window.check();
            break;
        case TOGGLE_KEY:
            if (mode == INSERT_KEY) {
                changeMode(PENCIL_KEY);
            }
            else {
                changeMode(INSERT_KEY);
            }
            break;
        default:
            if ((ch > '0' && ch <= '9') || ch == ' ') {
                window.select(ch);
                if (mode == INSERT_KEY) {
                    insert(ch);
                }
                else if (mode == PENCIL_KEY) {
                    pencil(ch);
                }
            }
        }

        board.isWon();
    }
    if (START_TIMER)
        timer.stop();
    if (!board.isWon()) {
        return;
    }

    if (START_TIMER)
        window.changeMode(timer.timeTaken());
    window.printBoard();

    getch();
}

void Game::insert(char val) {
    board.insert(val, row, col);
}

void Game::pencil(char val) {
    board.pencil(val, row, col);
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
    window.changeMode(s);
}


void Game::up() {
    if (row <= 0) {
        row = 9;
    }
    window.moveCursor(--row, col);
}

void Game::down() {
    if (row >= 8) {
        row = -1;
    }
    window.moveCursor(++row, col);
}

void Game::left() {
    if (col <= 0) {
        col = 9;
    }
    window.moveCursor(row, --col);
}

void Game::right() {
    if (col >= 8) {
        col = -1;
    }
    window.moveCursor(row, ++col);
}

void Game::go() {
    char r = 0;
    char c = 0;

    const char rChar = ALPHABETICAL_ROW ? 'a' : '1';
    const char cChar = ALPHABETICAL_COL ? 'a' : '1';
    while (c < cChar || c > cChar + 8) {
        c = getch();
        if (c == 'q' || c == TOGGLE_KEY) {
            return;
        }
    }

    while (r < rChar || r > rChar + 8) {
        r = getch();
        if (r == 'q' || r == TOGGLE_KEY) {
            return;
        }
    }
    row = r - rChar;
    col = c - cChar;
    window.moveCursor(row, col);
}