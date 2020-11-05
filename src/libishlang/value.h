#ifndef VALUE_H
#define VALUE_H

#include <iostream>
#include <memory>
#include <string>
#include <variant>

namespace Ishlang {
    class Lambda;
    class Struct;
    class Instance;
    class Sequence;

    using StringPtr = std::shared_ptr<std::string>;
    using LambdaPtr = std::shared_ptr<Lambda>;
    using StructPtr = std::shared_ptr<Struct>;
    using InstancePtr = std::shared_ptr<Instance>;
    using SequencePtr = std::shared_ptr<Sequence>;

    struct Value {
    public:
        static const Value True;
        static const Value False;
        static const Value Zero;
        static const Value Null;

    private:
        static std::string NullText;
        static Lambda      NullFunc;
        static Struct      NullUserType;
        static Instance    NullObject;
        static Sequence    NullSequence;
        
    public:
        enum Type {
            eNone       = '0',
            eInteger    = 'I',
            eReal       = 'R',
            eCharacter  = 'C',
            eBoolean    = 'B',
            eString     = 'S',
            eClosure    = 'F',
            eUserType   = 'U',
            eUserObject = 'O',
            eArray      = 'A',
        };

        using Long       = long long int;
        using Double     = double;
        using Char       = char;
        using Bool       = bool;
        using Text       = std::string;
        using Func       = Lambda;
        using UserType   = Struct;
        using UserObject = Instance;
        using Array      = Sequence;
        
    public:
        Value()         : type_(eNone) {}
        Value(Long i)   : type_(eInteger),   value_(i) {}
        Value(Double r) : type_(eReal),      value_(r) {}
        Value(Char c)   : type_(eCharacter), value_(c) {}
        Value(Bool b)   : type_(eBoolean),   value_(b) {}
        Value(const char *t);
        Value(const std::string &t);
        Value(const Lambda &f);
        Value(const Struct &s);
        Value(const Instance &o);
        Value(const Sequence &s);

        Type type() const { return type_; }
        
        bool isNull() const       { return type_ == eNone; }
        bool isInt()  const       { return type_ == eInteger; }
        bool isReal() const       { return type_ == eReal; }
        bool isChar() const       { return type_ == eCharacter; }
        bool isBool() const       { return type_ == eBoolean; }
        bool isString() const     { return type_ == eString; }
        bool isClosure() const    { return type_ == eClosure; }
        bool isUserType() const   { return type_ == eUserType; }
        bool isUserObject() const { return type_ == eUserObject; }
        bool isArray() const      { return type_ == eArray; }
        
        bool isNumber() const { return type_ == eInteger || type_ == eReal; }
        
        Long integer()                 const { return isInt()        ? std::get<Long>(value_)  : 0; }
        Double real()                  const { return isReal()       ? std::get<Double>(value_)  : (isInt() ? (Double)std::get<Long>(value_) : 0.0); }
        Char character()               const { return isChar()       ? std::get<Char>(value_)  : '\0'; }
        Bool boolean()                 const { return isBool()       ? std::get<Bool>(value_)  : false; }
        const Text &text()             const { return isString()     ? *std::get<StringPtr>(value_) : NullText; }
        Text &text()                         { return isString()     ? *std::get<StringPtr>(value_) : NullText; }
        const Func &closure()          const { return isClosure()    ? *std::get<LambdaPtr>(value_) : NullFunc; }
        const UserType &userType()     const { return isUserType()   ? *std::get<StructPtr>(value_) : NullUserType; }
        const UserObject &userObject() const { return isUserObject() ? *std::get<InstancePtr>(value_) : NullObject; }
        UserObject &userObject()             { return isUserObject() ? *std::get<InstancePtr>(value_) : NullObject; }
        const Array &array()           const { return isArray()      ? *std::get<SequencePtr>(value_) : NullSequence; }
        Array &array()                       { return isArray()      ? *std::get<SequencePtr>(value_) : NullSequence; }

        Value asInt() const;
        Value asReal() const;
        Value asChar() const;
        Value asBool() const;
        Value asString() const;
        
        bool operator==(const Value &rhs) const;
        bool operator!=(const Value &rhs) const;
        bool operator<(const Value &rhs) const;
        bool operator>(const Value &rhs) const;
        bool operator<=(const Value &rhs) const;
        bool operator>=(const Value &rhs) const;

        static std::string typeToString(Type type);
        static Type stringToType(const std::string &str);
        
        std::string typeToString() const { return typeToString(type_); }

        Value clone() const;
        Value asType(Type otherType) const;

        friend std::ostream &operator<<(std::ostream &out, const Value &value) {
            printC(out, value);
            return out;
        }

    public:
        static void print(const Value &value);
        static void printC(std::ostream &out, const Value &value);

    public:
        struct Hash {
            std::size_t operator()(const Value &value) const noexcept;
        };

    private:
        using VariantValue = std::variant<Long,
                                          Double,
                                          Char,
                                          Bool,
                                          StringPtr,
                                          LambdaPtr,
                                          StructPtr,
                                          InstancePtr,
                                          SequencePtr>;

        Type type_;
        VariantValue value_;
    };
}

#endif	// VALUE_H
