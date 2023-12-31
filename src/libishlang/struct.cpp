#include "struct.h"

#include <iostream>

using namespace Ishlang;

// -------------------------------------------------------------
Struct::Struct(const std::string &name, const MemberList &members)
    : name_(name)
    , members_(members)
{}

// -------------------------------------------------------------
void Struct::describe() const {
    std::cout << "Struct " << name_;
    for (const auto &mem : members_) {
        std::cout << "\n  " << mem;
    }
    std::cout << std::endl;
}
