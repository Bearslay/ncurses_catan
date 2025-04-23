#ifndef DATALIST
#define DATALIST

// The idea is to have a list be displayed where
// each row is one datapoint to display and
// each column (of varying sizes) is an attribute of that datapoint
// Selecting a piece of data will return its ID which can be used to perform some kind of action
// 16 datapoints will be available per page and the index of the datapoint will always be the first column

#include <ncurses.h>
#include <vector>
#include <string>
#include <utility>

// Stores Datapoints consisting of Integers, Decimals, Strings, and Integer/Decimal Pairs to be displayed in a navigatable list
// Aspects of these datapoints are displayed in columns and in pre-formatted ways
//
// COLUMN WIDTHS:
// Integer: 7, Decimal: 6, String: Undefined, Integer Pair: 7, Decimal Pair: 13
//
// CONSTRUCTOR ARGUEMENTS:
// (std::vector<int> columnStart, std::vector<int> columnWidth, std::vector<std::string> columnTitles, std::string title)
class DataList {
    private:
        // All of the internal data storage stuff is here
        struct {
            std::vector<int> start;
            std::vector<int> width;
            std::vector<std::string> titles;
        } ColumnData;

        // Stores the data for each item in the list
        struct dataPoint {
            // Different kinds of data the list can display
            std::vector<int> integers;
            std::vector<double> doubles;
            std::vector<std::string> strings;
            // First integer determines the seperating character
            // 0 = '-', 1 = '/', 2 = ':'
            // Other two numbers match the data actually displayed
            std::vector<std::pair<int, std::pair<int, int>>> intPairs;
            std::vector<std::pair<int, std::pair<double, double>>> doublePairs;

            // Keep track of each data type
            // 0 = int, 1 = double, 2 = string, 3 = intCombo, 4 = doubleCombo
            std::vector<int> types;
        } DataStorage;

        std::vector<dataPoint> Data;

        // Displaying the list
        std::string Title;

        int ListLocation = 0;
        int Page = 0, LastPage = Data.size() / 16;
        std::pair<int, int> windowDims = {25, 50};

        // The List doesn't do anything fancy, but some color is still good
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

        // Render the list
        void render(WINDOW *win) {
            // Title
            mvwprintw(win, 1, windowDims.second / 2 - Title.length() / 2 - 4, "--- %s ---", Title.c_str());

            // Erase any potentially lingering data
            for (int i = 0; i < 16; i++) {
                mvwprintw(win, 5 + i, 1, "                                                ");
            }

            // Column Names
            mvwprintw(win, 3, 4, "Index");
            for (int i = 0; i < ColumnData.start.size(); i++) {
                mvwprintw(win, 3, 10 + ColumnData.start[i] + ColumnData.width[i] / 2 - ColumnData.titles[i].length() / 2, ColumnData.titles[i].c_str());
            }

            // The 16 lines of actual data and stuff
            for (int i = 0; i < 16; i++) {                
                // Quit early if there aren't enough items left to fill the page
                if (i == (Data.size() - 1) % 16 + 1 && Page == LastPage) {
                    break;
                }

                // To keep track of which piece of data should be used from each vector, these are used
                int intCount = 0, doubleCount = 0, stringCount = 0, intPairCount = 0, doublePairCount = 0;

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

                // Indexes
                mvwprintw(win, 5 + i, 4, "[   ]");
                wattron(win, COLOR_PAIR(rowColor));
                mvwprintw(win, 5 + i, 5, "%03d", i + Page * 16);

                // Item information
                for (int j = 0; j < Data[i].types.size(); j++) {
                    // Integer Display
                    if (Data[i + Page * 16].types[j] == 0) {
                        mvwprintw(win, 5 + i, 10 + ColumnData.start[j], "%07d", Data[i + Page * 16].integers[intCount]);
                        intCount++;
                    }
                    // Double display
                    else if (Data[i + Page * 16].types[j] == 1) {
                        mvwprintw(win, 5 + i, 10 + ColumnData.start[j], "%06.2f", Data[i + Page * 16].doubles[doubleCount]);
                        doubleCount++;
                    }
                    // String display
                    else if (Data[i + Page * 16].types[j] == 2) {
                        wattron(win, A_BOLD);
                        mvwprintw(win, 5 + i, 10 + ColumnData.start[j], "%s", Data[i + Page * 16].strings[stringCount].c_str());
                        wattroff(win, A_BOLD);
                        stringCount++;
                    }
                    // Integer Pair display
                    else if (Data[i + Page * 16].types[j] == 3) {
                        mvwprintw(win, 5 + i, 10 + ColumnData.start[j], "%02d   %02d", Data[i + Page * 16].intPairs[intPairCount].second.first, Data[i + Page * 16].intPairs[intPairCount].second.second);

                        wattroff(win, COLOR_PAIR(rowColor));
                        switch (Data[i + Page * 16].intPairs[intPairCount].first) {
                            case 0:
                                mvwaddch(win, 5 + i, 10 + ColumnData.start[j] + 3, '-');
                                break;
                            case 1:
                                mvwaddch(win, 5 + i, 10 + ColumnData.start[j] + 3, '/');
                                break;
                            case 2:
                                mvwaddch(win, 5 + i, 10 + ColumnData.start[j] + 3, ':');
                                break;
                        }
                        wattron(win, COLOR_PAIR(rowColor));
                        intPairCount++;
                    }
                    // Double Pair display
                    else if (Data[i + Page * 16].types[j] == 4) {
                        mvwprintw(win, 5 + i, 10 + ColumnData.start[j], "%05.1f   %05.1f", Data[i + Page * 16].doublePairs[doublePairCount].second.first, Data[i + Page * 16].doublePairs[doublePairCount].second.second);

                        wattroff(win, COLOR_PAIR(rowColor));
                        switch (Data[i + Page * 16].intPairs[intPairCount].first) {
                            case 0:
                                mvwaddch(win, 5 + i, 10 + ColumnData.start[j] + 6, '-');
                                break;
                            case 1:
                                mvwaddch(win, 5 + i, 10 + ColumnData.start[j] + 6, '/');
                                break;
                            case 2:
                                mvwaddch(win, 5 + i, 10 + ColumnData.start[j] + 6, ':');
                                break;
                        }
                        wattron(win, COLOR_PAIR(rowColor));
                        doublePairCount++;
                    }
                }
                wattroff(win, COLOR_PAIR(rowColor));
            }

            // Page indicator
            int pageColor = 2;
            if (Page == LastPage) {
                pageColor = 1;
            }

            mvwprintw(win, 22, 40, "Page:");
            wattron(win, COLOR_PAIR(pageColor));
            mvwprintw(win, 23, 39, "%02d   %02d", Page, LastPage);
            wattroff(win, COLOR_PAIR(pageColor));
            mvwaddch(win, 23, 42, '/');

            // Cursor
            wattron(win, COLOR_PAIR(COLOR_CYAN));
            wattron(win, A_BOLD);
            mvwaddch(win, 5 + ListLocation, 2, '<');
            mvwaddch(win, 5 + ListLocation, 47, '>');
            wattroff(win, A_BOLD);
            wattroff(win, COLOR_PAIR(COLOR_CYAN));

            // Submenu (non-functional)
            mvwprintw(win, 22, 16, "  Quit Selection  ");
            mvwprintw(win, 23, 17, "  Quit Program  ");

            box(win, 0, 0);
            wrefresh(win);
        }

        // Change which item is selected
        void moveCursor(int input) {
            int endPos = (Data.size() - 1) % 16;

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

        // Return Values:
        // 0 = Continue with launcher, 1 = Return to Main Launcher (quit fn), -1 = Quit Program
        int submenu(WINDOW *win) {
            int location = 0;

            while (true) {
                // Render Text
                mvwprintw(win, 22, 16, "  Quit Selection  ");
                mvwprintw(win, 23, 17, "  Quit Program  ");

                // Add the cursor
                if (location == 0) {
                    wattron(win, COLOR_PAIR(COLOR_YELLOW));
                    mvwaddch(win, 22, 16, '<');
                    mvwaddch(win, 22, 33, '>');
                    wattroff(win, COLOR_PAIR(COLOR_YELLOW));
                }
                else {
                    wattron(win, COLOR_PAIR(COLOR_RED));
                    mvwaddch(win, 23, 17, '<');
                    mvwaddch(win, 23, 32, '>');
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

        void clearDataStorage() {
            DataStorage.types.clear();
            DataStorage.integers.clear();
            DataStorage.doubles.clear();
            DataStorage.strings.clear();
            DataStorage.intPairs.clear();
            DataStorage.doublePairs.clear();
        }

        void stop(WINDOW *win) {
            wclear(win);
            wrefresh(win);
            delwin(win);

            ListLocation = 0;
            Page = 0;
        }

    public:
        // Since columns are consistent throughout the entire list, the headers can be defined upon creation
        DataList(std::vector<int> columnStart, std::vector<int> columnWidth, std::vector<std::string> columnTitles, std::string title) {
            for (int i = 0; i < columnStart.size(); i++) {
                ColumnData.start.emplace_back(columnStart[i]);
                ColumnData.width.emplace_back(columnWidth[i]);
                ColumnData.titles.emplace_back(columnTitles[i]);
            }

            Title = title;
        }

        // Add attributes to a datapoint, one attribute at a time unfortunately
        void addInt(int index, int value) {
            if (index < Data.size()) {
                DataStorage.integers = Data[index].integers;
                DataStorage.types = Data[index].types;
            }

            DataStorage.integers.emplace_back(value);
            DataStorage.types.emplace_back(0);

            if (index >= Data.size()) {
                Data.emplace_back(DataStorage);
            }
            else {
                Data[index].integers = DataStorage.integers;
                Data[index].types = DataStorage.types;
            }
            
            clearDataStorage();
        }
        void addDouble(int index, double value) {
            if (index < Data.size()) {
                DataStorage.doubles = Data[index].doubles;
                DataStorage.types = Data[index].types;
            }

            DataStorage.doubles.emplace_back(value);
            DataStorage.types.emplace_back(1);

            if (index >= Data.size()) {
                Data.emplace_back(DataStorage);
            }
            else {
                Data[index].doubles = DataStorage.doubles;
                Data[index].types = DataStorage.types;
            }
            
            clearDataStorage();
        }
        void addString(int index, std::string value) {
            if (index < Data.size()) {
                DataStorage.strings = Data[index].strings;
                DataStorage.types = Data[index].types;
            }

            DataStorage.strings.emplace_back(value);
            DataStorage.types.emplace_back(2);

            if (index >= Data.size()) {
                Data.emplace_back(DataStorage);
            }
            else {
                Data[index].strings = DataStorage.strings;
                Data[index].types = DataStorage.types;
            }
            
            clearDataStorage();
        }
        void addIntPair(int index, int sepChar, std::pair<int, int> value) {
            if (index < Data.size()) {
                DataStorage.intPairs = Data[index].intPairs;
                DataStorage.types = Data[index].types;
            }
            
            std::pair<int, std::pair<int, int>> outputPair = {sepChar, value};
            DataStorage.intPairs.emplace_back(outputPair);
            DataStorage.types.emplace_back(3);

            if (index >= Data.size()) {
                Data.emplace_back(DataStorage);
            }
            else {
                Data[index].intPairs = DataStorage.intPairs;
                Data[index].types = DataStorage.types;
            }

            clearDataStorage();
        }
        void addDoublePair(int index, int sepChar, std::pair<double, double> value) {
            if (index < Data.size()) {
                DataStorage.doublePairs = Data[index].doublePairs;
                DataStorage.types = Data[index].types;
            }
            
            std::pair<int, std::pair<double, double>> outputPair = {sepChar, value};
            DataStorage.doublePairs.emplace_back(outputPair);
            DataStorage.types.emplace_back(4);

            if (index >= Data.size()) {
                Data.emplace_back(DataStorage);
            }
            else {
                Data[index].doublePairs = DataStorage.doublePairs;
                Data[index].types = DataStorage.types;
            }

            clearDataStorage();
        }

        // Return -1 to quit program, -2 to quit launcher without exiting
        // A return value of 0+ indicates the index chosen and also which action will be triggered
        int start(std::pair<int, int> origin) {
            if (Data.size() < 1) {
                return -1;
            }
            
            if (origin.first < 0) {
                origin.first = 0;
            }

            if (origin.second < 0) {
                origin.second = 0;
            }

            LastPage = Data.size() / 16;
            if (Data.size() % 16 == 0) {
                LastPage--;
            }

            // Since the launcher takes up the entire window, there isn't a need for an origin point
            // The point to change is instead the place where the window is created
            WINDOW *win = newwin(windowDims.first, windowDims.second, origin.first, origin.second);
            keypad(win, TRUE);

            // Get the colors going
            activateColors();

            while (true) {
                render(win);

                int input = wgetch(win);
                int listIndex = ListLocation + Page * 16;

                if (input == KEY_UP || input == KEY_DOWN) {
                    moveCursor(input);
                }
                else if (input == 'z') {
                    stop(win);
                    return listIndex;
                }
                else if (input == 'x') {
                    int menuAction = submenu(win);

                    if (menuAction != 0) {
                        stop(win);
                    }

                    if (menuAction == 1) {
                        return -2;
                    }
                    else if (menuAction == -1) {
                        return -1;
                    }
                }
                else if (input == 'q') {
                    stop(win);
                    return -1;
                }
            }
        }
};

#endif
