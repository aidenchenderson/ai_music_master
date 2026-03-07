#include <ncurses.h>
#include <string>

#include "ui_pages.hpp"
#include "ui_controller.hpp"
#include "gpio_buttons.hpp"
#include "string_utils.hpp"

PageResult runSummaryPage(WINDOW* win, const UIContext& ctx, GPIOButtons& gpio_buttons) {
    int input = 0;

    int yWin;
    int xWin;
    getmaxyx(win, yWin, xWin);

    while (true) {
        werase(win);

        mvwprintw(win, yWin / 28, xWin / 2, "Session Summary");
        mvwprintw(win, 2, 1, "Feedback:");

        auto lines = wrapText(ctx.llmFeedback, xWin - 4);
        int lineY = 4;
        for (const auto& l : lines) {
            mvwprintw(win, lineY++, 2, "%s", l.c_str());
        }
        
        wrefresh(win);
        input = getInput(win, gpio_buttons);
        if (input == 10 || input == KEY_ENTER) {
            return {PageId::MainMenu, ctx};
        }
    }
}