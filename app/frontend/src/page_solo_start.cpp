#include <ncurses.h>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <cstring>
#include <cstdlib>
#include <array>
#include <memory>
#include <filesystem>
#include "ui_pages.hpp"
#include "feature_writer.hpp"
#include "audio_config.hpp"
#include "gpio_buttons.hpp"
#include "llm.hpp"
#include "string_utils.hpp"

std::string execPython(const std::string& cmd) {
    std::array<char, 128> buffer;
    std::string result;

    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }

    if (!result.empty() && result.back() == '\n') result.pop_back();

    return result;
}

PageResult runSoloPlayerPage(WINDOW* win, const UIContext& ctx, GPIOButtons& gpio_buttons) {
    RealtimeAudioProcessor audio_processor(ctx.selectedDeviceIndex);
    if (!audio_processor.start()) {
        return {PageId::Summary, ctx};
    }

    const double RECORD_SECONDS = AudioConfig::FREESTYLE_SECONDS;
    auto start_time = std::chrono::high_resolution_clock::now();

    std::vector<std::vector<float>> collected_features;
    collected_features.reserve(1000);

    while (true) {
        auto now = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration<double>(now - start_time).count();
        if (elapsed >= RECORD_SECONDS) break;

        audio_processor.process_available_audio(
            [&](const float* mel_frame) {
                std::vector<float> frame(AudioConfig::NUM_MELS);
                std::memcpy(frame.data(), mel_frame, AudioConfig::NUM_MELS * sizeof(float));
                collected_features.push_back(std::move(frame));
            }
        );

        werase(win);
        mvwprintw(win, 2, 2, "Freestyle! Show off your skills:");
        mvwprintw(win, 4, 2, "Time: %.2f / %.2f sec", elapsed, RECORD_SECONDS);
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
    } catch (const std::exception&) {
        saved = false;
    }

    werase(win);

    if (!saved) {
        mvwprintw(win, 4, 2, "ERROR: Failed to save CSV.");
        wrefresh(win);
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));
        return {PageId::Summary, ctx};
    }

    mvwprintw(win, 3, 2, "Recording complete:");
    wrefresh(win);

    std::string genre = "Unknown";
    float confidence = 0.0f;
    try {
        std::filesystem::path sourceDir = std::filesystem::path(__FILE__).parent_path();
        std::filesystem::path pythonScript = sourceDir / "../../AI/genre_inference.py";
        std::filesystem::path venvPython = sourceDir / "../../AI/.venv/bin/python3";

        std::string python_cmd = venvPython.string() + " " + pythonScript.string() + " --csv " + filename;
        std::string python_output = execPython(python_cmd);
        size_t comma_pos = python_output.find(',');
        if (comma_pos != std::string::npos) {
            genre = python_output.substr(0, comma_pos);
            confidence = std::stof(python_output.substr(comma_pos + 1));
            mvwprintw(win, 4, 2, "Predicted genre: %s (%.2f)", genre.c_str(), confidence);
        } else {
            mvwprintw(win, 9, 2, "Genre detection failed");
        }
    } catch (const std::exception&) {
        mvwprintw(win, 9, 2, "ERROR running genre detection");
    }

    wrefresh(win);

    UIContext nextCtx = ctx;

    if (genre != "Unknown") {
        LLM::Request req;
        req.baseUrl = "http://localhost:11434";
        req.llmModel = "gemma3:1b";

        req.prompt = 
            "You are a professional music teacher. Based on the genre '" + genre + 
            "', provide concise, human-readable feedback for a guitar player who just completed a freestyle session. "
            "Include:\n"
            "- One positive observation about the style or performance.\n"
            "- One suggestion to improve technique or musicality.\n"
            "- Optionally, a short fun fact about the genre.\n"
            "Limit the response to 3 sentences maximum. Avoid starting with 'Okay' or other filler. "
            "Make it engaging, accurate, and clear for display on a small terminal screen."
            "Do not include an introduction line such as 'Okay, ...'";
        
        LLM::Result res = LLM::Generate(req);
        if (res.status == LLM::Status::Ok) {
            nextCtx.llmFeedback = sanitizeAscii(res.text);
        } else {
            nextCtx.llmFeedback = "LLM feedback unavailable.";
        }
    }

    // whats this dude doing here?
    // std::this_thread::sleep_for(std::chrono::milliseconds(2500)); 
    return {PageId::Summary, nextCtx};
}