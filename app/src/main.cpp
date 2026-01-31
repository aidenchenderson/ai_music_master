#include <iostream>
#include "ui_controller.hpp"

#include <vector>
#include <thread>
#include <chrono>
#include <cstring>
#include <fstream>

#include "miniaudio.h"
#include "audio_engine.hpp"
#include "feature_extractor.hpp"

// TODO: Include backend audio engine
// TODO: Include AI inference module

int main(int argc, char** argv) {
    std::cout << "Starting AI Music Master...\n" << std::endl;
    
    // Run the UI and get selected device
    std::string selectedDevice = runMainUI();
    
    std::cout << "Selected device: " << selectedDevice << std::endl;
    
    // TODO: Initialize audio engine with selected device
    // TODO: Load AI model
    // TODO: Process audio and run inference
    
    return 0;
}
