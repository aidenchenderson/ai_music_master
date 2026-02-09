#include <thread>
#include <atomic>
#include <mutex>
#include <vector>

#include "audio_engine.hpp"
#include "feature_extractor.hpp"
#include "app_controller.hpp"

AppController::AppController() {
    FeatureExtractorConfig config;
    extractor = new FeatureExtractor(config);
}

bool AppController::init_audio(int deviceIndex) {
    if (engine) delete engine;
    engine = new AudioEngine(deviceIndex);
    return engine->init() == AudioEngine::InitResult::success;
}

void AppController::start_capture() {
    if (is_running || !engine) return;
    engine->start();
    is_running = true;
    worker = std::thread(&AppController::processing_loop, this);
}

void AppController::processing_loop() {
    const uint32_t fft_size = 2048; 
    std::vector<float> buffer(fft_size);

    while (is_running) {
        if (engine->read_chunk(buffer.data(), fft_size)) {
            auto mels = extractor->compute_frame(buffer.data());
            
            std::lock_guard<std::mutex> lock(data_mutex);
            latest_mels = mels;
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }
}

void AppController::stop_capture() {
    is_running = false;
    if (worker.joinable()) worker.join();
    if (engine) engine->stop();
}

std::vector<float> AppController::get_latest_mels() {
    std::lock_guard<std::mutex> lock(data_mutex);
    return latest_mels;
}

AppController::~AppController() {
    stop_capture();
    delete engine;
    delete extractor;
}