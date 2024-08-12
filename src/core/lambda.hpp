// Usage:
// #include "lambda.hpp"
//
// class Foo {
// public:
//     void run() { cout << "run" << endl; }
//     void run2(int a) { cout << a << endl; }
// };
//
// int main(int argc, char* argv[]) {
//     Foo* f = new Foo;
//     Lambda::store("foo", [](int a, int b) { return a + b; });
//     Lambda::store("bar", [](int a, int b) { return a - b; });
//     Lambda::store("run", [=]() { f->run();});
//     Lambda::store("run", [=]() { boost::bind(&Foo::run, f)();});
//     Lambda::store("run2", [=](int a) { f->run2(a);});
//     auto a = Lambda::call("foo", 2, 3);
//     auto b = Lambda::call("bar", 3, 2);
//     std::cout << std::any_cast<int>(a) << std::endl;
//     std::cout << std::any_cast<int>(b) << std::endl;
//     Lambda::call("run");
//     Lambda::call("run2", 1);
//     Lambda::call("run2", 2);
//     delete f;
//     return 0;
// }
// TODO: support following feature:
//     Lambda::store("run", &Foo::run, f);

#ifndef _UTIL_LAMBDA_HPP_
#define _UTIL_LAMBDA_HPP_

#include <spdlog/spdlog.h>

#include <any>
#include <exception>
#include <functional>  // for std::bind
#include <map>
#include <string>

namespace imn {

namespace lambda {
// https://stackoverflow.com/questions/59897767/c-storing-callbacks-with-variadic-arguments
class LambdaManager {
   public:
    template <typename F>
    void store(const std::string& key, F&& f) {
        using arg_tuple = typename function_traits<F>::arg_tuple;
        using return_type = typename function_traits<F>::return_type;

        myHash[key] = [f = std::forward<F>(f)](std::any args) -> std::any {
            if constexpr (std::is_same_v<return_type, void>) {
                std::apply(f, std::any_cast<arg_tuple&>(args));
                return {};
            } else {
                return std::apply(f, std::any_cast<arg_tuple&>(args));
            }
        };
    }
    template <typename... Args>
    std::any call(const std::string& key, Args... arg) {
        if (myHash.count(key) == 0) {
            throw std::runtime_error("LambdaManager::call: key not found: " + key);
        }
        return _call_func(myHash[key], arg...);
    }

   private:
    std::map<std::string, std::function<std::any(std::any)>> myHash;

    template <typename T>
    struct function_traits : public function_traits<decltype(&T::operator())> {
    };

    template <typename ClassType, typename ReturnType, typename... Args>
    struct function_traits<ReturnType (ClassType::*)(Args...) const> {
        using arg_tuple = std::tuple<Args...>;
        using return_type = ReturnType;
    };

    template <typename... Args>
    std::any _call_func(const std::function<std::any(std::any)>& f, Args&&... args) {
        return f(std::make_tuple(std::forward<Args>(args)...));
    }
};

inline LambdaManager& _load_manager() {
    static LambdaManager m;
    return m;
}

template <typename F>
void store(const std::string& key, F&& f) {
    auto& m = _load_manager();
    m.store(key, std::forward<F>(f));
}

template <typename... Args>
std::any call(const std::string& key, Args... arg) {
    auto& m = _load_manager();
    try {
        return m.call(key, std::forward<Args>(arg)...);
    } catch (const std::exception& e) {
        SPDLOG_ERROR("fail to call lambda function : {} {}", key, e.what());
        throw;
    }
}

}  // namespace lambda
}  // namespace imn

#endif
