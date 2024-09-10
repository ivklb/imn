
#ifndef UI__NODES__PYTHON_NODES_HPP
#define UI__NODES__PYTHON_NODES_HPP

#include <any>
#include <memory>
#include <string>

#include "core/object.hpp"
#include "nodes.hpp"

namespace imn::ui {

struct PythonNode : public Node, public core::AutoRegistered<PythonNode, Node> {
    std::string filename;
    std::shared_ptr<Pin> pin_vol;
    std::vector<std::string> output_order;
    std::map<std::string, std::any> results;

    PythonNode();
    void fit_json(json data) override;
    json to_json() override;
    static std::string registered_name() { return "python_node"; }
    std::string name() override { return registered_name(); }
    ColorTheme color() override { return ColorTheme::Purple; }
    std::any get_output(int pid) override;
    // void set_filename(const std::string& name);
    void set_filename(std::string fname);
    void _draw_titlebar() override;
    void _process() override;

    std::shared_ptr<Pin> type_to_pin(const std::string& type, const std::string& pin_name, PinKind kind);
};

std::vector<std::string> find_python_nodes();

}  // namespace imn::ui
#endif
