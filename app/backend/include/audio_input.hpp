#ifndef AUDIO_INPUT_H
#define AUDIO_INPUT_H

#include <algorithm>
#include <cmath>
#include <iostream>
#include <portaudio.h>
#include "audio_processing.hpp"

typedef struct CallbackData {
    int input_channel_count;
    AudioProcessor* processor;
} CallbackData;

int pa_stream_callback(
    const void* input_buffer, 
    void* output_buffer, unsigned long frames_per_buffer, 
    const PaStreamCallbackTimeInfo* time_info, 
    PaStreamCallbackFlags status_flags, 
    void* user_data 
);

#endif // AUDIO_INPUT_H