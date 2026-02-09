#ifndef APP_CONTROLLER_H
#define APP_CONTROLLER_H

#include <thread>
#include <atomic>
#include <mutex>
#include <vector>

#include "audio_engine.hpp"
#include "feature_extractor.hpp"

class AppController {
public:
    AppController();
    ~AppController();

    bool init_audio(int deviceIndex);
    void start_capture();
    void stop_capture();

    std::vector<float> get_latest_mels();
    bool is_active() const { return is_running; }

private:
    void processing_loop();

    AudioEngine* engine = nullptr;
    FeatureExtractor* extractor = nullptr;
    
    std::thread worker;
    std::atomic<bool> is_running{false};

    std::mutex data_mutex;
    std::vector<float> latest_mels;
};

#endif // APP_CONTROLLER_H