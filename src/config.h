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
#define TOGGLE_KEY 27 // escape key. Switches between insert and pencil

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

#define ERROR_COLOR COLOR_RED
#define CORRECT_COLOR COLOR_BLUE
#define HIGHLIGHT_COLOR COLOR_YELLOW
#define LOWLIGHT_COLOR COLOR_CYAN

// Visual settings

#define PRINT_TITLE true
#define PRINT_HELP true
#define PRINT_COORDS true
#define PRINT_STATUS true // If false time taken to solve a puzzle will not be shown.

// Behavioural settings

#define START_TIMER true
#define REMOVE_MARKS true
#define HIGHLIGHT_SELECTED true
#define DIM_COMPLETED true