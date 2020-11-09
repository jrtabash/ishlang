#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "util.h"
#include "value.h"

#include <ostream>
#include <unordered_map>

namespace Ishlang {

    class Hashtable {
    public:
        using Table = std::unordered_map<Value, Value, Value::Hash>;

    public:
        Hashtable() = default;
        Hashtable(Table && table);

        bool operator==(const Hashtable &rhs) const { return tableEqual(table_, rhs.table_); }
        bool operator!=(const Hashtable &rhs) const { return !tableEqual(table_, rhs.table_); }

        bool operator<(const Hashtable &rhs)  const { return table_.size() < rhs.table_.size(); }
        bool operator>(const Hashtable &rhs)  const { return table_.size() > rhs.table_.size(); }
        bool operator<=(const Hashtable &rhs) const { return table_.size() <= rhs.table_.size(); }
        bool operator>=(const Hashtable &rhs) const { return table_.size() >= rhs.table_.size(); }

        const Value & get(const Value &key, const Value &defaultValue = Value::Null) const {
            auto iter = table_.find(key);
            return iter != table_.end() ? iter->second : defaultValue;
        }

        void set(const Value &key, const Value &value) { table_[key] = value; }
        void remove(const Value &key) { table_.erase(key); }
        void clear() { table_.clear(); }

        bool exists(const Value &key) const { return table_.find(key) != table_.end(); }

        const Value &find(const Value &value) const;
        std::size_t count(const Value &value) const;

        std::size_t size() const { return table_.size(); }

        friend std::ostream &operator<<(std::ostream &out, const Hashtable &ht) {
            return Util::printContainer(out,
                                        ht.table_,
                                        [](std::ostream &os, const auto &nameValue) {
                                            os << nameValue.first << '=' << nameValue.second;
                                        },
                                        '{',
                                        '}');
        }

    private:
        static bool tableEqual(const Table &lhs, const Table &rhs) {
            return (lhs.size() == rhs.size() &&
                    std::equal(lhs.begin(), lhs.end(), rhs.begin(),
                               [](const auto &lhsNV, const auto &rhsNV) {
                                   return lhsNV.first == rhsNV.first && lhsNV.second == rhsNV.second;
                               }));
        }

    private:
        static std::size_t maxDisplaySize_;
        Table table_;
    };
}

#endif // HASHTABLE_H
