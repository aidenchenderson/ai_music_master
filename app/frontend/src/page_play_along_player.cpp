#include <ncurses.h>
#include <string>
#include <chrono>
#include <thread>
#include <iostream>
#include "ui_pages.hpp"
#include "recording_session.hpp"
#include "feature_writer.hpp"
#include "gpio_buttons.hpp"

#include "grader.hpp"
#include "llm.hpp"
#include "string_utils.hpp"


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

PageResult runPlayAlongPlayerPage(WINDOW* win, const UIContext& ctx, GPIOButtons& gpio_buttons) {
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
    auto lastBeatTime = std::chrono::steady_clock::now();

    const int tabStartX = 2;
    const int tabStartY = 3;
    const int barWidth = 22;
    const int barGap = barWidth;

    while (playing) {
        nodelay(win, TRUE);

        int hw = gpio_buttons.getKey();
        if (hw != -1) {
            ungetch(hw);
        }

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

    std::vector<BeatFrameData> beatData;

    auto melFrames = session.getBeatAlignedFrames();

    for (const auto& frame : melFrames) {

        BeatFrameData beat;

        beat.bar_index = frame.barIndex;
        beat.beat_index = frame.beatIndex;

        beat.detected_frequency = estimatePitch(frame.melFrame);
        beat.energy = computeEnergy(frame.melFrame);

        beat.time_stamp_ms = 0;

        beatData.push_back(beat);
    }

    SessionGrade grade = PlayAlongGrader::grade_session(ctx.trackData, beatData);

    std::string prompt =
    "Role: Professional Guitar Teacher.\n"
    "Task: Provide immediate, direct feedback based on scores (be critical where needed but fluff some compliments where they did well).\n"
    "Constraint 1: Start your response immediately with the critique.\n"
    "Constraint 2: DO NOT use conversational filler like 'Okay,' 'Let's see,' or 'Here is your feedback.'\n"
    "Constraint 3: No headings, no bullets, max 3 sentences.\n\n"
    "Include what went well, what needs improvement, and one practice tip.\n"
    
    "Scores:\n"
    "- Timing: " + std::to_string(grade.timing_score) + "/100\n"
    "- Pitch: " + std::to_string(grade.pitch_score) + "/100\n\n"
    "- Consistency: " + std::to_string(grade.consistency_score) + "/100\n"
    
    "Feedback (start directly):";

    LLM::Request req;
    req.baseUrl = "http://localhost:11434";
    req.llmModel = "gemma3:1b";
    req.prompt = prompt;

    LLM::Result llmResult = LLM::Generate(req);

    UIContext nextCtx = ctx;
    nextCtx.lastGrade = grade;

    if (llmResult.status == LLM::Status::Ok) {
        nextCtx.llmFeedback = sanitizeAscii(llmResult.text);
    } else {
        nextCtx.llmFeedback = "LLM feedback unavailable.";
    }

    return {PageId::Summary, nextCtx};
}
