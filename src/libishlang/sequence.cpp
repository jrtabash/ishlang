#include "sequence.h"
#include "exception.h"

#include <algorithm>

using namespace Ishlang;

std::size_t Sequence::maxDisplaySize_ = 10;

// -------------------------------------------------------------
Sequence::Sequence(Vector && vec)
    : vector_(vec)
{}

// -------------------------------------------------------------
Sequence::Sequence(std::size_t size, const Value &value)
    : vector_(size, value)
{}

// -------------------------------------------------------------
std::optional<std::size_t> Sequence::find(const Value &val, std::size_t pos) const {
    auto iter = std::find(vector_.begin() + pos, vector_.end(), val);
    if (iter != vector_.end()) {
        return std::distance(vector_.begin(), iter);
    }
    return std::nullopt;
}

// -------------------------------------------------------------
std::size_t Sequence::count(const Value &value) const {
    return std::count(vector_.begin(), vector_.end(), value);
}
