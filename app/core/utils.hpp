#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <string>
#include <sstream>
#include <cstdio>
#include <memory>
#include <array>

inline std::string sanitize_ascii(const std::string& input) {
    std::string out;
    for (unsigned char c : input) {
        if (c >= 32 && c <= 126) {
            out += c;
        }
        else if (c == '\n') {
            out += c;
        }
    }
    return out;
}

inline std::vector<std::string> wrap_text(const std::string& text, int width) {
    std::vector<std::string> lines;
    std::istringstream iss(text);
    std::string word;
    std::string line;

    while (iss >> word) {
        // If adding this word exceeds width
        if (!line.empty() && (int)line.size() + (int)word.size() + 1 > width) {
            lines.push_back(line);
            line = word; // Start new line with the word (no leading space)
        } else {
            if (!line.empty()) line += " ";
            line += word;
        }
    }
    
    if (!line.empty()) lines.push_back(line);
    return lines;
}

struct PcloseDeleter {
    void operator()(FILE* f) const { pclose(f); }
};

inline std::string getGenreFromPython(const std::string& csvPath) {
    std::string command = "TF_CPP_MIN_LOG_LEVEL=3 TF_ENABLE_ONEDNN_OPTS=0 python3 AI/genre_inference.py " + csvPath + " 2>/dev/null";
    
    std::array<char, 128> buffer;
    std::string lastLine;
    
    struct PcloseDeleter { void operator()(FILE* f) const { pclose(f); } };
    std::unique_ptr<FILE, PcloseDeleter> pipe(popen(command.c_str(), "r"));
    
    if (!pipe) return "Unknown";

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        std::string line = buffer.data();
        if(!line.empty() && line.back() == '\n') line.pop_back();
        if(!line.empty()) lastLine = line; 
    }
    
    return lastLine.empty() ? "Unknown" : lastLine;
}

#endif // UTILS_HPP