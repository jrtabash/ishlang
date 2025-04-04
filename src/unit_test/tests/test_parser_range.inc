#include "unit_test_function.h"

#include "environment.h"
#include "parser.h"
#include "sequence.h"
#include "value.h"

#include <vector>

using namespace Ishlang;

// -------------------------------------------------------------
DEFINE_TEST(testParserMakeRange) {
    auto env = Environment::make();
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(range 10)",        Value(IntegerRange(10)),          true));
    TEST_CASE(parserTest(parser, env, "(range 1 10)",      Value(IntegerRange(1, 10)),       true));
    TEST_CASE(parserTest(parser, env, "(range 0 12 3)",    Value(IntegerRange(0, 12, 3)),    true));
    TEST_CASE(parserTest(parser, env, "(range -10 -1)",    Value(IntegerRange(-10, -1, 1)),  true));
    TEST_CASE(parserTest(parser, env, "(range -10 0 2)",   Value(IntegerRange(-10, 0, 2)),   true));
    TEST_CASE(parserTest(parser, env, "(range -1 -10 -1)", Value(IntegerRange(-1, -10, -1)), true));

    TEST_CASE(parserTest(parser, env, "(range)",          Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(range -10)",      Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(range -1 -10)",   Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(range 10 1)",     Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(range -1 -10 2)", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(range 10 1 2)",   Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(range 'a')",      Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(range 1 'a')",    Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(range 1 10 'a')", Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserRangeGetters) {
    auto env = Environment::make();
    Parser parser;

    env->def("rng", Value(IntegerRange(1, 25, 2)));

    TEST_CASE(parserTest(parser, env, "(rngbegin rng)", Value(1ll),  true));
    TEST_CASE(parserTest(parser, env, "(rngend rng)",   Value(25ll), true));
    TEST_CASE(parserTest(parser, env, "(rngstep rng)",  Value(2ll),  true));
    TEST_CASE(parserTest(parser, env, "(rnglen rng)",   Value(12ll), true));

    TEST_CASE(parserTest(parser, env, "(rngbegin)",   Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(rngbegin 5)", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(rngend)",     Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(rngend 5)",   Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(rngstep)",    Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(rngstep 5)",  Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(rnglen)",     Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(rnglen 5)",   Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserExpand) {
    auto env = Environment::make();
    Parser parser;

    env->def("rng1", Value(IntegerRange(1, 6, 2)));
    env->def("rng2", Value(IntegerRange(7, 9, 2)));

    const auto val1 = Value(Sequence(std::vector{Value(1ll), Value(3ll), Value(5ll)}));
    const auto val2 = Value(Sequence(std::vector{Value(1ll), Value(3ll), Value(5ll), Value(7ll)}));

    TEST_CASE(parserTest(parser, env, "(expand rng1)",      val1, true));
    TEST_CASE(parserTest(parser, env, "(expand rng1 rng2)", val2, true));

    TEST_CASE(parserTest(parser, env, "(expand)",        Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(expand 1)",      Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(expand rng1 1)", Value::Null, false));
}
