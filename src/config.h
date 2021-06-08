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

#define TITLE "Tuidoku"

/*
 * To set a key to a special character such as
 * DEL please refer to these two sites to 
 * set the correct value. Do not put this value inside
 * of quotation marks.
 * https://github.com/python/cpython/blob/main/Lib/curses/ascii.py
 * https://docs.python.org/3/library/curses.ascii.html#module-curses.ascii
 * 
 *  For example to set the QUIT_KEY to delete:
 *      #define QUIT_KEY 0x7f
 */

// Movement keys
// These keys will work along side the arrow keys
#define UP_KEY 'k'
#define DOWN_KEY 'j'
#define LEFT_KEY 'h'
#define RIGHT_KEY 'l'

// Other keys

#define INSERT_KEY 'i'
#define PENCIL_KEY 'p'
#define QUIT_KEY 'q'
#define CHECK_KEY 'c'
#define ERASE_KEY ' '
#define GO_KEY 'g'
#define TOGGLE_KEY 27 // 27 = ESC. Switches between insert and pencil

// Colors

/* Available colors are:
 *  COLOR_BLACK
 *  COLOR_RED
 *  COLOR_GREEN
 *  COLOR_YELLOW
 *  COLOR_BLUE
 *  COLOR_MAGENTA
 *  COLOR_CYAN
 *  COLOR_WHITE
 * 
 * To disable any color set the value to -1
 */

#define FOREGROUND_COLOR -1
#define BACKGROUND_COLOR -1

#define ERROR_COLOR COLOR_RED
#define CORRECT_COLOR COLOR_BLUE

#define HIGHLIGHT_COLOR COLOR_YELLOW
#define LOWLIGHT_COLOR COLOR_CYAN

#define BOARD_COLOR FOREGROUND_COLOR
#define GIVEN_COLOR FOREGROUND_COLOR
#define PLACED_COLOR FOREGROUND_COLOR

// Visual settings

#define PRINT_TITLE true
#define PRINT_HELP true
#define PRINT_COORDS true
#define PRINT_STATUS true // If false time taken to solve a puzzle will not be shown.
#define ALPHABETICAL_ROW false
#define ALPHABETICAL_COL false

/*
 * Check out https://en.wikipedia.org/wiki/Box-drawing_character#Unicode
 * for other characters that you can use
 */ 
#define TOPROW "╔═══╤═══╤═══╦═══╤═══╤═══╦═══╤═══╤═══╗"
#define ROW1   "║   │   │   ║   │   │   ║   │   │   ║"
#define ROW2   "╟───┼───┼───╫───┼───┼───╫───┼───┼───╢"
#define ROW3   "╠═══╪═══╪═══╬═══╪═══╪═══╬═══╪═══╪═══╣"
#define BOTROW "╚═══╧═══╧═══╩═══╧═══╧═══╩═══╧═══╧═══╝" 

/*
// Example of alternate box drawing characters
#define TOPROW "┏━━━┯━━━┯━━━┳━━━┯━━━┯━━━┳━━━┯━━━┯━━━┓"
#define ROW1   "┃   │   │   ┃   │   │   ┃   │   │   ┃"
#define ROW2   "┠───┼───┼───╂───┼───┼───╂───┼───┼───┨"
#define ROW3   "┣━━━┿━━━┿━━━╋━━━┿━━━┿━━━╋━━━┿━━━┿━━━┫"
#define BOTROW "┗━━━┷━━━┷━━━┻━━━┷━━━┷━━━┻━━━┷━━━┷━━━┛"
*/


// Behavioural settings

#define START_TIMER true
#define REMOVE_MARKS true
#define HIGHLIGHT_SELECTED true
#define DIM_COMPLETED true