#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <fstream>

class FeatureWriter {
    private:
        std::string filePath;
        std::ofstream file;
        bool isOpen = false;

    public:
        FeatureWriter(const std::string& filePath);
        ~FeatureWriter();

        void openFile();
        void writeRow(std::vector<float> &features);
        void closeFile();


};