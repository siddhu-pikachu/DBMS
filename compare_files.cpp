#include <iostream>
#include <fstream>
#include <string>

bool compareBinaryFiles(const std::string& file1, const std::string& file2) {
    std::ifstream f1(file1, std::ios::binary);
    std::ifstream f2(file2, std::ios::binary);

    if (!f1.is_open() || !f2.is_open()) {
        std::cerr << "Error opening one or both files." << std::endl;
        return false;
    }

    // Check if the files have the same size
    f1.seekg(0, std::ios::end);
    f2.seekg(0, std::ios::end);
    if (f1.tellg() != f2.tellg()) {
        return false; // Files have different sizes
    }

    // Reset to the beginning of the files for byte-by-byte comparison
    f1.seekg(0);
    f2.seekg(0);

    // Compare files byte by byte
    char byte1, byte2;
    while (f1.get(byte1) && f2.get(byte2)) {
        if (byte1 != byte2) {
            return false; // Files differ at some byte
        }
    }

    return true; // Files are identical
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <file1> <file2>" << std::endl;
        return 1;
    }

    if (compareBinaryFiles(argv[1], argv[2])) {
        std::cout << "Files are identical." << std::endl;
        return 0;
    } else {
        std::cout << "Files are different." << std::endl;
        return 1;
    }
}
