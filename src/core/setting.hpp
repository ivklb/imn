
#ifndef CORE__SETTING_HPP
#define CORE__SETTING_HPP

#include <string>
#include <vector>
#include <nlohmann/json.hpp>


struct MoonSettings {
    std::string font_file;
    float font_size;
    std::string lang;
    std::string default_lang;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MoonSettings,
        font_file,
        font_size,
        lang,
        default_lang
    )
};


MoonSettings& global_setting();

#endif
