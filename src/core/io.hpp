
// load / dump

#include <fstream>
#include <iostream>
#include <string>


namespace Moon {

    void load(const std::string &filename, std::vector<uint8_t> &data) {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << filename << std::endl;
        }
    }
}
