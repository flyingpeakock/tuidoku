#include "Sudoku/Sudoku.h"
#include "HumanSolve.h"
#include "Tui/Tui.h"
#include "Play.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include <random>

void startCurses();
void endCurses();
WINDOW * createWindow();

WINDOW *initialize_tui();
Tui::main_menu_choices mainMenu(WINDOW *parentWin);
void play(WINDOW *mainWindow);
void solve(WINDOW *mainWindow);
void generate(WINDOW *mainWindow);

int main(int argc, char *argv[]) {
    SCREEN *screen = Tui::init_curses();
    WINDOW *main_window = initialize_tui();
    if (!main_window) {
        std::cout << "This terminal does not support color." << std::endl;
        return 1;
    }
    Tui::printOutline(main_window);
    wrefresh(main_window);
    while (true) {
        Tui::addMessage(main_window, Tui::title);
        Tui::main_menu_choices choice = mainMenu(main_window);
        switch (choice) {
            case Tui::PLAY:
                play(main_window);
                break;
            case Tui::SOLVE:
                solve(main_window);
                break;
            case Tui::GENERATE:
                generate(main_window);
                break;
            case Tui::EXIT:
            Tui::end_curses(screen, main_window);
                return 0;
            default:
                break;
        }
    }
    Tui::end_curses(screen, main_window);
    return 0;
}

WINDOW *initialize_tui() {
    WINDOW * ret = Tui::createMainWindow();
    if (!ret) {
        return ret;
    }
    Tui::printOutline(ret);
    return ret;
}

Tui::main_menu_choices mainMenu(WINDOW *parentWin) {
    std::vector<Tui::MenuItem<Tui::main_menu_choices>> mainMenu = {
        {"Play", '1', Tui::PLAY},
        {"Solve", '2', Tui::SOLVE},
        {"Generate", '3', Tui::GENERATE},
        {"Exit", '9', Tui::EXIT},
    };

    WINDOW *menuWin = Tui::printMenu<Tui::main_menu_choices>(parentWin, mainMenu, Tui::title);
    Tui::main_menu_choices choice = Tui::handleMenu<Tui::main_menu_choices>(menuWin, mainMenu);
    delwin(menuWin);
    return choice;
}

void play(WINDOW *mainWindow) {
    // Will be based on number of unknowns for now
    std::vector<Tui::MenuItem<Sudoku::difficulty>> playMenu = {
        {"Any", '1', Sudoku::ANY},
        {"Beginner", '2', Sudoku::BEGINNER},
        {"Easy", '3', Sudoku::EASY},
        {"Normal", '4', Sudoku::MEDIUM},
        {"Difficult", '5', Sudoku::HARD},
        {"Expert", '6', Sudoku::EXPERT},
    };

    WINDOW *menuWin = Tui::printMenu<Sudoku::difficulty>(mainWindow, playMenu, "Select difficulty");
    Sudoku::difficulty choice = Tui::handleMenu<Sudoku::difficulty>(menuWin, playMenu);
    delwin(menuWin);
    Tui::printOutline(mainWindow);
    wrefresh(mainWindow);

    Sudoku::puzzle grid;
    if (choice == Sudoku::ANY) {
        grid = Sudoku::generate();
    }
    else {
        int min, max;
        switch (choice) {
            case Sudoku::BEGINNER:
                min = 30;
                max = 40;
                break;
            case Sudoku::EASY:
                min = 40;
                max = 45;
                break;
            case Sudoku::MEDIUM:
                min = 45;
                max = 50;
                break;
            case Sudoku::HARD:
                min = 50;
                max = 56;
                break;
            case Sudoku::EXPERT:
                min = 56;
                max = 60;
                break;
        }
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distrib(min, max);
        grid = Sudoku::generate(distrib(gen));
    }
    try {
        Play game(grid, mainWindow);
        game.play();
    }
    catch (const std::invalid_argument& e) {
        return;
    }
}

void solve(WINDOW *window) {
    Tui::addMessage(window, "Insert numbers until the puzzle is unique");
    Sudoku::puzzle grid = {};
    Sudoku::puzzle solved;
    int row = 0;
    int col = 0;
    bool isUnique = false;
    while (!isUnique){
        Tui::printPuzzle(window, grid, A_NORMAL);
        Tui::highlightCell(window, row, col);
        int c = wgetch(window);
        switch (c) {
            case KEY_UP:
            case 'k':
                row--;
                break;
            case KEY_DOWN:
            case 'j':
                row++;
                break;
            case KEY_LEFT:
            case 'h':
                col--;
                break;
            case KEY_RIGHT:
            case 'l':
                col++;
                break;
            case KEY_BACKSPACE:
            case ' ':
                grid[row][col] = 0;
            case 'q':
            case 'Q':
            case 27:
                return;
            default:
                if (c >= '0' && c <= '9') {
                    grid[row][col] = c - '0';
                }
                break;
        }
        if (col < 0) {
            col = Sudoku::SIZE - 1;
        }
        if (col >= Sudoku::SIZE) {
            col = 0;
        }
        if (row < 0) {
            row = Sudoku::SIZE - 1;
        }
        if (row >= Sudoku::SIZE) {
            row = 0;
        }
        solved = grid;
        isUnique = Sudoku::solve(solved);
    }
    Tui::highlightCell(window, row, col);
    Tui::printPuzzle(window, solved, A_NORMAL);
    Tui::addMessage(window, "Solved!");
    getch();
}

void generate(WINDOW *mainWindow) {
    int numb_of_puzzles = Tui::userInputBox(mainWindow, "No. of puzzles:");
    Tui::printOutline(mainWindow);
    Tui::addMessage(mainWindow, "Printing puzzles to stdout");
    Sudoku::puzzle grid = {};
    for (auto i = 0; i < numb_of_puzzles; i++) {
        grid = Sudoku::generate();
        Tui::printPuzzle(mainWindow, grid, A_UNDERLINE);
        std::stringstream gridStrstream;
        for (auto &r : grid) {
            for (auto &n : r) {
                gridStrstream << n;
            }
        }
        def_prog_mode();
        endwin();
        std::cout << gridStrstream.str() << '\n';
        reset_prog_mode();
        wrefresh(mainWindow);
        Tui::printOutline(mainWindow);
    }
    Tui::printPuzzle(mainWindow, grid, A_UNDERLINE);
}
