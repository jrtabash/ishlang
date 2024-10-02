#include "integer_range.h"
#include "exception.h"
#include "util.h"

using namespace Ishlang;

IntegerRange::IntegerRange(Long end)
    : begin_(0)
    , end_(end)
    , step_(1)
{
    checkValid();
}

IntegerRange::IntegerRange(Long begin, Long end, Long step)
    : begin_(begin)
    , end_(end)
    , step_(step)
{
    checkValid();

    if (step_ < 0) {
        pred_ = std::greater<Long>();
    }
}

void IntegerRange::checkValid() {
    assert(!size_.has_value());

    if (step_ == 0) {
        throw Exception("Range step 0");
    }

    if (Util::sign(end_ - begin_) != Util::sign(step_)) {
        throw Exception(Exception::format("Invalid range %lld:%lld:%lld", begin_, step_, end_));
    }
}

IntegerRange::Generator::Generator(const IntegerRange & rng)
    : rng_(rng)
    , next_(rng.start())
{}

auto IntegerRange::Generator::next() -> std::optional<Long> {
    if (next_) {
        auto cur = next_;
        next_ = rng_.next(*cur);
        return cur;
    }
    return std::nullopt;
}

#undef RANGE_EXCEPTION
