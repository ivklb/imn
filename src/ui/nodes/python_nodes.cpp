
#include "python_nodes.hpp"

#include <pybind11/eigen.h>
#include <pybind11/embed.h>
#include <spdlog/spdlog.h>

#include <eigen3/Eigen/Dense>
#include <filesystem>

#include "core/python.hpp"
#include "ui/nodes/pins.hpp"

namespace fs = std::filesystem;
namespace py = pybind11;

namespace imn::ui {

PythonNode::PythonNode() : Node(), filename("") {
}

void PythonNode::fit_json(json data) {
    Node::fit_json(data);
    set_filename(data.value("filename", ""));

    // reset pin ids
    for (auto& [name, pin] : inputs) {
        pin->id = data["inputs"][name]["id"].get<int>();
    }
    for (auto& [name, pin] : outputs) {
        pin->id = data["outputs"][name]["id"].get<int>();
    }
}

json PythonNode::to_json() {
    json rv = Node::to_json();
    rv["filename"] = filename;
    return rv;
}

std::any PythonNode::get_output(int pid) {
    auto pin = graph->get_pin(pid, PinKind::Out);
    return results[pin->name];
}

void PythonNode::set_filename(std::string fname) {
    filename = fname;
    auto [in_type_dict, out_type_list] = python::load_pins(filename);
    for (const auto& [fname, type] : in_type_dict) {
        auto pin = type_to_pin(type, fname, PinKind::In);
        inputs[fname] = pin;
    }
    for (int i = 0; i < out_type_list.size(); i++) {
        auto pin_name = "out" + std::to_string(i);
        auto pin = type_to_pin(out_type_list[i], pin_name, PinKind::Out);
        outputs[pin_name] = pin;
        output_order.push_back(pin_name);
    }
}

void PythonNode::_draw_titlebar() {
    ImGui::TextUnformatted(filename.c_str());
}

void PythonNode::_process() {
    python::exec_sync([&]() {
        py::dict kwargs;
        for (const auto& [fname, pin] : inputs) {
            if (auto img_pin = std::dynamic_pointer_cast<ImagePin>(pin)) {
                auto mat = get_input<std::shared_ptr<cv::Mat>>(pin->id);
                int bpp = 1;
                const char* format_descriptor = nullptr;
                if (mat->type() == CV_8UC1) {
                    bpp = sizeof(uint8_t);
                    format_descriptor = py::format_descriptor<uint8_t>::value;
                } else if (mat->type() == CV_16UC1) {
                    bpp = sizeof(uint16_t);
                    format_descriptor = py::format_descriptor<uint16_t>::value;
                } else if (mat->type() == CV_32SC1) {
                    bpp = sizeof(uint32_t);
                    format_descriptor = py::format_descriptor<uint32_t>::value;
                } else if (mat->type() == CV_32FC1) {
                    bpp = sizeof(float);
                    format_descriptor = py::format_descriptor<float>::value;
                } else {
                    throw std::runtime_error("Unsupported dtype: " + mat->type());
                }
                std::vector<int64_t> shape;
                for (int i = 0; i < mat->dims; i++) {
                    shape.push_back(mat->size[i]);
                }
                std::vector<int64_t> strides(mat->dims);
                size_t total = 1;
                for (int i = shape.size() - 1; i >= 0; i--) {
                    strides[i] = total * bpp;
                    total *= shape[i];
                }

                bool read_only = false;
                auto bb = py::memoryview::from_buffer(
                    mat->data,  // buffer pointer
                    bpp,
                    format_descriptor,
                    shape,    // shape (rows, cols)
                    strides,  // strides in bytes
                    read_only);
                py::object np = py::module::import("numpy");
                py::object asarray = np.attr("asarray");
                kwargs[fname.c_str()] = asarray(bb);

            } else if (auto int_pin = std::dynamic_pointer_cast<IntPin>(pin)) {
                kwargs[fname.c_str()] = get_input<int>(pin->id);
            }
        }
        py::module_ mod = py::module_::import(filename.c_str());
        py::object function = mod.attr("run");
        py::tuple rv = function(**kwargs);

        for (int i = 0; i < output_order.size(); i++) {
            auto pname = output_order[i];
            auto pin = outputs[pname];
            if (auto img_pin = std::dynamic_pointer_cast<ImagePin>(pin)) {
                // rv = np.copy(rv, order='C')
                py::module_ np = py::module_::import("numpy");
                py::buffer buf = np.attr("copy")(rv[i], "C");

                // py::buffer buf = rv[i];
                py::buffer_info info = buf.request(true);

                auto p = static_cast<float*>(info.ptr);
                std::vector<int> sizes{info.shape.begin(), info.shape.end()};

                std::shared_ptr<cv::Mat> mat;
                if (info.format == py::format_descriptor<float>::format()) {
                    // mat = std::make_shared<cv::Mat>(sizes, CV_32FC1, info.ptr);
                    mat = std::make_shared<cv::Mat>(sizes, CV_32FC1);
                } else if (info.format == py::format_descriptor<uint8_t>::format()) {
                    mat = std::make_shared<cv::Mat>(sizes, CV_8UC1);
                } else if (info.format == py::format_descriptor<uint16_t>::format()) {
                    mat = std::make_shared<cv::Mat>(sizes, CV_16UC1);
                } else if (info.format == py::format_descriptor<uint32_t>::format()) {
                    mat = std::make_shared<cv::Mat>(sizes, CV_32SC1);
                }
                // TODO: avoid copy via using imn.create_buffer() in python side
                memcpy(mat->data, info.ptr, mat->elemSize() * mat->total());
                results[pname] = mat;
            } else if (auto int_pin = std::dynamic_pointer_cast<IntPin>(pin)) {
                results[pname] = rv[i].cast<int>();
                SPDLOG_INFO("out int : {} {}", pname, rv[i].cast<int>());
            }
        }
    });
}

std::vector<std::string> find_python_nodes() {
    std::vector<std::string> nodes;
    for (const auto& entry : fs::directory_iterator("python/nodes")) {
        fs::path path = entry.path();
        if (path.extension() == ".py") {
            nodes.push_back(path.stem().string());
        }
    }
    return nodes;
}

std::shared_ptr<Pin> PythonNode::type_to_pin(const std::string& type, const std::string& pin_name, PinKind kind) {
    // refer to inspect_util.py
    if (type == "bool") {
    } else if (type == "int") {
        return std::make_shared<IntPin>(this, pin_name.c_str(), kind);
    } else if (type == "float") {
    } else if (type == "str") {
    } else if (type == "ndarray") {
        return std::make_shared<ImagePin>(this, pin_name.c_str(), kind);
    }
    return nullptr;
}

}  // namespace imn::ui
