
#ifndef CORE__I18N_HPP
#define CORE__I18N_HPP

#include <spdlog/spdlog.h>

#include <format>
#include <fstream>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#include "core/setting.hpp"

#define I18N_STR(key) imn::i18n::text(key).c_str()

using json = nlohmann::json;

namespace imn::i18n {
namespace impl {
json text_map;
json text_map_default;
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

    std::string filename = std::format("asset/config/lang/{}.json", setting::global_setting().default_lang);
    text_map_default = parse_file(filename);
    filename = std::format("asset/config/lang/{}.json", setting::global_setting().lang);
    text_map = parse_file(filename);

    init = true;
}
}  // namespace impl

inline std::string text(const std::string& key) {
    impl::init_text_map();

    if (impl::text_map.contains(key)) {
        return impl::text_map[key];
    } else if (impl::text_map_default.contains(key)) {
        SPDLOG_DEBUG("no translation for key: {} , fallback to default lang", key);
        return impl::text_map_default[key];
    } else {
        SPDLOG_DEBUG("no translation for key: {}", key);
        return key;
    }
}

}  // namespace imn::i18n

#endif
