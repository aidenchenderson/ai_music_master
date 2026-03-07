    #ifndef UTILS_HPP
    #define UTILS_HPP

    #include <vector>
    #include <string>
    #include <sstream>

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

    #endif // UTILS_HPP