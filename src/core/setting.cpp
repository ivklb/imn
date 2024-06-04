
#include "setting.hpp"

#include <exception>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace fs = std::filesystem;
using json = nlohmann::json;


std::string _search_setting_file() {
    std::vector<fs::path> file_list = {
        fs::current_path() / "asset" / "config" / "setting.json",
        fs::current_path() / "asset" / "config" / "setting_default.json",
    };
    for (auto filename : file_list) {
        if (fs::exists(filename)) {
            return filename.string();
        }
    }
    return "";
}

MoonSettings _load_setting(const std::string& filename) {
    std::ifstream ifs(filename);
    try {
        json jf = json::parse(ifs, nullptr, true, true);
        return jf.get<MoonSettings>();
    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cerr << "Invalid setting file: " << filename << std::endl;
    }
    return {};
}

MoonSettings _load_setting() {
    auto filename = _search_setting_file();
    return _load_setting(filename);
}

MoonSettings& global_setting() {
    static MoonSettings setting = _load_setting();
    return setting;
}
