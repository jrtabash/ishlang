#ifndef ISHLANG_MATH_FUNCTIONS_H
#define ISHLANG_MATH_FUNCTIONS_H

#include "value.h"

#include <algorithm>
#include <cassert>
#include <cmath>

namespace Ishlang {

    // -------------------------------------------------------------
    class Math {
    public:
        using Values = std::vector<Value>;

    public:
        static inline Value abs(const Values &nums) {
            assert(nums.size() == 1);
            assert(nums[0].isNumber());
            const Value &num = nums[0];
            return num < Value::Zero
                ? num.isReal() ? Value(-num.real()) : Value(-num.integer())
                : num;
        }

        static inline Value min(const Values &nums) {
            assert(nums.size() >= 2);
            assert(std::ranges::all_of(nums, [](const Value &v) { return v.isNumber(); }));
            return *std::min_element(nums.begin(), nums.end());
        }

        static inline Value max(const Values &nums) {
            assert(nums.size() >= 2);
            assert(std::ranges::all_of(nums, [](const Value &v) { return v.isNumber(); }));
            return *std::max_element(nums.begin(), nums.end());
        }

        static inline Value sign(const Values &nums) {
            assert(nums.size() == 1);
            assert(nums[0].isNumber());
            const Value &num = nums[0];
            if      (num > Value::Zero) { return Value::PlusOne; }
            else if (num < Value::Zero) { return Value::MinusOne; }
            else                        { return Value::Zero; }
        }

        static inline Value sqrt(const Values &nums) {
            assert(nums.size() == 1);
            assert(nums[0].isNumber());
            return Value(std::sqrt(nums[0].real()));
        }

        static inline Value ceil(const Values &nums) {
            assert(nums.size() == 1);
            assert(nums[0].isNumber());
            return Value(std::ceil(nums[0].real()));
        }

        static inline Value floor(const Values &nums) {
            assert(nums.size() == 1);
            assert(nums[0].isNumber());
            return Value(std::floor(nums[0].real()));
        }

        static inline Value round(const Values &nums) {
            assert(nums.size() == 1);
            assert(nums[0].isNumber());
            return Value(std::round(nums[0].real()));
        }
    };

}

#endif // ISHLANG_MATH_FUNCTIONS_H
