#include "genre_inference.hpp"
#include <cstdlib>
#include <fstream>
#include "json.hpp"

bool runGenreInference(const std::string& csvPath, const std::string& outputJsonPath) {
    std::string command = "python3 scripts/genre_inference.py " + csvPath + " " + outputJsonPath;

    int result = system(command.c_str());

    return result == 0;
}

std::map<std::string, float> loadGenreResults(const std::string& jsonPath) {
    std::ifstream file(jsonPath);
    if (!file.is_open()) {
        return {};
    }

    nlohmann::json j;
    file >> j;

    std::map<std::string, float> results;

    for (auto& el : j.items()) {
        results[el.key()] = el.value();
    }

    return results;
}
