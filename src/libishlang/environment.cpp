#include "environment.h"

using namespace Ishlang;

Environment::Environment(SharedPtr parent)
    : parent_(parent)
    , table_()
{}

const Value &Environment::def(const std::string &name, const Value &value) {
    auto [iter, success] = table_.emplace(name, value);
    if (!success) {
        throw DuplicateDef(name);
    }
    return iter->second;
}

const Value &Environment::set(const std::string &name, const Value &value) {
    auto iter = table_.find(name);
    if (iter == table_.end()) {
        if (parent_) {
            return parent_->set(name, value);
        }
        throw UnknownSymbol(name);
    }
    else {
        return iter->second = value;
    }
}

const Value &Environment::get(const std::string &name) const {
    auto iter = table_.find(name);
    if (iter == table_.end()) {
        if (parent_) {
            return parent_->get(name);
        }
        throw UnknownSymbol(name);
    }
    return iter->second;
}
