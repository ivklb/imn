
#ifndef CORE__PY_HPP
#define CORE__PY_HPP

#include <functional>

namespace imn::python {

void init();

void run();

void enqueue(const std::function<void()>& func);

}  // namespace imn::python

#endif