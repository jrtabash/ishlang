#include "instance.h"
#include "exception.h"

using namespace Int;

// -------------------------------------------------------------
Instance::Instance()
    : type_()
    , members_()
{}

// -------------------------------------------------------------
Instance::Instance(const Struct &type)
    : type_(type)
    , members_()
{
    for (const auto &m : type.members()) {
        members_.insert(MemberTable::value_type(m, Value::Null));
    }
}

// -------------------------------------------------------------
const Value &Instance::get(const std::string &name) const {
    MemberTable::const_iterator iter = members_.find(name);
    if (iter == members_.end()) {
        throw UnknownMember(type_.name(), name);
    }
    return iter->second;
}

// -------------------------------------------------------------
void Instance::set(const std::string &name, const Value &value) {
    MemberTable::iterator iter = members_.find(name);
    if (iter == members_.end()) {
        throw UnknownMember(type_.name(), name);
    }
    iter->second = value;
}
