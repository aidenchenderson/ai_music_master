#ifndef UI_TYPES_HPP
#define UI_TYPES_HPP

#include <string>
#include <vector>
#include "session_types.hpp"

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

enum class PageId {
    MainMenu,
    DeviceSelect,
    PlayAlongList,
    PlayAlongPlayer,
    SoloStart,
    Summary,
    Exit
};

struct UIContext {
    int selectedDeviceIndex = -1;
    std::string selectedDevice;
    std::string selectedTrack;
    std::string trackFilename;
    std::vector<std::vector<float>> lastSessionFeatures;
    Track trackData;
    bool playAlong;
    SessionGrade lastGrade;
    std::string llmFeedback;
    std::string sessionGenre;
};

struct PageResult {
    PageId nextPage;
    UIContext context;
};

#endif // UI_TYPES_HPP