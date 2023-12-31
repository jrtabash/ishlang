#include "instance.h"
#include "exception.h"

#include <iomanip>

using namespace Ishlang;

// -------------------------------------------------------------
Instance::Instance(const Struct &type, InitArgs const & initArgs)
    : type_(type)
    , members_()
{
    for (const auto &m : type.members()) {
        members_.emplace(m, initArgOrNull(initArgs, m));
    }
}

// -------------------------------------------------------------
const Value &Instance::get(const std::string &name) const {
    auto iter = members_.find(name);
    if (iter == members_.end()) {
        throw UnknownMember(type_.name(), name);
    }
    return iter->second;
}

// -------------------------------------------------------------
void Instance::set(const std::string &name, const Value &value) {
    auto iter = members_.find(name);
    if (iter == members_.end()) {
        throw UnknownMember(type_.name(), name);
    }
    iter->second = value;
}

// -------------------------------------------------------------
void Instance::describe() const {
    const auto iter = std::max_element(
        members_.begin(),
        members_.end(),
        [](const auto &lhs, const auto &rhs) {
            return lhs.first.size() < rhs.first.size();
        });
    const std::size_t width = iter != members_.end()
        ? iter->first.size()
        : 0;

    std::cout << "Instance of " << type_.name();
    for (const auto & [mem, val] : members_) {
        std::cout << "\n  " << std::setw(width) << mem << ": " << val;
    }
    std::cout << std::endl;
}
