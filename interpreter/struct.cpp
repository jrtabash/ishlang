#include "struct.h"

using namespace Int;

// -------------------------------------------------------------
Struct::Struct()
    : name_()
    , members_()
{}

// -------------------------------------------------------------
Struct::Struct(const std::string &name, const MemberList &members)
    : name_(name)
    , members_(members)
{}
