#ifndef GRADER_H
#define GRADER_H

#include <vector>
#include "beat_data.hpp"
#include "ui_types.hpp"
#include "session_types.hpp"

class PlayAlongGrader {
public:
    static SessionGrade grade_session(
        const Track& track,
        const std::vector<BeatFrameData>& recordedBeats
    );

private:
    static float get_expected_frequency(int stringIndex, int fret);
    static float compute_pitch_error_cents(float detected, float expected);
};

#endif // GRADER_H