#ifndef GRADER_H
#define GRADER_H

#include <vector>
#include "beat_data.hpp"
#include "track_types.hpp"

// grade for a single beat
struct BeatGrade {
    int bar;
    int beat;
    bool note_played;
    float timing_error_ms;
    float pitch_error_cents;
    float score;
};

// final grade for entire session
struct SessionGrade {
    float overall_score = 0.0f;
    float timing_score = 0.0f;
    float pitch_score = 0.0f;
    float consistency_score = 0.0f;
    std::vector<BeatGrade> beat_grades;
};

class PlayAlongGrader {
public:
    static SessionGrade grade_session(const Track& track, const std::vector<BeatFrameData>& recordedBeats);

private:
    static float get_expected_frequency(int stringIndex, int fret);
    static float compute_pitch_error_cents(float detected, float expected);
};

#endif // GRADER_H