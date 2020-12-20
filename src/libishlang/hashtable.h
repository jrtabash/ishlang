#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "sequence.h"
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

        inline bool operator==(const Hashtable &rhs) const;
        inline bool operator!=(const Hashtable &rhs) const;

        inline bool operator<(const Hashtable &rhs) const;
        inline bool operator>(const Hashtable &rhs) const;
        inline bool operator<=(const Hashtable &rhs) const;
        inline bool operator>=(const Hashtable &rhs) const;

        inline const Value & get(const Value &key, const Value &defaultValue = Value::Null) const;

        inline void set(const Value &key, const Value &value);
        inline void remove(const Value &key);
        inline void clear();

        inline bool exists(const Value &key) const;

        const Value &find(const Value &value) const;
        std::size_t count(const Value &value) const;

        Sequence keys() const;
        Sequence values() const;
        Sequence items() const;

        inline std::size_t size() const;

    public:
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
        static inline bool tableEqual(const Table &lhs, const Table &rhs);

    private:
        static std::size_t maxDisplaySize_;
        Table table_;
    };

    // --------------------------------------------------------------------------------
    // INLINE

    inline bool Hashtable::operator==(const Hashtable &rhs) const {
        return tableEqual(table_, rhs.table_);
    }

    inline bool Hashtable::operator!=(const Hashtable &rhs) const {
        return !tableEqual(table_, rhs.table_);
    }

    inline bool Hashtable::operator<(const Hashtable &rhs) const {
        return table_.size() < rhs.table_.size();
    }

    inline bool Hashtable::operator>(const Hashtable &rhs) const {
        return table_.size() > rhs.table_.size();
    }

    inline bool Hashtable::operator<=(const Hashtable &rhs) const {
        return table_.size() <= rhs.table_.size();
    }

    inline bool Hashtable::operator>=(const Hashtable &rhs) const {
        return table_.size() >= rhs.table_.size();
    }

    inline const Value & Hashtable::get(const Value &key, const Value &defaultValue) const {
        auto iter = table_.find(key);
        return iter != table_.end() ? iter->second : defaultValue;
    }

    inline void Hashtable::set(const Value &key, const Value &value) {
        table_[key] = value;
    }

    inline void Hashtable::remove(const Value &key) {
        table_.erase(key);
    }

    inline void Hashtable::clear() {
        table_.clear();
    }

    inline bool Hashtable::exists(const Value &key) const {
        return table_.find(key) != table_.end();
    }

    inline std::size_t Hashtable::size() const {
        return table_.size();
    }

    inline bool Hashtable::tableEqual(const Table &lhs, const Table &rhs) {
        return (lhs.size() == rhs.size() &&
                std::equal(lhs.begin(), lhs.end(), rhs.begin(),
                           [](const auto &lhsNV, const auto &rhsNV) {
                               return lhsNV.first == rhsNV.first && lhsNV.second == rhsNV.second;
                           }));
    }

}

#endif // HASHTABLE_H
