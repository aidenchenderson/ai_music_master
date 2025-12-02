#ifndef AUDIO_PROCESSING_H
#define AUDIO_PROCESSING_H

#include <fftw3.h>
#include <cmath>
#include <vector>
#include <iostream>

#define FFT_SIZE 1024
#define OVERLAP 512

class AudioProcessor {
public:
    AudioProcessor(int sample_rate, int fft_size = FFT_SIZE, int overlap = OVERLAP)
        : sample_rate(sample_rate), fft_size(fft_size), overlap(overlap) {
        input_buffer.resize(fft_size);
        output_buffer.resize(fft_size);
        plan = fftw_plan_r2r_1d(fft_size, input_buffer.data(), output_buffer.data(),
                                FFTW_R2HC, FFTW_MEASURE);
    }

    ~AudioProcessor() {
        fftw_destroy_plan(plan);
    }

    void process(const int16_t* samples, size_t count) {
        size_t i = 0;
        while (i + fft_size <= count) {
            for (int j = 0; j < fft_size; j++) {
                input_buffer[j] = samples[i + j] / 32768.0; // normalize
            }
            fftw_execute(plan);

            std::vector<double> frame(fft_size/2);
            for (int k = 0; k < fft_size/2; k++) {
                double re = output_buffer[k];
                double im = (k == 0 || k == fft_size/2) ? 0 : output_buffer[fft_size-k];
                frame[k] = 20.0 * log10(std::sqrt(re*re + im*im) + 1e-6);
            }
            spectrogram.push_back(frame);
            i += (fft_size - overlap);
        }
    }

    // Accessor
    const std::vector<std::vector<double>>& getSpectrogram() const {
        return spectrogram;
    }

private:
    int sample_rate;
    int fft_size;
    int overlap;
    fftw_plan plan;
    std::vector<double> input_buffer;
    std::vector<double> output_buffer;
    std::vector<std::vector<double>> spectrogram; // time x frequency bins
};



#endif // AUDIO_PROCESSING_H