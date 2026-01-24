#include <feature_writer.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <cstdio>
// Use this line in powershell to compile:
//g++ tests/test_feature_writer.cpp app/backend/src/feature_writer.cpp -Iapp/backend/include -o test_feature_writer.exe


void assertTrue(bool condition, const std::string& message) {
    if(condition) {
        std::cout << "Pass: " << message << std::endl;
    }
    else {
        std::cout << "Fail: " << message << std::endl;
    }
}

int main() {
    const std::string testFile = "test_output.csv";

    std::remove(testFile.c_str());

    std::cout << "Running FeatureWriter manual tests...\n";

    // Test 1: Open File
    FeatureWriter writer(testFile);

    try {
        writer.openFile();
        assertTrue(true, "openFile() opens file succesfully");
    } catch(...) {
        assertTrue(false, "openFile() opens file succesfully");
    }

    //Test 2: Write Row

    std::vector<float> features = {1.0f, 2.5f, 3.75f};

    try{
        writer.writeRow(features);
        assertTrue(true, "writeRow() writes data without throwing");
    } catch(...) {
        assertTrue(false, "writeRow() writes data without throwing");
    }

    writer.closeFile();

    // Test 3: Verify file contents

    std::ifstream in(testFile);
    std::string line;

    bool fileOpened = in.is_open();
    assertTrue(fileOpened, "CSV file can be opened for reading");

    if(fileOpened){
        std::getline(in, line);

        const std::string expected = "1,2.5,3.75";
        assertTrue(line == expected, "CSV row matches expected content");

    }
    in.close();

    //test 4: cleanup
    std::remove(testFile.c_str());

    std::cout << "Manual tests complete.\n";
    return 0;
}


