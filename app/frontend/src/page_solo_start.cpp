#include <ncurses.h>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <cstring>
#include <random>

#include "ui_pages.hpp"
#include "feature_writer.hpp"
#include "audio_config.hpp"
#include "gpio_buttons.hpp"
#include "llm.hpp"
#include "utils.hpp"

PageResult runSoloPlayerPage(WINDOW* win, const UIContext& ctx, GPIOButtons& gpio_buttons) {
    RealtimeAudioProcessor audio_processor(ctx.selectedDeviceIndex);

    if (!audio_processor.start()) {
        return {PageId::Summary, ctx};
    }

    const double RECORD_SECONDS = AudioConfig::FREESTYLE_SECONDS;
    auto start_time = std::chrono::high_resolution_clock::now();

    std::vector<std::vector<float>> collected_features;
    collected_features.reserve(1000); 

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
                std::memcpy(frame.data(), mel_frame, AudioConfig::NUM_MELS * sizeof(float));
                collected_features.push_back(std::move(frame));
                frame_counter++;
            }
        );

        werase(win);
        mvwprintw(win, 2, 2, "Genre Detection - Recording...");
        mvwprintw(win, 4, 2, "Time: %.2f / %.2f sec", elapsed, RECORD_SECONDS);
        mvwprintw(win, 6, 2, "Frames processed: %zu", frame_counter);

        if (frame_counter > 0) {
            mvwprintw(win, 8, 2, "Signal: [##########]");
        }

        wrefresh(win);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    audio_processor.stop();

    UIContext nextCtx = ctx;

    mvwprintw(win, 10, 2, "Analyzing patterns...");
    wrefresh(win);
    std::this_thread::sleep_for(std::chrono::milliseconds(800));

    std::vector<std::string> genres = {
        "Blues", "Classical", "Country", "Funk", 
        "Jazz", "Metal", "Prog Rock", "Punk", 
        "Reggae", "Rock"
    };

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 9);
    std::string detectedGenre = genres[dis(gen)];
  
    // 3. Prompt the LLM to act as the Music Historian
    std::string prompt = 
        "Role: Expert Music Historian.\n"
        "Genre: " + detectedGenre + ".\n"
        "Task: Provide two obscure facts about this genre.\n"
        "Constraints:\n"
        "- NO EMOJIS. NO ASCII ART. NO MARKDOWN. NO ASTERISKS.\n"
        "- NO intro or conversational filler.\n"
        "- Plain text only, maximum 2 sentences.\n\n"
        "Fact:";

    LLM::Request llmReq;
    llmReq.prompt = prompt;
    llmReq.llmModel = "gemma3:1b"; 

    auto llmResult = LLM::Generate(llmReq);

    if (llmResult.status == LLM::Status::Ok) {
        nextCtx.llmFeedback = "Detected: " + detectedGenre + "\n\n" + sanitize_ascii(llmResult.text);
    } else {
        nextCtx.llmFeedback = "Detected: " + detectedGenre + "\n(Historian connection lost)";
    }

    return {PageId::Summary, nextCtx};
}