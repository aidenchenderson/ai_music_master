#include <ncurses.h>
#include <string>
#include <vector>
#include "ui_pages.hpp"

static std::string clean_device_name(std::string name) {
    const std::vector<std::string> prefixes = {
        "Monitor of ",
        "Built-in Audio ",
        "USB Audio Device - ",
        "alsa_output.",
        "alsa_input.",
        "sysdefault:",
        "default:"
    };

    for (const auto& prefix : prefixes) {
        if (name.rfind(prefix, 0) == 0) {
            name = name.substr(prefix.length());
        }
    }

    return name;
}

static std::string shorten_device_name(const std::string& name, int maxWidth) {
    std::string cleaned = clean_device_name(name);
    if ((int)cleaned.size() <= maxWidth) return cleaned;
    if (maxWidth <= 3) return cleaned.substr(cleaned.size() - maxWidth);
    return "..." + cleaned.substr(cleaned.size() - (maxWidth - 3));
}


PageResult runDeviceSelectPage(WINDOW* win, const UIContext& ctx, const std::vector<std::string>& devices) {
    int highlighted = 0;
    int input = 0;
    int scrollOffset = 0;

    int yWin, xWin;
    getmaxyx(win, yWin, xWin);
    (void)yWin;

    int listTop = 3;
    int visibleCount = std::min(7, (int)devices.size());

    while (true) {
        werase(win);

        for (int row = 0; row < visibleCount; ++row) {
            int deviceIndex = scrollOffset + row;
            if (deviceIndex >= (int)devices.size()) break;

            int y = listTop + row;
            if (deviceIndex == highlighted) {
                wattron(win, A_REVERSE);
            }

            std::string label = shorten_device_name(devices[deviceIndex], xWin - 6);
            mvwprintw(win, y, 4, "[%s]", label.c_str());

            if (deviceIndex == highlighted) {
                wattroff(win, A_REVERSE);
            }
        }

        mvwprintw(win, 1, 2, "Select Device");
        mvwprintw(win, 13, 2, "** ENTER to select, LEFT to go back **");

        if (static_cast<int>(devices.size()) > visibleCount) {
            mvwprintw(win, 11, 4, "Device %d-%d of %zu", scrollOffset + 1, std::min(scrollOffset + visibleCount, (int)devices.size()), devices.size());
        }

        wrefresh(win);
        input = wgetch(win);

        switch (input) {
            case KEY_UP:
                highlighted--;
                if (highlighted < 0) highlighted = (int)devices.size() - 1;
                if (highlighted < scrollOffset) {
                    scrollOffset = highlighted;
                }
                break;
            case KEY_DOWN:
                highlighted++;
                if (highlighted >= (int)devices.size()) highlighted = 0;
                if (highlighted >= scrollOffset + visibleCount) {
                    scrollOffset = highlighted - visibleCount + 1;
                }
                break;
            case 10:
            case KEY_ENTER: {
                UIContext nextCtx = ctx;
                nextCtx.selectedDeviceIndex = highlighted;
                nextCtx.selectedDevice = devices[highlighted];
                return {PageId::MainMenu, nextCtx};
            }
            case KEY_LEFT:
                return {PageId::MainMenu, ctx};
        }
    }
}