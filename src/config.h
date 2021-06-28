/*
 * This is the configuration file for tuidoku.
 * Modify this file before installation to change
 * the appearance and behavior of the program
 * 
 * If there is anything that you would like to configure
 * that you cannot do in this file please let me know at
 * philipphuket AT gmail.
 */

#pragma once

static const char *TITLE = "Tuidoku";

/*
 * To set a key to a special character such as
 * DEL please refer to these two sites to 
 * set the correct value. Do not put this value inside
 * of quotation marks.
 * https://github.com/python/cpython/blob/main/Lib/curses/ascii.py
 * https://docs.python.org/3/library/curses.ascii.html#module-curses.ascii
 * 
 *  For example to set the QUIT_KEY to delete:
 *      QUIT_KEY = 0x7f,
 */

enum keybinds {
    // Movement Keys
    // These keys will work along side the arrow keys
    UP_KEY = 'k',
    DOWN_KEY = 'j',
    LEFT_KEY = 'h',
    RIGHT_KEY = 'l',

    // Other keys
    INSERT_KEY = 'i',
    PENCIL_KEY = 'p',
    QUIT_KEY = 'q',
    CHECK_KEY = 'c',
    ERASE_KEY = ' ', // 127 = backspace, or '\b' = backspace
    GO_KEY = 'g',
    TOGGLE_KEY =  27, // 27 = ESC. Switches between incert and pencil
    AUTO_PENCIL_KEY = 'a',
};

// Colors
/* Available colors are:
 *  COLOR_BLACK = 0
 *  COLOR_RED = 1
 *  COLOR_GREEN = 2
 *  COLOR_YELLOW = 3
 *  COLOR_BLUE = 4
 *  COLOR_MAGENTA = 5
 *  COLOR_CYAN = 6
 *  COLOR_WHITE = 7
 *  DEFAULT = -1
 * To disable any color set the value to -1
 */
enum colors {
    FOREGROUND_COLOR = -1,
    BACKGROUND_COLOR = -1,
    ERROR_COLOR = 1,
    CORRECT_COLOR = 4,
    HIGHLIGHT_COLOR = 3,
    LOWLIGHT_COLOR = 6,
    BOARD_COLOR = FOREGROUND_COLOR,
    GIVEN_COLOR = FOREGROUND_COLOR,
    PLACED_COLOR = FOREGROUND_COLOR,
};


// Visual settings

static const bool PRINT_TITLE = true;
static const bool PRINT_HELP = true;
static const bool PRINT_COORDS = true;
static const bool PRINT_STATUS = true; // If false time taken to solve a puzzle will not be shown.
static const char ROW_CHAR = '1'; // Change to 'a' for alphabetical coords. Or '0' to start index at 0.
static const char COL_CHAR = '1';
static const char START_CHAR = '1';  // Change to 'a' for alphabetical.

/*
 * Check out https://en.wikipedia.org/wiki/Box-drawing_character#Unicode
 * for other characters that you can use
 */ 
static const char *TOPROW = "╔═══╤═══╤═══╦═══╤═══╤═══╦═══╤═══╤═══╗";
static const char *ROW1   = "║   │   │   ║   │   │   ║   │   │   ║";
static const char *ROW2   = "╟───┼───┼───╫───┼───┼───╫───┼───┼───╢";
static const char *ROW3   = "╠═══╪═══╪═══╬═══╪═══╪═══╬═══╪═══╪═══╣";
static const char *BOTROW = "╚═══╧═══╧═══╩═══╧═══╧═══╩═══╧═══╧═══╝";

static const char *BIGTOP    = "╔═══════╤═══════╤═══════╦═══════╤═══════╤═══════╦═══════╤═══════╤═══════╗";
static const char *BIGROW1   = "║       │       │       ║       │       │       ║       │       │       ║";
static const char *BIGROW2   = "╟───────┼───────┼───────╫───────┼───────┼───────╫───────┼───────┼───────╢";
static const char *BIGROW3   = "╠═══════╪═══════╪═══════╬═══════╪═══════╪═══════╬═══════╪═══════╪═══════╣";
static const char *BIGBOTTOM = "╚═══════╧═══════╧═══════╩═══════╧═══════╧═══════╩═══════╧═══════╧═══════╝";

/*
// Example of alternate box drawing characters
static const char *TOPROW = "┏━━━┯━━━┯━━━┳━━━┯━━━┯━━━┳━━━┯━━━┯━━━┓";
static const char *ROW1   = "┃   │   │   ┃   │   │   ┃   │   │   ┃";
static const char *ROW2   = "┠───┼───┼───╂───┼───┼───╂───┼───┼───┨";
static const char *ROW3   = "┣━━━┿━━━┿━━━╋━━━┿━━━┿━━━╋━━━┿━━━┿━━━┫";
static const char *BOTROW = "┗━━━┷━━━┷━━━┻━━━┷━━━┷━━━┻━━━┷━━━┷━━━┛";

static const char *BIGTOP    = "┏━━━━━━━┯━━━━━━━┯━━━━━━━┳━━━━━━━┯━━━━━━━┯━━━━━━━┳━━━━━━━┯━━━━━━━┯━━━━━━━┓";
static const char *BIGROW1   = "┃       │       │       ┃       │       │       ┃       │       │       ┃";
static const char *BIGROW2   = "┠───────┼───────┼───────╂───────┼───────┼───────╂───────┼───────┼───────┨";
static const char *BIGROW3   = "┣━━━━━━━┿━━━━━━━┿━━━━━━━╋━━━━━━━┿━━━━━━━┿━━━━━━━╋━━━━━━━┿━━━━━━━┿━━━━━━━┫";
static const char *BIGBOTTOM = "┗━━━━━━━┷━━━━━━━┷━━━━━━━┻━━━━━━━┷━━━━━━━┷━━━━━━━┻━━━━━━━┷━━━━━━━┷━━━━━━━┛";
*/

// Behavioural settings

static const bool START_TIMER = true;
static const bool REMOVE_MARKS = true;
static const bool HIGHLIGHT_SELECTED = true;
static const bool DIM_COMPLETED = true;
