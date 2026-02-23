#include "grader.hpp"
#include <cmath>
#include <map>

// standard guitar tuning
static const float OPEN_STRING_FREQ[6] = {
    82.41f,   // E
    110.00f,  // A
    146.83f,  // D
    196.00f,  // G
    246.94f,  // B
    329.63f   // e
};

float PlayAlongGrader::get_expected_frequency(int string_index, int fret) {
    if (string_index < 0 || string_index >= 6) return 0.0f;

    float open_freq = OPEN_STRING_FREQ[string_index];
    return open_freq * std::pow(2.0f, fret / 12.0f);
}

float PlayAlongGrader::compute_pitch_error_cents(float detected, float expected) {
    if (detected <= 0.0f || expected <= 0.0f) return 9999.0f;
    return 1200.0f * std::log2(detected / expected);
}

SessionGrade PlayAlongGrader::grade_session(const Track& track, const std::vector<BeatFrameData>& recorded_beats) {
    SessionGrade result;

    float total_timing_score = 0.0f;
    float total_pitch_score = 0.0f;
    int graded_beats = 0;

    double beat_duration_ms = 60000.0 / track.bpm;

    std::map<std::pair<int,int>, std::pair<int,int>> expected_notes;

    for (int bar_idx = 0; bar_idx < static_cast<int>(track.bars.size()); ++bar_idx) {
        for (const auto& note : track.bars[bar_idx].beats) { 
            expected_notes[{bar_idx, note.beat}] = {note.string, note.fret};
        }
    }

    for (const auto& beat : recorded_beats) {

        auto key = std::make_pair(beat.bar_index, beat.beat_index);

        if (expected_notes.find(key) == expected_notes.end())
            continue;

        BeatGrade grade{};
        grade.bar = beat.bar_index;
        grade.beat = beat.beat_index;

        graded_beats++;

        auto [string_index, fret] = expected_notes[key];
        float expected_freq = get_expected_frequency(string_index, fret);

        double expectedTimeMs = (beat.bar_index * 4 + (beat.beat_index - 1)) * beat_duration_ms;

        grade.timing_error_ms = std::abs(beat.time_stamp_ms - expectedTimeMs);

        float timingScore = 0.0f;
        if (grade.timing_error_ms < 40) timingScore = 100;
        else if (grade.timing_error_ms < 80) timingScore = 80;
        else if (grade.timing_error_ms < 120) timingScore = 60;
        else timingScore = 30;

        float cents_error = compute_pitch_error_cents(beat.detected_frequency, expected_freq);

        grade.pitch_error_cents = cents_error;

        float pitch_score = 0.0f;
        float abs_cents = std::abs(cents_error);

        if (abs_cents < 20) pitch_score = 100;
        else if (abs_cents < 40) pitch_score = 80;
        else if (abs_cents < 70) pitch_score = 60;
        else pitch_score = 30;

        grade.score = 0.5f * timingScore + 0.5f * pitch_score;
        grade.note_played = beat.energy > 0.01f;

        total_timing_score += timingScore;
        total_pitch_score += pitch_score;

        result.beat_grades.push_back(grade);
    }

    if (graded_beats > 0) {
        result.timing_score = total_timing_score / graded_beats;
        result.pitch_score = total_pitch_score / graded_beats;
        result.consistency_score = 100.0f;
        result.overall_score = 0.4f * result.timing_score + 0.4f * result.pitch_score + 0.2f * result.consistency_score;
    }

    return result;
}