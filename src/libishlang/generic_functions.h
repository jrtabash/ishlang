#ifndef ISHLANG_GENERIC_FUNCTIONS_H
#define ISHLANG_GENERIC_FUNCTIONS_H

#include "value.h"

#include <concepts>

namespace Ishlang {

    // -------------------------------------------------------------
    template <typename ObjectType>
    concept Sizable = requires (ObjectType obj) {
        { obj.size() } -> std::same_as<std::size_t>;
    };

    // -------------------------------------------------------------
    class Generic {
    public:
        template <typename ObjectType>
        static inline Value length(const ObjectType &obj) {
            return Value(Value::Long(obj.size()));
        }

        template <typename ObjectType>
        static inline Value get(const ObjectType &obj, const Value &key, const Value &defaultRet = Value::Null) {
            if constexpr (std::is_same_v<ObjectType, Value::HashMap>) {
                return Value(obj.get(key, defaultRet));
            }
            else if constexpr (std::is_same_v<ObjectType, Value::UserObject>) {
                if (!key.isString()) {
                    throw InvalidOperandType("String", key.typeToString());
                }
                return obj.get(key.text());
            }
            else {
                static_assert(std::is_same_v<ObjectType, Value::Text> || std::is_same_v<ObjectType, Value::Array>);

                if (!key.isInt()) {
                    throw InvalidOperandType("Integer", key.typeToString());
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
    };

}

#endif // ISHLANG_GENERIC_FUNCTIONS_H
