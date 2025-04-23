#include "../include/wrap.h"
#include "../include/customText.h"
#include "../include/boxes.h"
#include "../include/basicList.h"
#include "../include/largeText.h"

#include "catanBoard.h"
#include "catanPlayer.h"

#include <ncurses.h>
#include <utility>
#include <vector>
#include <ctime>
#include <cmath>

class catanGame {
    private:
        // I have no idea why but a 7 is always rolled on the first turn, and thats not fun so heres this:
        bool firstTurn = true;
        std::pair<int, int> Origin;

        // General Catan variables
        int PlayerAmt = 0;
        int Turn = 0;
        std::pair<int, int> Achievers = {-1, -1};

        // Interface stuff
        std::pair<int, int> Location = {0, 0};
        WINDOW *win[4];

        // Board and players
        catanBoard Board;
        catanPlayer Players[4] = {{0, 1}, {1, 1}, {2, 1}, {3, 1}};

        // Allow or disallow trading
        bool CanTrade[11] = {false, false, false, false, false, false, false, false, false, false, false};

        // Catan has a whole bunch of different colors going into it
        void activateColors() {
            // Defaults
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
            
            // The four player colors (still not sure why Catan chose such strange ones but here we are)
            #define PLAYER_RED 7
            #define PLAYER_ORANGE 8
            #define PLAYER_BLUE 9
            #define PLAYER_WHITE 10

            init_color(8, 999, 0, 0);
            init_color(9, 920, 304, 0);
            init_color(10, 0, 0, 999);
            init_color(11, 750, 750, 750);

            init_pair(PLAYER_RED, 8, COLOR_BLACK);
            init_pair(PLAYER_ORANGE, 9, COLOR_BLACK);
            init_pair(PLAYER_BLUE, 10, COLOR_BLACK);
            init_pair(PLAYER_WHITE, 11, COLOR_BLACK);

            // Colors used to represent each resource (as well as the desert and any resource)
            #define RESOURCE_CLAY 11
            #define RESOURCE_ROCK 12
            #define RESOURCE_WHEAT 13
            #define RESOURCE_WOOD 14
            #define RESOURCE_WOOL 15
            #define RESOURCE_DESERT 16
            #define RESOURCE_GENERAL 17

            init_color(12, 900, 150, 200);
            init_color(13, 512, 512, 512);
            init_color(14, 832, 600, 0);
            init_color(15, 0, 800, 150);
            init_color(16, 832, 832, 832);
            init_color(17, 888, 692, 472);
            init_color(18, 584, 160, 888);

            init_pair(RESOURCE_CLAY, 12, COLOR_BLACK);
            init_pair(RESOURCE_ROCK, 13, COLOR_BLACK);
            init_pair(RESOURCE_WHEAT, 14, COLOR_BLACK);
            init_pair(RESOURCE_WOOD, 15, COLOR_BLACK);
            init_pair(RESOURCE_WOOL, 16, COLOR_BLACK);
            init_pair(RESOURCE_DESERT, 17, COLOR_BLACK);
            init_pair(RESOURCE_GENERAL, 18, COLOR_BLACK);

            // Resource tile rarity (like the dots on each number tile in regular Catan)
            #define RARITY_SCARCE 18
            #define RARITY_RARE 19
            #define RARITY_UNCOMMON 20
            #define RARITY_COMMON 21
            #define RARITY_ABUNDANT 22

            init_color(19, 756, 472, 400);
            init_color(20, 692, 408, 140);
            init_color(21, 80, 888, 160);
            init_color(22, 232, 756, 928);
            init_color(23, 880, 212, 928);

            init_pair(RARITY_SCARCE, 19, COLOR_BLACK);
            init_pair(RARITY_RARE, 20, COLOR_BLACK);
            init_pair(RARITY_UNCOMMON, 21, COLOR_BLACK);
            init_pair(RARITY_COMMON, 22, COLOR_BLACK);
            init_pair(RARITY_ABUNDANT, 23, COLOR_BLACK);

            // Dev card colors (hidden card as well as the three "classes" of dev card)
            #define DEV_CARD 23
            #define DEV_VP 24
            #define DEV_KNIGHT 25
            #define DEV_ECON 26

            init_color(24, 772, 448, 816);
            init_color(25, 760, 304, 896);
            init_color(26, 868, 388, 200);
            init_color(27, 24, 500, 48);

            init_pair(DEV_CARD, 24, COLOR_BLACK);
            init_pair(DEV_KNIGHT, 25, COLOR_BLACK);
            init_pair(DEV_VP, 26, COLOR_BLACK);
            init_pair(DEV_ECON, 27, COLOR_BLACK);

            // The robber, mainly for the indicator on a tile
            #define ROBBER 27

            init_color(28, 644, 684, 836);
            
            init_pair(ROBBER, 28, COLOR_BLACK);
        }

        // Render the little line of stats at the top of the window
        void renderWindowStats(int roll, bool pregame = false, int player = 0) {
            std::string playerNames[4] = {"Red", "Orange", "Blue", "White"};
            int xOffset = 0;
            if (pregame) {
                xOffset = 65;
            }

            int numberColor = 0;
            switch (roll) {
                case 0:
                    numberColor = 0;
                    break;
                case 2:
                case 12:
                    numberColor = 18;
                    break;
                case 3:
                case 11:
                    numberColor = 19;
                    break;
                case 4:
                case 10:
                    numberColor = 20;
                    break;
                case 5:
                case 9:
                    numberColor = 21;
                    break;
                case 6:
                case 8:
                    numberColor = 22;
                    break;
                case 7:
                    numberColor = ROBBER;
                    break;
            }

            if (!pregame) {
                drawBox(stdscr, Origin.first - 1, Origin.second - 2, 52, 201);
            }
            else {
                drawBox(stdscr, Origin.first - 1, Origin.second - 2 + xOffset, 38, 71);
            }

            mvwaddch(stdscr, Origin.first - 1, Origin.second + xOffset, ACS_URCORNER);

            wattron(stdscr, A_BOLD);
            wattron(stdscr, COLOR_PAIR(DEV_VP));
            mvwprintw(stdscr, Origin.first - 1, Origin.second + 1 + xOffset, " Catan ");
            wattroff(stdscr, COLOR_PAIR(DEV_VP));
            wattroff(stdscr, A_BOLD);

            mvwaddch(stdscr, Origin.first - 1, Origin.second + 8 + xOffset, ACS_ULCORNER);
            mvwaddch(stdscr, Origin.first - 1, Origin.second + 9 + xOffset, ACS_HLINE);
            mvwaddch(stdscr, Origin.first - 1, Origin.second + 10 + xOffset, ACS_URCORNER);

            mvwprintw(stdscr, Origin.first - 1, Origin.second + 11 + xOffset, " Current Player: ");
            
            if (!pregame) {
                wattron(stdscr, A_BOLD);
                wattron(stdscr, COLOR_PAIR(Turn + 7));
                mvwprintw(stdscr, Origin.first - 1, Origin.second + 28, "%s ", playerNames[Turn].c_str());
                wattroff(stdscr, COLOR_PAIR(Turn + 7));
                wattroff(stdscr, A_BOLD);

                mvwaddch(stdscr, Origin.first - 1, Origin.second + 29 + playerNames[Turn].length(), ACS_ULCORNER);
            }
            else {
                wattron(stdscr, A_BOLD);
                wattron(stdscr, COLOR_PAIR(player + 7));
                mvwprintw(stdscr, Origin.first - 1, Origin.second + 28 + xOffset, "%s ", playerNames[player].c_str());
                wattroff(stdscr, COLOR_PAIR(player + 7));
                wattroff(stdscr, A_BOLD);

                mvwaddch(stdscr, Origin.first - 1, Origin.second + 29 + playerNames[player].length() + xOffset, ACS_ULCORNER);
            }

            if (!pregame) {
                mvwaddch(stdscr, Origin.first - 1, Origin.second + 30 + playerNames[Turn].length(), ACS_HLINE);
                mvwaddch(stdscr, Origin.first - 1, Origin.second + 31 + playerNames[Turn].length(), ACS_URCORNER);

                mvwprintw(stdscr, Origin.first - 1, Origin.second + 32 + playerNames[Turn].length(), " Last Number Rolled: ");

                wattron(stdscr, A_BOLD);
                wattron(stdscr, COLOR_PAIR(numberColor));
                mvwprintw(stdscr, Origin.first - 1, Origin.second + 53 + playerNames[Turn].length(), "%02d ", roll);
                wattroff(stdscr, COLOR_PAIR(numberColor));
                wattroff(stdscr, A_BOLD);

                mvwaddch(stdscr, Origin.first - 1, Origin.second + 56 + playerNames[Turn].length(), ACS_ULCORNER);
            }
            
            refresh();
        }

        // Dynamically updates selections based on if they can be afforded or not as well as display the price for each item
        void renderShop() {
            std::string selections[4] = {"Road", "Settlement", "City", "Dev Card"};

            // Colorize selections based off of whether its affordable
            int noBuy[4] = {Turn, Turn, Turn, Turn};

            if (Players[Turn].getStat(5) < 1 || Players[Turn].getStat(8) < 1) {
                noBuy[0] = 9;
            }
            if (Players[Turn].getStat(5) < 1 || Players[Turn].getStat(7) < 1 || Players[Turn].getStat(8) < 1 || Players[Turn].getStat(9) < 1) {
                noBuy[1] = 9;
            }
            if (Players[Turn].getStat(6) < 2 || Players[Turn].getStat(7) < 3) {
                noBuy[2] = 9;
            }
            if (Players[Turn].getStat(6) < 1 || Players[Turn].getStat(7) < 1 || Players[Turn].getStat(9) < 1) {
                noBuy[3] = 9;
            }

            for (int i = 0; i < 4; i++) {
                wattron(win[0], COLOR_PAIR(7 + noBuy[i]));
                mvwprintw(win[0], 3 + i * 2, 4, "%s", selections[i].c_str());
                wattroff(win[0], COLOR_PAIR(7 + noBuy[i]));
            }

            // Prices
            customText(win[0], {3, 17}, {{{}, "(Costs "}, {{11}, "1 Clay"}, {{}, ", "}, {{14}, "1 Wood"}, {{}, ")"}});
            customText(win[0], {5, 17}, {{{}, "(Costs "}, {{11}, "1 Clay"}, {{}, ", "}, {{14}, "1 Wood"}, {{}, ", "}, {{13}, "1 Wheat"}, {{}, ", "}, {{15}, "1 Wool"}, {{}, ")"}});
            customText(win[0], {7, 17}, {{{}, "(Costs "}, {{12}, "2 Rock"}, {{}, ", "}, {{13}, "3 Wheat"}, {{}, "; Replaces Settlement)"}});
            customText(win[0], {9, 17}, {{{}, "(Costs "}, {{12}, "1 Rock"}, {{}, ", "}, {{13}, "1 Wheat"}, {{}, ", "}, {{15}, "1 Wool"}, {{}, ")"}});

            // Titles and Borders
            mvwprintw(win[0], getmaxy(win[0]) - 2, 1, "PURCHASE A BUILDING OR A DEVELOPMENT CARD");
            mvwprintw(win[0], 1, 1, "PURCHASE BUILDING/DEVELOPMENT CARD:");

            box(win[0], 0, 0);
            drawHLine(win[0], 11, 0, getmaxx(win[0]));
            wrefresh(win[0]);
        }

        // Only renders dev cards if they are chosen to be shown, descriptions for each type of card is also provided
        void renderDevCards(bool show) {
            customText(win[1], {3, 4}, {{{23}, "Show/Hide Dev Cards"}});

            if (show) {
                // Add the cards and their count
                wattron(win[1], COLOR_PAIR(25));
                mvwprintw(win[1], 6, 4, "Knight (x%02d)", Players[Turn].getStat(11));
                wattroff(win[1], COLOR_PAIR(25));

                wattron(win[1], COLOR_PAIR(26));
                mvwprintw(win[1], 7, 4, "Monopoly (x%02d)", Players[Turn].getStat(12));
                mvwprintw(win[1], 8, 4, "Year of Plenty (x%02d)", Players[Turn].getStat(13));
                mvwprintw(win[1], 9, 4, "Road Building (x%02d)", Players[Turn].getStat(14));
                wattroff(win[1], COLOR_PAIR(26));
            }
            else {
                // Remove cards when not shown
                for (int i = 0; i < 5; i++) {
                    for (int j = 0; j < 25; j++) {
                        mvwaddch(win[1], 6 + i, 2 + j, ' ');
                    }
                }
            }

            // Store descriptions
            std::string descriptions[5] = {
                "N/A: No Development Card selected",
                "The knight allows you to move the robber wherever you would like and then steal 1 random resource from 1 player with either a city or a settlement that is on the the robber was moved to.",
                "Playing the Monopoly card allows you to declare one of the five resource types to monopolize. When it is monopolized, all players with a resource of that type must give you their entire supply of that resource. This card is discarded when played.",
                "Playing the Year of Plenty card allows you to take 2 resources of your choice from the bank. This card is discarded when played.",
                "Playing the Road Building card allows you to place 2 roads, abiding by normal rules, wherever you would like for free. This card is discarded when played."
            };

            // Remove previous description
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 39; j++) {
                    mvwaddch(win[1], 3 + i, 28 + j, ' ');
                }
            }

            // Add new description
            if (Location.second == 1) {
                std::vector<std::string> description = commonWrap(descriptions[Location.first], 37);

                int color = 0;
                if (Location.first == 1) {
                    color = 25;
                }
                else if (Location.first >= 2) {
                    color = 26;
                }

                wattron(win[1], COLOR_PAIR(color));
                for (int i = 0; i < description.size(); i++) {
                    mvwprintw(win[1], 3 + i, 28, description[i].c_str());
                }
                wattroff(win[1], COLOR_PAIR(color));
            }
            else {
                mvwprintw(win[1], 3, 28, descriptions[0].c_str());
            }

            // Titles and Borders
            mvwprintw(win[1], getmaxy(win[1]) - 2, 1, "USE A DEVELOPMENT CARD");
            mvwprintw(win[1], 1, 1, "SELECT DEVELOPMENT CARD:");
            mvwprintw(win[1], 1, 28, "DESCRIPTION:");

            box(win[1], 0, 0);
            drawHLine(win[1], 11, 0, getmaxx(win[1]));
            drawVLine(win[1], 0, 27, 12);
            wrefresh(win[1]);
        }

        // Bank trades are miscolored until unlocked, player trades are miscolored if the game doesn't include that player or is the player's turn
        void renderTrades() {
            // Determines how many different kinds of trades the player can make
            int noTrade[11] = {17, 17, 11, 12, 13, 14, 15, 7, 8, 9, 10};

            // Discolor trades that cannot be made
            for (int i = 0; i < 11; i++) {
                if (!CanTrade[i]) {
                    noTrade[i] = 16;
                }
            }

            // Bank Trades
            customText(win[2], {3, 4}, {{{noTrade[0]}, "4:1 Any Trade"}});
            customText(win[2], {4, 4}, {{{noTrade[1]}, "3:1 Any Trade"}});
            customText(win[2], {6, 4}, {{{noTrade[2]}, "2:1 Clay Trade"}});
            customText(win[2], {7, 4}, {{{noTrade[3]}, "2:1 Rock Trade"}});
            customText(win[2], {8, 4}, {{{noTrade[4]}, "2:1 Wheat Trade"}});
            customText(win[2], {9, 4}, {{{noTrade[5]}, "2:1 Wood Trade"}});
            customText(win[2], {10, 4}, {{{noTrade[6]}, "2:1 Wool Trade"}});

            // Player Trades
            customText(win[2], {3, 27}, {{{noTrade[7]}, "Trade with Red"}});
            customText(win[2], {5, 27}, {{{noTrade[8]}, "Trade with Orange"}});
            customText(win[2], {7, 27}, {{{noTrade[9]}, "Trade with Blue"}});
            customText(win[2], {9, 27}, {{{noTrade[10]}, "Trade with White"}});

            // Titles and Borders
            mvwprintw(win[2], getmaxy(win[2]) - 2, 1, "TRADE WITH THE BANK AND/OR A PLAYER");
            mvwprintw(win[2], 1, 24, "TRADE WITH A PLAYER:");
            mvwprintw(win[2], 1, 1, "TRADE WITH THE BANK:");

            box(win[2], 0, 0);
            drawHLine(win[2], 11, 0, getmaxx(win[2]));
            drawVLine(win[2], 0, 23, 12);
            wrefresh(win[2]);
        }

        // Really just loads the three options in this extra menu
        void renderManager() {
            // Actions
            mvwprintw(win[3], 2, 6, "End");
            mvwprintw(win[3], 3, 5, "Round");
            mvwprintw(win[3], 5, 5, "Start");
            mvwprintw(win[3], 6, 5, "Again");
            mvwprintw(win[3], 8, 5, "Leave");
            mvwprintw(win[3], 9, 5, "Catan");
            
            // Titles and Borders
            mvwprintw(win[3], getmaxy(win[3]) - 2, 2, "MANAGE GAME");

            box(win[3], 0, 0);
            drawHLine(win[3], 11, 0, getmaxx(win[3]));
            wrefresh(win[3]);
        }

        // Indicate which option the player has selected, changes color to match the player's turn
        void renderCursor() {
            int offsets[4][8] = {
                {
                    7, 13, 7, 11
                },
                {
                    22, 15, 17, 23, 22
                },
                {
                    16, 16, 17, 17, 18, 17, 17
                },
                {
                    17, 20, 18, 19
                }
            };

            // Remove previous cursors
            for (int i = 0; i < 4; i++) {
                mvwaddch(win[0], 3 + i * 2, 2, ' ');
                mvwaddch(win[0], 3 + i * 2, 2 + offsets[0][i], ' ');
            }
            for (int i = 0; i < 5; i++) {
                if (i == 0) {
                    mvwaddch(win[1], 3, 2, ' ');
                    mvwaddch(win[1], 3, 2 + offsets[1][i], ' ');
                }
                else {
                    mvwaddch(win[1], 5 + i, 2, ' ');
                    mvwaddch(win[1], 5 + i, 2 + offsets[1][i], ' ');
                }
            }
            for (int i = 0; i < 7; i++) {
                if (i < 2) {
                    mvwaddch(win[2], 3 + i, 2, ' ');
                    mvwaddch(win[2], 3 + i, 2 + offsets[2][i], ' ');
                }
                else {
                    mvwaddch(win[2], 4 + i, 2, ' ');
                    mvwaddch(win[2], 4 + i, 2 + offsets[2][i], ' ');
                }
            }
            for (int i = 0; i < 4; i++) {
                mvwaddch(win[2], 3 + i * 2, 25, ' ');
                mvwaddch(win[2], 3 + i * 2, 25 + offsets[3][i], ' ');
            }

            // Add cursor to current location
            int window = Location.second;
            if (window == 3) {
                window--;
            }
            wattron(win[window], COLOR_PAIR(7 + Turn));
            wattron(win[window], A_BOLD);

            if (Location.second == 0) {
                mvwaddch(win[0], 3 + Location.first * 2, 2, '<');
                mvwaddch(win[0], 3 + Location.first * 2, 2 + offsets[0][Location.first], '>');
            }
            else if (Location.second == 1) {
                if (Location.first == 0) {
                    mvwaddch(win[1], 3, 2, '<');
                    mvwaddch(win[1], 3, 2 + offsets[1][Location.first], '>');
                }
                else {
                    mvwaddch(win[1], 5 + Location.first, 2, '<');
                    mvwaddch(win[1], 5 + Location.first, 2 + offsets[1][Location.first], '>');
                }
            }
            else if (Location.second == 2) {
                if (Location.first < 2) {
                    mvwaddch(win[2], 3 + Location.first, 2, '<');
                    mvwaddch(win[2], 3 + Location.first, 2 + offsets[2][Location.first], '>');
                }
                else {
                    mvwaddch(win[2], 4 + Location.first, 2, '<');
                    mvwaddch(win[2], 4 + Location.first, 2 + offsets[2][Location.first], '>');
                }
            }
            else if (Location.second == 3) {
                mvwaddch(win[2], 3 + Location.first * 2, 25, '<');
                mvwaddch(win[2], 3 + Location.first * 2, 25 + offsets[3][Location.first], '>');
            }

            wattroff(win[window], A_BOLD);
            wattroff(win[window], COLOR_PAIR(7 + Turn));
        }

        // Based on the arrows used, the cursor will move through the current column or to the next one
        void moveCursor(int input, bool show) {
            switch (input) {
                case KEY_UP:
                    // When in dev card selection, you can only move up or down if your dev cards are shown
                    if (show || Location.second != 1) {
                        Location.first--;
                    }

                    if (Location.first < 0) {
                        if (Location.second == 0 || Location.second == 3) {
                            Location.first = 3;
                        }
                        else if (Location.second == 1) {
                            Location.first = 4;
                        }
                        else if (Location.second == 2) {
                            Location.first = 6;
                        }
                    }
                    break;
                case KEY_DOWN:
                    // When in dev card selection, you can only move up or down if your dev cards are shown
                    if (show || Location.second != 1) {
                        Location.first++;
                    }
                    
                    if ((Location.second == 0 || Location.second == 3) && Location.first > 3) {
                        Location.first = 0;
                    }
                    else if (Location.second == 1 && Location.first > 4) {
                        Location.first = 0;
                    }
                    else if (Location.second == 2 && Location.first > 6) {
                        Location.first = 0;
                    }
                    break;
                case KEY_LEFT:
                    Location.first = 0;
                    Location.second--;

                    if (Location.second < 0) {
                        Location.second = 3;
                    }
                    break;
                case KEY_RIGHT:
                    Location.first = 0;
                    Location.second++;

                    if (Location.second > 3) {
                        Location.second = 0;
                    }
                    break;
            }
        }

        // Combine all of the other rendering functions to render the interface
        void render(bool show) {
            Board.render();

            for (int i = 0; i < PlayerAmt; i++) {
                // Only reveal your dev cards, if chosen to do so
                if (i == Turn) {
                    Players[i].render(show);
                }
                else {
                    Players[i].render(false);
                }
            }

            renderShop();
            renderDevCards(show);
            renderTrades();
            renderManager();
            renderCursor();
        }

        // Determine if the current player is allowed to make any of the trades
        void updateTrades() {
            // Reset all trades to possible
            for (int i = 0; i < 11; i++) {
                CanTrade[i] = true;
            }

            // Can't trade at all if you don't have any resources
            if (Players[Turn].getStat(25) < 1) {
                for (int i = 0; i < 11; i++) {
                    CanTrade[i] = false;
                }
            }
            else {
                // Determine if a 4:1 or 3:1 trade is possible
                if (Players[Turn].getStat(5) < 4 && Players[Turn].getStat(6) < 4 && Players[Turn].getStat(7) < 4 && Players[Turn].getStat(8) < 4 && Players[Turn].getStat(9) < 4) {
                    CanTrade[0] = false;
                }
                if (Players[Turn].getStat(17) == 0 || (Players[Turn].getStat(5) < 3 && Players[Turn].getStat(6) < 3 && Players[Turn].getStat(7) < 3 && Players[Turn].getStat(8) < 3 && Players[Turn].getStat(9) < 3)) {
                    CanTrade[1] = false;
                }

                // Determine if any of the 2:1 trades are possible
                for (int i = 0; i < 5; i++) {
                    if (Players[Turn].getStat(i + 18) == 0 || (Players[Turn].getStat(i + 5) < 2)) {
                        CanTrade[i + 2] = false;
                    }
                }

                // Can't trade with yourself
                CanTrade[7 + Players[Turn].getStat(0)] = false;
                
                // Can't trade with players not in the game
                if (PlayerAmt == 2) {
                    CanTrade[9] = false;
                }
                if (PlayerAmt <= 3) {
                    CanTrade[10] = false;
                }

                // Can't trade with a player that doesn't have any resources
                for (int i = 0; i < PlayerAmt; i++) {
                    if (Players[i].getStat(i + 5) + Players[i].getStat(i + 6) + Players[i].getStat(i + 7) + Players[i].getStat(i + 8) + Players[i].getStat(i + 9) < 1) {
                        CanTrade[7 + i] = false;
                    }
                }
            }
        }

        // Based on a ratio and resourceID, you get to choose 1 of four resources to collect
        // ratio: 2 = 2:1 trade, 3 = 3:1 trade, 4 = 4:1 trade
        // resourceID: 0 = Clay, 1 = Rock, 2 = Wheat, 3 = Wood, 4 = Wool
        // Return false to quit catan or true to keep playing
        bool outBankTrade(int ratio, int resourceID) {
            // Clear trading space
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 22; j++) {
                    mvwaddch(win[2], 3 + i, 1 + j, ' ');
                }
            }

            basicList outTrade(win[2], {3, 2}, {{{{11}, "Receive 1 Clay"}}, {{{12}, "Receive 1 Rock"}}, {{{13}, "Receive 1 Wheat"}}, {{{14}, "Receive 1 Wood"}}, {{{15}, "Receive 1 Wool"}}}, {16, {resourceID}}, 1, Turn + 7, true);          
            std::pair<bool, int> listOutput;

            int location = 0;
            while (true) {
                std::string resources[5] = {"Clay", "Rock", "Wheat", "Wood", "Wool"};

                mvwprintw(win[2], 9, 4, "Current Trade:");
                customText(win[2], {10, 4}, {{{11 + resourceID}, std::to_string(ratio)}, {{0}, " "}, {{11 + resourceID}, resources[resourceID]}, {{0}, " : "}, {{11 + listOutput.second}, "1 "}, {{11 + listOutput.second}, resources[listOutput.second]}, {{0}, " "}});

                listOutput = outTrade.start(location, true);

                if (listOutput.first) {
                    switch (listOutput.second) {
                        case -1:
                            return false;
                            break;
                        case -2:
                            return true;
                            break;
                        default:
                            Players[Turn].removeResource(resourceID, ratio);
                            Players[Turn].addResource(listOutput.second);
                            return true;
                            break;
                    }
                }
                else {
                    location = listOutput.second;
                }
            }
        }

        // Choose which of the five resources to give to the bank during a trade, it then leads directly into outBankTrade
        // Return false to quit catan or true to keep playing
        bool inBankTrade(int ratio) {
            // Clear trading space
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 22; j++) {
                    mvwaddch(win[2], 3 + i, 1 + j, ' ');
                }
            }

            // As a quality-of-life, the starting location is set to the first resource that the player has at least 4 of
            int location = 0;
            for (int i = 0; i < 5; i++) {
                if (Players[Turn].getStat(i + 5) >= ratio) {
                    location = i;
                    break;
                }
            }

            // Ban a trade if the player can not afford it
            std::vector<int> bannedTrades;
            for (int i = 0; i < 5; i++) {
                if (Players[Turn].getStat(i + 5) < ratio) {
                    bannedTrades.emplace_back(i);
                }
            }

            basicList inTrade(win[2], {3, 2}, {{{{11}, "Trade in " + std::to_string(ratio) + " Clay"}}, {{{12}, "Trade in " + std::to_string(ratio) + " Rock"}}, {{{13}, "Trade in " + std::to_string(ratio) + " Wheat"}}, {{{14}, "Trade in " + std::to_string(ratio) + " Wood"}}, {{{15}, "Trade in " + std::to_string(ratio) + " Wool"}}}, {16, bannedTrades}, 1, Turn + 7, true);
            std::pair<bool, int> listOutput = {false, location};

            while (true) {
                std::string resources[5] = {"Clay", "Rock", "Wheat", "Wood", "Wool"};

                mvwprintw(win[2], 9, 4, "Current Trade:");
                customText(win[2], {10, 4}, {{{11 + listOutput.second}, std::to_string(ratio)}, {{0}, " "}, {{11 + listOutput.second}, resources[listOutput.second]}, {{0}, " : 1 ????? "}});

                listOutput = inTrade.start(location, true);

                if (listOutput.first) {
                    switch (listOutput.second) {
                        case -1:
                            return false;
                            break;
                        case -2:
                            return true;
                            break;
                        default:
                            return outBankTrade(ratio, listOutput.second);
                            break;
                    }
                }
                else {
                    location = listOutput.second;
                }
            }
        }

        // Make a trade with another player
        // Return false to quit catan or true to keep playing
        bool playerTrade() {
            int location = 0, color = 7 + Turn;
            std::pair<int, int> tradingAmt[5] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}};

            std::string resources[5] = {"Clay", "Rock", "Wheat", "Wood", "Wool"};

            // Clear the trading section of the box
            for (int i = 0; i < 8; i ++) {
                for (int j = 0; j < 22; j++) {
                    mvwaddch(win[2], 3 + i, 24 + j, ' ');
                }
            }

            // Render Each Counter
            for (int i = 0; i < 5; i++) {
                // Correct spacing
                std::string spaces = "     ";
                if (i == 2) {
                    spaces.erase(0, 1);
                }

                customText(win[2], {3 + i, 26}, {{{11 + i}, resources[i].c_str()}, {{0}, ":"}, {{0}, spaces.c_str()}, {{COLOR_RED}, "- "}, {{11 + i}, "x00"}, {{COLOR_GREEN}, " +"}});
            }
            wrefresh(win[2]);

            bool isP2 = false;
            while (true) {
                // Render cursor
                if (isP2) {
                    color = 7 + Location.first;
                }

                wattron(win[2], COLOR_PAIR(color));
                wattron(win[2], A_BOLD);
                mvwaddch(win[2], 3 + location, 34, '<');
                mvwaddch(win[2], 3 + location, 44, '>');
                wattroff(win[2], A_BOLD);
                wattroff(win[2], COLOR_PAIR(color));

                // Update numbers in each counter
                for (int i = 0; i < 5; i++) {
                    wattron(win[2], COLOR_PAIR(i + 11));
                    if (!isP2) {
                        mvwprintw(win[2], 3 + i, 39, "%02d", tradingAmt[i].first);
                    }
                    else {
                        mvwprintw(win[2], 3 + i, 39, "%02d", tradingAmt[i].second);
                    }
                    wattron(win[2], COLOR_PAIR(i + 11));
                }

                // Render current trade
                mvwprintw(win[2], 9, 29, "You Will Get:");
                if (isP2) {
                    for (int i = 0; i < 5; i++) {
                        wattron(win[2], COLOR_PAIR(i + 11));
                        mvwprintw(win[2], 10, 26 + i * 4, "x%02d", tradingAmt[i].first);
                        wattroff(win[2], COLOR_PAIR(i + 11));
                    }
                }
                else {
                    for (int i = 0; i < 5; i++) {
                        wattron(win[2], COLOR_PAIR(i + 11));
                        mvwprintw(win[2], 10, 26 + i * 4, "x??");
                        wattroff(win[2], COLOR_PAIR(i + 11));
                    }
                }
                
                int input = wgetch(win[2]);

                // Remove cursor
                for (int i = 0; i < 5; i++) {
                    mvwaddch(win[2], 3 + i, 34, ' ');
                    mvwaddch(win[2], 3 + i, 44, ' ');
                }

                switch (input) {
                    case KEY_UP:
                        location--;

                        if (location < 0) {
                            location = 4;
                        }
                        break;
                    case KEY_DOWN:
                        location++;

                        if (location > 4) {
                            location = 0;
                        }
                        break;
                    case KEY_LEFT:
                        if (!isP2) {
                            tradingAmt[location].first--;

                            if (tradingAmt[location].first < 0) {
                                tradingAmt[location].first = 0;
                            }
                        }
                        else {
                            tradingAmt[location].second--;

                            if (tradingAmt[location].second < 0) {
                                tradingAmt[location].second = 0;
                            }
                        }
                        break;
                    case KEY_RIGHT:
                        if (!isP2) {
                            tradingAmt[location].first++;

                            if (tradingAmt[location].first > Players[Turn].getStat(location + 5)) {
                                tradingAmt[location].first = Players[Turn].getStat(location + 5);
                            }
                        }
                        else {
                            tradingAmt[location].second++;

                            if (tradingAmt[location].second > Players[Location.first].getStat(location + 5)) {
                                tradingAmt[location].second--;
                            }
                        }
                        break;
                    case 'z':
                        if (!isP2 && tradingAmt[0].first + tradingAmt[1].first + tradingAmt[2].first + tradingAmt[3].first + tradingAmt[4].first > 0) {
                            isP2 = true;
                        }
                        else if (isP2 && tradingAmt[0].second + tradingAmt[1].second + tradingAmt[2].second + tradingAmt[3].second + tradingAmt[4].second > 0) {
                            for (int i = 0; i < 5; i++) {
                                // Player 1 -> Player 2
                                Players[Turn].removeResource(i, tradingAmt[i].first);
                                Players[Location.first].addResource(i, tradingAmt[i].first);

                                // Player 2 -> Player 1
                                Players[Location.first].removeResource(i, tradingAmt[i].second);
                                Players[Turn].addResource(i, tradingAmt[i].second);
                            }

                            return true;
                        }
                        break;
                    case 'x':
                        return true;
                        break;
                    case 'q':
                        return false;
                        break;
                }
            }
        }

        // Renders a player's resources in what is normally the dev card window
        void renderResourceCards(int player) {
            std::string resources[5] = {"Clay", "Rock", "Wheat", "Wood", "Wool"};
            int unrenderedResources = 0;

            for (int i = 0; i < 7; i++) {
                for (int j = 0; j < 35; j++) {
                    mvwaddch(win[1], 3 + i, 1 + j, ' ');
                }
            }

            for (int i = 0; i < 5; i++) {
                if (Players[player].getStat(i + 5) > 0) {
                    // Border of each card is rendered with the desert color
                    wattron(win[1], COLOR_PAIR(16));
                    drawBox(win[1], 3, 3 + i * 7 - unrenderedResources * 7, 6, 7);
                    wattron(win[1], COLOR_PAIR(16));

                    // Resource type of each card is rendered with its respective color
                    wattron(win[1], COLOR_PAIR(11 + i));
                    wattron(win[1], A_BOLD);
                    mvwprintw(win[1], 3, 4 + i * 7 - unrenderedResources * 7, "%s", resources[i].c_str());
                    wattroff(win[1], A_BOLD);
                    wattroff(win[1], COLOR_PAIR(11 + i));

                    // The quantity of each resource is displayed beneath each card
                    mvwprintw(win[1], 9, 5 + i * 7 - unrenderedResources * 7, "x%02d", Players[player].getStat(i + 5));
                }
                else {
                    unrenderedResources++;
                }
            }
        }

        // Return false to quit catan or true to keep playing
        bool returnResources(int id) {
            int location = 0, color = 7 + id;
            int maxResources = Players[id].getStat(25) / 2;
            int returningResources[5] = {0, 0, 0, 0, 0};

            std::string playerNames[5] = {"RED", "ORANGE", "BLUE", "WHITE"};
            std::string resources[5] = {"Clay", "Rock", "Wheat", "Wood", "Wool"};

            // Create the basic interface
            wclear(win[1]);
            box(win[1], 0, 0);
            drawHLine(win[1], 11, 0, getmaxx(win[1]));
            drawVLine(win[1], 0, 40, getmaxy(win[1]) - 2);
            mvwprintw(win[1], 1, 3, "YOUR RESOURCE CARDS:");
            mvwprintw(win[1], 1, 43, "RETURNING RESOURCES:");
            customText(win[1], {getmaxy(win[1]) - 2, 1}, {{{0}, "PLAYER "}, {{id + 7, -1}, playerNames[id]}, {{0}, ": RETURN "}, {{RESOURCE_GENERAL, -1}, std::to_string(maxResources)}, {{0}, " CARDS TO THE BANK"}});
            
            // Render resources cards
            renderResourceCards(id);

            // Render Each Counter
            for (int i = 0; i < 5; i++) {
                // Correct spacing
                std::string spaces = "     ";
                if (i == 2) {
                    spaces.erase(0, 1);
                }

                customText(win[1], {3 + i, 44}, {{{11 + i}, resources[i].c_str()}, {{0}, ":"}, {{0}, spaces.c_str()}, {{COLOR_RED}, "- "}, {{11 + i}, "x00"}, {{COLOR_GREEN}, " +"}});
            }
            wrefresh(win[1]);

            while (true) {
                // Render cursor
                wattron(win[1], COLOR_PAIR(color));
                wattron(win[1], A_BOLD);
                mvwaddch(win[1], 3 + location, 52, '<');
                mvwaddch(win[1], 3 + location, 62, '>');
                wattroff(win[1], A_BOLD);
                wattroff(win[1], COLOR_PAIR(color));

                // Update numbers in each counter
                for (int i = 0; i < 5; i++) {
                    wattron(win[1], COLOR_PAIR(i + 11));
                    mvwprintw(win[1], 3 + i, 57, "%02d", returningResources[i]);
                    wattron(win[1], COLOR_PAIR(i + 11));
                }

                // Update total count
                int color = 3;
                if (returningResources[0] + returningResources[1] + returningResources[2] + returningResources[3] + returningResources[4] == maxResources) {
                    color = 2;
                }
                else if (returningResources[0] + returningResources[1] + returningResources[2] + returningResources[3] + returningResources[4] > maxResources) {
                    color = 1;
                }

                mvwprintw(win[1], 9, 44, "Returned: 00 / 00");
                wattron(win[1], COLOR_PAIR(color));
                mvwprintw(win[1], 9, 54, "%02d", returningResources[0] + returningResources[1] + returningResources[2] + returningResources[3] + returningResources[4]);
                mvwprintw(win[1], 9, 59, "%02d", maxResources);
                wattroff(win[1], COLOR_PAIR(color));

                int input = wgetch(win[1]);

                // Remove cursor
                for (int i = 0; i < 5; i++) {
                    mvwaddch(win[1], 3 + i, 52, ' ');
                    mvwaddch(win[1], 3 + i, 62, ' ');
                }

                switch (input) {
                    case KEY_UP:
                        location--;

                        if (location < 0) {
                            location = 4;
                        }
                        break;
                    case KEY_DOWN:
                        location++;

                        if (location > 4) {
                            location = 0;
                        }
                        break;
                    case KEY_LEFT:
                        returningResources[location]--;

                        if (returningResources[location] < 0) {
                            returningResources[location] = 0;
                        }
                        break;
                    case KEY_RIGHT:
                        returningResources[location]++;

                        if (returningResources[location] > Players[id].getStat(location + 5)) {
                            returningResources[location]--;
                        }
                        break;
                    case 'z':
                        if (returningResources[0] + returningResources[1] + returningResources[2] + returningResources[3] + returningResources[4] == maxResources) {
                            for (int i = 0; i < 5; i++) {
                                Players[id].removeResource(i, returningResources[i]);
                            }

                            return true;
                        }
                        break;
                    case 'x':
                        for (int i = 0; i < 5; i++) {
                            returningResources[i] = 0;
                        }
                        break;
                    case 'q':
                        return false;
                        break;
                }
            }
        }

        // Steal a resource from a player
        // Even when using a knight, you can not back out of this
        bool stealResource(std::vector<bool> stealable) {
            std::string playerNames[4] = {"RED", "ORANGE", "BLUE", "WHITE"};
            std::string labels[4] = {"Steal from Red", "Steal from Orange", "Steal from Blue", "Steal from White"};
            std::string resources[5] = {"Clay", "Rock", "Wheat", "Wood", "Wool"};
            int labelColors[4] = {7, 8, 9, 10};
            int location = 0;

            for (int i = 1; i < getmaxy(win[1]) - 1; i++) {
                for (int j = 1; j < getmaxx(win[1]) - 1; j++) {
                    mvwaddch(win[1], i, j, ' ');
                }
            }
            box(win[1], 0, 0);
            drawHLine(win[1], getmaxy(win[1]) - 3, 0, getmaxx(win[1]));
            drawVLine(win[1], 0, 40, getmaxy(win[1]) - 2);
            customText(win[1], {getmaxy(win[1]) - 2, 1}, {{{0}, "PLAYER "}, {{7 + Turn, -1}, playerNames[Turn]}, {{0}, ", CHOOSE A PLAYER TO STEAL A RANDOM RESOURCE FROM"}});
            mvwprintw(win[1], 1, 44, "STEAL FROM:");

            // Render the list of players and discolor the ones that can't be stolen from
            labelColors[Turn] = 16;
            for (int i = 0; i < 4; i++) {
                // If a player doesn't have a building on the new tile or doesn't have any resources, they can't be stolen from
                if (!stealable[i] || Players[i].getStat(25) < 1) {
                    labelColors[i] = 16;
                }

                wattron(win[1], COLOR_PAIR(labelColors[i]));
                mvwprintw(win[1], 3 + i * 2, 44, "%s", labels[i].c_str());
                wattroff(win[1], COLOR_PAIR(labelColors[i]));
            }

            // Quit if none of the players can be stolen from
            if (labelColors[0] == 16 && labelColors[1] == 16 && labelColors[2] == 16 && labelColors[3] == 16) {
                return true;
            }

            // Calculate the amount of players that can be stolen from to be used a bit later
            int eligiblePlayers = 0;
            for (int i = 0; i < PlayerAmt; i++) {
                if (labelColors[i] != 16) {
                    eligiblePlayers++;
                }
            }

            // Automatically move the cursor to the first player that can be stolen from
            for (int i = 0; i < PlayerAmt; i++) {
                if (labelColors[i] != 16) {
                    location = i;
                    break;
                }
            }

            // If only one player can be stolen from, then it is done automatically
            if (eligiblePlayers == 1) {
                int resource = rand() % Players[location].getStat(25);

                std::vector<int> playerResources;

                // Convert the target player's resources into a list that the random number can be used on
                for (int i = 0; i < 5; i++) {
                    for (int j = 0; j < Players[location].getStat(5 + i); j++) {
                        playerResources.emplace_back(i);
                    }
                }

                // Transfer resources
                Players[Turn].addResource(playerResources[resource]);
                Players[location].removeResource(playerResources[resource]);

                return true;
            }

            // Stealing interface
            while (true) {
                // Render the current player's resource cards
                customText(win[1], {1, 3}, {{{0}, "PLAYER "}, {{7 + location, -1}, playerNames[location]}, {{0}, "'S RESOURCE CARDS:   "}});

                int unrenderedResources = 0;
                for (int i = 0; i < 7; i++) {
                    for (int j = 0; j < 35; j++) {
                        mvwaddch(win[1], 3 + i, 1 + j, ' ');
                    }
                }
                for (int i = 0; i < 5; i++) {
                    if (Players[location].getStat(i + 5) > 0) {
                        // Border of each card is rendered with the desert color
                        wattron(win[1], COLOR_PAIR(16));
                        drawBox(win[1], 3, 3 + i * 7 - unrenderedResources * 7, 6, 7);
                        wattron(win[1], COLOR_PAIR(16));

                        // Resource type of each card is rendered with its respective color
                        wattron(win[1], COLOR_PAIR(11 + i));
                        wattron(win[1], A_BOLD);
                        mvwprintw(win[1], 3, 4 + i * 7 - unrenderedResources * 7, "%s", resources[i].c_str());
                        wattroff(win[1], A_BOLD);
                        wattroff(win[1], COLOR_PAIR(11 + i));

                        // The quantity of each resource is displayed beneath each card
                        mvwprintw(win[1], 9, 5 + i * 7 - unrenderedResources * 7, "x%02d", Players[location].getStat(i + 5));
                    }
                    else {
                        unrenderedResources++;
                    }
                }

                // Render cursor
                wattron(win[1], COLOR_PAIR(Turn + 7));
                wattron(win[1], A_BOLD);
                mvwaddch(win[1], 3 + location * 2, 42, '<');
                mvwaddch(win[1], 3 + location * 2, 45 + labels[location].length(), '>');
                wattroff(win[1], A_BOLD);
                wattroff(win[1], COLOR_PAIR(Turn + 7));

                int input = wgetch(win[1]);

                // Remove cursors
                for (int i = 0; i < 4; i++) {
                    mvwaddch(win[1], 3 + i * 2, 42, ' ');
                    mvwaddch(win[1], 3 + i * 2, 45 + labels[i].length(), ' ');
                }

                switch (input) {
                    case KEY_UP:
                        location--;

                        if (location < 0) {
                            location = 3;
                        }
                        break;
                    case KEY_DOWN:
                        location++;

                        if (location > 3) {
                            location = 0;
                        }
                        break;
                    case 'z':
                        if (labelColors[location] != 16) {
                            int resource = rand() % Players[location].getStat(25);

                            std::vector<int> playerResources;

                            // Convert the target player's resources into a list that the random number can be used on
                            for (int i = 0; i < 5; i++) {
                                for (int j = 0; j < Players[location].getStat(5 + i); j++) {
                                    playerResources.emplace_back(i);
                                }
                            }

                            // Transfer resources
                            Players[Turn].addResource(playerResources[resource]);
                            Players[location].removeResource(playerResources[resource]);

                            return true;
                        }
                        break;
                    case 'q':
                        return false;
                        break;
                }
            }
        }

        // Return false to quit catan or true to keep playing
        bool monopoly() {
            int location = 0, color = 7 + Turn;

            std::string playerNames[5] = {"RED", "ORANGE", "BLUE", "WHITE"};
            std::string resources[5] = {"Clay", "Rock", "Wheat", "Wood", "Wool"};

            int availableResources[5] = {0, 0, 0, 0, 0};
            int resourceColors[5] = {11, 12, 13, 14, 15};
            for (int i = 0; i < 5; i++) {
                // Add all players' amount of a resource together to get the total amount
                for (int j = 0; j < PlayerAmt; j++) {
                    availableResources[i] += Players[j].getStat(5 + i);
                }
                // Substract the current player's resources
                availableResources[i] -= Players[Turn].getStat(5 + i);

                // Discolor a resource if there is none of it to monopolize
                if (availableResources[i] < 1) {
                    resourceColors[i] = 16;
                }
            }

            // Quit if none of the resources can be monopolized
            if (resourceColors[0] == 16 && resourceColors[1] == 16 && resourceColors[2] == 16 && resourceColors[3] == 16 && resourceColors[4] == 16) {
                return true;
            }

            // Automatically move the cursor to the first available resource to monopolize
            for (int i = 0; i < 5; i++) {
                if (resourceColors[i] != 16) {
                    location = i;
                    break;
                }
            }

            // Create the basic interface
            wclear(win[1]);
            box(win[1], 0, 0);
            drawHLine(win[1], 11, 0, getmaxx(win[1]));
            drawVLine(win[1], 0, 40, getmaxy(win[1]) - 2);
            mvwprintw(win[1], 1, 3, "YOUR RESOURCE CARDS:");
            mvwprintw(win[1], 1, 43, "CHOOSE A RESOURCE:");
            mvwprintw(win[1], 9, 46, "Receive 00 ?????");
            customText(win[1], {getmaxy(win[1]) - 2, 1}, {{{0}, "PLAYER "}, {{Turn + 7, -1}, playerNames[Turn]}, {{0}, ": CHOOSE A RESOURCE TO MONOPOLIZE"}});
            
            // Render resources cards
            renderResourceCards(Turn);

            // Render the list of resources to monopolize
            for (int i = 0; i < 5; i++) {
                wattron(win[1], COLOR_PAIR(resourceColors[i]));
                mvwprintw(win[1], 3 + i, 46, "Monopolize %s", resources[i].c_str());
                wattroff(win[1], COLOR_PAIR(resourceColors[i]));
            }

            // Monopoly interface
            while (true) {
                // Render cursor
                wattron(win[1], COLOR_PAIR(Turn + 7));
                wattron(win[1], A_BOLD);
                mvwaddch(win[1], 3 + location, 44, '<');
                mvwaddch(win[1], 3 + location, 58 + resources[location].length(), '>');
                wattroff(win[1], A_BOLD);
                wattroff(win[1], COLOR_PAIR(Turn + 7));

                // Update resource gains
                wattron(win[1], COLOR_PAIR(11 + location));
                mvwprintw(win[1], 9, 54, "%02d %s ", availableResources[location], resources[location].c_str());
                wattroff(win[1], COLOR_PAIR(11 + location));

                int input = wgetch(win[1]);

                // Remove cursors
                for (int i = 0; i < 5; i++) {
                    mvwaddch(win[1], 3 + i, 44, ' ');
                    mvwaddch(win[1], 3 + i, 58 + resources[i].length(), ' ');
                }

                switch (input) {
                    case KEY_UP:
                        location--;

                        if (location < 0) {
                            location = 4;
                        }
                        break;
                    case KEY_DOWN:
                        location++;

                        if (location > 4) {
                            location = 0;
                        }
                        break;
                    case 'z':
                        if (resourceColors[location] != 16) {
                            // Remove resources from other players
                            for (int i = 0; i < PlayerAmt; i++) {
                                if (i != Turn) {
                                    Players[i].removeResource(location, Players[i].getStat(5 + location));
                                }
                            }

                            // Add resources to the current player
                            Players[Turn].addResource(location, availableResources[location]);

                            // Remove dev card
                            Players[Turn].useDevCard(1);

                            return true;
                        }
                        break;
                    case 'x':
                        return true;
                        break;
                    case 'q':
                        return false;
                        break;
                }
            }
        }

        // Return false to quit catan or true to keep playing
        bool yearOfPlenty() {
            int location = 0, color = 7 + Turn;
            int returningResources[5] = {0, 0, 0, 0, 0};

            std::string playerNames[5] = {"RED", "ORANGE", "BLUE", "WHITE"};
            std::string resources[5] = {"Clay", "Rock", "Wheat", "Wood", "Wool"};

            // Create the basic interface
            wclear(win[1]);
            box(win[1], 0, 0);
            drawHLine(win[1], 11, 0, getmaxx(win[1]));
            drawVLine(win[1], 0, 40, getmaxy(win[1]) - 2);
            mvwprintw(win[1], 1, 3, "YOUR RESOURCE CARDS:");
            mvwprintw(win[1], 1, 43, "RETURNING RESOURCES:");
            customText(win[1], {getmaxy(win[1]) - 2, 1}, {{{0}, "PLAYER "}, {{Turn + 7, -1}, playerNames[Turn]}, {{0}, ": CHOOSE "}, {{RESOURCE_GENERAL, -1}, "TWO"}, {{0}, " RESOURCES TO GET FROM THE BANK"}});
            
            // Render resources cards
            renderResourceCards(Turn);

            // Render Each Counter
            for (int i = 0; i < 5; i++) {
                // Correct spacing
                std::string spaces = "      ";
                if (i == 2) {
                    spaces.erase(0, 1);
                }

                customText(win[1], {3 + i, 43}, {{{11 + i}, resources[i].c_str()}, {{0}, ":"}, {{0}, spaces.c_str()}, {{COLOR_RED}, "- "}, {{11 + i}, "x00"}, {{COLOR_GREEN}, " +"}});
            }
            wrefresh(win[1]);

            while (true) {
                // Render cursor
                wattron(win[1], COLOR_PAIR(color));
                wattron(win[1], A_BOLD);
                mvwaddch(win[1], 3 + location, 52, '<');
                mvwaddch(win[1], 3 + location, 62, '>');
                wattroff(win[1], A_BOLD);
                wattroff(win[1], COLOR_PAIR(color));

                // Update numbers in each counter
                for (int i = 0; i < 5; i++) {
                    wattron(win[1], COLOR_PAIR(i + 11));
                    mvwprintw(win[1], 3 + i, 57, "%02d", returningResources[i]);
                    wattron(win[1], COLOR_PAIR(i + 11));
                }

                // Update count
                int color = 3;
                if (returningResources[0] + returningResources[1] + returningResources[2] + returningResources[3] + returningResources[4] == 2) {
                    color = 2;
                }
                else if (returningResources[0] + returningResources[1] + returningResources[2] + returningResources[3] + returningResources[4] > 2) {
                    color = 1;
                }
                customText(win[1], {9, 46}, {{{color}, std::to_string(returningResources[0] + returningResources[1] + returningResources[2] + returningResources[3] + returningResources[4])}, {{0}, " / "}, {{color}, "2"}, {{0}, " Resources"}});

                int input = wgetch(win[1]);

                // Remove cursor
                for (int i = 0; i < 5; i++) {
                    mvwaddch(win[1], 3 + i, 52, ' ');
                    mvwaddch(win[1], 3 + i, 62, ' ');
                }

                switch (input) {
                    case KEY_UP:
                        location--;

                        if (location < 0) {
                            location = 4;
                        }
                        break;
                    case KEY_DOWN:
                        location++;

                        if (location > 4) {
                            location = 0;
                        }
                        break;
                    case KEY_LEFT:
                        returningResources[location]--;

                        if (returningResources[location] < 0) {
                            returningResources[location] = 0;
                        }
                        break;
                    case KEY_RIGHT:
                        returningResources[location]++;
                        break;
                    case 'z':
                        if (returningResources[0] + returningResources[1] + returningResources[2] + returningResources[3] + returningResources[4] == 2) {
                            for (int i = 0; i < 5; i++) {
                                Players[Turn].addResource(i, returningResources[i]);
                            }

                            Players[Turn].useDevCard(2);

                            return true;
                        }
                        break;
                    case 'x':
                        return true;
                        break;
                    case 'q':
                        return false;
                        break;
                }
            }
        }

        // Return false to quit catan or true to keep playing
        bool roadBuilding() {
            int keepPlaying = Board.placeRoad(Turn);

            if (keepPlaying == -1) {
                return false;
            }
            else if (keepPlaying == 1) {
                return true;
            }

            do {
                keepPlaying = Board.placeRoad(Turn);
            } while (keepPlaying == 1);

            if (keepPlaying == -1) {
                return false;
            }

            Players[Turn].useDevCard(3);

            return true;
        }

        // Increase turn and roll the dice, automatically collecting resources
        // Return false to quit catan or true to keep playing
        bool advanceTurn() {
            Turn++;
            Turn %= PlayerAmt;

            int diceRoll = 2 + rand() % 6 + rand() % 6;

            if (firstTurn) {
                firstTurn = false;

                while (diceRoll == 7) {
                    diceRoll = 2 + rand() % 6 + rand() % 6;
                }
            }

            renderWindowStats(diceRoll);

            if (diceRoll == 7) {
                render(false);

                for (int i = 0; i < PlayerAmt; i++) {
                    if (Players[i].getStat(25) > 7) {
                        bool keepPlaying = returnResources(i);
                        wclear(win[1]);

                        if (!keepPlaying) {
                            return false;
                        }
                    }
                }

                render(false);
                // Move robber and get a list of players that can be stolen from
                std::vector<bool> stealable = Board.moveRobber(Turn, false);

                if (!stealable[4]) {
                    return false;
                }

                bool keepPlaying = stealResource(stealable);
                wclear(win[1]);

                if (!keepPlaying) {
                    return false;
                }
            }
            else {
                // Figure out how many resources each player should get from the dice roll
                std::vector<std::vector<int>> resources = Board.collectResources(diceRoll);

                // Give the players the resources
                for (int i = 0; i < PlayerAmt; i++) {
                    for (int j = 0; j < 5; j++) {
                        Players[i].addResource(j, resources[i][j]);
                    }
                }
            }

            return true;
        }

        // Place the starting buildings and start play on the correct player's turn
        bool pregame() {
            // Determine the order that the players will place their starting buildings
            int order[4];
            std::vector<int> pool = {0, 1, 2, 3};

            for (int i = 0; i < 4;) {
                // Choose a player (using 64 to increase randomness to a degree)
                int player = rand() % 64;

                // Check to see if the player is still in the pool
                bool valid = false;
                for (int j = 0; j < pool.size(); j++) {
                    if (player / 16 == pool[j]) {
                        pool.erase(pool.begin() + j);
                        valid = true;
                        break;
                    }
                }

                // If the player is in the pool, then they get put into queue
                if (valid) {
                    order[i] = player / 16;
                    i++;
                }
            }

            // First round of placing settlements and roads
            for (int i = 0; i < 4; i++) {
                if (order[i] < PlayerAmt) {
                    int keepPlaying = 0;
                    renderWindowStats(0, true, order[i]);

                    do {
                        keepPlaying = Board.placeBuilding(order[i], false);
                    } while (keepPlaying == 1);

                    if (keepPlaying == -1) {
                        return false;
                    }

                    Players[order[i]].updateTrades(Board.updateHarbors(order[i]));

                    do {
                        keepPlaying = Board.placeRoad(order[i]);
                    } while (keepPlaying == 1);

                    if (keepPlaying == -1) {
                        return false;
                    }
                }
            }

            // Second round of placing settlements and roads
            for (int i = 3; i >= 0; i--) {
                if (order[i] < PlayerAmt) {
                    int keepPlaying = 0;
                    renderWindowStats(0, true, order[i]);

                    do {
                        keepPlaying = Board.placeBuilding(order[i], false);
                    } while (keepPlaying == 1);

                    if (keepPlaying == -1) {
                        return false;
                    }

                    Players[order[i]].updateTrades(Board.updateHarbors(order[i]));

                    do {
                        keepPlaying = Board.placeRoad(order[i]);
                    } while (keepPlaying == 1);

                    if (keepPlaying == -1) {
                        return false;                        
                    }
                }
            }

            clear();

            if (order[0] >= PlayerAmt) {
                Turn = order[1] - 1;
            }
            else {
                Turn = order[0] - 1;
            }
            if (!advanceTurn()) {
                return false;
            }

            for (int i = 0; i < PlayerAmt; i++) {
                Players[i].checkWin();
            }

            return true;
        }

        // Render the large text to announce the winner and also show all players' dev cards
        void renderEndScreen() {
            WINDOW *endScreen = newwin(14, 181, Origin.first + 36, Origin.second);
            wclear(endScreen);
            box(endScreen, 0, 0);

            // Render each player while showing their dev cards
            for (int i = 0; i < PlayerAmt; i++) {
                Players[i].useDevCard(4);
                Players[i].render(true);
            }

            // Render the win screen
            std::string names[5] = {"RED", "ORANGE", "BLUE", "WHITE"};
            largeText(endScreen, {4, 181 / 2 - (names[Turn].length() + 11) * 8 / 2}, names[Turn] + " TEAM WINS!", Turn + 7, true);
            wrefresh(endScreen);
        }

        // Return Values:
        // -1 = Quit, 0 = Nothing, 1 = Next Turn, 2 = New Game
        int submenu(bool endgame = false) {
            int location = 0;
            int color = 7 + Turn;

            while (true) {
                // Render cursor
                wattron(win[3], COLOR_PAIR(color));
                wattron(win[3], A_BOLD);
                mvwaddch(win[3], 2 + location * 3, 3, '/');
                mvwaddch(win[3], 3 + location * 3, 3, '\\');
                mvwaddch(win[3], 2 + location * 3, 11, '\\');
                mvwaddch(win[3], 3 + location * 3, 11, '/');
                wattroff(win[3], A_BOLD);
                wattroff(win[3], COLOR_PAIR(color));

                int input = wgetch(win[3]);

                // Remove cursor
                for (int i = 0; i < 8; i++) {
                    mvwaddch(win[3], 2 + i, 3, ' ');
                    mvwaddch(win[3], 2 + i, 11, ' ');
                }

                switch (input) {
                    case KEY_UP:
                        location--;

                        if (location < 0) {
                            location = 2;
                        }
                        break;
                    case KEY_DOWN:
                        location++;

                        if (location > 2) {
                            location = 0;
                        }
                        break;
                    case 'z':
                        if (location == 0 && !endgame) {
                            return 1;
                        }
                        else if (location == 1) {
                            return 2;
                        }
                        else if (location == 2) {
                            return -1;
                        }
                        break;
                    case 'x':
                        if (!endgame) {
                            return 0;
                        }
                        break;
                    case 'q':
                        return -1;
                        break;
                }
            }
        }

        // Restore the default state of Catan
        void stop() {
            Board.reset();
            for (int i = 0; i < PlayerAmt; i++) {
                Players[i].reset();
            }
            
            for (int i = 0; i < 4; i++) {
                wclear(win[i]);
                wrefresh(win[i]);
                delwin(win[i]);
            }

            clear();
            refresh();

            PlayerAmt = 0;
            Turn = 1;
        }

    public:
        catanGame(int players) {
            PlayerAmt = players;
        }

        // Returns -1 to quit game or a 0 to start a new game
        int start(std::pair<int, int> origin) {           
            if (origin.first < 0) {
                origin.first = 0;
            }
            if (origin.second < 0) {
                origin.second = 0;
            }

            Origin = origin;

            activateColors();
            
            // Setup for each section of the game
            Board = {{Origin.first, Origin.second + 65}};
            switch (PlayerAmt) {
                case 2:
                    Players[0] = {0, 2, {Origin.first, Origin.second}};
                    Players[1] = {1, 2, {Origin.first, Origin.second + 133}};
                    break;
                case 3:
                    Players[0] = {0, 1, {Origin.first, Origin.second}};
                    Players[1] = {1, 1, {Origin.first + 18, Origin.second}};
                    Players[2] = {2, 2, {Origin.first, Origin.second + 133}};
                    break;
                case 4:
                    Players[0] = {0, 1, {Origin.first, Origin.second}};
                    Players[1] = {1, 1, {Origin.first + 18, Origin.second}};
                    Players[2] = {2, 1, {Origin.first, Origin.second + 133}};
                    Players[3] = {3, 1, {Origin.first + 18, Origin.second + 133}};
                    break;
            }
            win[0] = newwin(14, 64, Origin.first + 36, Origin.second);
            win[1] = newwin(14, 67, Origin.first + 36, Origin.second + 65);
            win[2] = newwin(14, 48, Origin.first + 36, Origin.second + 133);
            win[3] = newwin(14, 15, Origin.first + 36, Origin.second + 182);
            keypad(win[0], TRUE);
            keypad(win[1], TRUE);
            keypad(win[2], TRUE);
            keypad(win[3], TRUE);

            // Pregame stuff
            if (!pregame()) {
                stop();
                return -1;
            }

            bool show = false;
            while (true) {
                updateTrades();
                render(show);

                // Check if any players have won
                for (int i = 0; i < PlayerAmt; i++) {
                    if (Players[i].checkWin()) {
                        renderEndScreen();
                        int submenuAction = submenu(true);

                        if (submenuAction == -1) {
                            stop();
                            return -1;
                        }
                        else if (submenuAction == 2) {
                            stop();
                            return 0;
                        }
                    }
                }

                int input = wgetch(win[0]);

                if (input == KEY_UP || input == KEY_DOWN || input == KEY_LEFT || input == KEY_RIGHT) {
                    moveCursor(input, show);
                }
                else if (input == 'z') {
                    // Shopping
                    if (Location.second == 0) {
                        bool purchased = Players[Turn].buyItem(Location.first);
                        
                        if (purchased) {
                            int placeAction = 0;

                            if (Location.first == 0) {
                                placeAction = Board.placeRoad(Turn);

                                if (placeAction == 1) {
                                    Players[Turn].refundItem(0);
                                }
                            }
                            else if (Location.first == 1) {
                                placeAction = Board.placeBuilding(Turn, false);

                                if (placeAction == 1) {
                                    Players[Turn].refundItem(1);
                                }
                            }
                            else if (Location.first == 2) {
                                placeAction = Board.placeBuilding(Turn, true);

                                if (placeAction == 1) {
                                    Players[Turn].refundItem(2);
                                }
                            }

                            if (placeAction == -1) {
                                stop();
                                return -1;
                            }
                            else {
                                // If something is purchased, check for new trades
                                Players[Turn].updateTrades(Board.updateHarbors(Turn));
                            }
                        }
                    }
                    // Dev Cards
                    else if (Location.second == 1) {
                        // Toggle development card visibility
                        if (Location.first == 0) {
                            show = !show;
                        }
                        // Use a knight dev card
                        else if (Location.first == 1 && Players[Turn].getStat(11) > 0) {
                            int beforeResources = Players[Turn].getStat(25);

                            render(show);
                            // Move robber and get a list of players that can be stolen from
                            std::vector<bool> stealable = Board.moveRobber(Turn, true);

                            if (!stealable[4]) {
                                stop();
                                return false;
                            }

                            if (stealable[5] == true) {
                                Players[Turn].useDevCard(0);
                            }

                            bool keepPlaying = stealResource(stealable);
                            wclear(win[1]);

                            if (!keepPlaying) {
                                stop();
                                return false;
                            }
                        }
                        // Use a monopoly dev card
                        else if (Location.first == 2 && Players[Turn].getStat(12) > 0) {
                            bool keepPlaying = monopoly();
                            wclear(win[1]);

                            if (!keepPlaying) {
                                stop();
                                return false;
                            }
                        }
                        // Use a year of plenty dev card
                        else if (Location.first == 3 && Players[Turn].getStat(13) > 0) {
                            bool keepPlaying = yearOfPlenty();
                            wclear(win[1]);

                            if (!keepPlaying) {
                                stop();
                                return false;
                            }
                        }
                        // Use a road building dev card
                        else if (Location.first == 4 && Players[Turn].getStat(14) > 0) {
                            bool keepPlaying = roadBuilding();

                            if (!keepPlaying) {
                                stop();
                                return false;
                            }
                        }
                    }
                    // Bank Trading
                    else if (Location.second == 2) {
                        if (CanTrade[Location.first]) {
                            bool keepPlaying = true;

                            if (Location.first < 2) {
                                keepPlaying = inBankTrade(4 - Location.first);
                            }
                            else {
                                keepPlaying = outBankTrade(2, Location.first - 2);
                            }

                            if (!keepPlaying) {
                                stop();
                                return -1;
                            }

                            // Remove any residual trading ui
                            for (int i = 0; i < 8; i ++) {
                                for (int j = 0; j < 22; j++) {
                                    mvwaddch(win[2], 3 + i, 1 + j, ' ');
                                }
                            }
                        }
                    }
                    // Player Trading
                    else if (Location.second == 3) {
                        if (CanTrade[Location.first + 7]) {
                            bool keepPlaying = playerTrade();

                            if (!keepPlaying) {
                                stop();
                                return -1;
                            }

                            // Remove any residual trading ui
                            for (int i = 0; i < 8; i ++) {
                                for (int j = 0; j < 22; j++) {
                                    mvwaddch(win[2], 3 + i, 24 + j, ' ');
                                }
                            }
                        }
                    }
                }
                else if (input == 'x') {
                    int menuAction = submenu();

                    if (menuAction == -1) {
                        stop();
                        return -1;
                    }
                    // Progress to the next player's turn
                    else if (menuAction == 1) {
                        // Because the dev cards are automatically hidden, this prevents the cursor from being placed on an option that doesnt show up
                        if (Location.second == 1) {
                            Location.first = 0;
                        }

                        show = false;
                        
                        bool keepPlaying = advanceTurn();

                        if (!keepPlaying) {
                            stop();
                            return -1;
                        }
                    }
                    else if (menuAction == 2) {
                        stop();
                        return 0;
                    }
                }
                else if (input == 'q') {
                    stop();
                    return -1;
                }

                // Largest Army
                if (Players[Turn].getStat(15) >= 3 && Turn != Achievers.first) {
                    bool doTransfer = true;

                    if (Achievers.first != -1) {
                        if (Players[Turn].getStat(15) <= Players[Achievers.first].getStat(15)) {
                            doTransfer = false;
                        }
                    }

                    if (doTransfer) {
                        Players[Turn].updateAchievement(0);
                        if (Achievers.first != -1) {
                            Players[Achievers.first].updateAchievement(0);
                        }

                        Achievers.first = Turn;
                    }
                }

                // Longest Road
                if (Players[Turn].getStat(4) >= 5 && Turn != Achievers.second) {
                    bool doTransfer = true;

                    if (Achievers.second != -1) {
                        if (Players[Turn].getStat(4) <= Players[Achievers.second].getStat(4)) {
                            doTransfer = false;
                        }
                    }

                    if (doTransfer) {
                        Players[Turn].updateAchievement(1);
                        if (Achievers.second != -1) {
                            Players[Achievers.second].updateAchievement(1);
                        }

                        Achievers.second = Turn;
                    }
                }

                // Easy resource gathering (for testing purposes)
                switch (input) {
                    case '1':
                        Players[Turn].addResource(0);
                        break;
                    case '2':
                        Players[Turn].addResource(1);
                        break;
                    case '3':
                        Players[Turn].addResource(2);
                        break;
                    case '4':
                        Players[Turn].addResource(3);
                        break;
                    case '5':
                        Players[Turn].addResource(4);
                        break;
                }

                // Move the robber whenever
                if (input == '6') {
                    render(false);
                    // Move robber and get a list of players that can be stolen from
                    std::vector<bool> stealable = Board.moveRobber(Turn, true);

                    if (!stealable[4]) {
                        stop();
                        return false;
                    }

                    bool keepPlaying = stealResource(stealable);
                    wclear(win[1]);

                    if (!keepPlaying) {
                        stop();
                        return false;
                    }
                }

                if (input == '0') {
                    if (Location.second == 1 && Location.first > 0) {
                        Players[Turn].devCardShortcut(Location.first);
                    }
                }

                updateTrades();
                render(show);
            }
        }
};
