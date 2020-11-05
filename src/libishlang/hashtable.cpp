#include "hashtable.h"

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
std::ostream &Hashtable::print(std::ostream &out) const {
    out << '{';
    std::size_t count = 0;
    for (const auto &nameValue : table_) {
        if (count > 0) { out << ' '; }
        out << nameValue.first << ": " << nameValue.second;
        if (++count >= maxDisplaySize_ && table_.size() > maxDisplaySize_) {
            out << " ...";
            break;
        }
    }
    out << '}';
    return out;
}
