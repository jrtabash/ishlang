#ifndef ISHLANG_GENERIC_TABLE_H
#define ISHLANG_GENERIC_TABLE_H

#include "sequence.h"
#include "util.h"
#include "value.h"
#include "value_pair.h"

#include <algorithm>
#include <map>
#include <ostream>
#include <unordered_map>

namespace Ishlang {

    template <typename TableType>
    class GenericTable {
    public:
        using Table = TableType;

    public:
        inline GenericTable() = default;
        inline GenericTable(Table && table);

        inline bool operator==(const GenericTable &rhs) const;
        inline bool operator!=(const GenericTable &rhs) const;

        inline bool operator<(const GenericTable &rhs) const;
        inline bool operator>(const GenericTable &rhs) const;
        inline bool operator<=(const GenericTable &rhs) const;
        inline bool operator>=(const GenericTable &rhs) const;

        inline const Value & get(const Value &key, const Value &defaultValue = Value::Null) const;

        inline void set(const Value &key, const Value &value);
        inline void remove(const Value &key);
        inline void clear();

        inline bool exists(const Value &key) const;

        inline const Value &find(const Value &value) const;
        inline std::size_t count(const Value &value) const;

        inline Sequence keys() const;
        inline Sequence values() const;
        inline Sequence items() const;

        inline Sequence reverseKeys() const;
        inline Sequence reverseValues() const;
        inline Sequence reverseItems() const;

        inline std::size_t size() const;

        inline Table::const_iterator begin() const noexcept;
        inline Table::const_iterator end() const noexcept;

    public:
        friend std::ostream &operator<<(std::ostream &out, const GenericTable &gt) {
            return Util::printContainer(out,
                                        gt.table_,
                                        [](std::ostream &os, const auto &nameValue) {
                                            os << nameValue.first << '=' << nameValue.second;
                                        },
                                        '{',
                                        '}');
        }

    private:
        static inline bool tableEqual(const Table &lhs, const Table &rhs);

    private:
        Table table_;
    };

    class Hashtable : public GenericTable<std::unordered_map<Value, Value, Value::Hash>> {};

    class OrderedTable : public GenericTable<std::map<Value, Value>> {};

    // --------------------------------------------------------------------------------
    // INLINE

    template <typename TableType>
    inline GenericTable<TableType>::GenericTable(Table && table)
        : table_(table)
    {}

    template <typename TableType>
    inline bool GenericTable<TableType>::operator==(const GenericTable &rhs) const {
        return tableEqual(table_, rhs.table_);
    }

    template <typename TableType>
    inline bool GenericTable<TableType>::operator!=(const GenericTable &rhs) const {
        return !tableEqual(table_, rhs.table_);
    }

    template <typename TableType>
    inline bool GenericTable<TableType>::operator<(const GenericTable &rhs) const {
        return table_.size() < rhs.table_.size();
    }

    template <typename TableType>
    inline bool GenericTable<TableType>::operator>(const GenericTable &rhs) const {
        return table_.size() > rhs.table_.size();
    }

    template <typename TableType>
    inline bool GenericTable<TableType>::operator<=(const GenericTable &rhs) const {
        return table_.size() <= rhs.table_.size();
    }

    template <typename TableType>
    inline bool GenericTable<TableType>::operator>=(const GenericTable &rhs) const {
        return table_.size() >= rhs.table_.size();
    }

    template <typename TableType>
    inline const Value & GenericTable<TableType>::get(const Value &key, const Value &defaultValue) const {
        auto iter = table_.find(key);
        return iter != table_.end() ? iter->second : defaultValue;
    }

    template <typename TableType>
    inline void GenericTable<TableType>::set(const Value &key, const Value &value) {
        table_[key] = value;
    }

    template <typename TableType>
    inline void GenericTable<TableType>::remove(const Value &key) {
        table_.erase(key);
    }

    template <typename TableType>
    inline void GenericTable<TableType>::clear() {
        table_.clear();
    }

    template <typename TableType>
    inline bool GenericTable<TableType>::exists(const Value &key) const {
        return table_.find(key) != table_.end();
    }

    template <typename TableType>
    inline const Value &GenericTable<TableType>::find(const Value &value) const {
        auto iter = std::find_if(
            table_.begin(),
            table_.end(),
            [&value](const auto &nameValue) {
                return nameValue.second == value;
            });
        return iter != table_.end() ? iter->first : Value::Null;
    }

    template <typename TableType>
    inline std::size_t GenericTable<TableType>::count(const Value &value) const {
        return std::count_if(
            table_.begin(),
            table_.end(),
            [&value](const auto &nameValue) {
                return nameValue.second == value;
            });
    }

    template <typename TableType>
    inline Sequence GenericTable<TableType>::keys() const {
        return Sequence::generate(
            table_.size(),
            [iter = table_.begin()]() mutable {
                return (iter++)->first;
            });
    }

    template <typename TableType>
    inline Sequence GenericTable<TableType>::reverseKeys() const {
        return Sequence::generate(
            table_.size(),
            [iter = table_.rbegin()]() mutable {
                return (iter++)->first;
            });
    }

    template <typename TableType>
    inline Sequence GenericTable<TableType>::values() const {
        return Sequence::generate(
            table_.size(),
            [iter = table_.begin()]() mutable {
                return (iter++)->second;
            });
    }

    template <typename TableType>
    inline Sequence GenericTable<TableType>::reverseValues() const {
        return Sequence::generate(
            table_.size(),
            [iter = table_.rbegin()]() mutable {
                return (iter++)->second;
            });
    }

    template <typename TableType>
    inline Sequence GenericTable<TableType>::items() const {
        return Sequence::generate(
            table_.size(),
            [iter = table_.begin()]() mutable {
                auto kv = *iter++;
                return Value(ValuePair(kv.first, kv.second));
            });
    }

    template <typename TableType>
    inline Sequence GenericTable<TableType>::reverseItems() const {
        return Sequence::generate(
            table_.size(),
            [iter = table_.rbegin()]() mutable {
                auto kv = *iter++;
                return Value(ValuePair(kv.first, kv.second));
            });
    }

    template <typename TableType>
    inline std::size_t GenericTable<TableType>::size() const {
        return table_.size();
    }

    template <typename TableType>
    inline auto GenericTable<TableType>::begin() const noexcept -> Table::const_iterator {
        return table_.begin();
    }

    template <typename TableType>
    inline auto GenericTable<TableType>::end() const noexcept -> Table::const_iterator {
        return table_.end();
    }

    template <typename TableType>
    inline bool GenericTable<TableType>::tableEqual(const Table &lhs, const Table &rhs) {
        return Util::isEqualMapping(lhs, rhs);
    }

}

#endif // ISHLANG_GENERIC_TABLE_H
