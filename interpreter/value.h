#ifndef VALUE_H
#define	VALUE_H

#include <string>
#include <iostream>
#include <variant>
#include <memory>

namespace Int {
    class Lambda;
    class Struct;
    class Instance;

    using StringPtr = std::shared_ptr<std::string>;
    using LambdaPtr = std::shared_ptr<Lambda>;
    using StructPtr = std::shared_ptr<Struct>;
    using InstancePtr = std::shared_ptr<Instance>;

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
        };

        using Long       = long long int;
        using Double     = double;
        using Char       = char;
        using Bool       = bool;
        using Text       = std::string;
        using Func       = Lambda;
        using UserType   = Struct;
        using UserObject = Instance;
        
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
        
        bool isNumber() const { return type_ == eInteger || type_ == eReal; }
        
        Long integer()                 const { return isInt()        ? std::get<Long>(value_)  : 0; }
        Double real()                  const { return isReal()       ? std::get<Double>(value_)  : (isInt() ? (Double)std::get<Long>(value_) : 0.0); }
        Char character()               const { return isChar()       ? std::get<Char>(value_)  : '\0'; }
        Bool boolean()                 const { return isBool()       ? std::get<Bool>(value_)  : false; }
        const Text &text()             const { return isString()     ? *std::get<StringPtr>(value_) : NullText; }
        const Func &closure()          const { return isClosure()    ? *std::get<LambdaPtr>(value_) : NullFunc; }
        const UserType &userType()     const { return isUserType()   ? *std::get<StructPtr>(value_) : NullUserType; }
        const UserObject &userObject() const { return isUserObject() ? *std::get<InstancePtr>(value_) : NullObject; }
        UserObject &userObject()             { return isUserObject() ? *std::get<InstancePtr>(value_) : NullObject; }
        
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

        friend std::ostream &operator<<(std::ostream &out, const Value &value) {
            switch (value.type_) {
            case Value::eNone:       out << "null";                                            break;
            case Value::eInteger:    out << std::get<Long>(value.value_);                      break;
            case Value::eReal:       out << std::get<Double>(value.value_);                    break;
            case Value::eCharacter:  out << '\'' << std::get<Char>(value.value_) << '\'';      break;
            case Value::eBoolean:    out << (std::get<Bool>(value.value_) ? "true" : "false"); break;
            case Value::eString:     out << '"' << *std::get<StringPtr>(value.value_) << '"';  break;
            case Value::eClosure:    out << "[Lambda]";                                        break;
            case Value::eUserType:   out << "[Struct]";                                        break;
            case Value::eUserObject: out << "[Instance]";                                      break;
            }
            return out;
        }

    public:
        static void print(const Value &value);

    private:
        using VariantValue = std::variant<Long,
                                          Double,
                                          Char,
                                          Bool,
                                          StringPtr,
                                          LambdaPtr,
                                          StructPtr,
                                          InstancePtr>;

        Type type_;
        VariantValue value_;
    };
}

#endif	// VALUE_H
