#ifndef BOXES
#define BOXES

#include <ncurses.h>
#include <utility>

// Create a rectangle with the standard ncurses border
void drawBox(WINDOW * window = stdscr, int startY = 0, int startX = 0, int dimY = 2, int dimX = 2) {
    // Negative values are cringe
    if (startY < 0) {
        startY = 0;
    }
    if (startX < 0) {
        startX = 0;
    }
    
    // Boxes must be at least 2x2
    if (dimY > 1 && dimX > 1) {
        for (int i = 0; i < dimY; i++) {
            for (int j = 0; j < dimX; j++) {
                if (i == 0 && j == 0) {
                    mvwaddch(window, startY, startX, ACS_ULCORNER);
                }
                else if (i == 0 && j == dimX - 1) {
                    mvwaddch(window, startY, startX + j, ACS_URCORNER);
                }
                else if (i == dimY - 1 && j == 0) {
                    mvwaddch(window, startY + i, startX, ACS_LLCORNER);
                }
                else if (i == dimY - 1 && j == dimX - 1) {
                    mvwaddch(window, startY + i, startX + j, ACS_LRCORNER);
                }
                else if (i == 0 || i == dimY - 1) {
                    mvwaddch(window, startY + i, startX + j, ACS_HLINE);
                }
                else if (j == 0 || j == dimX - 1) {
                    mvwaddch(window, startY + i, startX + j, ACS_VLINE);
                }
            }
        }
    }
}

// Mainly used in conjunction with the drawGrid() function to help with positioning (especially centering)
std::pair<int, int> calcGridDims(int cellsY, int cellsX, int cellSizeY, int cellSizeX) {
    return {cellsY * cellSizeY + (cellsY + 1), cellsX * cellSizeX + (cellsX + 1)};
}

// Create a grid with variable cell size and cell dimensions
void drawGrid(WINDOW *win = stdscr, int startY = 0, int startX = 0, int cellsY = 1, int cellsX = 1, int cellSizeY = 0, int cellSizeX = 0) {
    // Negative values are cringe
    if (startY < 0) {
        startY = 0;
    }
    if (startX < 0) {
        startX = 0;
    }
    if (cellSizeY < 0) {
        cellSizeY = 0;
    }
    if (cellSizeX < 0) {
        cellSizeX = 0;
    }

    // While this function is surprisingly flexible, it isn't this flexible
    if (cellsY > 0 && cellsX > 0) {
        int dimY = cellsY * cellSizeY + cellsY + 1;
        int dimX = cellsX * cellSizeX + cellsX + 1;
        
        for (int i = 0; i < dimY; i++) {
            for (int j = 0; j < dimX; j++) {
                wmove(win, startY + i, startX + j);

                // I must have had a vision or smth because it is a miracle I managed to come up with this
                int iLine = (i + cellSizeY + 1) % (cellSizeY + 1);
                int jLine = (j + cellSizeX + 1) % (cellSizeX + 1);

                if (i == 0 && j == 0) {
                    waddch(win, ACS_ULCORNER);
                }
                else if (i == 0 && jLine == 0 && j != dimX - 1) {
                    waddch(win, ACS_TTEE);
                }
                else if (i == 0 && j == dimX - 1) {
                    waddch(win, ACS_URCORNER);
                }
                else if (iLine == 0 && i != dimY - 1 && j == 0) {
                    waddch(win, ACS_LTEE);
                }
                else if (iLine == 0 && i != dimY - 1 && jLine == 0 && j != dimX - 1) {
                    waddch(win, ACS_PLUS);
                }
                else if (iLine == 0 && i != dimY - 1 && j == dimX - 1) {
                    waddch(win, ACS_RTEE);
                }
                else if (i == dimY - 1 && j == 0) {
                    waddch(win, ACS_LLCORNER);
                }
                else if (i == dimY - 1 && jLine == 0 && j != dimX - 1) {
                    waddch(win, ACS_BTEE);
                }
                else if (i == dimY - 1 && j == dimX - 1) {
                    waddch(win, ACS_LRCORNER);
                }
                else if (iLine == 0) {
                    waddch(win, ACS_HLINE);
                }
                else if (jLine == 0) {
                    waddch(win, ACS_VLINE);
                }
            }
        }
    }
}

void drawHLine(WINDOW *win = stdscr, int startY = 0, int startX = 0, int length = 1, bool addTees = true) {
    // Negative values are cringe
    if (startY < 0) {
        startY = 0;
    }
    if (startX < 0) {
        startX = 0;
    }

    // Minimum length for the line depends on whether tees are included or not
    if ((!addTees && length > 0) || (addTees && length > 2)) {
        for (int i = 0; i < length; i++) {
            mvwaddch(win, startY, startX + i, ACS_HLINE);
        }

        if (addTees) {
            mvwaddch(win, startY, startX, ACS_LTEE);
            mvwaddch(win, startY, startX + length - 1, ACS_RTEE);
        }
    }
}

void drawVLine(WINDOW *win = stdscr, int startY = 0, int startX = 0, int length = 1, bool addTees = true) {
    // Negative values are cringe
    if (startY < 0) {
        startY = 0;
    }
    if (startX < 0) {
        startX = 0;
    }

    // Minimum length for the line depends on whether tees are included or not
    if ((!addTees && length > 0) || (addTees && length > 2)) {
        for (int i = 0; i < length; i++) {
            mvwaddch(win, startY + i, startX, ACS_VLINE);
        }

        if (addTees) {
            mvwaddch(win, startY, startX, ACS_TTEE);
            mvwaddch(win, startY + length - 1, startX, ACS_BTEE);
        }
    }
}

#endif
