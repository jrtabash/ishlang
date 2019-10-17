#ifndef SEQUENCE_H
#define SEQUENCE_H

#include "value.h"

#include <vector>
#include <ostream>

namespace Int {

    class Sequence {
    public:
        using Vector = std::vector<Value>;

    public:
        Sequence();
        Sequence(Vector && vec);
        Sequence(std::size_t size, const Value &value);

        bool operator==(const Sequence &rhs) const { return vectorEqual(vector_, rhs.vector_); }
        bool operator!=(const Sequence &rhs) const { return !vectorEqual(vector_, rhs.vector_); }

        bool operator<(const Sequence &rhs)  const { return vector_.size() < rhs.vector_.size(); }
        bool operator>(const Sequence &rhs)  const { return vector_.size() > rhs.vector_.size(); }
        bool operator<=(const Sequence &rhs) const { return vector_.size() <= rhs.vector_.size(); }
        bool operator>=(const Sequence &rhs) const { return vector_.size() >= rhs.vector_.size(); }

        const Value & get(std::size_t idx) const { return vector_[idx]; }
        void set(std::size_t idx, const Value &value) { vector_[idx] = value; }
        void add(const Value &value) { vector_.push_back(value); }

        std::size_t size() const { return vector_.size(); }

        friend std::ostream &operator<<(std::ostream &out, const Sequence &sequence) {
            sequence.print(out);
            return out;
        }

    private:
        std::ostream &print(std::ostream &out) const;

    private:
        static bool vectorEqual(const Vector &lhs, const Vector &rhs) {
            return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
        }

    private:
        static std::size_t maxDisplaySize_;
        Vector vector_;
    };
}

#endif // VECTOR_H
