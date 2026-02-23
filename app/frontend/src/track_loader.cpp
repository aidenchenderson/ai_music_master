#include "track_loader.hpp"
#include <fstream>
#include <sstream>

// JSON parser for tracks
bool loadTrack(const std::string& filename, Track& track) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    // clear any existing track data
    track.title.clear();
    track.timeSignature.clear();
    track.bpm = 0;
    track.bars.clear();

    std::string line;
    std::string content;
    while (std::getline(file, line)) {
        content += line;
    }
    file.close();

    size_t titlePos = content.find("\"title\":");
    if (titlePos != std::string::npos) {
        size_t start = content.find("\"", titlePos + 8) + 1;
        size_t end = content.find("\"", start);
        track.title = content.substr(start, end - start);
    }

    // extract bpm
    size_t bpmPos = content.find("\"bpm\":");
    if (bpmPos != std::string::npos) {
        size_t start = bpmPos + 6;
        while (content[start] == ' ' || content[start] == ':') start++;
        size_t end = start;
        while (content[end] >= '0' && content[end] <= '9') end++;
        track.bpm = std::stoi(content.substr(start, end - start));
    }

    // extract time signature
    size_t tsPos = content.find("\"timeSignature\":");
    if (tsPos != std::string::npos) {
        size_t start = content.find("\"", tsPos + 16) + 1;
        size_t end = content.find("\"", start);
        track.timeSignature = content.substr(start, end - start);
    }

    // parse bars
    size_t barsPos = content.find("\"bars\":");
    if (barsPos != std::string::npos) {
        size_t currentPos = barsPos;
        
        while (true) {
            // find next bar
            size_t barStart = content.find("\"number\":", currentPos);
            if (barStart == std::string::npos) break;

            Bar bar;
            
            // extract bar number
            size_t numStart = barStart + 9;
            while (content[numStart] == ' ' || content[numStart] == ':') numStart++;
            size_t numEnd = numStart;
            while (content[numEnd] >= '0' && content[numEnd] <= '9') numEnd++;
            bar.number = std::stoi(content.substr(numStart, numEnd - numStart));

            // find beats array for this bar
            size_t beatsStart = content.find("\"beats\":", barStart);
            size_t beatsArrayStart = content.find("[", beatsStart);
            size_t beatsArrayEnd = content.find("]", beatsArrayStart);

            // parse all notes in this bar
            size_t notePos = beatsArrayStart;
            while (notePos < beatsArrayEnd) {
                size_t beatKey = content.find("\"beat\":", notePos);
                if (beatKey == std::string::npos || beatKey > beatsArrayEnd) break;

                Note note;
                
                // extract beat
                size_t beatStart = beatKey + 7;
                while (content[beatStart] == ' ' || content[beatStart] == ':') beatStart++;
                size_t beatEnd = beatStart;
                while (content[beatEnd] >= '0' && content[beatEnd] <= '9') beatEnd++;
                note.beat = std::stoi(content.substr(beatStart, beatEnd - beatStart));

                // extract string
                size_t stringKey = content.find("\"string\":", beatKey);
                size_t stringStart = stringKey + 9;
                while (content[stringStart] == ' ' || content[stringStart] == ':') stringStart++;
                size_t stringEnd = stringStart;
                while (content[stringEnd] >= '0' && content[stringEnd] <= '9') stringEnd++;
                note.string = std::stoi(content.substr(stringStart, stringEnd - stringStart));

                // extract fret
                size_t fretKey = content.find("\"fret\":", stringKey);
                size_t fretStart = fretKey + 7;
                while (content[fretStart] == ' ' || content[fretStart] == ':') fretStart++;
                size_t fretEnd = fretStart;
                while (content[fretEnd] >= '0' && content[fretEnd] <= '9') fretEnd++;
                note.fret = std::stoi(content.substr(fretStart, fretEnd - fretStart));

                bar.beats.push_back(note);
                notePos = fretEnd + 1;
            }

            track.bars.push_back(bar);
            currentPos = beatsArrayEnd + 1;
        }
    }

    return true;
}