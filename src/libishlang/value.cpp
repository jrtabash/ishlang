#include "value.h"
#include "exception.h"
#include "instance.h"
#include "lambda.h"
#include "sequence.h"
#include "struct.h"

using namespace Ishlang;

// -------------------------------------------------------------
const Value Value::True(true);
const Value Value::False(false);
const Value Value::Zero(0ll);
const Value Value::Null;

// -------------------------------------------------------------
std::string Value::NullText;
Lambda      Value::NullFunc;
Struct      Value::NullUserType;
Instance    Value::NullObject;
Sequence    Value::NullSequence;

// -------------------------------------------------------------
Value::Value(const char *t)
    : type_(eString)
    , value_(std::make_shared<std::string>(t))
{}

// -------------------------------------------------------------
Value::Value(const std::string &t)
    : type_(eString)
    , value_(std::make_shared<std::string>(t))
{}

// -------------------------------------------------------------
Value::Value(const Lambda &f)
    : type_(eClosure)
    , value_(std::make_shared<Lambda>(f))
{}

// -------------------------------------------------------------
Value::Value(const Struct &s)
    : type_(eUserType)
    , value_(std::make_shared<Struct>(s))
{}

// -------------------------------------------------------------
Value::Value(const Instance &o)
    : type_(eUserObject)
    , value_(std::make_shared<Instance>(o))
{}

// -------------------------------------------------------------
Value::Value(const Sequence &s)
    : type_(eArray)
    , value_(std::make_shared<Sequence>(s))
{}

// -------------------------------------------------------------
bool Value::operator==(const Value &rhs) const {
    if (type_ == rhs.type_) {
        switch (type_) {
        case eInteger:    return std::get<Long>(value_) == std::get<Long>(rhs.value_);
        case eReal:       return std::get<Double>(value_) == std::get<Double>(rhs.value_);
        case eCharacter:  return std::get<Char>(value_) == std::get<Char>(rhs.value_);
        case eBoolean:    return std::get<Bool>(value_) == std::get<Bool>(rhs.value_);
        case eString:     return *std::get<StringPtr>(value_) == *std::get<StringPtr>(rhs.value_);
        case eClosure:    return *std::get<LambdaPtr>(value_) == *std::get<LambdaPtr>(rhs.value_);
        case eUserType:   return *std::get<StructPtr>(value_) == *std::get<StructPtr>(rhs.value_);
        case eUserObject: return *std::get<InstancePtr>(value_) == *std::get<InstancePtr>(rhs.value_);
        case eArray:      return *std::get<SequencePtr>(value_) == *std::get<SequencePtr>(rhs.value_);
        case eNone:       return true;
        }
    }
    else if (isNumber() && rhs.isNumber()) {
        return real() == rhs.real();
    }
    return false;
}

// -------------------------------------------------------------        
bool Value::operator!=(const Value &rhs) const {
    if (type_ == rhs.type_) {
        switch (type_) {
        case eInteger:    return std::get<Long>(value_) != std::get<Long>(rhs.value_);
        case eReal:       return std::get<Double>(value_) != std::get<Double>(rhs.value_);
        case eCharacter:  return std::get<Char>(value_) != std::get<Char>(rhs.value_);
        case eBoolean:    return std::get<Bool>(value_) != std::get<Bool>(rhs.value_);
        case eString:     return *std::get<StringPtr>(value_) != *std::get<StringPtr>(rhs.value_);
        case eClosure:    return *std::get<LambdaPtr>(value_) != *std::get<LambdaPtr>(rhs.value_);
        case eUserType:   return *std::get<StructPtr>(value_) != *std::get<StructPtr>(rhs.value_);
        case eUserObject: return *std::get<InstancePtr>(value_) != *std::get<InstancePtr>(rhs.value_);
        case eArray:      return *std::get<SequencePtr>(value_) != *std::get<SequencePtr>(rhs.value_);
        case eNone:       return false;
        }
    }
    else if (isNumber() && rhs.isNumber()) {
        return real() != rhs.real();
    }
    return true;
}

// -------------------------------------------------------------        
bool Value::operator<(const Value &rhs) const {
    if (type_ == rhs.type_) {
        switch (type_) {
        case eInteger:    return std::get<Long>(value_) < std::get<Long>(rhs.value_);
        case eReal:       return std::get<Double>(value_) < std::get<Double>(rhs.value_);
        case eCharacter:  return std::get<Char>(value_) < std::get<Char>(rhs.value_);
        case eBoolean:    return std::get<Bool>(value_) < std::get<Bool>(rhs.value_);
        case eString:     return *std::get<StringPtr>(value_) < *std::get<StringPtr>(rhs.value_);
        case eClosure:    return false;
        case eUserType:   return false;
        case eUserObject: return false;
        case eArray:      return *std::get<SequencePtr>(value_) < *std::get<SequencePtr>(rhs.value_);
        case eNone:       return false;
        }
    }
    else if (isNumber() && rhs.isNumber()) {
        return real() < rhs.real();
    }
    return false;
}

// -------------------------------------------------------------
bool Value::operator>(const Value &rhs) const {
    if (type_ == rhs.type_) {
        switch (type_) {
        case eInteger:    return std::get<Long>(value_) > std::get<Long>(rhs.value_);
        case eReal:       return std::get<Double>(value_) > std::get<Double>(rhs.value_);
        case eCharacter:  return std::get<Char>(value_) > std::get<Char>(rhs.value_);
        case eBoolean:    return std::get<Bool>(value_) > std::get<Bool>(rhs.value_);
        case eString:     return *std::get<StringPtr>(value_) > *std::get<StringPtr>(rhs.value_);
        case eClosure:    return false;
        case eUserType:   return false;
        case eUserObject: return false;
        case eArray:      return *std::get<SequencePtr>(value_) > *std::get<SequencePtr>(rhs.value_);
        case eNone:       return false;
        }
    }
    else if (isNumber() && rhs.isNumber()) {
        return real() > rhs.real();
    }
    return false;
}

// -------------------------------------------------------------        
bool Value::operator<=(const Value &rhs) const {
    if (type_ == rhs.type_) {
        switch (type_) {
        case eInteger:    return std::get<Long>(value_) <= std::get<Long>(rhs.value_);
        case eReal:       return std::get<Double>(value_) <= std::get<Double>(rhs.value_);
        case eCharacter:  return std::get<Char>(value_) <= std::get<Char>(rhs.value_);
        case eBoolean:    return std::get<Bool>(value_) <= std::get<Bool>(rhs.value_);
        case eString:     return *std::get<StringPtr>(value_) <= *std::get<StringPtr>(rhs.value_);
        case eClosure:    return false;
        case eUserType:   return false;
        case eUserObject: return false;
        case eArray:      return *std::get<SequencePtr>(value_) <= *std::get<SequencePtr>(rhs.value_);
        case eNone:       return false;
        }
    }
    else if (isNumber() && rhs.isNumber()) {
        return real() <= rhs.real();
    }
    return false;
}

// -------------------------------------------------------------
bool Value::operator>=(const Value &rhs) const {
    if (type_ == rhs.type_) {
        switch (type_) {
        case eInteger:    return std::get<Long>(value_) >= std::get<Long>(rhs.value_);
        case eReal:       return std::get<Double>(value_) >= std::get<Double>(rhs.value_);
        case eCharacter:  return std::get<Char>(value_) >= std::get<Char>(rhs.value_);
        case eBoolean:    return std::get<Bool>(value_) >= std::get<Bool>(rhs.value_);
        case eString:     return *std::get<StringPtr>(value_) >= *std::get<StringPtr>(rhs.value_);
        case eClosure:    return false;
        case eUserType:   return false;
        case eUserObject: return false;
        case eArray:      return *std::get<SequencePtr>(value_) >= *std::get<SequencePtr>(rhs.value_);
        case eNone:       return false;
        }
    }
    else if (isNumber() && rhs.isNumber()) {
        return real() >= rhs.real();
    }
    return false;
}

// -------------------------------------------------------------
std::string Value::typeToString(Type type) {
    switch (type) {
        case eNone:       return "none";
        case eInteger:    return "int";
        case eReal:       return "real";
        case eCharacter:  return "char";
        case eBoolean:    return "bool";
        case eString:     return "string";
        case eClosure:    return "closure";
        case eUserType:   return "usertype";
        case eUserObject: return "userobject";
        case eArray:      return "array";
    }
    return "unknown";
}

// -------------------------------------------------------------
Value::Type Value::stringToType(const std::string &str) {
    if      (str == "none")       { return Value::eNone; }
    else if (str == "int")        { return Value::eInteger; }
    else if (str == "real")       { return Value::eReal; }
    else if (str == "char")       { return Value::eCharacter; }
    else if (str == "bool")       { return Value::eBoolean; }
    else if (str == "string")     { return Value::eString; }
    else if (str == "closure")    { return Value::eClosure; }
    else if (str == "usertype")   { return Value::eUserType; }
    else if (str == "userobject") { return Value::eUserObject; }
    else if (str == "array")      { return Value::eArray; }
    throw InvalidExpression("unknown value type", str);
    return Value::eNone;
}

// -------------------------------------------------------------
Value Value::clone() const {
    switch (type_) {
    case eNone:
    case eInteger:
    case eReal:
    case eCharacter:
    case eBoolean:
        return *this;

    case eString:
        return Value(*std::get<StringPtr>(value_));

    case eClosure:
        return Value(*std::get<LambdaPtr>(value_));

    case eUserType:
        return Value(*std::get<StructPtr>(value_));

    case eUserObject:
        return Value(*std::get<InstancePtr>(value_));

    case eArray:
        return Value(*std::get<SequencePtr>(value_));
    }

    return Value::Null;
}

// -------------------------------------------------------------
void Value::printC(std::ostream &out, const Value &value) {
    switch (value.type_) {
    case Value::eNone:       out << "null";                                            break;
    case Value::eInteger:    out << std::get<Long>(value.value_);                      break;
    case Value::eReal:       out << std::get<Double>(value.value_);                    break;
    case Value::eCharacter:  out << '\'' << std::get<Char>(value.value_) << '\'';      break;
    case Value::eBoolean:    out << (std::get<Bool>(value.value_) ? "true" : "false"); break;
    case Value::eString:     out << '"' << *std::get<StringPtr>(value.value_) << '"';  break;
    case Value::eClosure:    out << "[Lambda]";                                        break;
    case Value::eUserType:   out << *std::get<StructPtr>(value.value_);                break;
    case Value::eUserObject: out << *std::get<InstancePtr>(value.value_);              break;
    case Value::eArray:      out << *std::get<SequencePtr>(value.value_);              break;
    }
}

// -------------------------------------------------------------
void Value::print(const Value &value) {
    switch (value.type_) {
    case Value::eNone:       std::cout << "null";                                            break;
    case Value::eInteger:    std::cout << std::get<Long>(value.value_);                      break;
    case Value::eReal:       std::cout << std::get<Double>(value.value_);                    break;
    case Value::eCharacter:  std::cout << std::get<Char>(value.value_);                      break;
    case Value::eBoolean:    std::cout << (std::get<Bool>(value.value_) ? "true" : "false"); break;
    case Value::eString:     std::cout << *std::get<StringPtr>(value.value_);                break;
    case Value::eClosure:    std::cout << "[Lambda]";                                        break;
    case Value::eUserType:   std::cout << *std::get<StructPtr>(value.value_);                break;
    case Value::eUserObject: std::cout << *std::get<InstancePtr>(value.value_);              break;
    case Value::eArray:      std::cout << *std::get<SequencePtr>(value.value_);              break;
    }
}