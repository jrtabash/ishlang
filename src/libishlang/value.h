#ifndef VALUE_H
#define VALUE_H

#include <iostream>
#include <memory>
#include <string>
#include <variant>

namespace Ishlang {
    class ValuePair;
    class Lambda;
    class Struct;
    class Instance;
    class Sequence;
    class Hashtable;
    class IntegerRange;

    using PairPtr = std::shared_ptr<ValuePair>;
    using StringPtr = std::shared_ptr<std::string>;
    using LambdaPtr = std::shared_ptr<Lambda>;
    using StructPtr = std::shared_ptr<Struct>;
    using InstancePtr = std::shared_ptr<Instance>;
    using SequencePtr = std::shared_ptr<Sequence>;
    using HashtablePtr = std::shared_ptr<Hashtable>;
    using IntegerRangePtr = std::shared_ptr<IntegerRange>;

    struct Value {
    public:
        static const Value True;
        static const Value False;
        static const Value Zero;
        static const Value Null;
        static const Value EmptyPair;

    private:
        static ValuePair    NullPair;
        static std::string  NullText;
        static Lambda       NullFunc;
        static Struct       NullUserType;
        static Instance     NullObject;
        static Sequence     NullSequence;
        static Hashtable    NullHashtable;
        static IntegerRange NullIntegerRange;
        
    public:
        enum Type {
            eNone       = '0',
            eInteger    = 'I',
            eReal       = 'R',
            eCharacter  = 'C',
            eBoolean    = 'B',
            ePair       = 'P',
            eString     = 'S',
            eClosure    = 'F',
            eUserType   = 'U',
            eUserObject = 'O',
            eArray      = 'A',
            eHashMap    = 'H',
            eRange      = 'G',
        };

        using Long       = long long int;
        using Double     = double;
        using Char       = char;
        using Bool       = bool;
        using Pair       = ValuePair;
        using Text       = std::string;
        using Func       = Lambda;
        using UserType   = Struct;
        using UserObject = Instance;
        using Array      = Sequence;
        using HashMap    = Hashtable;
        using Range      = IntegerRange;
        
    public:
        inline Value();
        inline Value(Long i);
        inline Value(Double r);
        inline Value(Char c);
        inline Value(Bool b);
        Value(const Pair &p);
        Value(const char *t);
        Value(const std::string &t);
        Value(const Lambda &f);
        Value(const Struct &s);
        Value(const Instance &o);
        Value(const Sequence &s);
        Value(const Hashtable &h);
        Value(const IntegerRange &r);

        inline Type type() const;
        
        inline bool isNull() const;
        inline bool isInt()  const;
        inline bool isReal() const;
        inline bool isChar() const;
        inline bool isBool() const;
        inline bool isPair() const;
        inline bool isString() const;
        inline bool isClosure() const;
        inline bool isUserType() const;
        inline bool isUserObject() const;
        inline bool isArray() const;
        inline bool isHashMap() const;
        inline bool isRange() const;
        
        inline bool isNumber() const;
        
        inline Long integer() const;
        inline Double real() const;
        inline Char character() const;
        inline Bool boolean() const;
        inline const Pair &pair() const;
        inline const Text &text() const;
        inline Text &text();
        inline const Func &closure() const;
        inline const UserType &userType() const;
        inline const UserObject &userObject() const;
        inline UserObject &userObject();
        inline const Array &array() const;
        inline Array &array();
        inline const HashMap &hashMap() const;
        inline HashMap &hashMap();
        inline const Range &range() const;
        inline Range &range();

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
        
        inline std::string typeToString() const;

        Value clone() const;
        Value asType(Type otherType) const;

    public:
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
            std::size_t operator()(const ValuePair &value) const noexcept;
            std::size_t operator()(const IntegerRange &rng) const noexcept;

            static inline std::size_t combine(std::size_t hash1, std::size_t hash2);
        };

    private:
        using VariantValue = std::variant<Long,
                                          Double,
                                          Char,
                                          Bool,
                                          PairPtr,
                                          StringPtr,
                                          LambdaPtr,
                                          StructPtr,
                                          InstancePtr,
                                          SequencePtr,
                                          HashtablePtr,
                                          IntegerRangePtr>;

        Type type_;
        VariantValue value_;
    };

    // --------------------------------------------------------------------------------
    // INLINE

    inline Value::Value()
        : type_(eNone)
    {}

    inline Value::Value(Long i)
        : type_(eInteger)
        , value_(i)
    {}

    inline Value::Value(Double r)
        : type_(eReal)
        , value_(r)
    {}

    inline Value::Value(Char c)
        : type_(eCharacter)
        , value_(c)
    {}

    inline Value::Value(Bool b)
        : type_(eBoolean)
        , value_(b)
    {}

    inline auto Value::type() const -> Type {
        return type_;
    }

    inline bool Value::isNull() const {
        return type_ == eNone;
    }

    inline bool Value::isInt() const {
        return type_ == eInteger;
    }

    inline bool Value::isReal() const {
        return type_ == eReal;
    }

    inline bool Value::isChar() const {
        return type_ == eCharacter;
    }

    inline bool Value::isBool() const {
        return type_ == eBoolean;
    }

    inline bool Value::isPair() const {
        return type_ == ePair;
    }

    inline bool Value::isString() const {
        return type_ == eString;
    }

    inline bool Value::isClosure() const {
        return type_ == eClosure;
    }

    inline bool Value::isUserType() const {
        return type_ == eUserType;
    }

    inline bool Value::isUserObject() const {
        return type_ == eUserObject;
    }

    inline bool Value::isArray() const {
        return type_ == eArray;
    }

    inline bool Value::isHashMap() const {
        return type_ == eHashMap;
    }

    inline bool Value::isRange() const {
        return type_ == eRange;
    }

    inline bool Value::isNumber() const {
        return type_ == eInteger || type_ == eReal;
    }

    inline auto Value::integer() const -> Long {
        return isInt() ? std::get<Long>(value_) : 0;
    }

    inline auto Value::real() const -> Double {
        return isReal() ? std::get<Double>(value_) : (isInt() ? (Double)std::get<Long>(value_) : 0.0);
    }

    inline auto Value::character() const -> Char {
        return isChar() ? std::get<Char>(value_) : '\0';
    }

    inline auto Value::boolean() const -> Bool {
        return isBool() ? std::get<Bool>(value_) : false;
    }

    inline auto Value::pair() const -> const Pair & {
        return isPair() ? *std::get<PairPtr>(value_) : NullPair;
    }

    inline auto Value::text() const -> const Text & {
        return isString() ? *std::get<StringPtr>(value_) : NullText;
    }

    inline auto Value::text() -> Text & {
        return isString() ? *std::get<StringPtr>(value_) : NullText;
    }

    inline auto Value::closure() const -> const Func & {
        return isClosure() ? *std::get<LambdaPtr>(value_) : NullFunc;
    }

    inline auto Value::userType() const -> const UserType & {
        return isUserType() ? *std::get<StructPtr>(value_) : NullUserType;
    }

    inline auto Value::userObject() const -> const UserObject & {
        return isUserObject() ? *std::get<InstancePtr>(value_) : NullObject;
    }

    inline auto Value::userObject() -> UserObject & {
        return isUserObject() ? *std::get<InstancePtr>(value_) : NullObject;
    }

    inline auto Value::array() const -> const Array & {
        return isArray() ? *std::get<SequencePtr>(value_) : NullSequence;
    }

    inline auto Value::array() -> Array & {
        return isArray() ? *std::get<SequencePtr>(value_) : NullSequence;
    }

    inline auto Value::hashMap() const -> const HashMap & {
        return isHashMap() ? *std::get<HashtablePtr>(value_) : NullHashtable;
    }

    inline auto Value::hashMap() -> HashMap & {
        return isHashMap() ? *std::get<HashtablePtr>(value_) : NullHashtable;
    }

    inline auto Value::range() const -> const Range & {
        return isRange() ? *std::get<IntegerRangePtr>(value_) : NullIntegerRange;
    }

    inline auto Value::range() -> Range & {
        return isRange() ? *std::get<IntegerRangePtr>(value_) : NullIntegerRange;
    }

    inline std::string Value::typeToString() const {
        return typeToString(type_);
    }

    inline std::size_t Value::Hash::combine(std::size_t hash1, std::size_t hash2) {
        return hash1 ^ (hash2 << 1);
    }

}

#endif	// VALUE_H
