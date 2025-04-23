#include "../include/boxes.h"

#include <vector>
#include <ncurses.h>
#include <string>
#include <cmath>
#include <ctime>

#ifndef CATAN_PLAYER
#define CATAN_PLAYER

class catanPlayer {
    private:
        // Which player this is, entirely for display purposes
        int PlayerID = 0;
        int Format = 0;

        WINDOW *win;

        // Pretty much all of the stuff the player can have in Catan
        struct {
            // Excludes victory points from dev cards, those are only taken into account when the winner is calculated
            // This is entirely for display purposes as it is much nicer to add than to subtract
            int victoryPoints = 2;
            // Help calculate victory points and shortcut the longest road
            int settlements = 2, cities = 0, roads = 2;
            
            // How many of each resource the player has
            // IDs:
            // 0 = Clay, 1 = Rock, 2 = Wheat, 3 = Wood, 4 = Wool
            int resources[5] = {0, 0, 0, 0, 0};

            // Dev Card Indexes:
            // 0 = Victory Point, 1 = Knight, 2 = Monopoly, 3 = Road Building, 4 = Year of Plenty
            int devCards[5] = {0, 0, 0, 0, 0};
            int armySize = 0;

            // Unlocked bank trades, updated whenever a new settlement is placed
            // 0 = 4:1, 1 = 3:1, 2 = 2:1 Clay, 3 = 2:1 Rock, 4 = 2:1 Wheat, 5 = 2:1 Wood, 6 = 2:1 Wool
            int trades[7] = {1, 0, 0, 0, 0, 0, 0};

            // Special Card IDs:
            // 0 = Largest Army, 1 = Longest Road
            std::pair<bool, bool> specialCards = {false, false};
        } Stats;

        // Render the resource cards
        void renderResources() {
            int unrenderedResources = 0, spacing = 7;
            std::string resourceNames[5] = {"Clay", "Rock", "Wheat", "Wood", "Wool"};

            if (Format == 2) {
                spacing = 8;
            }

            for (int i = 0; i < 7; i++) {
                for (int j = 0; j < 40; j++) {
                    mvwaddch(win, 2 + i, 1 + j, ' ');
                }
            }

            for (int i = 0; i < 5; i++) {
                if (Stats.resources[i] > 0) {
                    // Border of each card is rendered with the desert color
                    wattron(win, COLOR_PAIR(16));
                    drawBox(win, 2, 1 + i * spacing - unrenderedResources * spacing, 6, 7);
                    wattron(win, COLOR_PAIR(16));

                    // Resource type of each card is rendered with its respective color
                    wattron(win, COLOR_PAIR(11 + i));
                    wattron(win, A_BOLD);
                    mvwprintw(win, 2, 2 + i * spacing - unrenderedResources * spacing, "%s", resourceNames[i].c_str());
                    mvwprintw(win, 8, 3 + i * spacing - unrenderedResources * spacing, "x%02d", Stats.resources[i]);
                    wattroff(win, A_BOLD);
                    wattroff(win, COLOR_PAIR(11 + i));
                }
                else {
                    unrenderedResources++;
                }
            }
        }

        // Render the dev cards
        void renderDevCards(bool show = false) {
            int unrenderedDevCards = 0, cardLocation = 0, spacing = 7, setback = 0;
            std::string devCardNames[5] = {"VP", "Kn", "Mono", "YoP", "RB"};
            std::pair<int, int> origin = {2, 40};
            
            if (Format == 2) {
                origin = {12, 1};
                spacing = 8;
            }

            // Hide cards from view
            int lenY = 15, lenX = 21, offsetY = 2, offsetX = 40;
            if (Format == 2) {
                lenY = 7;
                lenX = 48;
                offsetY = 12;
                offsetX = 1;
            }
            for (int i = 0; i < lenY; i++) {
                for (int j = 0; j < lenX; j++) {
                    mvwaddch(win, offsetY + i, offsetX + j, ' ');
                }
            }

            if (Stats.armySize > 0) {
                wattron(win, COLOR_PAIR(16));
                drawBox(win, origin.first, origin.second, 6, 7);
                wattroff(win, COLOR_PAIR(16));

                wattron(win, COLOR_PAIR(23));
                wattron(win, A_BOLD);
                mvwprintw(win, origin.first, origin.second + 1, "Army");
                mvwprintw(win, origin.first + 6, origin.second + 2, "%03d", Stats.armySize);
                wattroff(win, A_BOLD);
                wattroff(win, COLOR_PAIR(23));

                cardLocation++;
                origin.second += spacing;
            }

            if (show) {
                for (int i = 0; i < 5; i++) {
                    if (Stats.devCards[i] > 0) {
                        if (Format == 1 && cardLocation > 2) {
                            origin.first = 10;
                            setback = 21;
                        }

                        // Card outline
                        wattron(win, COLOR_PAIR(16));
                        drawBox(win, origin.first, origin.second + i * spacing - unrenderedDevCards * spacing - setback, 6, 7);
                        wattron(win, COLOR_PAIR(16));

                        // Figure out the color of each dev card
                        int devCardColor = 24;
                        if (i == 1) {
                            devCardColor = 25;
                        }
                        else if (i > 1) {
                            devCardColor = 26;
                        }

                        // Dev Card type of each card is rendered with its respective color
                        wattron(win, COLOR_PAIR(devCardColor));
                        wattron(win, A_BOLD);
                        mvwprintw(win, origin.first, origin.second + 1 + i * spacing - unrenderedDevCards * spacing - setback, "%s", devCardNames[i].c_str());
                        mvwprintw(win, origin.first + 6, origin.second + 2 + i * spacing - unrenderedDevCards * spacing - setback, "x%02d", Stats.devCards[i]);
                        wattroff(win, A_BOLD);
                        wattroff(win, COLOR_PAIR(devCardColor));
                        cardLocation++;
                    }
                    else {
                        unrenderedDevCards++;
                    }
                }
            }
            else {
                // Render the hidden cards
                int devCardAmt = Stats.devCards[0] + Stats.devCards[1] + Stats.devCards[2] + Stats.devCards[3] + Stats.devCards[4];
                if (devCardAmt > 0) {
                    wattron(win, COLOR_PAIR(16));
                    drawBox(win, origin.first, origin.second, 6, 7);
                    wattroff(win, COLOR_PAIR(16));

                    wattron(win, COLOR_PAIR(23));
                    wattron(win, A_BOLD);
                    mvwprintw(win, origin.first, origin.second + 1, "???");
                    mvwprintw(win, origin.first + 6, origin.second + 2, "x%02d", devCardAmt);
                    wattroff(win, A_BOLD);
                    wattroff(win, COLOR_PAIR(23));
                }
            }
        }

        // Building Count Statistics
        void renderBuildingCount() {
            // Calculate the amount of harbors the player has access to
            int harborCount = 0;
            for (int i = 1; i < 7; i++) {
                if (Stats.trades[i] > 0) {
                    harborCount += Stats.trades[i];
                }
            }
            
            int teamColor = 7;
            switch (PlayerID) {
                case 1:
                    teamColor = 8;
                    break;
                case 2:
                    teamColor = 9;
                    break;
                case 3:
                    teamColor = 10;
                    break;
            }
            
            int multiplier = 1, offset = 11;
            std::string spaces = "  ";
            if (Format == 2) {
                offset = 23;
                multiplier = 2;
                spaces = "         ";
            }

            // Actual counts
            wattron(win, COLOR_PAIR(teamColor));
            mvwprintw(win, offset, 1,                  "Roads:      %s%02d/15", spaces.c_str(), Stats.roads);
            mvwprintw(win, offset + 1 * multiplier, 1, "Settlements:%s%02d/05", spaces.c_str(), Stats.settlements);
            mvwprintw(win, offset + 2 * multiplier, 1, "Cities:     %s%02d/04", spaces.c_str(), Stats.cities);
            mvwprintw(win, offset + 3 * multiplier, 1, "Harbors:    %s%02d/09", spaces.c_str(), harborCount);
            wattroff(win, COLOR_PAIR(teamColor));

            // Extra Characters
            mvwaddch(win, offset, 6, ':');
            mvwaddch(win, offset + 1 * multiplier, 12, ':');
            mvwaddch(win, offset + 2 * multiplier, 7, ':');
            mvwaddch(win, offset + 3 * multiplier, 8, ':');
            mvwaddch(win, offset, 15 + spaces.length(), '/');
            mvwaddch(win, offset + 1 * multiplier, 15 + spaces.length(), '/');
            mvwaddch(win, offset + 2 * multiplier, 15 + spaces.length(), '/');
            mvwaddch(win, offset + 3 * multiplier, 15 + spaces.length(), '/');
        }

        // Amount of victory points a player has
        void renderVictoryPoints() {
            wattron(win, COLOR_PAIR(24));
            wattron(win, A_BOLD);
            if (Format == 1) {
                mvwprintw(win, getmaxy(win) - 2, 21, "VICTORY PTS: %02d/10", Stats.victoryPoints);
                wattroff(win, A_BOLD);
                wattroff(win, COLOR_PAIR(24));
                mvwaddch(win, getmaxy(win) - 2, 32, ':');
                mvwaddch(win, getmaxy(win) - 2, 36, '/');
            }
            else {
                mvwprintw(win, getmaxy(win) - 2, 28, "VICTORY POINTS: %02d/10", Stats.victoryPoints);
                wattroff(win, A_BOLD);
                wattroff(win, COLOR_PAIR(24));
                mvwaddch(win, getmaxy(win) - 2, 42, ':');
                mvwaddch(win, getmaxy(win) - 2, 46, '/');
            }
        }

        // Team Name and Number
        void renderTeamName() {
            // Player-specific customization
            std::string teamName = "RED";
            int teamColor = 7;
            switch (PlayerID) {
                case 1:
                    teamName = "ORANGE";
                    teamColor = 8;
                    break;
                case 2:
                    teamName = "BLUE";
                    teamColor = 9;
                    break;
                case 3:
                    teamName = "WHITE";
                    teamColor = 10;
                    break;
            }

            wattron(win, COLOR_PAIR(teamColor));
            wattron(win, A_BOLD);
            mvwprintw(win, getmaxy(win) - 2, 1, "TEAM %s", teamName.c_str());
            if (Format == 1) {
                mvwprintw(win, getmaxy(win) - 2, 7 + teamName.length(), "- P%d", PlayerID + 1);
            }
            else {
                mvwprintw(win, getmaxy(win) - 2, 7 + teamName.length(), "- Player %d", PlayerID + 1);
            }
            wattroff(win, A_BOLD);
            wattroff(win, COLOR_PAIR(teamColor));
            mvwaddch(win, getmaxy(win) - 2, 7 + teamName.length(), '-');
        }

        // The borders and headers that indicate what each section displays
        void renderBox() {
            // Section Headers
            mvwprintw(win, 1, 1, "RESOURCE CARDS:");
            if (Format == 1) {
                mvwprintw(win, 1, 40, "DEVELOPMENT CARDS:");
                mvwprintw(win, 10, 1, "PLACED BUILDINGS:");
                mvwprintw(win, 10, 21, "SPECIAL CARDS:");
            }
            else {
                mvwprintw(win, 11, 1, "DEVELOPMENT CARDS:");
                mvwprintw(win, 21, 1, "PLACED BUILDINGS:");
                mvwprintw(win, 21, 28, "SPECIAL CARDS:");
            }

            // Render the various borders
            if (Format == 1) {
                drawVLine(win, 0, 39, 18);
                drawHLine(win, 9, 0, 40);
                drawHLine(win, 15, 0, 40);
                drawVLine(win, 9, 20, 9);

                mvwaddch(win, 15, 20, ACS_PLUS);
            }
            else {
                drawHLine(win, 10, 0, 64);
                drawHLine(win, 20, 0, 64);
                drawHLine(win, 33, 0, 64);
                drawVLine(win, 20, 27, 16);

                mvwaddch(win, 33, 27, ACS_PLUS);
            }
        }

        // The two cards you get for either largest army or longest road
        void renderSpecialCards() {
            int unrenderedCards = 1, nameOffset = 0;
            std::string cardNames[4] = {"LA", "LR", "Largest Army", "Longest Road"};
            std::pair<int, int> origin = {11, 21}, dims = {4, 5};

            if (Format == 2) {
                origin = {22, 28};
                dims = {9, 15};
                nameOffset = 2;
            }

            // Clear the cards
            for (int i = 0; i < dims.first; i++) {
                for (int j = 0; j < dims.second * 2 + 2; j++) {
                    mvwaddch(win, origin.first + i, origin.second + j, ' ');
                }
            }

            // Render Largest Army
            if (Stats.specialCards.first) {
                wattron(win, COLOR_PAIR(16));
                drawBox(win, origin.first, origin.second, dims.first, dims.second);
                wattroff(win, COLOR_PAIR(16));

                wattron(win, COLOR_PAIR(23));
                mvwprintw(win, origin.first, origin.second + 1, "%s", cardNames[nameOffset].c_str());
                wattroff(win, COLOR_PAIR(23));
            }
            else {
                unrenderedCards--;
            }

            // Render Longest Road
            if (Stats.specialCards.second) {
                wattron(win, COLOR_PAIR(16));
                drawBox(win, origin.first, origin.second + (dims.second + 2) * unrenderedCards, dims.first, dims.second);
                wattroff(win, COLOR_PAIR(16));

                wattron(win, COLOR_PAIR(23));
                mvwprintw(win, origin.first, origin.second + 1 + (dims.second + 2) * unrenderedCards, "%s", cardNames[1 + nameOffset].c_str());
                wattroff(win, COLOR_PAIR(23));
            }
        }

    public:
        catanPlayer(int playerID, int format, std::pair<int, int> winOrigin = {0, 0}) {
            PlayerID = playerID;
            Format = format;

            if (format == 1) {
                win = newwin(18, 64, winOrigin.first, winOrigin.second);
            }
            else {
                win = newwin(36, 64, winOrigin.first, winOrigin.second);
            }
        }

        // All-in-one combo of the other rendering functions
        void render(bool show = false) {
            renderTeamName();
            renderVictoryPoints();
            renderBuildingCount();
            renderResources();
            renderDevCards(show);
            renderSpecialCards();

            box(win, 0, 0);
            renderBox();
            wrefresh(win);
        }

        // Add an amount of resources to someone
        // resourceID: 0 = Clay, 1 = Rock, 2 = Wheat, 3 = Wood, 4 = Wool
        void addResource(int resourceID, int amount = 1) {
            Stats.resources[resourceID] += amount;
        }

        // Remove an amount of resources to someone
        // resourceID: 0 = Clay, 1 = Rock, 2 = Wheat, 3 = Wood, 4 = Wool
        void removeResource(int resourceID, int amount = 1) {
            Stats.resources[resourceID] -= amount;
        }

        // Basically allows the reading of a player variable outside of the class
        int getStat(int id) {
            switch (id) {
                case 0:
                    return PlayerID;
                    break;
                case 1:
                    return Stats.victoryPoints;
                    break;
                case 2:
                    return Stats.settlements;
                    break;
                case 3:
                    return Stats.cities;
                    break;
                case 4:
                    return Stats.roads;
                    break;
                case 5:
                case 6:
                case 7:
                case 8:
                case 9:
                    return Stats.resources[id - 5];
                    break;
                case 10:
                case 11:
                case 12:
                case 13:
                case 14:
                    return Stats.devCards[id - 10];
                    break;
                case 15:
                    return Stats.armySize;
                    break;
                case 16:
                case 17:
                case 18:
                case 19:
                case 20:
                case 21:
                case 22:
                    return Stats.trades[id - 16];
                    break;
                case 23:
                    if (Stats.specialCards.first) {
                        return 1;
                    }
                    return 0;
                    break;
                case 24:
                    if (Stats.specialCards.second) {
                        return 1;
                    }
                    return 0;
                    break;
                case 25:
                    return Stats.resources[0] + Stats.resources[1] + Stats.resources[2] + Stats.resources[3] + Stats.resources[4];
                    break;
                default:
                    return -32767;
                    break;
            }
        }

        // itemIDs:
        // 0 = Road, 1 = Settlements, 2 = City, 3 = Development Card
        // Returns true if an item is bought and false if no item is bought
        bool buyItem(int itemID) {
            switch (itemID) {
                case 0:
                    if (Stats.resources[0] >= 1 && Stats.resources[3] >= 1 && Stats.roads < 15) {
                        Stats.resources[0]--;
                        Stats.resources[3]--;

                        Stats.roads++;
                        return true;
                    }
                    break;
                case 1:
                    if (Stats.resources[0] >= 1 && Stats.resources[2] >= 1 && Stats.resources[3] >= 1 && Stats.resources[4] >= 1 && Stats.settlements < 5) {
                        Stats.resources[0]--;
                        Stats.resources[2]--;
                        Stats.resources[3]--;
                        Stats.resources[4]--;

                        Stats.settlements++;
                        Stats.victoryPoints++;
                        return true;
                    }
                    break;
                case 2:
                    if (Stats.resources[1] >= 2 && Stats.resources[2] >= 3 && Stats.cities < 4 && Stats.settlements > 0) {
                        Stats.resources[1] -= 2;
                        Stats.resources[2] -= 3;

                        Stats.cities++;
                        Stats.settlements--;
                        Stats.victoryPoints++;
                        return true;
                    }
                    break;
                case 3:
                    if (Stats.resources[1] >= 1 && Stats.resources[2] >= 1 && Stats.resources[4] >= 1) {
                        Stats.resources[1]--;
                        Stats.resources[2]--;
                        Stats.resources[4]--;

                        int card = rand() % 25 + 1;
                        if (card <= 5) {
                            Stats.devCards[0]++;
                        }
                        else if (card <= 19) {
                            Stats.devCards[1]++;
                        }
                        else if (card <= 21) {
                            Stats.devCards[2]++;
                        }
                        else if (card <= 23) {
                            Stats.devCards[3]++;
                        }
                        else {
                            Stats.devCards[4]++;
                        }
                        return true;
                    }
                    break;
            }

            return false;
        }

        // itemIDs:
        // 0 = Road, 1 = Settlements, 2 = City
        // Dev Cards can not be refunded
        void refundItem(int itemID) {
            switch (itemID) {
                case 0:
                    Stats.resources[0]++;
                    Stats.resources[3]++;

                    Stats.roads--;
                    break;
                case 1:
                    Stats.resources[0]++;
                    Stats.resources[2]++;
                    Stats.resources[3]++;
                    Stats.resources[4]++;

                    Stats.settlements--;
                    Stats.victoryPoints--;
                    break;
                case 2:
                    Stats.resources[1] += 2;
                    Stats.resources[2] += 3;

                    Stats.cities--;
                    Stats.settlements++;
                    Stats.victoryPoints--;
                    break;
            }
        }

        // Return true if the player has enough victory points to win
        bool checkWin() {
            if (Stats.victoryPoints + Stats.devCards[0] >= 10) {
                return true;
            }

            return false;
        }

        // id: 0 = Knight, 1 = Monopoly, 2 = Year of Plenty, 3 = Road Building
        void useDevCard(int id) {
            if (id < 4) {
                Stats.devCards[id + 1]--;

                if (id == 0) {
                    Stats.armySize++;
                }
                else if (id == 3) {
                    Stats.roads += 2;
                }
            }
            else {
                Stats.victoryPoints += Stats.devCards[0];
            }
        }

        // Update any newly unlocked trades due to harbors
        void updateTrades(std::vector<int> trades) {
            for (int i = 1; i < 7; i++) {
                Stats.trades[i] = trades[i - 1];
            }
        }

        // Easier dev card testing
        void devCardShortcut(int id) {
            Stats.devCards[id]++;
        }

        // Flip the state of either achievement
        void updateAchievement(int id) {
            if (id == 0) {
                Stats.specialCards.first = !Stats.specialCards.first;

                if (Stats.specialCards.first) {
                    Stats.victoryPoints += 2;
                }
                else {
                    Stats.victoryPoints -= 2;
                }
            }
            else {
                Stats.specialCards.second = !Stats.specialCards.second;

                if (Stats.specialCards.second) {
                    Stats.victoryPoints += 2;
                }
                else {
                    Stats.victoryPoints -= 2;
                }
            }
        }

        void reset() {
            bool trades[7] = {true, true, false, false, false, false, false};

            Stats.victoryPoints = 0;
            Stats.settlements = 0;
            Stats.cities = 0;
            Stats.roads = 0;
            Stats.armySize = 0;
            
            for (int i = 0; i < 5; i++) {
                Stats.resources[i] = 0;
                Stats.devCards[i] = 0;
            }

            for (int i = 0; i < 7; i++) {
                Stats.trades[i] = trades[i];
            }

            // Delete window
            wclear(win);
            wrefresh(win);
            delwin(win);
        }
};

#endif
