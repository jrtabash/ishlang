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
    Sequence keys;
    for (const auto & nameValue : table_) {
        keys.add(nameValue.first);
    }
    return keys;
}

// -------------------------------------------------------------
Sequence Hashtable::values() const {
    Sequence values;
    for (const auto & nameValue : table_) {
        values.add(nameValue.second);
    }
    return values;
}

// -------------------------------------------------------------
Sequence Hashtable::items() const {
    Sequence pairs;
    for (const auto & nameValue : table_) {
        pairs.add(Value(ValuePair(nameValue.first, nameValue.second)));
    }
    return pairs;
}
