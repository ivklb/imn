
#ifndef CORE__SETTING_HPP
#define CORE__SETTING_HPP

#include <string>
#include <vector>
#include <nlohmann/json.hpp>


struct IMNSettings {
    std::string font_file;
    std::string lang;
    std::string default_lang;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(IMNSettings,
        font_file,
        lang,
        default_lang
    )
};


IMNSettings& global_setting();

#endif
