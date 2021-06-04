/*
 * This is the configuration file for tuidoku.
 * Modify this file before installation to change
 * the appearance and behavior of the program
 */
#pragma once
#include <ncurses.h>

#define TITLE "Tuidoku"

// Movement keys
// These keys will work along side the arrow keys
const char UP_KEY = 'k';
const char DOWN_KEY = 'j';
const char LEFT_KEY = 'h';
const char RIGHT_KEY = 'l';

// Other keys

const char INSERT_KEY = 'i';
const char PENCIL_KEY = 'p';
const char QUIT_KEY = 'q';
const char CHECK_KEY = 'c';
const char ERASE_KEY = ' ';
const char GO_KEY = 'g';
const char TOGGLE_KEY = 27; // escape key. Switches between insert and pencil

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

const int ERROR_COLOR = COLOR_RED;
const int CORRECT_COLOR = COLOR_BLUE;
const int HIGHLIGHT_COLOR = COLOR_YELLOW;
const int LOWLIGHT_COLOR = COLOR_CYAN;

// Visual settings

const bool PRINT_TITLE = true;
const bool PRINT_HELP = true;
const bool PRINT_COORDS = true;
const bool PRINT_STATUS = true; // If false time taken to solve a puzzle will not be shown.

// Behavioural settings

const bool START_TIMER = true;
const bool REMOVE_MARKS = true;
const bool HIGHLIGHT_SELECTED = true;
const bool DIM_COMPLETED = true;