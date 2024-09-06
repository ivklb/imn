
#include "python.hpp"

#include <pybind11/embed.h>
#include <pybind11/functional.h>
#include <pybind11/numpy.h>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <functional>
#include <mutex>
#include <queue>
#include <string>
#include <vector>

#include "core/app.hpp"
#include "core/thread_pool.hpp"

namespace py = pybind11;

py::object create_array(std::vector<int>& shape, std::string& dtype) {
    size_t total = 1;
    std::vector<size_t> strides(shape.size());
    for (int i = shape.size() - 1; i >= 0; i--) {
        strides[i] = total;
        total *= shape[i];
    }

    void* buffer = nullptr;
    int bpp = 1;
    const char* format_descriptor = nullptr;
    if (dtype == "uint8") {
        buffer = new uint8_t[total];
        bpp = sizeof(uint8_t);
        format_descriptor = py::format_descriptor<uint8_t>::value;
    } else if (dtype == "uint16") {
        buffer = new uint16_t[total];
        bpp = sizeof(uint16_t);
        format_descriptor = py::format_descriptor<uint16_t>::value;
    } else if (dtype == "uint32") {
        buffer = new uint32_t[total];
        bpp = sizeof(uint32_t);
        format_descriptor = py::format_descriptor<uint32_t>::value;
    } else if (dtype == "float32") {
        buffer = new float[total];
        bpp = sizeof(float);
        format_descriptor = py::format_descriptor<float>::value;
    } else {
        throw std::runtime_error("Unsupported dtype: " + dtype);
    }
    std::for_each(strides.begin(), strides.end(), [&](size_t& s) { s *= bpp; });

    bool read_only = false;
    auto bb = py::memoryview::from_buffer(
        buffer,  // buffer pointer
        bpp,
        format_descriptor,
        shape,    // shape (rows, cols)
        strides,  // strides in bytes
        read_only);
    py::object np = py::module::import("numpy");
    py::object asarray = np.attr("asarray");
    return asarray(bb);
}

PYBIND11_EMBEDDED_MODULE(imn, m) {
    m.def("create_array", &create_array);
}

namespace imn::python {

static std::mutex _mutex{};
static std::queue<size_t> _queue_id{};
static std::queue<std::function<void()>> _queue_task{};
static size_t _next_id = 0;

void _run() {
    py::scoped_interpreter guard{};
    py::module_ mod_sys = py::module_::import("sys");
    py::object append_path_func = mod_sys.attr("path").attr("append");
    append_path_func("python/scripts");
    append_path_func("python/nodes");

    while (app::is_running()) {
        std::function<void()> func;
        {
            std::lock_guard lock(_mutex);
            if (_queue_task.size() == 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                continue;
            }
            func = _queue_task.front();
        }
        try {
            func();
        } catch (const py::error_already_set& e) {
            SPDLOG_ERROR("Python error: {}", e.what());
        }
        std::lock_guard lock(_mutex);
        _queue_id.pop();
        _queue_task.pop();
    }
}

std::tuple<
    std::map<std::string, std::string>,
    std::vector<std::string>>
load_pins_impl(const std::string& filename) {
    py::module_ mod_inspect = py::module_::import("inspect_util");
    py::tuple rv = mod_inspect.attr("inspect")(filename, "run");
    py::dict in_pin_dict = rv[0];
    py::list out_pin_list = rv[1];

    std::map<std::string, std::string> in_pins;
    std::vector<std::string> out_pins;
    for (auto item : in_pin_dict) {
        in_pins[py::str(item.first)] = py::str(item.second);
    }
    for (auto item : out_pin_list) {
        out_pins.push_back(py::str(item));
    }
    return {in_pins, out_pins};
}

void init() {
    pool::enqueue(_run);
}

size_t exec_sync(const std::function<void()>& func) {
    std::lock_guard guard(_mutex);
    auto task_id = _next_id++;
    _queue_id.push(task_id);
    _queue_task.push(func);
    return task_id;
}

bool is_done(size_t task_id) {
    std::lock_guard guard(_mutex);
    return _queue_id.empty() || _queue_id.front() > task_id;
}

void wait_done(size_t task_id) {
    while (!is_done(task_id)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void exec_async(const std::function<void()>& func) {
    auto task_id = exec_sync(func);
    wait_done(task_id);
}

std::tuple<
    std::map<std::string, std::string>,
    std::vector<std::string>>
load_pins(const std::string& filename) {
    // decltype(load_pins_impl(filename)) rv;
    std::map<std::string, std::string> in_pins;
    std::vector<std::string> out_pins;
    exec_async([&]() {
        auto [in_pins_, out_pins_] = load_pins_impl(filename);
        in_pins = in_pins_;
        out_pins = out_pins_;
    });
    return {in_pins, out_pins};
}

}  // namespace imn::python
