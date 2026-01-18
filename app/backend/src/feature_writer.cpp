#include <string>
#include <fstream>
#include <vector>
#include <stdio.h> 

#include "../include/feature_writer.hpp"

FeatureWriter::FeatureWriter(const std::string& filePath) {
    // Initialize file path
    this->filePath = filePath;
}

FeatureWriter::~FeatureWriter() {
    // Close file if open
    if(file.is_open()){
        file.close();
    }
}

void FeatureWriter::openFile() {
    // Open file, throw error if fails
    file.open(this->filePath);
    if(!file.is_open()){
        throw std::runtime_error("Failed to open CSV file");
    }
    isOpen = true;
}

void FeatureWriter::writeRow(std::vector<float> &features){
    if(!isOpen){
        throw std::runtime_error("File not open");
    }
    // Write features in separate collumns of a new row
    for(int i = 0; i<features.size(); i++){
        file << features[i];
        if(i<features.size()-1){
            file << ",";
        }
    }
    file << "\n";
}

void FeatureWriter::closeFile(){
    // Flush file and close
    if(isOpen){
        file.flush();
        file.close();
        isOpen = false;
    }
}



