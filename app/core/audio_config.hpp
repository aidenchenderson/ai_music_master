#ifndef AUDIO_CONFIG_H
#define AUDIO_CONFIG_H

#include <cstdint>

namespace AudioConfig {

// audio engine
static constexpr uint32_t SAMPLE_RATE = 48000;
static constexpr uint32_t CHANNELS = 1;
static constexpr uint32_t RECORD_SEC = 10;
static constexpr uint32_t CHUNK_FRAMES = 2048;

// feature extraction
static constexpr uint32_t FFT_SIZE = 2048;
static constexpr uint32_t HOP_SIZE = 512;
static constexpr uint32_t NUM_MELS = 40;

// recording
static constexpr double FREESTYLE_SECONDS = 7.0;

}

#endif // AUDIO_CONFIG_H