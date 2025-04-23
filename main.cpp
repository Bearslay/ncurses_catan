#include "src/catanGame.h"
#include "include/playerSelection.h"
#include <locale.h>

bool checkDims(int dimY = 25, int dimX = 50) {    
    if (LINES - 1 < dimY || COLS - 1 < dimX) {
        mvprintw(LINES / 2 - 3, 0, "[Error] The dimensions of your terminal are too small");
        mvprintw(LINES / 2 - 2, 0, " - Try decreasing the text size\n - Try increasing the window size of the terminal");
        mvprintw(LINES / 2 + 5, 0, "Press any key to quit...\n");

        getch();
        return false;
    }

    return true;
}

int main() {
    setlocale(LC_ALL, "");
    srand(time(NULL));

    initscr();
    noecho();
    curs_set(0);

    // If colors can't be used by the terminal, then a lot of applications become much harder to use
    if (!has_colors()) {
        mvprintw(LINES / 2, 0, "[Error] Your terminal does not support colors");
        mvprintw(LINES / 2 + 1, 0, " - Try using a different terminal or something");
        mvprintw(LINES / 2 + 5, 0, "Press any key to quit...\n");

        getch();
        return 0;
    }
    if (!can_change_color()) {
        mvprintw(LINES / 2 + 2, 0, "[Error] Your terminal does not support changing colors");
        mvprintw(LINES / 2 + 3, 0, " - Try using a different terminal or something");
        mvprintw(LINES / 2 + 5, 0, "Press any key to quit...\n");

        getch();
        return 0;
    }

    start_color();

    // End Terminal Validation

    int players = 0, endVal = 0;
    while (endVal != -1) {
        // Make sure the catan board can be displayed in full
        if (!checkDims(50, 195)) {
            endwin();
            return 0;
        }

        players = playerSelection.start(2, 4);

        if (players > 0) {
            catanGame catan(players);

            endVal = catan.start({LINES / 2 - 50 / 2, COLS / 2 - 195 / 2});
        }
        else {
            endVal = -1;
        }
    }

    endwin();
    return 0;
}
