#include "iden_table.h"

#include <cassert>

using namespace Ishlang;

IdenType IdenTable::mapName(const std::string & name) {
    auto iter = table_.find(name);
    if (iter == table_.end()) {
        std::tie(iter, std::ignore) = table_.emplace(name, nextIden());

        assert(iter != table_.end());
        reverseTable_.emplace(iter->second, iter->first);
    }

    assert(iter != table_.end());
    return iter->second;
}
