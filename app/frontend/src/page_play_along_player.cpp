#include <ncurses.h>
#include <string>
#include <chrono>
#include <thread>
#include <iostream>
#include "ui_pages.hpp"
#include "recording_session.hpp"
#include "feature_writer.hpp"

void renderTabBar(WINDOW* win, const Bar& bar, int startX, int startY) {
    const char* strings[] = {"e", "B", "G", "D", "A", "E"};

    for (int i = 0; i < 6; i++) {
        mvwprintw(win, startY + i, startX, "%s----|----|----|----", strings[i]);
    }

    for (const Note& note : bar.beats) {
        int stringIdx = 6 - note.string;
        int xPos = startX + 2 + (note.beat - 1) * 5;

        mvwprintw(win, startY + stringIdx, xPos, "%2d", note.fret);
    }
}

void renderEmptyBar(WINDOW* win, int startX, int startY) {
    const char* strings[] = {"e", "B", "G", "D", "A", "E"};

    for (int i = 0; i < 6; i++) {
        mvwprintw(win, startY + i, startX, "%s----|----|----|----", strings[i]);
    }
}

void renderEndBar(WINDOW* win, int startX, int startY) {
    renderEmptyBar(win, startX, startY);
    mvwprintw(win, startY + 2, startX + 8, "E N D");
}

PageResult runPlayAlongPlayerPage(WINDOW* win, const UIContext& ctx) {
    if (ctx.selectedDeviceIndex < 0) {
        werase(win);
        mvwprintw(win, 2, 2, "No capture device selected.");
        mvwprintw(win, 4, 2, "Press any key...");
        wrefresh(win);
        wgetch(win);
        return {PageId::MainMenu, ctx};
    }

    if (ctx.trackData.bpm <= 0 || ctx.trackData.bars.empty()) {
        werase(win);
        mvwprintw(win, 2, 2, "Invalid track data.");
        mvwprintw(win, 4, 2, "Press any key...");
        wrefresh(win);
        wgetch(win);
        return {PageId::MainMenu, ctx};
    }

    RecordingSession session(ctx.selectedDeviceIndex, ctx.trackData.bpm);

    if (!session.start()) {
        werase(win);
        mvwprintw(win, 2, 2, "Audio engine failed to start.");
        mvwprintw(win, 4, 2, "Press any key...");
        wrefresh(win);
        wgetch(win);
        return {PageId::MainMenu, ctx};
    }

    int currentBar = -2;
    int currentBeat = 1;
    bool playing = true;

    int beatDurationMs = 60000 / ctx.trackData.bpm;
    auto lastBeatTime  = std::chrono::steady_clock::now();

    const int tabStartX = 2;
    const int tabStartY = 3;
    const int barWidth  = 22;
    const int barGap    = barWidth;

    while (playing)
    {
        nodelay(win, TRUE);
        int input = wgetch(win);
        nodelay(win, FALSE);

        if (input == 27) {
            playing = false;
            break;
        }

        session.process();

        werase(win);

        // Header
        if (currentBar < 0) {
            int count = (currentBar + 2) * 4 + currentBeat;
            mvwprintw(win, 0, 2, "BPM:%d  Count:%d/8", ctx.trackData.bpm, count);
        } else {
            mvwprintw(win, 0, 2, "BPM:%d  Bar:%d/%zu", ctx.trackData.bpm, currentBar + 1, ctx.trackData.bars.size());
        }

        int leftBar  = currentBar;
        int rightBar = currentBar + 1;

        int indicatorX = tabStartX + 2 + (currentBeat - 1) * 5;
        mvwprintw(win, tabStartY - 1, indicatorX, "^");

        // left measure
        if (leftBar >= 0 && leftBar < (int)ctx.trackData.bars.size()) {
            renderTabBar(win, ctx.trackData.bars[leftBar], tabStartX, tabStartY);
        } else if (leftBar == (int)ctx.trackData.bars.size()) {
            renderEndBar(win, tabStartX, tabStartY);
        } else {
            renderEmptyBar(win, tabStartX, tabStartY);
        }

        // right measure
        if (rightBar >= 0 && rightBar < (int)ctx.trackData.bars.size()) {
            renderTabBar(win, ctx.trackData.bars[rightBar], tabStartX + barGap, tabStartY);
        } else if (rightBar == (int)ctx.trackData.bars.size()) {
            renderEndBar(win, tabStartX + barGap, tabStartY);
        } else {
            renderEmptyBar(win, tabStartX + barGap, tabStartY);
        }

        wrefresh(win);

        auto now = std::chrono::steady_clock::now();
        auto elapsed =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                now - lastBeatTime).count();

        if (elapsed >= beatDurationMs) {
            currentBeat++;

            if (currentBeat > 4) {
                currentBeat = 1;
                currentBar++;

                if (currentBar >= (int)ctx.trackData.bars.size()) {
                    playing = false;
                }
            }

            lastBeatTime = now;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    session.stop();

    auto frames = session.getAllMelFrames();

    if (!frames.empty()) {
        FeatureWriter writer("data/sessions/last_playalong_session.csv");
        writer.open_file(true, frames[0].size());
        writer.write_all(frames);
        writer.close_file();
    }

    UIContext nextCtx = ctx;
    nextCtx.lastSessionFeatures = session.getAllMelFrames();

    return {PageId::Summary, nextCtx};
}
