#include "sequence.h"
#include "exception.h"

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
