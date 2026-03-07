#include <ncurses.h>
#include <string>
#include <sstream>

#include "ui_pages.hpp"
#include "ui_controller.hpp"
#include "gpio_buttons.hpp"
#include "utils.hpp"

PageResult runSummaryPage(WINDOW* win, const UIContext& ctx, GPIOButtons& gpio_buttons) {
    int input = 0;
    int yWin, xWin;
    getmaxyx(win, yWin, xWin);

    while (true) {
        werase(win);

        mvwprintw(win, 1, (xWin - 15)/2, "Session Summary");

        mvwprintw(win, 2, 1, "AI Feedback:");

        auto wrapped = wrap_text(ctx.llmFeedback, xWin - 4);

        int line = 3;
        const int marginX = 1;

        for (const auto& l : wrapped) {
            mvwprintw(win, line++, marginX, "%s", l.c_str());
            if (line >= yWin - 2) break;
        }

        mvwprintw(win, yWin - 1, (xWin - 24)/2, "** Press ENTER to return **");

        wrefresh(win);
        input = getInput(win, gpio_buttons);

        if (input == 10 || input == KEY_ENTER) {
            return {PageId::MainMenu, ctx};
        }
    }
}