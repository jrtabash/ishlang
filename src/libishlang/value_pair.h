#ifndef VALUE_PAIR_H
#define VALUE_PAIR_H

#include "value.h"

#include <utility>

namespace Ishlang {

    class ValuePair {
    public:
        using Pair = std::pair<Value, Value>;

    public:
        ValuePair();
        ValuePair(const Value &first, const Value &second);
        ValuePair(const Pair & pair);

        bool operator==(const ValuePair &rhs) const {
            return first() == rhs.first() && second() == rhs.second();
        }

        bool operator!=(const ValuePair &rhs) const {
            return first() != rhs.first() || second() != rhs.second();
        }

        bool operator<(const ValuePair &rhs)  const {
            return first() < rhs.first() || (first() == rhs.first() && second() < rhs.second());
        }

        bool operator>(const ValuePair &rhs)  const {
            return first() > rhs.first() || (first() == rhs.first() && second() > rhs.second());
        }

        bool operator<=(const ValuePair &rhs) const {
            return first() < rhs.first() || (first() == rhs.first() && second() <= rhs.second());
        }

        bool operator>=(const ValuePair &rhs) const {
            return first() > rhs.first() || (first() == rhs.first() && second() >= rhs.second());
        }

        const Value & first() const noexcept { return pair_.first; }
        const Value & second() const noexcept { return pair_.second; }

        friend std::ostream &operator<<(std::ostream &out, const ValuePair &p) {
            out << '(' << p.first() << ' ' << p.second() << ')';
            return out;
        }

    private:
        Pair pair_;
    };

}

#endif // VALUE_PAIR_H
