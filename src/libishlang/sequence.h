#ifndef SEQUENCE_H
#define SEQUENCE_H

#include "util.h"
#include "value.h"

#include <optional>
#include <ostream>
#include <vector>

namespace Ishlang {

    class Sequence {
    public:
        using Vector = std::vector<Value>;

    public:
        Sequence() = default;
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

        std::optional<std::size_t> find(const Value &val, std::size_t pos = 0) const;
        std::size_t count(const Value &value) const;

        std::size_t size() const { return vector_.size(); }

        friend std::ostream &operator<<(std::ostream &out, const Sequence &sequence) {
            return Util::printContainer(out,
                                        sequence.vector_,
                                        [](std::ostream &os, const auto &value) { os << value; },
                                        '[',
                                        ']');
        }

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
