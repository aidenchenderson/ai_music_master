#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <thread>
#include <chrono>

#include "audio_engine.hpp"

ma_uint32 device_index = 4;

/**
 * hardware integration test
 * verifies both the OS and microphone are accessible and delivering data
 */
TEST_CASE("AudioEngine Hardware Liveliness", "[audio]") {
    
    AudioEngine engine(device_index);
    
    // check for successful engine initialization
    REQUIRE(engine.init() == AudioEngine::InitResult::success);
    
    // start the audio stream and allow the buffer to fill
    engine.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    engine.stop();

    // check that signal data was moved to the ring buffer
    ma_pcm_rb* rb = engine.get_ring_buffer();
    ma_uint32 available = ma_pcm_rb_available_read(rb);
    
    CHECK(available > 0);
}