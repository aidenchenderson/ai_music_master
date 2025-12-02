#include "audio_input.hpp"

int pa_stream_callback(
    const void* input_buffer, 
    void* output_buffer, unsigned long frames_per_buffer, 
    const PaStreamCallbackTimeInfo* time_info, 
    PaStreamCallbackFlags status_flags, 
    void* user_data 
) {
    if (input_buffer == nullptr) {
        std::cout << "Input buffer is null";
        return 0;
    }
    
    const int16_t* input = static_cast<const int16_t*>(input_buffer);
    (void)output_buffer;

    CallbackData* data = static_cast<CallbackData*>(user_data);
    int input_channels = data->input_channel_count;

    int display_size = 100;
    std::cout << "\r";

    float left_volume = 0;
    float right_volume = 0;

    if (input_channels == 1) {
        left_volume = right_volume = 0;
        for (unsigned long i = 0; i < frames_per_buffer; i++) {
            float v = std::abs(input[i]) / 32768.0f;
            left_volume = std::max(left_volume, v);
            right_volume = std::max(right_volume, v);
        }
    } else if (input_channels == 2) {
        left_volume = right_volume = 0;
        for (unsigned long i = 0; i < frames_per_buffer * 2; i += 2) {
            float l = std::abs(input[i]) / 32768.0f;
            float r = std::abs(input[i+1]) / 32768.0f;
            left_volume  = std::max(left_volume, l);
            right_volume = std::max(right_volume, r);
        }
    }

    for (int i = 0; i < display_size; i++) {
        float proportion = i / static_cast<float>(display_size);
        if (proportion <= left_volume && proportion <= right_volume) {
            std::cout << "█";
        } else if (proportion <= left_volume) {
            std::cout << "▀";
        } else if (proportion <= right_volume) {
            std::cout << "▄";
        } else {
            std::cout << " ";
        }
    }

    fflush(stdout);

    data->processor->process(input, frames_per_buffer * data->input_channel_count);

    return 0;
}