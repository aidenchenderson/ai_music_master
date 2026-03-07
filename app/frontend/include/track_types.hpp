#ifndef TRACK_TYPES_HPP
#define TRACK_TYPES_HPP

#include <vector>
#include <string>

struct Note {
    int beat;
    int string;
    int fret;
};

struct Bar {
    int number;
    std::vector<Note> beats;
};

struct Track {
    std::string title;
    int bpm;
    std::string timeSignature;
    std::vector<Bar> bars;
};

#endif