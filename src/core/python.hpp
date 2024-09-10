
#ifndef CORE__PY_HPP
#define CORE__PY_HPP

#include <functional>
#include <map>
#include <string>
#include <tuple>
#include <vector>

namespace imn::python {

void init();
size_t exec_async(const std::function<void()>& func);
void exec_sync(const std::function<void()>& func);
bool is_done(size_t task_id);
void wait_done(size_t task_id);

std::tuple<
    std::map<std::string, std::string>,
    std::vector<std::string> >
load_pins(const std::string& filename);

}  // namespace imn::python

#endif