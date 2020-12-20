#include "environment.h"

using namespace Ishlang;

Environment::Environment(SharedPtr parent)
    : parent_(parent)
    , table_()
{}

const Value &Environment::def(const std::string &name, const Value &value) {
    auto result = table_.insert(Table::value_type(name, value));
    if (!result.second) {
        throw DuplicateDef(name);
    }
    return result.first->second;
}

const Value &Environment::set(const std::string &name, const Value &value) {
    Table::iterator iter = table_.find(name);
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
    Table::const_iterator iter = table_.find(name);
    if (iter == table_.end()) {
        if (parent_) {
            return parent_->get(name);
        }
        throw UnknownSymbol(name);
    }
    return iter->second;
}

void Environment::foreach(ForeachFtn ftn) const {
    for (const auto & nameValue : table_) {
        ftn(nameValue.first, nameValue.second);
    }
}
