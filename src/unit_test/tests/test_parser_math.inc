#include "unit_test_function.h"

#include "environment.h"
#include "parser.h"
#include "value.h"

using namespace Ishlang;

// -------------------------------------------------------------
DEFINE_TEST(testParserMathAbs) {
    auto env = Environment::make();
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(abs 5)",    Value(5ll), true));
    TEST_CASE(parserTest(parser, env, "(abs -5)",   Value(5ll), true));
    TEST_CASE(parserTest(parser, env, "(abs 2.0)",  Value(2.0), true));
    TEST_CASE(parserTest(parser, env, "(abs -2.0)", Value(2.0), true));
    TEST_CASE(parserTest(parser, env, "(abs 0)",    Value(0ll), true));

    TEST_CASE(parserTest(parser, env, "(abs)",     Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(abs 'x')", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(abs 1 2)", Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserMathMin) {
    auto env = Environment::make();
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(min 2 5)",                Value(2ll),  true));
    TEST_CASE(parserTest(parser, env, "(min 5 2)",                Value(2ll),  true));
    TEST_CASE(parserTest(parser, env, "(min 2.0 -3.0)",           Value(-3.0), true));
    TEST_CASE(parserTest(parser, env, "(min -3.0 2.0)",           Value(-3.0), true));
    TEST_CASE(parserTest(parser, env, "(min 1.0 -2.0 3.0)",       Value(-2.0), true));
    TEST_CASE(parserTest(parser, env, "(min 1.0 -2 3.0)",         Value(-2ll), true));
    TEST_CASE(parserTest(parser, env, "(min -2.0 3 -5.0 10.0 8)", Value(-5.0), true));

    TEST_CASE(parserTest(parser, env, "(min)",       Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(min 1)",     Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(min 'x')",   Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(min 'x' 2)", Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserMathMax) {
    auto env = Environment::make();
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(max 2 5)",                Value(5ll),  true));
    TEST_CASE(parserTest(parser, env, "(max 5 2)",                Value(5ll),  true));
    TEST_CASE(parserTest(parser, env, "(max 2.0 -3.0)",           Value(2.0),  true));
    TEST_CASE(parserTest(parser, env, "(max -3.0 2.0)",           Value(2.0),  true));
    TEST_CASE(parserTest(parser, env, "(max 1.0 -2.0 3.0)",       Value(3.0),  true));
    TEST_CASE(parserTest(parser, env, "(max 1 -2.0 3)",           Value(3ll),  true));
    TEST_CASE(parserTest(parser, env, "(max -2.0 3 -5.0 10.0 8)", Value(10.0), true));

    TEST_CASE(parserTest(parser, env, "(max)",       Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(max 1)",     Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(max 'x')",   Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(max 'x' 2)", Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserMathSign) {
    auto env = Environment::make();
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(sign 5)",    Value(1ll),  true));
    TEST_CASE(parserTest(parser, env, "(sign -5)",   Value(-1ll), true));
    TEST_CASE(parserTest(parser, env, "(sign 2.0)",  Value(1ll),  true));
    TEST_CASE(parserTest(parser, env, "(sign -2.0)", Value(-1ll), true));
    TEST_CASE(parserTest(parser, env, "(sign 0)",    Value(0ll),  true));

    TEST_CASE(parserTest(parser, env, "(sign)",     Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(sign 'x')", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(sign 1 2)", Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserMathSqrt) {
    auto env = Environment::make();
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(sqrt 1.0)",   Value(1.0),  true));
    TEST_CASE(parserTest(parser, env, "(sqrt 1)",     Value(1.0),  true));
    TEST_CASE(parserTest(parser, env, "(sqrt 25)",    Value(5.0),  true));
    TEST_CASE(parserTest(parser, env, "(sqrt 100.0)", Value(10.0), true));
    TEST_CASE(parserTest(parser, env, "(sqrt 0)",     Value(0.0),  true));

    TEST_CASE(parserTest(parser, env, "(sqrt)",     Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(sqrt 'x')", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(sqrt 1 2)", Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserMathCeil) {
    auto env = Environment::make();
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(ceil 2.1)",   Value(3.0),  true));
    TEST_CASE(parserTest(parser, env, "(ceil -2.1)",  Value(-2.0), true));
    TEST_CASE(parserTest(parser, env, "(ceil 1.0)",   Value(1.0),  true));
    TEST_CASE(parserTest(parser, env, "(ceil -1.0)",  Value(-1.0), true));
    TEST_CASE(parserTest(parser, env, "(ceil 0.0)",   Value(0.0),  true));

    TEST_CASE(parserTest(parser, env, "(ceil)",     Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(ceil 'x')", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(ceil 1 2)", Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserMathFloor) {
    auto env = Environment::make();
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(floor 2.9)",   Value(2.0),  true));
    TEST_CASE(parserTest(parser, env, "(floor -2.1)",  Value(-3.0), true));
    TEST_CASE(parserTest(parser, env, "(floor 1.0)",   Value(1.0),  true));
    TEST_CASE(parserTest(parser, env, "(floor -1.0)",  Value(-1.0), true));
    TEST_CASE(parserTest(parser, env, "(floor 0.0)",   Value(0.0),  true));

    TEST_CASE(parserTest(parser, env, "(floor)",     Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(floor 'x')", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(floor 1 2)", Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserMathRound) {
    auto env = Environment::make();
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(round 2.9)",   Value(3.0),  true));
    TEST_CASE(parserTest(parser, env, "(round 2.1)",   Value(2.0),  true));
    TEST_CASE(parserTest(parser, env, "(round -2.1)",  Value(-2.0), true));
    TEST_CASE(parserTest(parser, env, "(round -2.7)",  Value(-3.0), true));
    TEST_CASE(parserTest(parser, env, "(round 1.0)",   Value(1.0),  true));
    TEST_CASE(parserTest(parser, env, "(round -1.0)",  Value(-1.0), true));
    TEST_CASE(parserTest(parser, env, "(round 0.0)",   Value(0.0),  true));

    TEST_CASE(parserTest(parser, env, "(round)",     Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(round 'x')", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(round 1 2)", Value::Null, false));
}
