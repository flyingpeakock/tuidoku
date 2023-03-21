#include "Sudoku/Sudoku.h"
#include "HumanSolve.h"
#include "Tui/Tui.h"
#include "Play.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include "Config.h"

/*global declaration of certain keys used in tui functions*/
int Tui::up_key;
int Tui::down_key;
int Tui::select_key;


WINDOW *initialize_tui();
Tui::main_menu_choices mainMenu(WINDOW *parentWin);
void play(WINDOW *mainWindow);
void solve(WINDOW *mainWindow);
void generate(WINDOW *mainWindow);

int main(int argc, char *argv[]) {
    Conf::init();
    Tui::up_key =Conf::keyBind("up");
    Tui::down_key = Conf::keyBind("down");
    Tui::select_key = '\n';
    
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
    grid = Sudoku::generate(choice);
    try {
        std::vector<Play::keymap> keys = {
            {"up", Conf::keyBind("up")},
            {"down", Conf::keyBind("down")},
            {"left", Conf::keyBind("left")},
            {"right", Conf::keyBind("right")},
            {"pencil", Conf::keyBind("pencil")},
            {"insert", Conf::keyBind("insert")},
            {"erase", Conf::keyBind("erase")},
            {"fillPencils", Conf::keyBind("fillPencils")},
            {"exit", Conf::keyBind("exit")},
            {"hint", Conf::keyBind("hint")},
        };
        Play game(keys, grid, mainWindow);
        game.play();
    }
    catch (const std::invalid_argument& e) {
        return;
    }
}

void solve(WINDOW *window) {
    const int up_key = Conf::keyBind("up");
    const int down_key = Conf::keyBind("down");
    const int left_key = Conf::keyBind("left");
    const int right_key = Conf::keyBind("right");
    const int erase_key = Conf::keyBind("erase");
    const int exit_key = Conf::keyBind("exit");

    Tui::addMessage(window, "Insert numbers until the puzzle is unique");
    wrefresh(window);
    Sudoku::puzzle grid = {};
    Sudoku::puzzle solved;
    int row = 0;
    int col = 0;
    bool isUnique = false;
    while (!isUnique){
        Tui::highlightCell(window, row, col);
        Tui::printPuzzle(window, grid, A_NORMAL);
        int c = wgetch(window);
        if (c == KEY_UP || c == up_key) {
            row--;
        }
        else if (c == KEY_DOWN || c == down_key) {
            row++;
        }
        else if (c == KEY_LEFT || c == left_key) {
            col--;
        }
        else if (c == KEY_RIGHT || c == right_key) {
            col++;
        }
        else if (c == KEY_BACKSPACE || c == ' ' || c == erase_key) {
            grid[row][col] = 0;
        }
        else if (c == 27 || c == exit_key) {
            return;
        }
        else if (c >= '0' && c <= '9') {
            grid[row][col] = c- '0';
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
