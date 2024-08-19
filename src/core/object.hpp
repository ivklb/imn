
#ifndef CORE__OBJECT_HPP
#define CORE__OBJECT_HPP

#include <functional>
#include <map>
#include <memory>
#include <ranges>
#include <string>
#include <type_traits>

namespace imn::core {

template <typename T>
class ObjectFactory {
   public:
    static void register_(const std::string& name, const std::function<std::shared_ptr<T>()>& creator) {
        _get_map().insert(std::make_pair(name, creator));
    }
    static std::shared_ptr<T> create(const std::string& name) {
        auto& map = _get_map();
        if (!map.contains(name)) {
            throw std::runtime_error("No creator for " + name);
        }
        return map[name]();
    }
    static std::vector<std::string> entries() {
        std::vector<std::string> keys;
        for (auto& [k, v] : _get_map()) {
            keys.push_back(k);
        }
        return keys;
    }

   private:
    static std::map<std::string, std::function<std::shared_ptr<T>()>>& _get_map() {
        static std::map<std::string, std::function<std::shared_ptr<T>()>> _map;
        return _map;
    }
};

// SFINAE (Substitution Failure Is Not An Error)
template <typename T>
struct has_registered_name {
    // Types "yes" and "no" are guaranteed to have different sizes,
    // specifically sizeof(yes) == 1 and sizeof(no) == 2.
    typedef char yes[1];
    typedef char no[2];

    template <typename C>
    static yes& test(decltype(&C::registered_name));

    template <typename>
    static no& test(...);

    // If the "sizeof" of the result of calling test<T>(nullptr) is equal to
    // sizeof(yes), the first overload worked and T has a nested type named
    // foobar.
    static const bool value = sizeof(test<T>(nullptr)) == sizeof(yes);
};

// CRTP (Curiously Recurring Template Pattern)
template <typename T, typename BaseType>
class AutoRegistered {
   public:
    virtual ~AutoRegistered() {}

   private:
    class Allocator {
       public:
        Allocator() {
            static_assert(has_registered_name<T>::value, "class has no member function named registered_name()");
            ObjectFactory<BaseType>::register_(T::registered_name(), []() -> std::shared_ptr<BaseType> {
                return std::make_shared<T>();
            });
        }
    };

    // use static val to register allocator function for class T;
    inline static Allocator _alloc;
};

}  // namespace imn::core

#endif
