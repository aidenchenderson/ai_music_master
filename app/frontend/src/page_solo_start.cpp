#include <ncurses.h>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <cstring>

#include "ui_pages.hpp"
#include "feature_writer.hpp"
#include "audio_config.hpp"

PageResult runSoloPlayerPage(WINDOW* win, const UIContext& ctx) {
    RealtimeAudioProcessor audio_processor(ctx.selectedDeviceIndex);

    if (!audio_processor.start()) {
        return {PageId::Summary, ctx};
    }

    const double RECORD_SECONDS = AudioConfig::FREESTYLE_SECONDS;
    auto start_time = std::chrono::high_resolution_clock::now();

    std::vector<std::vector<float>> collected_features;
    collected_features.reserve(1000); // ~650 expected

    size_t frame_counter = 0;

    while (true) {
        auto now = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration<double>(now - start_time).count();

        if (elapsed >= RECORD_SECONDS) {
            break;
        }

        audio_processor.process_available_audio(
            [&](const float* mel_frame) {
                std::vector<float> frame(AudioConfig::NUM_MELS);
                std::memcpy(frame.data(), mel_frame, 40 * sizeof(float));
                collected_features.push_back(std::move(frame));
                frame_counter++;
            }
        );

        werase(win);

        mvwprintw(win, 2, 2, "Genre Detection - Recording...");
        mvwprintw(win, 4, 2, "Time: %.2f / %.2f sec", elapsed, RECORD_SECONDS);
        mvwprintw(win, 6, 2, "Frames processed: %zu", frame_counter);
        mvwprintw(win, 7, 2, "Stored frames: %zu", collected_features.size());

        wrefresh(win);

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    audio_processor.stop();

    std::string filename = "data/sessions/freestyle_session.csv";

    bool saved = true;

    try {
        FeatureWriter writer(filename);
        writer.open_file(true, AudioConfig::NUM_MELS);
        writer.write_all(collected_features);
        writer.close_file();
    }
    catch (const std::exception& e) {
        saved = false;
    }

    werase(win);

    if (saved) {
        mvwprintw(win, 4, 2, "Recording complete.");
        mvwprintw(win, 6, 2, "Saved to: %s", filename.c_str());
        mvwprintw(win, 7, 2, "Total frames: %zu", collected_features.size());
    } else {
        mvwprintw(win, 4, 2, "ERROR: Failed to save CSV.");
    }

    wrefresh(win);
    std::this_thread::sleep_for(std::chrono::milliseconds(1500));

    return {PageId::Summary, ctx};
}
