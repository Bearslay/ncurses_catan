#ifndef BASICLIST
#define BASICLIST

#include <ncurses.h>
#include <vector>
#include <utility>
#include <string>
#include "customText.h"

// Creates a simple list of items that are displayed using the customText() function and can be cycled between using the arrow keys
//
// Input Vals (constructor):
// win = window to render the list; no built-in limits
// origin = top-left corner of the list; location of the left cursor when selecting the first item
// entries = a list containing inputs that would be fed into the customText() function; used to render each item in the list
// bannedEntries.first = the color that all banned entries use, regardless of its usual color
// bannedEntries.second = a list containing all of the indexes that are banned; prevents that index from being chosen
// spacing = the amount of lines between each list entry; 1 = no lines, 2 = 1 line, 3 = 2 lines, etc
// cursorColor = the color of the cursor that will indicate which item is being selected
// canQuit = a boolean to determine whether the list can be "soft-quit" using the 'x' key or not
//
// Return Vals (of start()):
// first = true or false, useful for feedback mode
// second = -2 = No Selection, -1 = Quit, 0+ = Index of the item from the list
//
// Example Constructor Input:
// (stdscr, {0, 0}, {{{{1}, "item1"}}, {{{2}, "item2"}}, {{{3}, "item3"}}, {{{4}, "item4"}}, {{{5}, "item5"}}}, {7, {1, 3}}, 1, 6, false)
class basicList {
    private:
        std::pair<int, int> Origin;
        int Spacing;
        int CursorColor;
        bool CanQuit;
        WINDOW *Win;

        std::vector<std::vector<std::pair<std::vector<int>, std::string>>> Entries;
        std::vector<int> LineWidths;
        std::pair<int, std::vector<int>> BannedEntries;

        // Return true if the index is unbanned or false if the index is banned
        bool checkForBan(int index) {
            for (int i = 0; i < BannedEntries.second.size(); i++) {
                if (index == BannedEntries.second[i]) {
                    return false;
                    break;
                }
            }

            return true;
        }
        
    public:
        basicList(WINDOW *win, std::pair<int, int> origin, std::vector<std::vector<std::pair<std::vector<int>, std::string>>> entries, std::pair<int, std::vector<int>> bannedEntries, int spacing, int cursorColor, bool canQuit) {
            Win = win;
            keypad(Win, true);

            Origin = origin;
            Spacing = spacing;
            CursorColor = cursorColor;
            CanQuit = canQuit;

            for (int i = 0; i < entries.size(); i++) {
                Entries.emplace_back(entries[i]);

                // Create a list of line widths that can be used to render the cursor
                LineWidths.emplace_back(3);
                for (int j = 0; j < entries[i].size(); j++) {
                    LineWidths[i] += entries[i][j].second.length();
                }
            }

            BannedEntries.first = bannedEntries.first;
            for (int i = 0; i < bannedEntries.second.size(); i++) {
                BannedEntries.second.emplace_back(bannedEntries.second[i]);
            }
        }

        // Replace the list of banned indexes with a new one and also adjustst the ban color
        // No changes to the list are made if the first item in the bans list is negative
        // No changes to the color are made if the new color is <=0
        void changeBans(int banColor, std::vector<int> bans) {
            if (bans.size() > 0) {
                if (bans[0] >= 0) {
                    BannedEntries.second.clear();

                    for (int i = 0; i < bans.size(); i++) {
                        BannedEntries.second.emplace_back(bans[i]);
                    }
                }
            }
            else {
                BannedEntries.second.clear();
            }

            BannedEntries.first = (banColor >= 0) ? banColor : BannedEntries.first;
        }

        // Change how many lines are between each entry in the list
        // No changes are made if the new spacing is <=0
        void changeSpacing(int spacing) {
            Spacing = (spacing > 0) ? spacing : Spacing;
        }

        // Change whether the list can be soft quitted or not
        void changeQuitability(bool canQuit) {
            CanQuit = canQuit;
        }

        // Change the color of the cursor
        // No changes are made if the new color is <=0
        void changeCursorColor(int cursorColor) {
            CursorColor = (cursorColor >= 0) ? cursorColor : CursorColor;
        }

        // Change the location the list originates from
        // No changes are made to the origin location if either coordinate is negative
        void changeOrigin(WINDOW *win, std::pair<int, int> origin) {
            Win = win;
            keypad(Win, true);

            Origin = (origin.first >= 0 && origin.second >= 0) ? origin : Origin;
        }

        // The more common attributes of a list to have to change while still using the same entries
        void redefine(std::pair<int, std::vector<int>> bannedEntries, int spacing, int cursorColors, bool canQuit) {
            changeBans(bannedEntries.first, bannedEntries.second);
            changeSpacing(spacing);
            changeCursorColor(cursorColors);
            changeQuitability(canQuit);
        }

        // Add a new entry to a specific location in the list
        // An index <0 places it at the start of the list, and index greater than the size of the list places it at the end
        void addEntry(std::vector<std::pair<std::vector<int>, std::string>> entry, int index = INT32_MAX) {
            int lwIndex = 0;

            if (index < 0) {
                Entries.insert(Entries.begin(), entry);
                
                LineWidths.insert(LineWidths.begin(), 3);
                for (int i = 0; i < entry.size(); i++) {
                    LineWidths[0] += entry[i].second.length();
                }
            }
            else if (index >= Entries.size()) {
                Entries.emplace_back(entry);

                LineWidths.emplace_back(3);
                for (int i = 0; i < entry.size(); i++) {
                    LineWidths[LineWidths.size() - 1] += entry[i].second.length();
                }
            }
            else {
                Entries.insert(Entries.begin() + index, entry);

                LineWidths.insert(LineWidths.begin() + index, 3);
                for (int i = 0; i < entry.size(); i++) {
                    LineWidths[index] += entry[i].second.length();
                }
            }
        }

        // Swap an existing entry out for a new one
        void replaceEntry(std::vector<std::pair<std::vector<int>, std::string>> entry, int index) {
            if (index >= 0 && index < Entries.size()) {
                Entries[index] = entry;

                LineWidths[index] = 3;
                for (int i = 0; i < entry.size(); i++) {
                    LineWidths[index] += entry[i].second.length();
                }
            }
        }

        // Remove an entry from the list
        void deleteEntry(int index) {
            if (index >= 0 && index < Entries.size()) {
                Entries.erase(Entries.begin() + index);
                LineWidths.erase(LineWidths.begin() + index);
            }
        }

        // Switch out the entire list of entries for a new one
        void redoEntries(std::vector<std::vector<std::pair<std::vector<int>, std::string>>> entries) {
            Entries.clear();
            LineWidths.clear();

            for (int i = 0; i < entries.size(); i++) {
                Entries.emplace_back(entries[i]);

                // Create a list of line widths that can be used to render the cursor
                LineWidths.emplace_back(3);
                for (int j = 0; j < entries[i].size(); j++) {
                    LineWidths[i] += entries[i][j].second.length();
                }
            }
        }

        // Render the list
        void render() {
            // Find the longest line
            int width = LineWidths[0] + 1;
            for (int i = 0; i < LineWidths.size(); i++) {
                if (LineWidths[i] > width) {
                    width = LineWidths[i] + 1;
                }
            }

            // Clear the space the list will need
            for (int i = 0; i < Entries.size() * Spacing; i++) {
                for (int j = 0; j < width; j++) {
                    mvwaddch(Win, Origin.first + i, Origin.second + j, ' ');
                }
            }

            // Render each entry
            for (int i = 0; i < Entries.size(); i++) {
                std::vector<std::vector<int>> tempColors;

                if (!checkForBan(i)) {
                    for (int j = 0; j < Entries[i].size(); j++) {
                        tempColors.emplace_back();

                        for (int k = 0; k < Entries[i][j].first.size(); k++) {
                            if (Entries[i][j].first[k] >= 0) {
                                tempColors[j].emplace_back(Entries[i][j].first[k]);
                                Entries[i][j].first[k] = BannedEntries.first;
                            }
                        }
                    }

                    customText(Win, {Origin.first + Spacing * i, Origin.second + 2}, Entries[i]);

                    for (int j = 0; j < Entries[i].size(); j++) {
                        for (int k = 0; k < Entries[i][j].first.size(); k++) {
                            if (Entries[i][j].first[k] == BannedEntries.first) {
                                Entries[i][j].first[k] = tempColors[j][0];
                                tempColors[j].erase(tempColors[j].begin());
                            }
                        }
                    }
                }
                else {
                    customText(Win, {Origin.first + Spacing * i, Origin.second + 2}, Entries[i]);
                }
            }
        }

        // The option to add a custom starting position is available
        // Return -2 for a soft quit, -1 for a hard quit, or any number greather than or equal to 0 for a chosen index
        std::pair<bool, int> start(int location = 0, bool feedbackMode = false) {
            render();

            if (location < 0 || location > Entries.size() - 1) {
                location = 0;
            }

            while (true) {
                // Render cursor
                wattron(Win, COLOR_PAIR(CursorColor));
                wattron(Win, A_BOLD);
                mvwaddch(Win, Origin.first + location * Spacing, Origin.second, '<');
                mvwaddch(Win, Origin.first + location * Spacing, Origin.second + LineWidths[location], '>');
                wattroff(Win, A_BOLD);
                wattroff(Win, COLOR_PAIR(CursorColor));

                int input = wgetch(Win);

                // Remove cursors
                for (int i = 0; i < LineWidths.size(); i++) {
                    mvwaddch(Win, Origin.first + i * Spacing, Origin.second, ' ');
                    mvwaddch(Win, Origin.first + i * Spacing, Origin.second + LineWidths[i], ' ');
                }

                switch (input) {
                    case KEY_UP:
                        location = (location < 1) ? location = Entries.size() - 1 : location - 1;
                        
                        if (feedbackMode) {
                            return {false, location};
                        }
                        break;
                    case KEY_DOWN:
                        location = (location > Entries.size() - 2) ? location = 0 : location + 1;

                        if (feedbackMode) {
                            return {false, location};
                        }
                        break;
                    case 'z':
                        if (checkForBan(location)) {
                            return {true, location};
                        }
                        break;
                    case 'x':
                        if (CanQuit) {
                            return {true, -2};
                        }
                        break;
                    case 'q':
                        return {true, -1};
                        break;
                }
            }
        }
};

#endif
