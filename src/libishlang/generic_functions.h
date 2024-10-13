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
    };

}

#endif // ISHLANG_GENERIC_FUNCTIONS_H
