#include <ncurses.h>
#include <utility>
#include <locale.h>
#include <string>
#include <cmath>
#include <vector>

class catanBoard {
    private:
        std::pair<int, int> Origin = {0, 65};
        std::pair<int, int> Location;

        WINDOW *win;

        // Stores the locations of game pieces on the board
        struct {
            // Building IDs:
            // -1 = Unused Spot, 0 = No Building
            // 1 = Red Settlement, 2 = Red City, 3 = Orange Settlement, 4 = Orange City
            // 5 = Blue Settlement, 6 = Blue City, 7 = White Settlement, 8 = White City
            int buildings[11][6] = {
                {
                    -1, -1, 0, 0, -1, -1
                },
                {
                    -1, 0, 0, 0, 0, -1
                },
                {
                    0, 0, 0, 0, 0, 0
                },
                {
                    0, 0, 0, 0, 0, 0
                },
                {
                    0, 0, 0, 0, 0, 0
                },
                {
                    0, 0, 0, 0, 0, 0
                },
                {
                    0, 0, 0, 0, 0, 0
                },
                {
                    0, 0, 0, 0, 0, 0
                },
                {
                    0, 0, 0, 0, 0, 0
                },
                {
                    -1, 0, 0, 0, 0, -1
                },
                {
                    -1, -1, 0, 0, -1, -1
                }
            };

            // Road IDs:
            // -1 = Unused Spot, 0 = No Road
            // 1 = Red Road, 2 = Orange Road, 3 = Blue Road, 4 = White Road
            int roads[21][11] = {
                {
                    -1, -1, -1, -1, -1, 0, -1, -1, -1, -1, -1
                },
                {
                    -1, -1, -1, -1, 0, -1, 0, -1, -1, -1, -1
                },
                {
                    -1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1
                },
                {
                    -1, -1, 0, -1, 0, -1, 0, -1, 0, -1, -1
                },
                {
                    -1, 0, -1, -1, -1, 0, -1, -1, -1, 0, -1
                },
                {
                    0, -1, 0, -1, 0, -1, 0, -1, 0, -1, 0
                },
                {
                    -1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1
                },
                {
                    0, -1, 0, -1, 0, -1, 0, -1, 0, -1, 0
                },
                {
                    -1, 0, -1, -1, -1, 0, -1, -1, -1, 0, -1
                },
                {
                    0, -1, 0, -1, 0, -1, 0, -1, 0, -1, 0
                },
                {
                    -1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1
                },
                {
                    0, -1, 0, -1, 0, -1, 0, -1, 0, -1, 0
                },
                {
                    -1, 0, -1, -1, -1, 0, -1, -1, -1, 0, -1
                },
                {
                    0, -1, 0, -1, 0, -1, 0, -1, 0, -1, 0
                },
                {
                    -1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1
                },
                {
                    0, -1, 0, -1, 0, -1, 0, -1, 0, -1, 0
                },
                {
                    -1, 0, -1, -1, -1, 0, -1, -1, -1, 0, -1
                },
                {
                    -1, -1, 0, -1, 0, -1, 0, -1, 0, -1, -1
                },
                {
                    -1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1
                },
                {
                    -1, -1, -1, -1, 0, -1, 0, -1, -1, -1, -1
                },
                {
                    -1, -1, -1, -1, -1, 0, -1, -1, -1, -1, -1
                }
            };

            // Hardcoded values match Catan's recommended board
            // Tile IDs:
            // -1 = Unused spot, 0 = Clay, 1 = Rock, 2 = Wheat, 3 = Wood, 4 = Wool, 5 = Desert
            int tiles[5][5] = {
                {
                    -1, 4, 3, 0, -1
                },
                {
                    1, 0, 4, 3, 1
                },
                {
                    2, 3, 5, 2, 4
                },
                {
                    2, 3, 1, 2, 4
                },
                {
                    -1, -1, 0, -1, -1
                }
            };

            // Hardcoded values match Catan's recommended board
            // -1 = Unused spot, all other values match actual numbers generated from 2d6
            int tileRarity[5][5] = {
                {
                    -1, 2, 9, 10, -1
                },
                {
                    10, 6, 4, 3, 8
                },
                {
                    12, 11, -1, 4, 5
                },
                {
                    9, 8, 3, 6, 11
                },
                {
                    -1, -1, 5, -1, -1
                }
            };
        
            // The tile that the robber is currently on
            std::pair<int, int> robber = {2, 2};
        } Markers;

        // Output the player color of a building at an inputted location
        int getBuildingColor(std::pair<int, int> location) {
            int ID = Markers.buildings[location.first][location.second];

            switch (ID) {
                case 0:
                    return 16;
                    break;
                case 1:
                case 2:
                    return 7;
                    break;
                case 3:
                case 4:
                    return 8;
                    break;
                case 5:
                case 6:
                    return 9;
                    break;
                case 7:
                case 8:
                    return 10;
                    break;
            }

            return 0;
        }
        
        // Resource tiles and the numbers each tile is marked with
        void renderTiles() {
            // The "art" used to make each tile
            char tileTemplates[6][5][9] = {
                {
                    {
                        ' ', ' ', ' ', '-', ' ', '_', ' ', 'n', ' '
                    },
                    {
                        'm', 'm', 'n', ' ', ' ', '_', '_', ' ', ' '
                    },
                    {
                        '_', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'm'
                    },
                    {
                        ' ', 'm', 'n', ' ', ' ', 'm', ' ', '_', ' '
                    },
                    {
                        ' ', 'n', ' ', ' ', '_', '_', '_', 'm', ' '
                    }
                },
                {
                    {
                        ' ', '/', '\\', ' ', '_', '/', '\\', ' ', ' '
                    },
                    {
                        '/', '^', ' ', '\\', ' ', ' ', '_', '^', ' '
                    },
                    {
                        '^', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '\\'
                    },
                    {
                        ' ', '^', '/', '\\', ' ', '/', '\\', ' ', '_'
                    },
                    {
                        ' ', '/', ' ', ' ', '/', ' ', '^', '\\', ' '
                    }
                },
                {
                    {
                        ' ', 'v', 'w', 'v', 'v', 'w', 'w', 'v', ' '
                    },
                    {
                        'v', 'w', '^', 'w', 'v', 'v', 'w', 'v', 'w'
                    },
                    {
                        'w', 'v', ' ', ' ', ' ', ' ', ' ', 'v', 'w'
                    },
                    {
                        'v', 'v', '^', 'w', 'v', 'v', '^', '^', 'w'
                    },
                    {
                        ' ', 'w', 'w', 'v', 'v', '^', 'w', 'w', ' '
                    }
                },
                {
                    {
                        ' ', '^', '^', ' ', '^', ' ', '|', '^', ' '
                    },
                    {
                        '^', '|', ' ', '^', ' ', '^', ' ', '^', '^'
                    },
                    {
                        '^', '^', ' ', ' ', ' ', ' ', ' ', '^', '|'
                    },
                    {
                        '^', '|', ' ', '^', ' ', '^', '^', ' ', '^'
                    },
                    {
                        ' ', '^', ' ', '^', '^', ' ', '|', '^', ' '
                    }
                },
                {
                    {
                        ' ', ' ', 'v', 'v', ' ', '.', '0', ' ', ' '
                    },
                    {
                        ' ', 'O', 'o', ' ', ' ', 'o', '.', ' ', 'v'
                    },
                    {
                        'v', ' ', ' ', ' ', ' ', ' ', ' ', '.', 'o'
                    },
                    {
                        ' ', ' ', 'o', '.', ' ', ' ', 'v', 'v', ' '
                    },
                    {
                        ' ', 'o', '.', ' ', ' ', 'o', 'O', ' ', ' '
                    }
                },
                {
                    {
                        ' ', ' ', '~', '~', ' ', ' ', ' ', ' ', ' '
                    },
                    {
                        ' ', ' ', ' ', ' ', ' ', ' ', '~', '~', ' '
                    },
                    {
                        '~', '~', ' ', ' ', ' ', ' ', ' ', '~', '~'
                    },
                    {
                        ' ', ' ', ' ', ' ', ' ', '~', '~', ' ', ' '
                    },
                    {
                        ' ', ' ', '~', '~', ' ', ' ', ' ', ' ', ' '
                    }
                }
            };
            
            int tilePosY = 0, tilePosX = 0, numPosY = 0, numPosX = 0;
            
            for (int i = 0; i < 5; i++) {
                for (int j = 0; j < 5; j++) {
                    if (Markers.tiles[i][j] != -1) {
                        // Find the positions that each tile should go in
                        if (j % 2 == 0) {
                            tilePosY = 1 + i * 6;
                            numPosY = 3 + i * 6;
                        }
                        else {
                            tilePosY = 4 + i * 6;
                            numPosY = 6 + i * 6;
                        }

                        numPosX = 8 + j * 11;
                        
                        // Render the bg for each tile
                        for (int k = 0; k < 5; k++) {
                            tilePosX = 5 + j * 11;

                            for (int l = 0; l < 9; l++) {
                                int tileColor = 16;
                                if (Markers.tiles[i][j] != 5) {
                                    tileColor = Markers.tiles[i][j] + 11;
                                }

                                wattron(win, COLOR_PAIR(tileColor));
                                mvwaddch(win, Origin.first + tilePosY, Origin.second + tilePosX, tileTemplates[Markers.tiles[i][j]][k][l]);
                                wattroff(win, COLOR_PAIR(tileColor));
                                
                                tilePosX += 1;
                            }
                            tilePosY += 1;
                        }

                        // Add the robber indicator
                        if (i == Markers.robber.first && j == Markers.robber.second) {
                            wattron(win, COLOR_PAIR(27));
                            wattron(win, A_BOLD);
                            mvwprintw(win, Origin.first + numPosY, Origin.second + numPosX, "ROB");
                            wattroff(win, A_BOLD);
                            wattroff(win, COLOR_PAIR(27));
                        }
                        else {
                            // Render the number for each tile
                            if (Markers.tiles[i][j] != 5) {
                                int numberColor = 0;
                                switch (Markers.tileRarity[i][j]) {
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
                                }

                                wattron(win, COLOR_PAIR(numberColor));
                                wattron(win, A_BOLD);
                                mvwprintw(win, Origin.first + numPosY, Origin.second + numPosX, "%03d", Markers.tileRarity[i][j]);
                                wattroff(win, A_BOLD);
                                wattroff(win, COLOR_PAIR(numberColor));
                            }
                        }
                    }
                }
            }
        }

        // The lines surrounding each tile as well as a color if a player has a road on one
        void renderRoads() {
            int posY = 0, posX = 0, roadType = 0;

            for (int i = 0; i < 21; i++) {
                for (int j = 0; j < 11; j++) {
                    if (Markers.roads[i][j] != -1) {
                        // Horizontal roads 
                        if (i % 2 == 0) {
                            posY = (i / 2) * 3;
                            posX = 6 + (j / 2) * 11;
                            roadType = 0;
                        }
                        // "Vertical" roads
                        else {
                            posY = 1 + (i / 2) * 3;
                            posX = 3 + (j / 2) * 11;
                            if ((i / 2) % 2 == 0) {
                                roadType = 2;
                            }
                            else {
                                roadType = 1;
                            }

                            if ((j / 2) % 2 == 0) {
                                if (roadType == 1) {
                                    roadType = 2;
                                }
                                else {
                                    roadType = 1;
                                }
                            }
                        }

                        // Render the road with the correct color
                        int roadColor = 16;
                        if (Markers.roads[i][j] != 0) {
                            roadColor = Markers.roads[i][j] + 6;
                        }

                        wattron(win, COLOR_PAIR(roadColor));
                        wattron(win, A_BOLD);
                        if (roadType == 0) {
                            mvwprintw(win, Origin.first + posY, Origin.second + posX, "-------");
                        }
                        else if (roadType == 1) {
                            mvwaddch(win, Origin.first + posY, Origin.second + posX + 1, '/');
                            mvwaddch(win, Origin.first + posY + 1, Origin.second + posX, '/');
                        }
                        else if (roadType == 2) {
                            mvwaddch(win, Origin.first + posY, Origin.second + posX, '\\');
                            mvwaddch(win, Origin.first + posY + 1, Origin.second + posX + 1, '\\');
                        }
                        wattroff(win, A_BOLD);
                        wattroff(win, COLOR_PAIR(roadColor));
                    }
                }
            }
        }

        // The vertecies of each tile as well as a colored 'O' for a settlement and a diamond for a city if placed there
        void renderBuildings() {
            int posY = 0, posX = 0;
            std::string buildingType;

            for (int i = 0; i < 11; i++) {
                for (int j = 0; j < 6; j++) {
                    if (Markers.buildings[i][j] != -1) {
                        posY = i * 3;
                        
                        if (i % 2 == 0) {
                            if (j % 2 == 0) {
                                posX = 5 + (j / 2) * 22;
                            }
                            else {
                                posX = 13 + (j / 2) * 22;
                            }
                        }
                        else {
                            if (j % 2 == 0) {
                                posX = 2 + (j / 2) * 22;
                            }
                            else {
                                posX = 16 + (j / 2) * 22;
                            }
                        }

                        if (Markers.buildings[i][j] == 0) {
                            if (i == 0 || Markers.buildings[i - 1][j] == -1) {
                                buildingType = ".";
                            }
                            else if (i == 10 || Markers.buildings[i + 1][j] == -1) {
                                buildingType = "\'";
                            }
                            else if ((i % 2 != 0 && j % 2 == 0) || (i % 2 == 0 && j % 2 != 0)) {
                                buildingType = "(";
                            }
                            else if ((i % 2 == 0 && j % 2 == 0) || (i % 2 != 0 && j % 2 != 0)) {
                                buildingType = ")";
                            }
                        }
                        else if (Markers.buildings[i][j] % 2 != 0) {
                            buildingType = "⏏";
                        }
                        else {
                            buildingType = "☗";
                        }

                        wattron(win, COLOR_PAIR(getBuildingColor({i, j})));
                        wattron(win, A_BOLD);
                        mvwprintw(win, Origin.first + posY, Origin.second + posX, "%s", buildingType.c_str());
                        wattroff(win, A_BOLD);
                        wattroff(win, COLOR_PAIR(getBuildingColor({i, j})));
                    }
                }
            }
        }

        // Each harbor as well as colors to indicate which resource it trades as well as which player(s) have access to it
        void renderHarbors() {
            // Harbors are ordered from top left to bottom right
            // Each harbor first renders the ratio it trades and then the two "docks" which are colored to indicate which player(s) can use it
            // Unfortunately, I can't figure out a better way to render these than to just nearly hardcode them all

            // Wheat harbor
            wattron(win, COLOR_PAIR(13));
            mvwprintw(win, Origin.first + 2, Origin.second + 19, "2:1");
            wattroff(win, COLOR_PAIR(13));

            wattron(win, COLOR_PAIR(getBuildingColor({1, 1})));
            mvwaddch(win, Origin.first + 2, Origin.second + 17, '/');
            wattroff(win, COLOR_PAIR(getBuildingColor({1, 1})));

            wattron(win, COLOR_PAIR(getBuildingColor({1, 2})));
            mvwaddch(win, Origin.first + 2, Origin.second + 23, '\\');
            wattroff(win, COLOR_PAIR(getBuildingColor({1, 2})));

            // Rock harbor
            wattron(win, COLOR_PAIR(12));
            mvwprintw(win, Origin.first + 2, Origin.second + 41, "2:1");
            wattroff(win, COLOR_PAIR(12));

            wattron(win, COLOR_PAIR(getBuildingColor({1, 3})));
            mvwaddch(win, Origin.first + 2, Origin.second + 39, '/');
            wattroff(win, COLOR_PAIR(getBuildingColor({1, 3})));

            wattron(win, COLOR_PAIR(getBuildingColor({1, 4})));
            mvwaddch(win, Origin.first + 2, Origin.second + 45, '\\');
            wattroff(win, COLOR_PAIR(getBuildingColor({1, 4})));

            // General harbor 1
            wattron(win, COLOR_PAIR(17));
            mvwprintw(win, Origin.first + 7, Origin.second, "3:1");
            wattroff(win, COLOR_PAIR(17));

            wattron(win, COLOR_PAIR(getBuildingColor({2, 0})));
            mvwprintw(win, Origin.first + 6, Origin.second + 2, "--");
            wattroff(win, COLOR_PAIR(getBuildingColor({2, 0})));

            wattron(win, COLOR_PAIR(getBuildingColor({3, 0})));
            mvwaddch(win, Origin.first + 8, Origin.second + 1, '\\');
            wattroff(win, COLOR_PAIR(getBuildingColor({3, 0})));

            // General harbor 2
            wattron(win, COLOR_PAIR(17));
            mvwprintw(win, Origin.first + 7, Origin.second + 60, "3:1");
            wattroff(win, COLOR_PAIR(17));

            wattron(win, COLOR_PAIR(getBuildingColor({2, 5})));
            mvwprintw(win, Origin.first + 6, Origin.second + 59, "--");
            wattroff(win, COLOR_PAIR(getBuildingColor({2, 5})));

            wattron(win, COLOR_PAIR(getBuildingColor({3, 5})));
            mvwaddch(win, Origin.first + 8, Origin.second + 61, '/');
            wattroff(win, COLOR_PAIR(getBuildingColor({3, 5})));

            // Wood harbor
            wattron(win, COLOR_PAIR(14));
            mvwprintw(win, Origin.first + 17, Origin.second, "2:1");
            wattroff(win, COLOR_PAIR(14));

            wattron(win, COLOR_PAIR(getBuildingColor({5, 0})));
            mvwaddch(win, Origin.first + 16, Origin.second + 1, '/');
            wattroff(win, COLOR_PAIR(getBuildingColor({5, 0})));

            wattron(win, COLOR_PAIR(getBuildingColor({6, 0})));
            mvwprintw(win, Origin.first + 18, Origin.second + 2, "--");
            wattroff(win, COLOR_PAIR(getBuildingColor({6, 0})));

            // Wool harbor
            wattron(win, COLOR_PAIR(15));
            mvwprintw(win, Origin.first + 17, Origin.second + 60, "2:1");
            wattroff(win, COLOR_PAIR(15));

            wattron(win, COLOR_PAIR(getBuildingColor({5, 5})));
            mvwaddch(win, Origin.first + 16, Origin.second + 61, '\\');
            wattroff(win, COLOR_PAIR(getBuildingColor({5, 5})));

            wattron(win, COLOR_PAIR(getBuildingColor({6, 5})));
            mvwprintw(win, Origin.first + 18, Origin.second + 59, "--");
            wattroff(win, COLOR_PAIR(getBuildingColor({6, 5})));

            // Clay harbor
            wattron(win, COLOR_PAIR(11));
            mvwprintw(win, Origin.first + 26, Origin.second + 11, "2:1");
            wattroff(win, COLOR_PAIR(11));

            wattron(win, COLOR_PAIR(getBuildingColor({8, 1})));
            mvwaddch(win, Origin.first + 25, Origin.second + 12, '/');
            wattroff(win, COLOR_PAIR(getBuildingColor({8, 1})));

            wattron(win, COLOR_PAIR(getBuildingColor({9, 1})));
            mvwprintw(win, Origin.first + 27, Origin.second + 13, "--");
            wattroff(win, COLOR_PAIR(getBuildingColor({9, 1})));

            // General harbor 3
            wattron(win, COLOR_PAIR(17));
            mvwprintw(win, Origin.first + 26, Origin.second + 49, "3:1");
            wattroff(win, COLOR_PAIR(17));

            wattron(win, COLOR_PAIR(getBuildingColor({8, 4})));
            mvwaddch(win, Origin.first + 25, Origin.second + 50, '\\');
            wattroff(win, COLOR_PAIR(getBuildingColor({8, 4})));

            wattron(win, COLOR_PAIR(getBuildingColor({9, 4})));
            mvwprintw(win, Origin.first + 27, Origin.second + 48, "--");
            wattroff(win, COLOR_PAIR(getBuildingColor({9, 4})));

            // General harbor 4
            wattron(win, COLOR_PAIR(17));
            mvwprintw(win, Origin.first + 31, Origin.second + 30, "3:1");
            wattroff(win, COLOR_PAIR(17));

            wattron(win, COLOR_PAIR(getBuildingColor({10, 2})));
            mvwaddch(win, Origin.first + 31, Origin.second + 28, '\\');
            wattroff(win, COLOR_PAIR(getBuildingColor({10, 2})));

            wattron(win, COLOR_PAIR(getBuildingColor({10, 3})));
            mvwaddch(win, Origin.first + 31, Origin.second + 34, '/');
            wattroff(win, COLOR_PAIR(getBuildingColor({10, 3})));
        }

        // Removes both building cursors and road cursors
        void removeCursors() {
            int posY = 0, posX = 0;
            
            // Buildings
            for (int i = 0; i < 11; i++) {
                for (int j = 0; j < 6; j++) {
                    if (Markers.buildings[i][j] != -1) {
                        posY = i * 3;
                        
                        if (i % 2 == 0) {
                            if (j % 2 == 0) {
                                posX = 5 + (j / 2) * 22;
                            }
                            else {
                                posX = 13 + (j / 2) * 22;
                            }
                        }
                        else {
                            if (j % 2 == 0) {
                                posX = 2 + (j / 2) * 22;
                            }
                            else {
                                posX = 16 + (j / 2) * 22;
                            }
                        }

                        mvwaddch(win, Origin.first + posY, Origin.second + posX - 2, ' ');
                        mvwaddch(win, Origin.first + posY, Origin.second + posX + 2, ' ');
                    }
                }
            }

            // Roads
            bool verticalRoad = false;

            for (int i = 0; i < 21; i++) {
                for (int j = 0; j < 11; j++) {
                    if (Markers.roads[i][j] != -1) {
                        // Horizontal roads 
                        if (i % 2 == 0) {
                            posY = (i / 2) * 3;
                            posX = 6 + (j / 2) * 11;

                            verticalRoad = false;
                        }
                        // "Vertical" roads
                        else {
                            posY = 1 + (i / 2) * 3;
                            posX = 3 + (j / 2) * 11;
                            
                            verticalRoad = true;
                        }

                        if (verticalRoad) {
                            mvwaddch(win, Origin.first + posY, Origin.second + posX - 2, ' ');
                            mvwaddch(win, Origin.first + posY + 1, Origin.second + posX - 2, ' ');
                            mvwaddch(win, Origin.first + posY, Origin.second + posX + 3, ' ');
                            mvwaddch(win, Origin.first + posY + 1, Origin.second + posX + 3, ' ');
                        }
                        else {
                            mvwaddch(win, Origin.first + posY, Origin.second + posX - 2, ' ');
                            mvwaddch(win, Origin.first + posY, Origin.second + posX + 8, ' ');
                        }
                    }
                }
            }
        }

        // Indicates which intersection is selected by applying a few offsets to the array location
        void renderCursorBuilding(int id) {
            int posY = Location.first * 3, posX = 0;

            if (Location.first % 2 == 0) {
                if (Location.second % 2 == 0) {
                    posX = 5 + (Location.second / 2) * 22;
                }
                else {
                    posX = 13 + (Location.second / 2) * 22;
                }
            }
            else {
                if (Location.second % 2 == 0) {
                    posX = 2 + (Location.second / 2) * 22;
                }
                else {
                    posX = 16 + (Location.second / 2) * 22;
                }
            }

            wattron(win, COLOR_PAIR(7 + id));
            wattron(win, A_BOLD);
            mvwaddch(win, Origin.first + posY, Origin.second + posX - 2, '<');
            mvwaddch(win, Origin.first + posY, Origin.second + posX + 2, '>');
            wattroff(win, A_BOLD);
            wattroff(win, COLOR_PAIR(7 + id));
        }

        // Move cursor as well as skip values marked with a -1
        void moveCursorBuilding(int input) {
            switch (input) {
                case KEY_UP:
                    do {
                        Location.first--;

                        if (Location.first < 0) {
                            Location.first = 10;
                        }
                    } while (Markers.buildings[Location.first][Location.second] == -1);
                    break;
                case KEY_DOWN:
                    do {
                        Location.first++;

                        if (Location.first > 10) {
                            Location.first = 0;
                        }
                    } while (Markers.buildings[Location.first][Location.second] == -1);
                    break;
                case KEY_LEFT:
                    do {
                        Location.second--;

                        if (Location.second < 0) {
                            Location.second = 5;
                        }
                    } while (Markers.buildings[Location.first][Location.second] == -1);
                    break;
                case KEY_RIGHT:
                    do {
                        Location.second++;

                        if (Location.second > 5) {
                            Location.second = 0;
                        }
                    } while (Markers.buildings[Location.first][Location.second] == -1);
                    break;
            }
        }

        // The road has two different kinds of cursors to render based on whether the highlighed road is vertical or horizontal, this does that
        void renderCursorRoad(int id) {
            int posY = 0, posX = 0;
            bool verticalRoad = false;

            // Horizontal roadss
            if (Location.first % 2 == 0) {
                posY = (Location.first / 2) * 3;
                posX = 6 + (Location.second / 2) * 11;
            }
            // "Vertical" roads
            else {
                posY = 1 + (Location.first / 2) * 3;
                posX = 3 + (Location.second / 2) * 11;

                verticalRoad = true;
            }

            wattron(win, COLOR_PAIR(7 + id));
            wattron(win, A_BOLD);

            if (verticalRoad) {
                mvwaddch(win, Origin.first + posY, Origin.second + posX - 2, '/');
                mvwaddch(win, Origin.first + posY + 1, Origin.second + posX - 2, '\\');
                mvwaddch(win, Origin.first + posY, Origin.second + posX + 3, '\\');
                mvwaddch(win, Origin.first + posY + 1, Origin.second + posX + 3, '/');
            }
            else {
                mvwaddch(win, Origin.first + posY, Origin.second + posX - 2, '<');
                mvwaddch(win, Origin.first + posY, Origin.second + posX + 8, '>');
            }
            
            wattroff(win, A_BOLD);
            wattroff(win, COLOR_PAIR(7 + id));
        }

        // Direction the cursor moves when going up/down is determined by which left/right direction was last moved, least janky way I could think of doing this
        //
        // Return true if went left, return false if went right
        bool moveCursorRoad(int input, bool wentLeft) {
            int scan = 1;

            switch (input) {
                case KEY_UP:
                    do {
                        Location.first--;

                        if (Location.second == 0) {
                            if (Markers.roads[Location.first][Location.second + 1] != -1) {
                                Location.second++;
                            }
                        }
                        else if (Location.second == 10) {
                            if (Markers.roads[Location.first][Location.second - 1] != -1) {
                                Location.second--;
                            }
                        }
                        else {
                            if (!wentLeft) {
                                scan = -1;
                            }

                            if (Markers.roads[Location.first][Location.second - scan] != -1) {
                                Location.second -= scan;
                            }
                            else if (Markers.roads[Location.first][Location.second + scan] != -1) {
                                Location.second += scan;
                            }
                        }

                        if (Location.first < 0) {
                            Location.first = 20;
                        }
                    } while (Markers.roads[Location.first][Location.second] == -1);

                    return wentLeft;
                    break;
                case KEY_DOWN:
                    do {
                        Location.first++;

                        if (Location.second == 0) {
                            if (Markers.roads[Location.first][Location.second + 1] != -1) {
                                Location.second++;
                            }
                        }
                        else if (Location.second == 10) {
                            if (Markers.roads[Location.first][Location.second - 1] != -1) {
                                Location.second--;
                            }
                        }
                        else {
                            if (!wentLeft) {
                                scan = -1;
                            }

                            if (Markers.roads[Location.first][Location.second - scan] != -1) {
                                Location.second -= scan;
                            }
                            else if (Markers.roads[Location.first][Location.second + scan] != -1) {
                                Location.second += scan;
                            }
                        }

                        if (Location.first > 20) {
                            Location.first = 0;
                        }
                    } while (Markers.roads[Location.first][Location.second] == -1);

                    return wentLeft;
                    break;
                case KEY_LEFT:
                    do {
                        Location.second--;

                        if (Location.second < 0) {
                            Location.second = 10;
                        }
                    } while (Markers.roads[Location.first][Location.second] == -1);

                    return true;
                    break;
                case KEY_RIGHT:
                    do {
                        Location.second++;

                        if (Location.second > 10) {
                            Location.second = 0;
                        }
                    } while (Markers.roads[Location.first][Location.second] == -1);

                    return false;
                    break;
            }

            return true;
        }

        void renderCursorRobber(int id) {
            int numPosY = 0, numPosX = 8 + Location.second * 11;

            // Find the position that the robber should go in
            if (Location.second % 2 == 0) {
                numPosY = 3 + Location.first * 6;
            }
            else {
                numPosY = 6 + Location.first * 6;
            }
            
            wattron(win, COLOR_PAIR(7 + id));
            wattron(win, A_BOLD);
            mvwprintw(win, Origin.first + numPosY, Origin.second + numPosX - 2, "< ROB >");
            wattroff(win, A_BOLD);
            wattroff(win, COLOR_PAIR(7 + id));
        }

        // Move cursor as well as skip values marked with a -1
        void moveCursorRobber(int input) {
            switch (input) {
                case KEY_UP:
                    do {
                        Location.first--;

                        if (Location.first < 0) {
                            Location.first = 4;
                        }
                    } while (Markers.tiles[Location.first][Location.second] == -1);
                    break;
                case KEY_DOWN:
                    do {
                        Location.first++;

                        if (Location.first > 4) {
                            Location.first = 0;
                        }
                    } while (Markers.tiles[Location.first][Location.second] == -1);
                    break;
                case KEY_LEFT:
                    do {
                        Location.second--;

                        if (Location.second < 0) {
                            Location.second = 4;
                        }
                    } while (Markers.tiles[Location.first][Location.second] == -1);
                    break;
                case KEY_RIGHT:
                    do {
                        Location.second++;

                        if (Location.second > 4) {
                            Location.second = 0;
                        }
                    } while (Markers.tiles[Location.first][Location.second] == -1);
                    break;
            }
        }

    public:
        catanBoard(std::pair<int, int> winOrigin = {0, 0}, std::pair<int, int> boardOrigin = {2, 2}) {
            if (boardOrigin.first < 0) {
                boardOrigin.first = 2;
            }
            if (boardOrigin.second < 0) {
                boardOrigin.second = 2;
            }

            if (winOrigin.first < 0) {
                winOrigin.first = 0;
            }
            if (winOrigin.second < 0) {
                winOrigin.second = 0;
            }

            Origin = boardOrigin;

            win = newwin(36, 67, winOrigin.first, winOrigin.second);
            keypad(win, TRUE);
        }

        // All-in-one function to render the board
        void render() {
            renderTiles();
            renderRoads();
            renderBuildings();
            renderHarbors();

            box(win, 0, 0);
            wrefresh(win);
        }

        // id: 0 = Red, 1 = Orange, 2 = Blue, 3 = White
        // city: True = placing a city, False = placing a settlement
        // Return Vals: -1 = Quit Catan, 0 = Placed, 1 = Refund
        int placeBuilding(int id, bool city) {
            Location = {1, 1};

            while (true) {
                removeCursors();
                render();
                renderCursorBuilding(id);

                int input = wgetch(win);

                removeCursors();

                if (input == KEY_UP || input == KEY_DOWN || input == KEY_LEFT || input == KEY_RIGHT) {
                    moveCursorBuilding(input);
                }
                else if (input == 'z') {
                    // When placing a city, it must replace a settlement
                    if (city && Markers.buildings[Location.first][Location.second] == id * 2 + 1) {
                        Markers.buildings[Location.first][Location.second] = id * 2 + 2;
                        return 0;
                    }

                    if (!city && Markers.buildings[Location.first][Location.second] == 0) {
                        Markers.buildings[Location.first][Location.second] = id * 2 + 1;
                        return 0;
                    }
                }
                else if (input == 'x') {
                    return 1;
                }
                else if (input == 'q') {
                    return -1;
                }
            }
        }

        // id: 0 = Red, 1 = Orange, 2 = Blue, 3 = White
        // Return Vals: -1 = Quit Catan, 0 = Placed, 1 = Refund
        int placeRoad(int id) {
            Location = {1, 4};
            bool wentLeft = true;

            while (true) {
                removeCursors();
                render();
                renderCursorRoad(id);

                int input = wgetch(win);

                removeCursors();

                if (input == KEY_UP || input == KEY_DOWN || input == KEY_LEFT || input == KEY_RIGHT) {
                    wentLeft = moveCursorRoad(input, wentLeft);
                }
                else if (input == 'z') {
                    if (Markers.roads[Location.first][Location.second] == 0) {
                        Markers.roads[Location.first][Location.second] = id + 1;
                        return 0;
                    }
                }
                else if (input == 'x') {
                    return 1;
                }
                else if (input == 'q') {
                    return -1;
                }
            }
        }

        // Return a list of buildings located on a specific tile
        std::vector<std::pair<int, int>> scanTile(std::pair<int, int> tile) {
            // These are used as the upper-left corners of each tile, 1/6 of the settlements each one can have
            // To find the other five with {y, x}, you use: {y, x + 1}, {y + 1, x}, {y + 1, x + 1}, {y + 2, x}, {y + 2, x + 1}
            std::pair<int, int> tileOrigins[5][5] = {
                {
                    {-1, -1}, {1, 1}, {0, 2}, {1, 3}, {-1, -1}
                },
                {
                    {2, 0}, {3, 1}, {2, 2}, {3, 3}, {2, 4}
                },
                {
                    {4, 0}, {5, 1}, {4, 2}, {5, 3}, {4, 4}
                },
                {
                    {6, 0}, {7, 1}, {6, 2}, {7, 3}, {6, 4}
                },
                {
                    {-1, -1}, {-1, -1}, {8, 2}, {-1, -1}, {-1, -1}
                }
            };

            std::vector<std::pair<int, int>> output;

            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 2; j++) {
                    std::pair<int, int> location = {tileOrigins[tile.first][tile.second].first + i, tileOrigins[tile.first][tile.second].second + j};

                    if (Markers.buildings[location.first][location.second] > 0) {
                        output.emplace_back(location);
                    }
                }
            }

            return output;
        }

        // isKnight: true = allows you to cancel moving the robber, false = must place robber
        // output[4] indicates whether catan should stop or continue, output[5] indicates whether to increase a player's army size or not
        std::vector<bool> moveRobber(int id, bool isKnight = false) {
            Location = Markers.robber;
            std::vector<bool> output = {false, false, false, false, true, false};

            while (true) {
                render();
                renderCursorRobber(id);

                int input = wgetch(win);

                if (input == KEY_UP || input == KEY_DOWN || input == KEY_LEFT || input == KEY_RIGHT) {
                    moveCursorRobber(input);
                }
                else if (input == 'z') {
                    if (Location != Markers.robber) {
                        // Move robber
                        Markers.robber = Location;
                        
                        // Get the buildings on the new tile
                        std::vector<std::pair<int, int>> buildings = scanTile(Markers.robber);

                        // Set a player to true if they have either a building or settlement on the new tile so that
                        // the player who moved the robber can choose who to steal from
                        for (int i = 0; i < buildings.size(); i++) {
                            switch (Markers.buildings[buildings[i].first][buildings[i].second]) {
                                case 1:
                                case 2:
                                    output[0] = true;
                                    break;
                                case 3:
                                case 4:
                                    output[1] = true;
                                    break;
                                case 5:
                                case 6:
                                    output[2] = true;
                                    break;
                                case 7:
                                case 8:
                                    output[3] = true;
                                    break;
                            }
                        }

                        if (isKnight) {
                            output[5] = true;
                        }

                        return output;
                    }
                }
                else if (input == 'x') {
                    if (isKnight) {
                        return {false, false, false, false, true, false};
                    }
                }
                else if (input == 'q') {
                    return {false, false, false, false, false, false};
                }
            }
        }

        // Return the trades that the current player has
        std::vector<int> updateHarbors(int id) {
            std::pair<int, int> harborLocations[18] = {{1, 1}, {1, 2}, {1, 3}, {1, 4}, {2, 0}, {3, 0}, {2, 5}, {3, 5}, {5, 0}, {6, 0}, {5, 5}, {6, 5}, {8, 1}, {9, 1}, {8, 4}, {9, 4}, {10, 2}, {10, 3}};

            // Set the default trades
            std::vector<int> output = {0, 0, 0, 0, 0, 0};

            for (int i = 0; i < 18; i++) {
                int harborLocation = Markers.buildings[harborLocations[i].first][harborLocations[i].second], tradeIndex = 0;

                if (i <= 1) {
                    tradeIndex = 3;
                }
                else if (i <= 3) {
                    tradeIndex = 2;
                }
                else if (i <= 7) {
                    tradeIndex = 0;
                }
                else if (i <= 9) {
                    tradeIndex = 4;
                }
                else if (i <= 11) {
                    tradeIndex = 5;
                }
                else if (i <= 13) {
                    tradeIndex = 1;
                }
                else if (i <= 17) {
                    tradeIndex = 0;
                }

                if (harborLocation == id * 2 + 1 || harborLocation == id * 2 + 2) {
                    output[tradeIndex]++;
                }
            }

            return output;
        }

        // Scan each tile for whether it matches the diceroll or not, then scan that tile for new buildings and give the matching player their resources
        std::vector<std::vector<int>> collectResources(int roll) {
            // Building locations that have already produced resources are stored here as to not repeat
            std::vector<std::pair<int, int>> usedLocation;

            // Stores the amount of each resource to add to each player
            std::vector<std::vector<int>> output = {
                {0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0}
            };

            // Scan numbers for one that matches the diceroll and is not robbed
            for (int i = 0; i < 5; i++) {
                for (int j = 0; j < 5; j++) {
                    // Which of the 19 tiles is being evaluated
                    std::pair<int, int> currentTile = {i, j};

                    // Trigger when a tile has the right number and isn't being robbed
                    if (Markers.tileRarity[i][j] == roll && Markers.robber != currentTile) {
                        // Get a list of all of the buildings on the current tile
                        std::vector<std::pair<int, int>> buildings = scanTile({i, j});

                        // Calculate how many resources need to be added to each player based on the buildings on the tile
                        // Already used tiles are also made sure to not produce resources twice, in case two tiles with the same number are next to each other
                        for (int k = 0; k < buildings.size(); k++) {
                            bool used = false;
                            for (int m = 0; m < usedLocation.size(); m++) {
                                if (usedLocation[m] == buildings[k]) {
                                    used = true;
                                    break;
                                }
                            }
                            if (!used) {
                                usedLocation.emplace_back(buildings[k]);

                                // Cities produce 2 resources while settlements produce one
                                int amount = 1;
                                if (Markers.buildings[buildings[k].first][buildings[k].second] % 2 == 0) {
                                    amount++;
                                }

                                switch (Markers.buildings[buildings[k].first][buildings[k].second]) {
                                    case 1:
                                    case 2:
                                        output[0][Markers.tiles[i][j]] += amount;
                                        break;
                                    case 3:
                                    case 4:
                                        output[1][Markers.tiles[i][j]] += amount;
                                        break;
                                    case 5:
                                    case 6:
                                        output[2][Markers.tiles[i][j]] += amount;
                                        break;
                                    case 7:
                                    case 8:
                                        output[3][Markers.tiles[i][j]] += amount;
                                        break;
                                }
                            }
                        }
                    }
                }
            }

            return output;
        }

        // Remove all placements and reset the board to the default
        void reset() {
            // Identical as the Markers struct, but this one's values won't change,
            // so they can be used to restore the default state of the game
            struct {
                int buildings[11][6] = {
                    {
                        -1, -1, 0, 0, -1, -1
                    },
                    {
                        -1, 0, 0, 0, 0, -1
                    },
                    {
                        0, 0, 0, 0, 0, 0
                    },
                    {
                        0, 0, 0, 0, 0, 0
                    },
                    {
                        0, 0, 0, 0, 0, 0
                    },
                    {
                        0, 0, 0, 0, 0, 0
                    },
                    {
                        0, 0, 0, 0, 0, 0
                    },
                    {
                        0, 0, 0, 0, 0, 0
                    },
                    {
                        0, 0, 0, 0, 0, 0
                    },
                    {
                        -1, 0, 0, 0, 0, -1
                    },
                    {
                        -1, -1, 0, 0, -1, -1
                    }
                };

                int roads[21][11] = {
                    {
                        -1, -1, -1, -1, -1, 0, -1, -1, -1, -1, -1
                    },
                    {
                        -1, -1, -1, -1, 0, -1, 0, -1, -1, -1, -1
                    },
                    {
                        -1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1
                    },
                    {
                        -1, -1, 0, -1, 0, -1, 0, -1, 0, -1, -1
                    },
                    {
                        -1, 0, -1, -1, -1, 0, -1, -1, -1, 0, -1
                    },
                    {
                        0, -1, 0, -1, 0, -1, 0, -1, 0, -1, 0
                    },
                    {
                        -1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1
                    },
                    {
                        0, -1, 0, -1, 0, -1, 0, -1, 0, -1, 0
                    },
                    {
                        -1, 0, -1, -1, -1, 0, -1, -1, -1, 0, -1
                    },
                    {
                        0, -1, 0, -1, 0, -1, 0, -1, 0, -1, 0
                    },
                    {
                        -1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1
                    },
                    {
                        0, -1, 0, -1, 0, -1, 0, -1, 0, -1, 0
                    },
                    {
                        -1, 0, -1, -1, -1, 0, -1, -1, -1, 0, -1
                    },
                    {
                        0, -1, 0, -1, 0, -1, 0, -1, 0, -1, 0
                    },
                    {
                        -1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1
                    },
                    {
                        0, -1, 0, -1, 0, -1, 0, -1, 0, -1, 0
                    },
                    {
                        -1, 0, -1, -1, -1, 0, -1, -1, -1, 0, -1
                    },
                    {
                        -1, -1, 0, -1, 0, -1, 0, -1, 0, -1, -1
                    },
                    {
                        -1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1
                    },
                    {
                        -1, -1, -1, -1, 0, -1, 0, -1, -1, -1, -1
                    },
                    {
                        -1, -1, -1, -1, -1, 0, -1, -1, -1, -1, -1
                    }
                };

                int tiles[5][5] = {
                    {
                        -1, 4, 3, 0, -1
                    },
                    {
                        1, 0, 4, 3, 1
                    },
                    {
                        2, 3, 5, 2, 4
                    },
                    {
                        2, 3, 1, 2, 4
                    },
                    {
                        -1, -1, 0, -1, -1
                    }
                };

                int tileRarity[5][5] = {
                    {
                        -1, 2, 9, 10, -1
                    },
                    {
                        10, 6, 4, 3, 8
                    },
                    {
                        12, 11, -1, 4, 5
                    },
                    {
                        9, 8, 3, 6, 11
                    },
                    {
                        -1, -1, 5, -1, -1
                    }
                };
            
                std::pair<int, int> robber = {2, 2};
            } Defaults;

            // Reset buildings
            for (int i = 0; i < 11; i++) {
                for (int j = 0; j < 6; j++) {
                    Markers.buildings[i][j] = Defaults.buildings[i][j];
                }
            }

            // Reset roads
            for (int i = 0; i < 21; i++) {
                for (int j = 0; j < 11; j++) {
                    Markers.roads[i][j] = Defaults.roads[i][j];
                }
            }

            // Reset tiles
            for (int i = 0; i < 5; i++) {
                for (int j = 0; j < 5; j++) {
                    Markers.tiles[i][j] = Defaults.tiles[i][j];
                    Markers.tileRarity[i][j] = Defaults.tileRarity[i][j];
                }
            }

            // Reset robber
            Markers.robber = Defaults.robber;

            // Delete the winow
            wclear(win);
            wrefresh(win);
            delwin(win);
        }
};
