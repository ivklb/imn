
#include "python_nodes.hpp"

#include <filesystem>

#include "core/python.hpp"
#include "ui/nodes/pins.hpp"

namespace fs = std::filesystem;

namespace imn::ui {

PythonNode::PythonNode() : Node(), filename("") {
}

void PythonNode::fit_json(json data) {
    Node::fit_json(data);
    set_filename(data.value("filename", ""));
}

json PythonNode::to_json() {
    json rv = Node::to_json();
    rv["filename"] = filename;
    return rv;
}

void PythonNode::set_filename(std::string fname) {
    filename = fname;
    auto [in_type_dict, out_type_list] = python::load_pins(filename);
    for (const auto& [fname, type] : in_type_dict) {
        auto pin = type_to_pin(type, fname, PinKind::In);
        inputs[fname] = pin;
    }
    for (int i=0; i<out_type_list.size(); i++) {
        auto pin_name = "out" + std::to_string(i);
        auto pin = type_to_pin(out_type_list[i], pin_name, PinKind::Out);
        outputs[pin_name] = pin;
    }
}

void PythonNode::_draw_titlebar() {
    ImGui::TextUnformatted(filename.c_str());
}

void PythonNode::_process() {
    // TODO: Fix me
    // python::enqueue()
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
