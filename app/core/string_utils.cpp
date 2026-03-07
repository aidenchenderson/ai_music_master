#include "string_utils.hpp"
#include <string>
#include <sstream>
#include <vector>

std::vector<std::string> wrapText(const std::string& text, int width) {
    std::vector<std::string> lines;
    std::istringstream iss(text);
    std::string word;
    std::string line;

    while (iss >> word) {
        if ((int)(line.length() + word.length() + 1) > width) {
            lines.push_back(line);
            line = word;
        } else {
            if (!line.empty()) line += " ";
            line += word;
        }
    }
    if (!line.empty()) lines.push_back(line);
    return lines;
}


std::string sanitizeAscii(const std::string& s) {
    std::string out;

    for (size_t i = 0; i < s.size(); ++i) {
        unsigned char c = s[i];

        // UTF-8 apostrophe ’
        if (i + 2 < s.size() &&
            (unsigned char)s[i] == 0xE2 &&
            (unsigned char)s[i+1] == 0x80 &&
            (unsigned char)s[i+2] == 0x99) {
            out += '\'';
            i += 2;
            continue;
        }

        if (c >= 32 && c <= 126) {
            out += c;
        } else {
            out += ' ';
        }
    }

    return out;
}