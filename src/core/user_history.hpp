
#ifndef _APP_USER_HISTORY_HPP_
#define _APP_USER_HISTORY_HPP_

#include <fstream>
#include <memory>
// TODO: use rapidjson / nlohmann json
// https://lokiastari.com/Json/Performance.linux.html#1.%20Parse
#include <json/json.h>

class UserHistory {
   public:
    UserHistory();
    static bool b(const std::string& key, bool def);
    static int i(const std::string& key, int def);
    static double d(const std::string& key, double def);
    static std::string s(const std::string& key, std::string def);

    template <typename T>
    static void set(const std::string& key, T value);

   private:
    UserHistory(const UserHistory&) = delete;
    UserHistory& operator=(const UserHistory&) = delete;
    static std::shared_ptr<UserHistory> instance();

    template <typename T>
    T get(const std::string& key, T def);

   private:
    std::string _filename;
    Json::Value _v;
};

template <typename T>
void UserHistory::set(const std::string& key, T value) {
    auto inst = instance();
    inst->_v[key] = value;

    std::ofstream f(inst->_filename);
    Json::StyledWriter writer;
    f << writer.write(inst->_v);
    f.close();
}

#endif
