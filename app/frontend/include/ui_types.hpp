#ifndef UI_TYPES_HPP
#define UI_TYPES_HPP

#include <string>
#include <vector>
#include "track_types.hpp"
#include "grader.hpp"

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
    bool playAlong = false;

    SessionGrade lastGrade;
    std::string llmFeedback;
};

struct PageResult {
    PageId nextPage;
    UIContext context;
};

#endif // UI_TYPES_HPP