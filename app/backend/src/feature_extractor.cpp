#include <cmath>
#include <algorithm>
#include <cstring>

#include "feature_extractor.hpp"


float FeatureExtractor::hz_to_mel(float hz) {
    return 2595.0f * std::log10(1.0f + hz / 700.0f);
}


float FeatureExtractor::mel_to_hz(float mel) {
    return 700.0f * (std::pow(10.0f, mel / 2595.0f) - 1.0f);
}


FeatureExtractor::FeatureExtractor(const FeatureExtractorConfig& c) : config(c) {
    // safety default to nyquist frequency
    if (config.fmax < 0.0f) {
        config.fmax = 0.5f * config.sample_rate;
    }

    window.resize(config.fft_size);
    fft_input.resize(config.fft_size, 0.0f);
    fft_output.resize(config.fft_size / 2 + 1);
    power_spectrum.resize(config.fft_size / 2 + 1);

    sample_buffer.resize(config.fft_size * 2);
    frame_buffer.resize(config.fft_size);
    mel_buffer.resize(config.num_mels);

    build_hann_window();
    build_mel_filterbank();

    fft_config = kiss_fftr_alloc(config.fft_size, 0, nullptr, nullptr);
}


FeatureExtractor::~FeatureExtractor() {
    if (fft_config) {
        free(fft_config);
    }
}


void FeatureExtractor::build_hann_window() {
    // catch divide by 0 errors
    if (config.fft_size <= 1) {
        window.assign(config.fft_size, 1.0f);
        return;
    }

    // compute hann window coefficients: w(n) = 0.5 * (1 - cos(2 * pi * n / (N - 1)))
    for (uint32_t i = 0; i < config.fft_size; ++i) {
        window[i] = 0.5f * (1.0f - std::cos(2.0f * M_PI * i / (config.fft_size - 1)));
    }
}


void FeatureExtractor::build_mel_filterbank() {
    // catch divide by 0 errors
    if (config.sample_rate <= 0) {
        return;
    }

    // triangular mel filters
    mel_filterbank.assign(config.num_mels, std::vector<float>(config.fft_size / 2 + 1, 0.0f));

    float mel_min = hz_to_mel(config.fmin);
    float mel_max = hz_to_mel(config.fmax);

    std::vector<float> mel_points(config.num_mels + 2);
    std::vector<int> bin_points(config.num_mels + 2);

    int max_bin = config.fft_size / 2;

    for (uint32_t i = 0; i < mel_points.size(); ++i) {
        // evenly space points in mel domain
        mel_points[i] = mel_min + (mel_max - mel_min) * i / (config.num_mels + 1);

        // convert mel to hz to fft bin index
        float hz = mel_to_hz(mel_points[i]);
        int bin_idx = static_cast<int>(std::floor((config.fft_size + 1)* hz / config.sample_rate));

        // clamp to valid fft bin range
        bin_points[i] = std::min(max_bin, bin_idx);
    }

    // build triangular filters
    for (uint32_t m = 1; m <= config.num_mels; ++m) {
        int left = bin_points[m - 1];
        int center = bin_points[m];
        int right = bin_points[m + 1];

        // rising slope
        int denom1 = center - left;
        if (denom1 > 0) {
            for (int k = left; k < center; ++k) {
                mel_filterbank[m - 1][k] = float(k - left) / denom1;
            }
        }

        // falling slope
        int denom2 = right - center;
        if (denom2 > 0) {
            for (int k = center; k < right; ++k) {
                mel_filterbank[m - 1][k] = float(right - k) / denom2;
            }
        }

        // normalize filter
        float sum = 0.0f;
        for (float v : mel_filterbank[m - 1]) {
            sum += v;
        }

        if (sum > 0.0f) {
            for (float& v : mel_filterbank[m - 1]) {
                v /= sum;
            }
        }
    }
}

void FeatureExtractor::compute_frame(const float* frame, float* out_mel) {
    float energy_sum = 0.0f;

    // apply window and get energy estimate
    for (uint32_t i = 0; i < config.fft_size; ++i) {
        fft_input[i] = frame[i] * window[i];
        energy_sum += std::abs(frame[i]);
    }

    // skip fft if below threshold
    if (energy_sum < config.silence_threshold) {
        for (uint32_t i = 0; i < config.num_mels; ++i) {
            out_mel[i] = -config.top_db;
        } 
        return;
    }

    kiss_fftr(fft_config, fft_input.data(), fft_output.data());

    for (size_t i = 0; i < power_spectrum.size(); ++i) {
        float re = fft_output[i].r;
        float im = fft_output[i].i;
        power_spectrum[i] = re * re + im * im;
    }

    float max_db = -1e9f;

    // apply mel filterbank and covert to db
    for (uint32_t m = 0; m < config.num_mels; ++m) {
        float sum = 0.0f;

        for (size_t k = 0; k < power_spectrum.size(); ++k)
            sum += power_spectrum[k] * mel_filterbank[m][k];

        float db = 10.0f * std::log10(sum + config.eps);
        out_mel[m] = db;
        max_db = std::max(max_db, db);
    }

    // range compression
    float min_db = max_db - config.top_db;

    for (uint32_t i = 0; i < config.num_mels; ++i) {
        out_mel[i] = std::max(out_mel[i], min_db);
    }
}


void FeatureExtractor::process_samples(const float* input, uint32_t num_samples, const FrameCallback& callback) {
    // add incoming samples to circular buffer
    for (uint32_t i = 0; i < num_samples; ++i) {
        sample_buffer[write_pos] = input[i];
        write_pos = (write_pos + 1) % sample_buffer.size();

        if (available_samples < sample_buffer.size()) {
            available_samples++;
        }
    }

    // while enough samples are present, process frames
    while (available_samples >= config.fft_size) {
        size_t start = (write_pos + sample_buffer.size() - available_samples) % sample_buffer.size();

        for (uint32_t i = 0; i < config.fft_size; ++i) {
            frame_buffer[i] = sample_buffer[(start + i) % sample_buffer.size()];
        }

        compute_frame(frame_buffer.data(), mel_buffer.data());
        callback(mel_buffer.data());
        available_samples -= config.hop_size;
    }
}

