#ifndef RECORDING_SESSION_HPP
#define RECORDING_SESSION_HPP

#include <vector>
#include <chrono>
#include <atomic>
#include "real_time_audio_processor.hpp"
#include "ui_types.hpp"

struct BeatFrameData {
    int barIndex;
    int beatIndex;
    std::vector<float> melFrame;
};

class RecordingSession {
public:
    RecordingSession(int deviceIndex, int bpm);

    bool start();
    void stop();
    void process();

    std::vector<std::vector<float>> getAllMelFrames() const;
    std::vector<BeatFrameData> getBeatAlignedFrames() const;

private:
    RealtimeAudioProcessor audio_processor;

    int bpm;
    int beatDurationMs;

    std::chrono::steady_clock::time_point startTime;

    std::atomic<bool> running{false};

    std::vector<std::vector<float>> allMelFrames;
    std::vector<BeatFrameData> beatFrames;
};

#endif // RECORDING_SESSION_HPP