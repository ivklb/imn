
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "nodes.hpp"

namespace imn::ui {

class NodeFactory {
   public:
    static void register_(const std::string& name, std::function<std::shared_ptr<Node>()> creator);
    static void register_all();
    static std::shared_ptr<Node> create(const std::string& name);
    static std::vector<std::string> entries();

   private:
    static std::map<std::string, std::function<std::shared_ptr<Node>()>> _creators;
};

}  // namespace imn::ui
