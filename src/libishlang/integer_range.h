#ifndef ISHLANG_INTEGER_RANGE_H
#define ISHLANG_INTEGER_RANGE_H

#include "value.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <functional>
#include <optional>
#include <ostream>

namespace Ishlang {

    class IntegerRange {
    public:
        using Long = Value::Long;
        using Predicate = std::function<bool (Long, Long)>;

    public:
        IntegerRange() = default;
        IntegerRange(Long end);
        IntegerRange(Long begin, Long end, Long step = 1ll);

        inline bool operator==(const IntegerRange &rhs) const;
        inline bool operator!=(const IntegerRange &rhs) const;

        inline bool operator<(const IntegerRange &rhs) const;
        inline bool operator>(const IntegerRange &rhs) const;
        inline bool operator<=(const IntegerRange &rhs) const;
        inline bool operator>=(const IntegerRange &rhs) const;

        inline Long begin() const noexcept;
        inline Long end() const noexcept;
        inline Long step() const noexcept;
        inline std::size_t size() const noexcept;
        inline const Predicate & predicate() const;

        inline std::optional<Long> start() const;
        inline std::optional<Long> next(Long cur) const;

    private:
        void checkValid();

    public:
        class Generator {
        public:
            Generator(const IntegerRange & rng);

            std::optional<Long> next();

        private:
            const IntegerRange &rng_;
            std::optional<Long> next_ = std::nullopt;
        };

        inline Generator generator() const;

    public:
        friend std::ostream &operator<<(std::ostream &out, const IntegerRange &range) {
            out << range.begin_ << ':' << range.step_ << ':' << range.end_;
            return out;
        }

    private:
        Long begin_ = 0;
        Long end_ = 0;
        Long step_ = 1;
        Predicate pred_ = std::less<Long>();
        mutable std::optional<std::size_t> size_;
    };

    // --------------------------------------------------------------------------------
    // INLINE

    inline bool IntegerRange::operator==(const IntegerRange &rhs) const {
        return begin_ == rhs.begin_ && end_ == rhs.end_ && step_ == rhs.step_;
    }

    inline bool IntegerRange::operator!=(const IntegerRange &rhs) const {
        return begin_ != rhs.begin_ || end_ != rhs.end_ || step_ != rhs.step_;
    }

    inline bool IntegerRange::operator<(const IntegerRange &rhs) const {
        return size() < rhs.size();
    }

    inline bool IntegerRange::operator>(const IntegerRange &rhs) const {
        return size() > rhs.size();
    }

    inline bool IntegerRange::operator<=(const IntegerRange &rhs) const {
        return size() <= rhs.size();
    }

    inline bool IntegerRange::operator>=(const IntegerRange &rhs) const {
        return size() >= rhs.size();
    }

    inline auto IntegerRange::begin() const noexcept -> Long {
        return begin_;
    }

    inline auto IntegerRange::end() const noexcept -> Long {
        return end_;
    }

    inline auto IntegerRange::step() const noexcept -> Long {
        return step_;
    }

    inline std::size_t IntegerRange::size() const noexcept {
        if (!size_) {
            size_ = std::ceil((std::max(begin_, end_) - std::min(begin_, end_)) / static_cast<double>(std::abs(step_)));
        }
        return *size_;
    }

    inline auto IntegerRange::predicate() const -> const Predicate & {
        return pred_;
    }

    inline auto IntegerRange::start() const -> std::optional<Long> {
        return pred_(begin_, end_) ? std::make_optional(begin_) : std::nullopt;
    }

    inline auto IntegerRange::next(Long cur) const -> std::optional<Long> {
        assert(pred_(cur, end_));
        Long n = cur + step_;
        return pred_(n, end_) ? std::make_optional(n) : std::nullopt;
    }

    inline auto IntegerRange::generator() const -> Generator {
        return Generator(*this);
    }

}

#endif // ISHLANG_INTEGER_RANGE_H
