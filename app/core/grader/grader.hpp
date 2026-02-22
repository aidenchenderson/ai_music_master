#ifndef GRADER_H
#define GRADER_H

#include <vector>
#include <cmath>
#include <map>

#include "beat_data.hpp"

// the expected data for the user to play
struct Note {
    int beat;
    int string_index;
    int fret;
};

struct Bar {
    std::vector<Note> notes;
};

struct Track {
    int bpm;
    std::vector<Bar> bars;
};

// grade for a single beat
struct BeatGrade {
    int bar;
    int beat;
    bool note_played;
    float timing_error_ms;
    float pitch_error_cents;
    float score;
};

// (final) grade for the entire session
struct SessionGrade {
    float overall_score;
    float timing_score;
    float pitch_score;
    float consistency_score;
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