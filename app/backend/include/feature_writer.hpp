#ifndef H_FEATURE_WRITER
#define H_FEATURE_WRITER

#include <string>
#include <vector>
#include <fstream>

class FeatureWriter {
private:
    std::string file_path;
    std::ofstream file;

public:
    explicit FeatureWriter(const std::string& file_path);
    ~FeatureWriter();

    void open_file(bool write_header = false, size_t feature_count = 0);
    void write_row(const std::vector<float>& features);
    void write_all(const std::vector<std::vector<float>>& all_features);
    void close_file();
};

#endif // H_FEATURE_WRITER
