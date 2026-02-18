// #include <iostream>
#include <vector>
#include <functional>

#include "real_time_audio_processor.hpp"


RealtimeAudioProcessor::RealtimeAudioProcessor(int deviceIndex) : engine(deviceIndex), extractor(FeatureExtractorConfig()) {
    // preallocate buffer for pulling from audio engine ring buffer
    chunk_buffer.resize(CHUNK_FRAMES);
}


bool RealtimeAudioProcessor::start() {
    // avoid reinitializing the engine
    if (running) { 
        return true;
    }

    AudioEngine::InitResult result = engine.init();

    if (result != AudioEngine::InitResult::success) {
        // std::cerr << "AudioEngine init failed\n";
        return false;
    }

    engine.start();
    running = true;
    return true;
}


void RealtimeAudioProcessor::stop() {
    // avoid redundant stop calls
    if (!running) {
        return;
    }

    engine.stop();
    running = false;
}


void RealtimeAudioProcessor::process_available_audio(const FrameCallback& callback) {
    if (!running) {
        return;
    }

    // drain available audio chunks from the ring buffer
    while (true) {
        ma_uint32 frames_read = CHUNK_FRAMES;

        if (!engine.read_chunk(chunk_buffer.data(), frames_read)) {
            break;
        }

        extractor.process_samples(chunk_buffer.data(), frames_read, callback);
    }
}