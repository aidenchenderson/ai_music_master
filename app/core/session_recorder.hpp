#ifndef SESSION_RECORDER_HPP
#define SESSION_RECORDER_HPP

#include <vector>
#include <functional>
#include "audio_engine.hpp"
#include "feature_extractor.hpp"

class SessionRecorder {
public:
    using FrameCallback = std::function<void(const float* mel_frame)>;

    SessionRecorder(int deviceIndex);

    bool start();
    void stop();

    void process_available_audio(const FrameCallback& callback);

private:
    AudioEngine engine;
    FeatureExtractor extractor;

    std::vector<float> chunk_buffer;
    bool running = false;
};

#endif // SESSION_RECORDER_HPP
