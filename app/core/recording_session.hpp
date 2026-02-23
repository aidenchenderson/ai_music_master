#ifndef RECORDING_SESSION_HPP
#define RECORDING_SESSION_HPP

#include <vector>
#include <chrono>
#include <atomic>
#include "real_time_audio_processor.hpp"
#include "ui_types.hpp"

struct BeatAlignedMelFrame {
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
    std::vector<BeatAlignedMelFrame> getBeatAlignedFrames() const;

private:
    RealtimeAudioProcessor audio_processor;

    int bpm;
    int beatDurationMs;

    std::chrono::steady_clock::time_point startTime;

    std::atomic<bool> running{false};

    std::vector<std::vector<float>> allMelFrames;
    std::vector<BeatAlignedMelFrame> beatFrames;

    int lastRecordedBeat = -1;
};

#endif // RECORDING_SESSION_HPP