#include "unit_test_function.h"

#include "exception.h"
#include "integer_range.h"

#include <string>

using namespace Ishlang;

// -------------------------------------------------------------
#define TEST_INT_RANGE(RNG, BEGIN, END, STEP, SIZE)                     \
    TEST_CASE_MSG(RNG.begin() == BEGIN, "actual=" << RNG.begin());      \
    TEST_CASE_MSG(RNG.end() == END, "actual=" << RNG.end());            \
    TEST_CASE_MSG(RNG.step() == STEP, "actual=" << RNG.step());         \
    TEST_CASE_MSG(RNG.size() == SIZE, "actual=" << RNG.size());
DEFINE_TEST(testIntegerRange) {
    TEST_INT_RANGE(IntegerRange(10), 0, 10, 1, 10);
    TEST_INT_RANGE(IntegerRange(0, 10), 0, 10, 1, 10);
    TEST_INT_RANGE(IntegerRange(0, 10, 1),  0, 10, 1, 10);
    TEST_INT_RANGE(IntegerRange(10,  0, -1), 10, 0, -1, 10);
    TEST_INT_RANGE(IntegerRange(-10, 0, 1), -10, 0, 1, 10);
    TEST_INT_RANGE(IntegerRange(0, -10, -1), 0, -10, -1, 10);

    TEST_INT_RANGE(IntegerRange(2, 23, 3), 2, 23, 3, 7);
    TEST_INT_RANGE(IntegerRange(23, 2, -3), 23, 2, -3, 7);
    TEST_INT_RANGE(IntegerRange(23, -2, -3), 23, -2, -3, 9);
    TEST_INT_RANGE(IntegerRange(-2, 23, 3), -2, 23, 3, 9);
    TEST_INT_RANGE(IntegerRange(-2, -23, -3), -2, -23, -3, 7);
    TEST_INT_RANGE(IntegerRange(-23, -2, 3), -23, -2, 3, 7);
    TEST_INT_RANGE(IntegerRange(1, 1, 1), 1, 1, 1, 0);
    TEST_INT_RANGE(IntegerRange(-1, -1, -1), -1, -1, -1, 0);

    try {
        IntegerRange(1, 10, 0);
        TEST_CASE(false);
    }
    catch (const Exception &ex) {
        TEST_CASE_MSG(std::string(ex.what()) == "Range step 0", "actual=" << ex.what());
    }

    try {
        IntegerRange(1, 10, -1);
        TEST_CASE(false);
    }
    catch (const Exception &ex) {
        TEST_CASE_MSG(std::string(ex.what()) == "Invalid range 1:-1:10", "actual=" << ex.what());
    }
}
#undef TEST_INT_RANGE

// -------------------------------------------------------------
DEFINE_TEST(testIntegerRangeComparison) {
    const auto r1 = IntegerRange(5);
    const auto r2 = IntegerRange(0, 5, 1);
    const auto r3 = IntegerRange(5, 0, -1);
    const auto r4 = IntegerRange(1, 5, 1);
    const auto r5 = IntegerRange(1, 6, 1);
    const auto r6 = IntegerRange(1, 7, 1);
    const auto r7 = IntegerRange(5, 1, -1);
    const auto r8 = IntegerRange(6, 1, -1);

    TEST_CASE(r1 == r1);
    TEST_CASE(r1 == r2);
    TEST_CASE(r1 != r3);
    TEST_CASE(r1 != r4);

    TEST_CASE(r1 < r6);
    TEST_CASE(r1 <= r6);
    TEST_CASE(r1 <= r5);

    TEST_CASE(r6 > r1);
    TEST_CASE(r6 >= r1);
    TEST_CASE(r5 >= r1);

    TEST_CASE(!(r1 < r3));
    TEST_CASE(!(r3 > r1));

    TEST_CASE(r8 >= r3);
    TEST_CASE(r3 > r7);

    TEST_CASE(r3 <= r8);
    TEST_CASE(r7 < r3);
}

// -------------------------------------------------------------
DEFINE_TEST(testIntegerRangeStartNext) {
    {
        const auto rng = IntegerRange(2, 12, 2);
        auto cur = rng.start(); TEST_CASE(cur == 2);
        cur = rng.next(*cur); TEST_CASE(cur == 4);
        cur = rng.next(*cur); TEST_CASE(cur == 6);
        cur = rng.next(*cur); TEST_CASE(cur == 8);
        cur = rng.next(*cur); TEST_CASE(cur == 10);
        cur = rng.next(*cur); TEST_CASE(cur == std::nullopt);
    }

    {
        const auto rng = IntegerRange(-2, -12, -2);
        auto cur = rng.start(); TEST_CASE(cur == -2);
        cur = rng.next(*cur); TEST_CASE(cur == -4);
        cur = rng.next(*cur); TEST_CASE(cur == -6);
        cur = rng.next(*cur); TEST_CASE(cur == -8);
        cur = rng.next(*cur); TEST_CASE(cur == -10);
        cur = rng.next(*cur); TEST_CASE(cur == std::nullopt);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testIntegerRangeGenerator) {
    {
        const auto rng = IntegerRange(2, 12, 2);
        auto gen = rng.generator();
        TEST_CASE(gen.next() == 2);
        TEST_CASE(gen.next() == 4);
        TEST_CASE(gen.next() == 6);
        TEST_CASE(gen.next() == 8);
        TEST_CASE(gen.next() == 10);
        TEST_CASE(gen.next() == std::nullopt);
    }

    {
        const auto rng = IntegerRange(-2, -12, -2);
        auto gen = rng.generator();
        TEST_CASE(gen.next() == -2);
        TEST_CASE(gen.next() == -4);
        TEST_CASE(gen.next() == -6);
        TEST_CASE(gen.next() == -8);
        TEST_CASE(gen.next() == -10);
        TEST_CASE(gen.next() == std::nullopt);
    }

    {
        const auto rng = IntegerRange(0, 0, 1);
        auto gen = rng.generator();
        TEST_CASE(gen.next() == std::nullopt);
    }

    {
        const auto rng = IntegerRange(1, 1, 1);
        auto gen = rng.generator();
        TEST_CASE(gen.next() == std::nullopt);
    }

    {
        const auto rng = IntegerRange(-1, -1, -1);
        auto gen = rng.generator();
        TEST_CASE(gen.next() == std::nullopt);
    }
}
