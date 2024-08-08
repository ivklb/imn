
#ifndef CORE__CACHE_HPP
#define CORE__CACHE_HPP

#include <any>
#include <map>
#include <mutex>
#include <set>
#include <string>
#include <tuple>
#include <vector>

namespace imn {
namespace Cache {
const int kSizeHintB = 0;
const int kSizeHintKB = 1;
const int kSizeHintMB = 2;
const int kSizeHintGB = 3;

inline auto _get_cache() -> std::tuple<
                             std::map<std::string, std::any>&,
                             std::map<std::string, int>&,
                             std::mutex&> {
    static std::map<std::string, std::any> cache;
    static std::map<std::string, int> size_hint;
    static std::mutex mtx;
    return {cache, size_hint, mtx};
}

template <typename T>
void add(const std::string& key, const T& value, int hint = kSizeHintB) {
    auto [cache, size_hint, mtx] = _get_cache();
    std::lock_guard<std::mutex> lock(mtx);
    cache[key] = value;
    size_hint[key] = hint;
}

template <typename T>
T get(const std::string& key) {
    auto [cache, size_hint, mtx] = _get_cache();
    std::lock_guard<std::mutex> lock(mtx);
    return std::any_cast<T>(cache[key]);
}

inline bool has(const std::string& key) {
    auto [cache, size_hint, mtx] = _get_cache();
    std::lock_guard<std::mutex> lock(mtx);
    return cache.contains(key);
}

inline void clear(int size_ge) {
    auto [cache, size_hint, mtx] = _get_cache();
    std::lock_guard<std::mutex> lock(mtx);
    std::set<std::string> keys_to_remove;
    for (auto& [key, value] : cache) {
        if (size_hint[key] >= size_ge) {
            keys_to_remove.insert(key);
        }
    }
    for (auto& key : keys_to_remove) {
        cache.erase(key);
        size_hint.erase(key);
    }
}

inline void clear() {
    auto [cache, size_hint, mtx] = _get_cache();
    std::lock_guard<std::mutex> lock(mtx);
    cache.clear();
    size_hint.clear();
}

}  // namespace Cache
}  // namespace imn
#endif
