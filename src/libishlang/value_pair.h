#ifndef ISHLANG_VALUE_PAIR_H
#define ISHLANG_VALUE_PAIR_H

#include "value.h"

#include <utility>

namespace Ishlang {

    class ValuePair {
    public:
        using Pair = std::pair<Value, Value>;

    public:
        ValuePair();
        ValuePair(const Value &first, const Value &second);
        ValuePair(const Pair &pair);

        inline bool operator==(const ValuePair &rhs) const;
        inline bool operator!=(const ValuePair &rhs) const;
        inline bool operator<(const ValuePair &rhs)  const;
        inline bool operator>(const ValuePair &rhs)  const;
        inline bool operator<=(const ValuePair &rhs) const;
        inline bool operator>=(const ValuePair &rhs) const;

        inline const Value & first() const noexcept;
        inline const Value & second() const noexcept;

    public:
        friend std::ostream &operator<<(std::ostream &out, const ValuePair &p) {
            out << '(' << p.first() << ' ' << p.second() << ')';
            return out;
        }

    private:
        Pair pair_;
    };

    // --------------------------------------------------------------------------------
    // INLINE

    inline bool ValuePair::operator==(const ValuePair &rhs) const {
        return first() == rhs.first() && second() == rhs.second();
    }

    inline bool ValuePair::operator!=(const ValuePair &rhs) const {
        return first() != rhs.first() || second() != rhs.second();
    }

    inline bool ValuePair::operator<(const ValuePair &rhs)  const {
        return first() < rhs.first() || (first() == rhs.first() && second() < rhs.second());
    }

    inline bool ValuePair::operator>(const ValuePair &rhs)  const {
        return first() > rhs.first() || (first() == rhs.first() && second() > rhs.second());
    }

    inline bool ValuePair::operator<=(const ValuePair &rhs) const {
        return first() < rhs.first() || (first() == rhs.first() && second() <= rhs.second());
    }

    inline bool ValuePair::operator>=(const ValuePair &rhs) const {
        return first() > rhs.first() || (first() == rhs.first() && second() >= rhs.second());
    }

    inline const Value & ValuePair::first() const noexcept {
        return pair_.first;
    }

    inline const Value & ValuePair::second() const noexcept {
        return pair_.second;
    }

}

#endif // ISHLANG_VALUE_PAIR_H
