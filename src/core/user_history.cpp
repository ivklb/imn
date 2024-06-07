
#include "app/user_history.hpp"

#include <filesystem>
#include <fstream>
#include "app/config/config.h"

namespace fs = std::filesystem;


UserHistory::UserHistory() {
    _filename = (fs::current_path() / "config" / "user.cache").u8string();
    if (fs::exists(_filename)) {
        std::ifstream json_file(_filename, std::ifstream::binary);
        json_file >> _v;
    }
}

std::shared_ptr<UserHistory> UserHistory::instance() {
    static std::shared_ptr<UserHistory> inst = std::make_shared<UserHistory>();
    return inst;
}

template<>
bool UserHistory::get<bool>(const std::string& key, bool def) {
    return _v.get(key, def).asBool();
}

template<>
int UserHistory::get<int>(const std::string& key, int def) {
    return _v.get(key, def).asInt();
}

template<>
double UserHistory::get<double>(const std::string& key, double def) {
    return _v.get(key, def).asDouble();
}

template<>
std::string UserHistory::get<std::string>(const std::string& key, std::string def) {
    return _v.get(key, def).asCString();
}

bool UserHistory::b(const std::string& key, bool def) {return instance()->get(key, def);}
int UserHistory::i(const std::string& key, int def) {return instance()->get(key, def);}
double UserHistory::d(const std::string& key, double def) {return instance()->get(key, def);}
std::string UserHistory::s(const std::string& key, std::string def) {return instance()->get(key, def);}
