#include <ncurses.h>
#include <string>
#include <vector>
#include "ui_pages.hpp"
#include "ui_controller.hpp"
#include "track_loader.hpp"
#include "gpio_buttons.hpp"

PageResult runPlayAlongListPage(WINDOW* win, const UIContext& ctx, GPIOButtons& gpio_buttons) {
    std::vector<std::string> tracks = {
        "Track 01 - Blues Jam",
        "Track 02 - Rock Groove",
        "Track 03 - Jazz Swing",
        "Track 04 - Jazz Midnight",
        "Track 05 - Pop Groove",
        "Track 06 - Rock Chug",
        "Track 07 - 12-Bar Blues",
        "Track 08 - Pop Bounce",
        "Track 09 - Rock Skyline",
        "Track 10 - Pop Neon Lights"
    };
    
    std::vector<std::string> filenames = {
        "data/tracks/blues_jam.json",
        "data/tracks/rock_groove.json",
        "data/tracks/jazz_swing.json",
        "data/tracks/jazz_midnight.json",
        "data/tracks/pop_loop.json",
        "data/tracks/rock_chug.json",
        "data/tracks/blues_12bar.json",
        "data/tracks/pop_bounce.json",
        "data/tracks/rock_skyline.json",
        "data/tracks/pop_neon_lights.json"
    };

    int highlighted = 0;
    int scrollOffset = 0;
    int input = 0;

    int yWin, xWin;
    getmaxyx(win, yWin, xWin);

    int listTop = 3;
    int visibleCount = yWin - 8;

    while (true) {  
        werase(win);

        mvwprintw(win, 1, 2, "Play Along - Select a Track");
        mvwprintw(win, yWin - 2, 2, "** ENTER to choose, LEFT to go back **");

        for (int row = 0; row < visibleCount; ++row) {
            int trackIndex = scrollOffset + row;
            if (trackIndex >= (int)tracks.size()) break;

            int y = listTop + row;

            if (trackIndex == highlighted) {
                wattron(win, A_REVERSE);
            }

            mvwprintw(win, y, 4, "[%s]", tracks[trackIndex].c_str());

            if (trackIndex == highlighted) {
                wattroff(win, A_REVERSE);
            }
        }

        wrefresh(win);
        input = getInput(win, gpio_buttons);

        switch (input) {
            case KEY_UP:
                highlighted--;
                if (highlighted < 0) highlighted = (int)tracks.size() - 1;

                if (highlighted < scrollOffset) {
                    scrollOffset = highlighted;
                }
                break;

            case KEY_DOWN:
                highlighted++;
                if (highlighted >= (int)tracks.size()) highlighted = 0;

                if (highlighted >= scrollOffset + visibleCount) {
                    scrollOffset = highlighted - visibleCount + 1;
                }
                break;

            case 10:
            case KEY_ENTER: {
                UIContext nextCtx = ctx;
                nextCtx.selectedTrack = tracks[highlighted];
                nextCtx.trackFilename = filenames[highlighted];
                nextCtx.playAlong = true;

                if (!loadTrack(nextCtx.trackFilename, nextCtx.trackData)) {
                    continue;
                }
                
                return {PageId::PlayAlongPlayer, nextCtx};
            }

            case KEY_LEFT:
                return {PageId::MainMenu, ctx};
        }
    }
}
