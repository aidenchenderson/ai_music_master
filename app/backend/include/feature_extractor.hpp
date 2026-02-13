#ifndef FEATURE_EXTRACTOR_H
#define FEATURE_EXTRACTOR_H

#include <vector>
#include <functional>
#include <cstdint>

extern "C" {
    #include "kiss_fftr.h"
}

struct FeatureExtractorConfig {
    uint32_t sample_rate = 48000;
    uint32_t fft_size = 2048;
    uint32_t hop_size = 512;
    uint32_t num_mels = 40;

    float fmin = 0.0f;
    float fmax = -1.0f;
    float top_db = 80.0f;
    float eps = 1e-10f;

    float silence_threshold = 1e-4f;
};

class FeatureExtractor {
public:
    using FrameCallback = std::function<void(const float* mel_frame)>;

    FeatureExtractor(const FeatureExtractorConfig& config);
    ~FeatureExtractor();

    void process_samples(
        const float* input,
        uint32_t num_samples,
        const FrameCallback& callback
    );

private:
    void compute_frame(const float* frame, float* out_mel);

    void build_hann_window();
    void build_mel_filterbank();

    static float hz_to_mel(float hz);
    static float mel_to_hz(float mel);

private:
    FeatureExtractorConfig config;

    std::vector<float> window;
    std::vector<float> fft_input;
    std::vector<kiss_fft_cpx> fft_output;
    std::vector<float> power_spectrum;
    std::vector<std::vector<float>> mel_filterbank;

    std::vector<float> sample_buffer;
    std::vector<float> frame_buffer;
    std::vector<float> mel_buffer;

    size_t write_pos = 0;
    size_t available_samples = 0;

    kiss_fftr_cfg fft_config = nullptr;
};

#endif // FEATURE_EXTRACTOR_H
