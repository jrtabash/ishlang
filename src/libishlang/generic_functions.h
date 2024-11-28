#ifndef ISHLANG_GENERIC_FUNCTIONS_H
#define ISHLANG_GENERIC_FUNCTIONS_H

#include "code_node_util.h"
#include "value.h"

#include <concepts>

namespace Ishlang {

    // -------------------------------------------------------------
    template <typename ObjectType>
    concept Sizable = requires (ObjectType obj) {
        { obj.size() } -> std::same_as<std::size_t>;
    };

    template <typename ObjectType>
    concept Clearable = requires (ObjectType obj) {
        Sizable<ObjectType>;
        obj.clear();
    };

    // -------------------------------------------------------------
    class Generic {
    public:
        template <Sizable ObjectType>
        static inline Value length(const ObjectType &obj) {
            return Value(Value::Long(obj.size()));
        }

        template <Sizable ObjectType>
        static inline Value empty(const ObjectType &obj) {
            return Value(obj.size() == 0);
        }

        template <Clearable ObjectType>
        static inline Value clear(ObjectType &obj) {
            Value cnt = length(obj);
            obj.clear();
            return cnt;
        }

        template <typename ObjectType>
        static inline Value get(const ObjectType &obj, const Value &key, const Value &defaultRet = Value::Null) {
            if constexpr (std::is_same_v<ObjectType, Value::HashMap>) {
                return Value(obj.get(key, defaultRet));
            }
            else if constexpr (std::is_same_v<ObjectType, Value::UserObject>) {
                if (!key.isString()) {
                    throw InvalidOperandType(Value::typeToString(Value::eString), key.typeToString());
                }
                return obj.get(key.text());
            }
            else {
                static_assert(std::is_same_v<ObjectType, Value::Text> || std::is_same_v<ObjectType, Value::Array>);

                if (!key.isInt()) {
                    throw InvalidOperandType(Value::typeToString(Value::eInteger), key.typeToString());
                }

                const auto pos = key.integer();
                if (pos < 0 || static_cast<std::size_t>(pos) >= obj.size()) {
                    throw OutOfRange(Exception::format("%s get access", Value::typeToString(key.type()).c_str()));
                }

                if constexpr (std::is_same_v<ObjectType, Value::Text>) {
                    return Value(obj[pos]);
                }
                else {
                    return obj.get(pos);
                }
            }
        }

        static inline Value get(const Value::UserObject &obj, const std::string &memberName) {
            return Value(obj.get(memberName));
        }

        template <typename ObjectType>
        static inline void set(ObjectType &obj, const Value &key, const Value &value) {
            if constexpr (std::is_same_v<ObjectType, Value::HashMap>) {
                obj.set(key, value);
            }
            else if constexpr (std::is_same_v<ObjectType, Value::UserObject>) {
                if (!key.isString()) {
                    throw InvalidOperandType(Value::typeToString(Value::eString), key.typeToString());
                }
                obj.set(key.text(), value);
            }
            else {
                static_assert(std::is_same_v<ObjectType, Value::Text> || std::is_same_v<ObjectType, Value::Array>);

                if (!key.isInt()) {
                    throw InvalidOperandType(Value::typeToString(Value::eInteger), key.typeToString());
                }

                const auto pos = key.integer();
                if (pos < 0 || static_cast<std::size_t>(pos) >= obj.size()) {
                    throw OutOfRange(Exception::format("%s get access", Value::typeToString(key.type()).c_str()));
                }

                if constexpr (std::is_same_v<ObjectType, Value::Text>) {
                    if (!value.isChar()) {
                        throw InvalidOperandType(Value::typeToString(Value::eCharacter), value.typeToString());
                    }
                    obj[pos] = value.character();
                }
                else {
                    obj.set(pos, value);
                }
            }
        }

        static inline void set(Value::UserObject &obj, const std::string &memberName, const Value &value) {
            obj.set(memberName, value);
        }

        template <typename ObjectType>
        static inline Value find(const ObjectType &obj, const Value &item, const Value &pos = Value::Zero) {
            if constexpr (std::is_same_v<ObjectType, Value::HashMap>) {
                return obj.find(item);
            }
            else {
                static_assert(std::is_same_v<ObjectType, Value::Text> || std::is_same_v<ObjectType, Value::Array>);

                if (!pos.isInt()) {
                    throw InvalidOperandType(Value::typeToString(Value::eInteger), pos.typeToString());
                }

                const auto rawPos = pos.integer();
                if (rawPos < 0 || static_cast<std::size_t>(rawPos) >= obj.size()) {
                    throw OutOfRange(Exception::format("%s find position access", std::is_same_v<ObjectType, Value::Text> ? "string" : "array"));
                }

                if constexpr (std::is_same_v<ObjectType, Value::Text>) {
                    if (!item.isChar()) {
                        throw InvalidOperandType(Value::typeToString(Value::eCharacter), item.typeToString());
                    }

                    auto result = obj.find(item.character(), rawPos);
                    return result != std::string::npos ? Value(Value::Long(result)) : Value(-1ll);
                }
                else {
                    auto result = obj.find(item, rawPos);
                    return result ? Value(Value::Long(*result)) : Value(-1ll);
                }
            }
        }
    };

}

#endif // ISHLANG_GENERIC_FUNCTIONS_H
