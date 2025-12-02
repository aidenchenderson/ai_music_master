#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <portaudio.h>

#include "audio_input.hpp"

#define SAMPLE_RATE 48000
#define FRAMES_PER_BUFFER 512

int main(int argc, char** argv) {
    PaError err;
    err = Pa_Initialize();
    if (err != paNoError) {
        std::cout << "error initializing port audio: " << Pa_GetErrorText(err) << "\n";
        return 1;
    }

    int num_devices = Pa_GetDeviceCount();
    std::cout << "number of devices: " << num_devices << "\n";

    if (num_devices < 0) {
        std::cout << "error getting device count\n";
        return 1;
    } else if (num_devices == 0) {
        std::cout << "no audio devices detected\n";
        return 0;
    }

    const PaDeviceInfo* device_info;
    for (int i = 0; i < num_devices; i++) {
        device_info = Pa_GetDeviceInfo(i);
        std::cout << "device: " << i << "\n";
        std::cout << "\tname: " << device_info->name << "\n";
        std::cout << "\tmax input channels: " << device_info->maxInputChannels << "\n";
        std::cout << "\tmax output channels: " << device_info->maxOutputChannels << "\n";
        std::cout << "\tdefault sample rate: " << device_info->defaultSampleRate << "\n";
    }

    int device = 11;

    PaStreamParameters input_parameters;
    PaStreamParameters output_parameters;

    memset(&input_parameters, 0, sizeof(input_parameters));
    input_parameters.channelCount = 2;
    input_parameters.device = device;
    input_parameters.hostApiSpecificStreamInfo = NULL;
    input_parameters.sampleFormat = paInt16;
    input_parameters.suggestedLatency = Pa_GetDeviceInfo(device)->defaultLowInputLatency;

    memset(&output_parameters, 0, sizeof(output_parameters));
    output_parameters.channelCount = Pa_GetDeviceInfo(device)->maxOutputChannels;
    output_parameters.device = device;
    output_parameters.hostApiSpecificStreamInfo = NULL;
    output_parameters.sampleFormat = paInt16;
    output_parameters.suggestedLatency = Pa_GetDeviceInfo(device)->defaultLowInputLatency;

    AudioProcessor processor(SAMPLE_RATE);

    CallbackData callback_data;
    callback_data.input_channel_count = 2;
    callback_data.processor = &processor;

    PaStream* stream;
    err = Pa_OpenStream(&stream, &input_parameters, nullptr, SAMPLE_RATE, FRAMES_PER_BUFFER, paNoFlag, pa_stream_callback, &callback_data);
    if (err != paNoError) {
        std::cout << "error opening audio stream: " << Pa_GetErrorText(err) << "\n";
        return 1;
    }

    err = Pa_StartStream(stream);
    if (err != paNoError) {
        std::cout << "error starting audio stream: " << Pa_GetErrorText(err) << "\n";
        return 1;
    }

    Pa_Sleep(10 * 1000);

    err = Pa_StopStream(stream);
    if (err != paNoError) {
        std::cout << "error stopping audio stream: " << Pa_GetErrorText(err) << "\n";
        return 1;
    }

    err = Pa_CloseStream(stream);
    if (err != paNoError) {
        std::cout << "error closing audio stream: " << Pa_GetErrorText(err) << "\n";
        return 1;
    }

    const auto& spec = processor.getSpectrogram();

    std::ofstream out("spectrogram.csv");
    for (const auto& frame : spec) {
        for (size_t i = 0; i < frame.size(); i++) {
            out << frame[i];
            if (i + 1 < frame.size()) out << ",";
        }
        out << "\n";
    }
    out.close();

    err = Pa_Terminate();
    if (err != paNoError) {
        std::cout << "error terminating port audio: " << Pa_GetErrorText(err) << "\n";
        return 1;
    }

    return 0;
}