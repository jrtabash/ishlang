#include "hashtable.h"
#include "value_pair.h"

#include <algorithm>

using namespace Ishlang;

std::size_t Hashtable::maxDisplaySize_ = 10;

// -------------------------------------------------------------
Hashtable::Hashtable(Table && table)
    : table_(table)
{}

// -------------------------------------------------------------
const Value &Hashtable::find(const Value &value) const {
    auto iter = std::find_if(table_.begin(),
                             table_.end(),
                             [&value](const auto &nameValue) {
                                 return nameValue.second == value;
                             });
    return iter != table_.end() ? iter->first : Value::Null;
}

// -------------------------------------------------------------
std::size_t Hashtable::count(const Value &value) const {
    return std::count_if(table_.begin(),
                         table_.end(),
                         [&value](const auto &nameValue) {
                             return nameValue.second == value;
                         });
}

// -------------------------------------------------------------
Sequence Hashtable::keys() const {
    return Sequence::generate(
        table_.size(),
        [iter = table_.begin()]() mutable {
            return (iter++)->first;
        });
}

// -------------------------------------------------------------
Sequence Hashtable::values() const {
    return Sequence::generate(
        table_.size(),
        [iter = table_.begin()]() mutable {
            return (iter++)->second;
        });
}

// -------------------------------------------------------------
Sequence Hashtable::items() const {
    return Sequence::generate(
        table_.size(),
        [iter = table_.begin()]() mutable {
            auto kv = *iter++;
            return Value(ValuePair(kv.first, kv.second));
        });
}
