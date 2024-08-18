
#include "factory.hpp"

#include <ranges>

#include "nodes.hpp"

using namespace imn::ui;

void NodeFactory::register_(const std::string& name, std::function<std::shared_ptr<Node>()> creator) {
    _creators[name] = creator;
}

void NodeFactory::register_all() {
    register_("text", []() { return std::make_shared<ImageLoaderNode>(); });
}

std::shared_ptr<Node> NodeFactory::create(const std::string& name) {
    return _creators[name]();
}

std::vector<std::string> NodeFactory::entries() {
    auto kv = std::views::keys(_creators);
    std::vector<std::string> keys{kv.begin(), kv.end()};
    std::sort(keys.begin(), keys.end());
    return keys;
}
