#include "value_pair.h"

using namespace Ishlang;

// -------------------------------------------------------------
ValuePair::ValuePair()
    : pair_(Value::Null, Value::Null)
{}

// -------------------------------------------------------------
ValuePair::ValuePair(const Value &first, const Value &second)
    : pair_(first, second)
{}

// -------------------------------------------------------------
ValuePair::ValuePair(const Pair &pair)
    : pair_(pair)
{}
