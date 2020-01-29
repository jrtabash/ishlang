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
std::optional<std::size_t> Sequence::find(Value const & val, std::size_t pos) const {
    auto iter = std::find(vector_.begin() + pos, vector_.end(), val);
    if (iter != vector_.end()) {
        return std::distance(vector_.begin(), iter);
    }
    return std::nullopt;
}

// -------------------------------------------------------------
std::ostream &Sequence::print(std::ostream &out) const {
    out << '[';
    std::size_t count = 0;
    for (const auto &value : vector_) {
        if (count > 0) { out << ' '; }
        out << value;
        if (++count >= maxDisplaySize_ && vector_.size() > maxDisplaySize_) {
            out << " ...";
            break;
        }
    }
    out << ']';
    return out;
}
