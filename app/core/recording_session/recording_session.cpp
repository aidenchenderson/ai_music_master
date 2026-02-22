#include "recording_session.hpp"
#include <cstring>

RecordingSession::RecordingSession(int deviceIndex, int bpm_) : audio_processor(deviceIndex), bpm(bpm_) {
    if (bpm <= 0) bpm = 120;
    beatDurationMs = 60000 / bpm;
}

bool RecordingSession::start() {
    if (!audio_processor.start()) {
        return false;
    }

    running = true;
    startTime = std::chrono::steady_clock::now();

    return true;
}

void RecordingSession::stop() {
    running = false;
    audio_processor.stop();
}

void RecordingSession::process() {
    if (!running) {
        return;
    }

    audio_processor.process_available_audio(
        [&](const float* mel_frame) {
            if (!running) {
                return;
            }
                
            std::vector<float> frame(AudioConfig::NUM_MELS);
            std::memcpy(frame.data(), mel_frame, 40 * sizeof(float));

            allMelFrames.push_back(frame);

            auto now = std::chrono::steady_clock::now();
            auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count();

            int totalBeats = elapsedMs / beatDurationMs;
            int beatIndex = (totalBeats % 4) + 1;
            int barIndex  = totalBeats / 4;

            beatFrames.push_back({barIndex, beatIndex, frame});
        }
    );
}

std::vector<std::vector<float>> RecordingSession::getAllMelFrames() const {
    return allMelFrames;
}

std::vector<BeatFrameData> RecordingSession::getBeatAlignedFrames() const {
    return beatFrames;
}
