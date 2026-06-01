#include "environment.h"

using namespace Ishlang;

IdenTable Environment::idenTable_ = IdenTable();

Environment::Environment(SharedPtr parent)
    : parent_(parent)
    , table_()
{}

const Value &Environment::def(IdenType iden, const Value &value) {
    auto [iter, success] = table_.emplace(iden, value);
    if (!success) {
        throw DuplicateDef(idenTable_.getName(iden));
    }
    return iter->second;
}

const Value &Environment::set(IdenType iden, const Value &value) {
    auto iter = table_.find(iden);
    if (iter == table_.end()) {
        if (parent_) {
            return parent_->set(iden, value);
        }
        throw UnknownSymbol(idenTable_.getName(iden));
    }
    else {
        return iter->second = value;
    }
}

const Value &Environment::get(IdenType iden) const {
    auto iter = table_.find(iden);
    if (iter == table_.end()) {
        if (parent_) {
            return parent_->get(iden);
        }
        throw UnknownSymbol(idenTable_.getName(iden));
    }
    return iter->second;
}
