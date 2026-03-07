#include "recording_session.hpp"
#include "audio_config.hpp"
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
    lastRecordedBeat = -1;

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
        [&](const float* mel_frame)
        {
            if (!running) {
                return;
            }

            std::vector<float> frame(AudioConfig::NUM_MELS);
            std::memcpy(frame.data(), mel_frame, AudioConfig::NUM_MELS * sizeof(float));

            allMelFrames.push_back(frame);

            auto now = std::chrono::steady_clock::now();
            auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count();

            int totalBeats = elapsedMs / beatDurationMs;

            int beatIndex = (totalBeats % 4) + 1;
            int barIndex  = totalBeats / 4;

            int absoluteBeat = barIndex * 4 + (beatIndex - 1);

            if (absoluteBeat != lastRecordedBeat) {
                beatFrames.push_back({barIndex, beatIndex, frame});
                lastRecordedBeat = absoluteBeat;
            }
        }
    );
}

std::vector<std::vector<float>> RecordingSession::getAllMelFrames() const {
    return allMelFrames;
}

std::vector<BeatAlignedMelFrame> RecordingSession::getBeatAlignedFrames() const {
    return beatFrames;
}


float computeEnergy(const std::vector<float>& mel) {
    float sum = 0.0f;

    for (float v : mel) {
        sum += v;
    }

    return sum / mel.size();
}

float estimatePitch(const std::vector<float>& mel) {
    int maxIndex = 0;

    for (int i = 1; i < static_cast<int>(mel.size()); i++) {
        if (mel[i] > mel[maxIndex])
            maxIndex = i;
    }

    // really (extremely) rough estimates
    float minFreq = 80.0f;
    float maxFreq = 1000.0f;

    float binWidth = (maxFreq - minFreq) / mel.size();

    return minFreq + maxIndex * binWidth;
}