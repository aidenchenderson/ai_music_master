#include <ncurses.h>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <cstring>
#include "ui_pages.hpp"

PageResult runSoloPlayerPage(WINDOW* win, const UIContext& ctx) {
    RealtimeAudioProcessor audio_processor(ctx.selectedDeviceIndex);

    if (!audio_processor.start()) {
        return {PageId::Summary, ctx};
    }

    bool playing = true;

    std::vector<std::vector<float>> collected_features;
    collected_features.reserve(5000);

    size_t frame_counter = 0;

    while (playing) {
        nodelay(win, TRUE);
        int input = wgetch(win);
        nodelay(win, FALSE);

        if (input == 27) {
            playing = false;
        }

        audio_processor.process_available_audio(
            [&](const float* mel_frame) {
                std::vector<float> frame(40);
                std::memcpy(frame.data(), mel_frame, 40 * sizeof(float));

                if (collected_features.size() < 5000) collected_features.push_back(std::move(frame));

                frame_counter++;
            }
        );

        werase(win);

        mvwprintw(win, 2, 2, "Solo Mode - Recording...");
        mvwprintw(win, 4, 2, "Press ESC to stop");
        mvwprintw(win, 6, 2, "Frames processed: %zu", frame_counter);
        mvwprintw(win, 7, 2, "Stored frames: %zu", collected_features.size());

        wrefresh(win);

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    audio_processor.stop();

    return {PageId::Summary, ctx};
}

