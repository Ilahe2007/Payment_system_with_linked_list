#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <stdbool.h>
#include "dsaproject.h"

int main() {

    // Start ncurses
    initscr();            // Start ncurses mode
    cbreak();             // Disable line buffering
    keypad(stdscr, TRUE); // Arrow keys

    // Start blockchain safely
    BlockChain *bc = malloc(sizeof(BlockChain));
    if (!bc) {
        endwin();
        printw("Memory allocation failed\n");
        return 1;
    }
    bc->blocks = NULL;

    menu(bc);

    endwin();
    free(bc);
    return 0;
}