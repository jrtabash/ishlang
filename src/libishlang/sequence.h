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

        inline bool operator==(const Sequence &rhs) const;
        inline bool operator!=(const Sequence &rhs) const;

        inline bool operator<(const Sequence &rhs) const;
        inline bool operator>(const Sequence &rhs) const;
        inline bool operator<=(const Sequence &rhs) const;
        inline bool operator>=(const Sequence &rhs) const;

        inline const Value &get(std::size_t idx) const;
        inline void set(std::size_t idx, const Value &value);
        inline void push(const Value &value);
        inline void pop();
        inline void clear();

        void insert(std::size_t pos, const Value & value);
        void erase(std::size_t pos);
        std::optional<std::size_t> find(const Value &val, std::size_t pos = 0) const;
        std::size_t count(const Value &value) const;
        void sort(bool descending);
        void reverse();

        inline std::size_t size() const;

    public:
        friend std::ostream &operator<<(std::ostream &out, const Sequence &sequence) {
            return Util::printContainer(out,
                                        sequence.vector_,
                                        [](std::ostream &os, const auto &value) { os << value; },
                                        '[',
                                        ']');
        }

    public:
        static inline Sequence generate(std::size_t size, auto && ftn);

    private:
        static inline bool vectorEqual(const Vector &lhs, const Vector &rhs);

    private:
        static std::size_t maxDisplaySize_;
        Vector vector_;
    };

    // --------------------------------------------------------------------------------
    // INLINE

    inline bool Sequence::operator==(const Sequence &rhs) const {
        return vectorEqual(vector_, rhs.vector_);
    }

    inline bool Sequence::operator!=(const Sequence &rhs) const {
        return !vectorEqual(vector_, rhs.vector_);
    }

    inline bool Sequence::operator<(const Sequence &rhs) const {
        return vector_.size() < rhs.vector_.size();
    }

    inline bool Sequence::operator>(const Sequence &rhs) const {
        return vector_.size() > rhs.vector_.size();
    }

    inline bool Sequence::operator<=(const Sequence &rhs) const {
        return vector_.size() <= rhs.vector_.size();
    }

    inline bool Sequence::operator>=(const Sequence &rhs) const {
        return vector_.size() >= rhs.vector_.size();
    }

    inline const Value &Sequence::get(std::size_t idx) const {
        return vector_[idx];
    }

    inline void Sequence::set(std::size_t idx, const Value &value) {
        vector_[idx] = value;
    }

    inline void Sequence::push(const Value &value) {
        vector_.push_back(value);
    }

    inline void Sequence::pop() {
        if (!vector_.empty()) {
            vector_.pop_back();
        }
    }

    inline void Sequence::clear() {
        vector_.clear();
    }

    inline std::size_t Sequence::size() const {
        return vector_.size();
    }

    inline Sequence Sequence::generate(std::size_t size, auto && ftn) {
        auto vec = std::vector<Value>(size, Value::Null);
        std::generate(vec.begin(), vec.end(), std::forward<std::remove_cvref_t<decltype(ftn)>>(ftn));
        return Sequence(std::move(vec));
    }

    inline bool Sequence::vectorEqual(const Vector &lhs, const Vector &rhs) {
        return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

}

#endif // VECTOR_H
