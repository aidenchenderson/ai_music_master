#ifndef GENRE_INFERENCE_H
#define GENRE_INFERENCE_H

#include <string>
#include <map>

bool runGenreInference(const std::string& csvPath, const std::string& outputJsonPath);
std::map<std::string, float> loadGenreResults(const std::string& jsonPath);

#endif // GENRE_INFERENCE_H