#include <ncurses.h>
#include <utility>

// Dynamically sized window that either returns the amount of players that would play a game
// or a -2 to not actually play the game but return to the launcher or a -1 to quit the program
class {
    private:
        int Range = 0, Min = 0, Max = 0;
        int FooterLocation = 0;
        int ListLocation = 0;
        int Page = 0, LastPage = 0;
        std::pair<int, int> windowDims = {25, 50};

        void activateColors() {
            init_color(0, 0, 0, 0);
            init_color(1, 999, 0, 0);
            init_color(2, 0, 999, 0);
            init_color(3, 999, 999, 0);
            init_color(4, 0, 0, 999);
            init_color(5, 999, 0, 999);
            init_color(6, 0, 999, 999);
            init_color(7, 999, 999, 999);

            init_pair(COLOR_RED, 1, COLOR_BLACK);
            init_pair(COLOR_GREEN, 2, COLOR_BLACK);
            init_pair(COLOR_YELLOW, 3, COLOR_BLACK);
            init_pair(COLOR_BLUE, 4, COLOR_BLACK);
            init_pair(COLOR_MAGENTA, 5, COLOR_BLACK);
            init_pair(COLOR_CYAN, 6, COLOR_BLACK);
        }

        void render(WINDOW *win) {
            // Header
            mvwprintw(win, 1, 32 / 2 - 13, "Choose a Number of Players");

            // Erase any potentially lingering data
            for (int i = 0; i < 16; i++) {
                mvwprintw(win, 3 + i, 1, "                              ");
            }

            // List the different amounts of players
            for (int i = 0; i < 16; i++) {                
                // Quit early if there aren't enough players left to fill the page
                if (i == (Range - 1) % 16 + 1 && Page == LastPage) {
                    break;
                }

                // Fancy color business
                int rowColor = 1;
                if (i % 4 == 1) {
                    rowColor = 3;
                }
                else if (i % 4 == 2) {
                    rowColor = 2;
                }
                else if (i % 4 == 3) {
                    rowColor = 4;
                }

                wattron(win, COLOR_PAIR(rowColor));
                mvwprintw(win, 3 + i, 10, "%02d Player(s)", Min + i + Page * 16);
                wattroff(win, COLOR_PAIR(rowColor));
            }

            // Page indicator
            int pageColor = 2;
            if (Page == LastPage) {
                pageColor = 1;
            }

            mvwprintw(win, FooterLocation, 9, "Page =", Page, LastPage);
            wattron(win, COLOR_PAIR(pageColor));
            mvwprintw(win, FooterLocation, 16, "%02d   %02d", Page, LastPage);
            wattroff(win, COLOR_PAIR(pageColor));
            mvwaddch(win, FooterLocation, 19, '/');

            // Cursor
            wattron(win, COLOR_PAIR(COLOR_CYAN));
            wattron(win, A_BOLD);
            mvwaddch(win, 3 + ListLocation, 8, '<');
            mvwaddch(win, 3 + ListLocation, 23, '>');
            wattroff(win, A_BOLD);
            wattroff(win, COLOR_PAIR(COLOR_CYAN));

            mvwprintw(win, FooterLocation - 2, 32 / 2 - 13, "  Cancel Game Initiation  ");
            mvwprintw(win, FooterLocation - 1, 32 / 2 - 8, "  Quit Program  ");

            box(win, 0, 0);
            wrefresh(win);
        }

        // Change which game is selected
        void moveCursor(int input) {
            int endPos = (Range - 1) % 16;

            switch (input) {
                case KEY_UP:
                    ListLocation--;

                    // Loops to the end of the list
                    if (Page == 0 && ListLocation < 0) {
                        Page = LastPage;

                        ListLocation = endPos;
                    }
                    // Move position and page normally
                    else {
                        if (ListLocation < 0) {
                            ListLocation = 15;

                            if (Page > 0) {
                                Page--;
                            }
                        }
                    }
                    break;
                case KEY_DOWN:
                    ListLocation++;

                    // Loops to the start of the list
                    if (Page == LastPage && ListLocation == endPos + 1) {
                        Page = 0;

                        ListLocation = 0;
                    }
                    // Move position and page normally
                    else {
                        if (ListLocation > 15) {
                            ListLocation = 0;

                            if (Page < LastPage) {
                                Page++;
                            }
                        }
                    }
                    break;
            }
        }

        int submenu(WINDOW *win) {
            int location = 0;

            while (true) {
                // Render Text
                mvwprintw(win, FooterLocation - 2, 32 / 2 - 13, "  Cancel Game Initiation  ");
                mvwprintw(win, FooterLocation - 1, 32 / 2 - 8, "  Quit Program  ");

                // Add the cursor
                if (location == 0) {
                    wattron(win, COLOR_PAIR(COLOR_YELLOW));
                    mvwaddch(win, FooterLocation - 2, 3, '<');
                    mvwaddch(win, FooterLocation - 2, 28, '>');
                    wattroff(win, COLOR_PAIR(COLOR_YELLOW));
                }
                else {
                    wattron(win, COLOR_PAIR(COLOR_RED));
                    mvwaddch(win, FooterLocation - 1, 8, '<');
                    mvwaddch(win, FooterLocation - 1, 23, '>');
                    wattroff(win, COLOR_PAIR(COLOR_RED));
                }

                int input = wgetch(win);

                switch (input) {
                    case KEY_UP:
                    case KEY_DOWN:
                        if (location == 0) {
                            location = 1;
                        }
                        else {
                            location = 0;
                        }
                        break;
                    case 'z':
                        if (location == 0) {
                            return 1;
                        }
                        else {
                            return -1;
                        }
                        break;
                    case 'x':
                        return 0;
                        break;
                    case 'q':
                        return -1;
                        break;
                }
            }
        }

        void stop(WINDOW *win) {
            wclear(win);
            wrefresh(win);
            delwin(win);

            ListLocation = 0;
            Page = 0;
        }

    public:
        int start(int min, int max) {
            Range = max - min + 1;
            Min = min;
            Max = max;

            LastPage = Range / 16;
            if (Range % 16 == 0) {
                LastPage--;
            }

            if (Range >= 16) {
                FooterLocation = 22;
            }
            else {
                FooterLocation = Range % 16 + 6;
            }
            
            WINDOW *win; 
            if (Range >= 16) {
                win = newwin(24, 32, LINES / 2 - 25 / 2, COLS / 2 - 32 / 2);
            }
            else {
                win = newwin(Range % 16 + 8, 32, LINES / 2 - (Range + 9) / 2, COLS / 2 - 32 / 2);
            }
            keypad(win, TRUE);

            activateColors();

            while (true) {
                render(win);

                int input = wgetch(win);
                int chosenPlayers = min + ListLocation + Page * 16;

                if (input == KEY_UP || input == KEY_DOWN) {
                    moveCursor(input);
                }
                else if (input == 'z') {
                    stop(win);
                    return chosenPlayers;
                }
                else if (input == 'x') {
                    int submenuChoice = submenu(win);

                    if (submenuChoice != 0) {
                        stop(win);
                    }

                    if (submenuChoice == 1) {
                        return -2;
                    }
                    else if (submenuChoice == -1) {
                        return -1;
                    }
                }
                else if (input == 'q') {
                    stop(win);
                    return -1;
                }
            }
        }

} playerSelection;
