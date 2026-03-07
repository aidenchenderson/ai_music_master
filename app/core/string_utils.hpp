#ifndef STRING_UTILS_HPP
#define STRING_UTILS_HPP

#include <string>
#include <sstream>
#include <vector>

std::string sanitizeAscii(const std::string& input);
std::vector<std::string> wrapText(const std::string& text, int width);

#endif // STRING_UTILS_HPP