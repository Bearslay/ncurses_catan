#ifndef CUSTOMTEXT
#define CUSTOMTEXT

#include <ncurses.h>
#include <vector>
#include <string>

// Attribute Vector IDs:
// -1 = Bold, -2 = Italic, -3 = Underline
// 0 and any positive number are assumed to be a color pair
//
// Input Vector Example:
// {{{Attribute 1, Attribute 2, ...}, "Text"}, {{Attribute 1, Attribute 2, ...}, "More Text"}}
void customText(WINDOW *win, std::pair<int, int> origin, std::vector<std::pair<std::vector<int>, std::string>> text) {
    int offset = 0;
    for (int i = 0; i < text.size(); i++) {
        // Apply attributes
        for (int j = 0; j < text[i].first.size(); j++) {
            switch (text[i].first[j]) {
                case -1:
                    wattron(win, A_BOLD);
                    break;
                case -2:
                    wattron(win, A_ITALIC);
                    break;
                case -3:
                    wattron(win, A_UNDERLINE);
                    break;
                default:
                    wattron(win, COLOR_PAIR(text[i].first[j]));
                    break;
            }
        }

        // Print the text
        mvwprintw(win, origin.first, origin.second + offset, "%s", text[i].second.c_str());

        // Remove attributes
        for (int j = 0; j < text[i].first.size(); j++) {
            switch (text[i].first[j]) {
                case -1:
                    wattroff(win, A_BOLD);
                    break;
                case -2:
                    wattroff(win, A_ITALIC);
                    break;
                case -3:
                    wattroff(win, A_UNDERLINE);
                    break;
                default:
                    wattroff(win, COLOR_PAIR(text[i].first[j]));
                    break;
            }
        }

        offset += text[i].second.length();
    }
}

#endif
