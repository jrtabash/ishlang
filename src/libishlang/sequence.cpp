#include "sequence.h"
#include "exception.h"

#include <algorithm>
#include <functional>

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
void Sequence::insert(std::size_t pos, const Value &value) {
    auto iter = vector_.begin();
    iter += pos;
    vector_.insert(iter, value);
}

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

// -------------------------------------------------------------
void Sequence::sort(bool descending) {
    if (descending) {
        std::sort(vector_.begin(), vector_.end(), std::greater<Value>());
    }
    else {
        std::sort(vector_.begin(), vector_.end());
    }
}

// -------------------------------------------------------------
void Sequence::reverse() {
    std::reverse(vector_.begin(), vector_.end());
}
