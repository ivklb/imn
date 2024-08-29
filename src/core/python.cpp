
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
        buffer,   // buffer pointer
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
static std::queue<std::function<void()>> _queue{};

void init() {
}

void run() {
    py::scoped_interpreter guard{};

    while (app::is_running()) {
        auto func = _queue.front();
        {
            std::lock_guard guard(_mutex);
            _queue.pop();
        }
        try {
            func();
        } catch (const py::error_already_set& e) {
            SPDLOG_ERROR("Python error: {}", e.what());
        }
        // py::module_ mod = py::module_::import("test");
        // py::object func = mod.attr("run2");
        // py::object rv = func(bb);
    }
}

void enqueue(const std::function<void()>& func) {
    std::lock_guard guard(_mutex);
    _queue.push(func);
}

}  // namespace imn::python
