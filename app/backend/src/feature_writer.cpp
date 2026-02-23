#include "feature_writer.hpp"
#include <stdexcept>

FeatureWriter::FeatureWriter(const std::string& file_path) : file_path(file_path) {}

FeatureWriter::~FeatureWriter() {
    close_file();
}

void FeatureWriter::open_file(bool write_header, size_t feature_count) {
    file.open(file_path, std::ios::out | std::ios::trunc);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open CSV file: " + file_path);
    }

    if (write_header && feature_count > 0) {
        for (size_t i = 0; i < feature_count; ++i) {
            file << "f" << i;
            if (i < feature_count - 1) {
                file << ",";
            }
        }
        file << "\n";
    }
}

void FeatureWriter::write_row(const std::vector<float>& features) {
    if (!file.is_open()) {
        throw std::runtime_error("CSV file not open");
    }

    for (size_t i = 0; i < features.size(); ++i) {
        file << features[i];
        if (i < features.size() - 1) {
            file << ",";
        }
    }

    file << "\n";
}

void FeatureWriter::write_all(const std::vector<std::vector<float>>& all_features) {
    if (!file.is_open()) {
        throw std::runtime_error("CSV file not open");
    }

    for (const auto& row : all_features) {
        write_row(row);
    }
}

void FeatureWriter::close_file() {
    if (file.is_open()) {
        file.flush();
        file.close();
    }
}
