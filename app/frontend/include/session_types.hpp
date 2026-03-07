#ifndef SESSION_TYPES_HPP
#define SESSION_TYPES_HPP

#include <vector>

struct BeatGrade {
    int bar;
    int beat;
    bool note_played;
    float timing_error_ms;
    float pitch_error_cents;
    float score;
};

struct SessionGrade {
    float overall_score = 0.0f;
    float timing_score = 0.0f;
    float pitch_score = 0.0f;
    float consistency_score = 0.0f;
    std::vector<BeatGrade> beat_grades;
};

#endif