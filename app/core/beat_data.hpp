#ifndef BEAT_DATA_H
#define BEAT_DATA_H

#include <vector>

struct BeatFrameData {
    int bar_index;
    int beat_index;
    float detected_frequency;
    float energy;
    double time_stamp_ms;
};

#endif // BEAT_DATA_H