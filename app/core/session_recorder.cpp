// #include <iostream>
#include <vector>
#include <functional>

#include "session_recorder.hpp"


SessionRecorder::SessionRecorder(int deviceIndex) : engine(deviceIndex), extractor(FeatureExtractorConfig()) {
    // preallocate buffer for pulling from audio engine ring buffer
    chunk_buffer.resize(CHUNK_FRAMES);
}


bool SessionRecorder::start() {
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


void SessionRecorder::stop() {
    // avoid redundant stop calls
    if (!running) {
        return;
    }

    engine.stop();
    running = false;
}


void SessionRecorder::process_available_audio(const FrameCallback& callback) {
    if (!running) {
        return;
    }

    // drain available audio chunks from the ring buffer
    while (engine.read_chunk(chunk_buffer.data(), CHUNK_FRAMES)) {
        extractor.process_samples(chunk_buffer.data(), CHUNK_FRAMES, callback);
    }
}

