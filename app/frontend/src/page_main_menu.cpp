#include <ncurses.h>
#include <string>
#include <vector>
#include "ui_pages.hpp"

PageResult runMainMenuPage(WINDOW* win, const UIContext& ctx) {
    std::vector<std::string> buttons = {
        "Play Along",
        "Freestyle Mode",
        "Change Capture Devices",
        "Exit"
    };

    int highlighted = 0;
    int input = 0;

    int yWin, xWin;
    getmaxyx(win, yWin, xWin);
    (void)yWin;

    while (true) {
        werase(win);

        int buttonY = 3;
        int buttonX = 4;

        for (size_t i = 0; i < buttons.size(); i++) {
            if ((int)i == highlighted) {
                wattron(win, A_REVERSE);
            }
            int exitMultiplier = (i == buttons.size() - 1) ? 2 : 1;
            mvwprintw(win, buttonY + i + exitMultiplier, buttonX, "[%s]", buttons[i].c_str());
            if ((int)i == highlighted) {
                wattroff(win, A_REVERSE);
            }
        }

        wattron(win, A_BOLD);
        mvwprintw(win, 1, 2, "AI-Music-Master");
        wattroff(win, A_BOLD);
        mvwprintw(win, 12, 2, "** Use UP/DOWN and SELECT **");
        mvwprintw(win, 13, 2, "Current device: %s", ctx.selectedDevice.c_str());

        wrefresh(win);
        input = wgetch(win);

        switch (input) {
            case KEY_UP:
                highlighted--;
                if (highlighted < 0) highlighted = (int)buttons.size() - 1;
                break;
            case KEY_DOWN:
                highlighted++;
                if (highlighted >= (int)buttons.size()) highlighted = 0;
                break;
            case 10:
            case KEY_ENTER:
                if (highlighted == 0) {
                    UIContext nextCtx = ctx;
                    nextCtx.playAlong = true;
                    return {PageId::PlayAlongList, nextCtx};
                }
                if (highlighted == 1) {
                    UIContext nextCtx = ctx;
                    nextCtx.playAlong = false;
                    return {PageId::SoloStart, nextCtx};
                }
                if (highlighted == 2) {
                    return {PageId::DeviceSelect, ctx};
                }
                if (highlighted == 3) {
                    return {PageId::Exit, ctx};
                }
        }
    }
}