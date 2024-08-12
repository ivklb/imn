
#ifndef CORE__I18N_HPP
#define CORE__I18N_HPP

#include <spdlog/spdlog.h>

#include <format>
#include <fstream>
#include <map>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#include "core/setting.hpp"

#define I18N_STR(key) imn::i18n::text(key).c_str()

using json = nlohmann::json;

namespace imn::i18n {
namespace impl {
std::map<std::string, json> lang_map;
bool init = false;

inline json parse_file(const std::string& filename) {
    std::ifstream ifs(filename);
    try {
        return json::parse(ifs, nullptr, true, true);
    } catch (std::exception& e) {
        SPDLOG_ERROR("fail to parse json file: {} {}", e.what(), filename);
    }
    return json{};
}

inline void init_text_map() {
    if (init) {
        return;
    }

    std::vector<std::string> lang_list = {"en", "zh"};
    for (const auto& lang : lang_list) {
        std::string filename = std::format("asset/config/lang/{}.json", lang);
        lang_map[lang] = parse_file(filename);
    }

    init = true;
}

inline std::string lang_text(const std::string& lang, const std::string& key) {
    if (impl::lang_map.contains(lang) && impl::lang_map[lang].contains(key)) {
        return impl::lang_map[lang][key];
    }
    return {};
}

}  // namespace impl

inline std::string text(const std::string& key) {
    impl::init_text_map();

    std::string rv;
    if (rv = impl::lang_text(setting::global_setting().lang, key), !rv.empty()) {
        return rv;
    } else if (rv = impl::lang_text(setting::global_setting().default_lang, key), !rv.empty()) {
        SPDLOG_DEBUG("no translation for key: {} , fallback to default lang", key);
        return rv;
    } else {
        SPDLOG_DEBUG("no translation for key: {}", key);
        return key;
    }
}

}  // namespace imn::i18n

#endif
