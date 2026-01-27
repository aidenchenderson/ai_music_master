#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <cstring>
#include <fstream>

#include "miniaudio.h"
#include "audio_engine.hpp"
#include "feature_extractor.hpp"

int main() {
    // -------------------- 1. Enumerate capture devices --------------------
    ma_context context;
    if (ma_context_init(nullptr, 0, nullptr, &context) != MA_SUCCESS) {
        std::cerr << "Failed to init miniaudio context\n";
        return 1;
    }

    ma_device_info* capture_infos;
    ma_uint32 capture_count;
    ma_context_get_devices(&context, nullptr, nullptr, &capture_infos, &capture_count);

    std::cout << "Capture devices:\n";
    for (ma_uint32 i = 0; i < capture_count; ++i) {
        std::cout << "  [" << i << "] " << capture_infos[i].name << "\n";
    }
    ma_context_uninit(&context);

    // -------------------- 2. Initialize audio engine --------------------
    constexpr ma_uint32 MIC_INDEX = 5; // change if needed
    AudioEngine engine(MIC_INDEX);
    if (engine.init() != AudioEngine::InitResult::success) {
        std::cerr << "Failed to initialize AudioEngine\n";
        return 1;
    }

    engine.start();
    std::cout << "Recording for 10 seconds...\n";

    // -------------------- 3. Allocate recording buffer --------------------
    constexpr uint32_t SAMPLE_RATE = 48000;   // must match your AudioEngine sample rate
    constexpr uint32_t RECORD_SEC = 10;
    constexpr uint32_t CHUNK_SIZE = 1024;

    std::vector<float> recording(SAMPLE_RATE * RECORD_SEC);
    size_t samples_copied = 0;

    // -------------------- 4. Read from ring buffer in chunks --------------------
    while (samples_copied < recording.size()) {
        size_t to_read = CHUNK_SIZE;
        if (samples_copied + to_read > recording.size()) {
            to_read = recording.size() - samples_copied;
        }

        if (engine.read_chunk(recording.data() + samples_copied, to_read)) {
            samples_copied += to_read;
        } else {
            // wait a tiny bit for more data
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    engine.stop();
    std::cout << "Recording complete!\n";

    // -------------------- 5. Compute mel spectrogram --------------------
    FeatureExtractorConfig feat_cfg;
    feat_cfg.sample_rate = SAMPLE_RATE;
    feat_cfg.chunk_size  = CHUNK_SIZE;
    feat_cfg.fft_size    = 1024;
    feat_cfg.num_mels    = 40;
    feat_cfg.fmin        = 0.0f;
    feat_cfg.fmax        = -1.0f;      // auto to Nyquist
    feat_cfg.eps         = 1e-10f;
    feat_cfg.top_db      = 80.0f;

    FeatureExtractor extractor(feat_cfg);

    size_t num_frames = recording.size() / CHUNK_SIZE;
    std::vector<std::vector<float>> mel_spectrogram;

    for (size_t i = 0; i < num_frames; ++i) {
        const float* chunk = recording.data() + i * CHUNK_SIZE;
        const auto& mel = extractor.compute_mel_spectrogram(chunk);
        mel_spectrogram.push_back(mel);
    }

    // -------------------- 6. Save mel spectrogram as CSV --------------------
    std::ofstream out("mel_spectrogram.csv");
    for (const auto& row : mel_spectrogram) {
        for (size_t j = 0; j < row.size(); ++j) {
            out << row[j];
            if (j + 1 < row.size()) out << ",";
        }
        out << "\n";
    }

    std::cout << "Saved mel_spectrogram.csv\n";

    return 0;
}
