#ifndef ISHLANG_CODE_NODE_UTILS_H
#define ISHLANG_CODE_NODE_UTILS_H

#include "code_node_bases.h"
#include "environment.h"
#include "exception.h"
#include "value.h"

#include <string>

namespace Ishlang {

    template <typename ... Type>
    inline constexpr bool anyOfType(Value::Type type, Type ... altTypes) {
        static_assert(sizeof...(altTypes) > 0);
        static_assert((std::is_same_v<Type, Value::Type> && ...));

        return ((type == altTypes) || ...);
    }

    template <typename ... Type>
    inline std::string typesToString(Value::Type first, Type ... rest) {
        static_assert((std::is_same_v<Type, Value::Type> && ...));

        std::string ret = Value::typeToString(first);
        ((ret += '|' + Value::typeToString(rest)), ...);
        return ret;
    }

    template <typename ... Type>
    inline Value evalOperand(Environment::SharedPtr env,
                             const CodeNode::SharedPtr &expr,
                             Type ... expectTypes) {
        Value val = expr->eval(env);
        if (!anyOfType(val.type(), expectTypes...)) {
            throw InvalidOperandType(typesToString(expectTypes...), val.typeToString());
        }
        return val;
    }

}

#endif	// ISHLANG_CODE_NODE_UTILS_H
