#include "unit_test_function.h"

#include "environment.h"
#include "parser.h"
#include "value.h"

using namespace Ishlang;

// -------------------------------------------------------------
DEFINE_TEST(testParserArith) {
    auto env = Environment::make();
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(+ 1 2)",     Value(3ll),  true));
    TEST_CASE(parserTest(parser, env, "(+ 1.2 2.3)", Value(3.5),  true));
    TEST_CASE(parserTest(parser, env, "(+ 2 2.5)",   Value(4.5),  true));
    TEST_CASE(parserTest(parser, env, "(- 3 1)",     Value(2ll),  true));
    TEST_CASE(parserTest(parser, env, "(- 2 5)",     Value(-3ll), true));
    TEST_CASE(parserTest(parser, env, "(* 2 3)",     Value(6ll),  true));
    TEST_CASE(parserTest(parser, env, "(/ 10 2)",    Value(5ll),  true));
    TEST_CASE(parserTest(parser, env, "(/ 2 0)",     Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(% 4 2)",     Value(0ll),  true));
    TEST_CASE(parserTest(parser, env, "(% 2.4 2)",   Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(^ 3.0 2)",   Value(9.0),  true));
    TEST_CASE(parserTest(parser, env, "(^ 9.0 0.5)", Value(3.0),  true));

    TEST_CASE(parserTest(parser, env, "(+ 1 2 3 4 5)", Value(15ll),  true));
    TEST_CASE(parserTest(parser, env, "(- 15 2 4 6)",  Value(3ll),   true));
    TEST_CASE(parserTest(parser, env, "(* 2 3 4 5)",   Value(120ll), true));
    TEST_CASE(parserTest(parser, env, "(/ 100 10 5)",  Value(2ll),   true));
    TEST_CASE(parserTest(parser, env, "(% 100 30 4)",  Value(2ll),   true));
    TEST_CASE(parserTest(parser, env, "(^ 2 3 2)",     Value(64.0),  true));
    TEST_CASE(parserTest(parser, env, "(+)",           Value::Null,  false));
    TEST_CASE(parserTest(parser, env, "(+ 1)",         Value::Null,  false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserArithAssign) {
    auto env = Environment::make();
    Parser parser;

    env->def("num", Value(10ll));
    env->def("txt", Value("text"));
    env->def("ten", Value(10ll));

    TEST_CASE(parserTest(parser, env, "(+= num 10)", Value(20ll), true));
    TEST_CASE(parserTest(parser, env, "(-= num 5)",  Value(15ll), true));
    TEST_CASE(parserTest(parser, env, "(*= num 4)",  Value(60ll), true));
    TEST_CASE(parserTest(parser, env, "(/= num 6)",  Value(10ll), true));
    TEST_CASE(parserTest(parser, env, "(%= num 4)",  Value(2ll),  true));
    TEST_CASE(parserTest(parser, env, "(^= num 4)",  Value(16.0), true));
    TEST_CASE(parserTest(parser, env, "(+= num 4)",  Value(20.0), true));

    TEST_CASE(parserTest(parser, env, "(+=)",         Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(+= 1)",       Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(+= txt 1)",   Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(+= num 'c')", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(/= ten 0)",   Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(%= ten 0)",   Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserComp) {
    auto env = Environment::make();
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(== 2 2)",     Value::True,  true));
    TEST_CASE(parserTest(parser, env, "(== 'a' 'b')", Value::False, true));
    TEST_CASE(parserTest(parser, env, "(!= 2 3)",     Value::True,  true));
    TEST_CASE(parserTest(parser, env, "(!= 'a' 'a')", Value::False, true));
    TEST_CASE(parserTest(parser, env, "(> 3 2)",      Value::True,  true));
    TEST_CASE(parserTest(parser, env, "(> 'a' 'b')",  Value::False, true));
    TEST_CASE(parserTest(parser, env, "(< 3 2)",      Value::False, true));
    TEST_CASE(parserTest(parser, env, "(< 'a' 'b')",  Value::True,  true));
    TEST_CASE(parserTest(parser, env, "(>= 3 2)",     Value::True,  true));
    TEST_CASE(parserTest(parser, env, "(>= 3 3)",     Value::True,  true));
    TEST_CASE(parserTest(parser, env, "(>= 'a' 'b')", Value::False, true));
    TEST_CASE(parserTest(parser, env, "(<= 3 2)",     Value::False, true));
    TEST_CASE(parserTest(parser, env, "(<= 3 3)",     Value::True,  true));
    TEST_CASE(parserTest(parser, env, "(<= 'a' 'b')", Value::True,  true));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserLogicOp) {
    auto env = Environment::make();
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(and true true)",    Value::True,  true));
    TEST_CASE(parserTest(parser, env, "(and true false)",   Value::False, true));
    TEST_CASE(parserTest(parser, env, "(and false true)",   Value::False, true));
    TEST_CASE(parserTest(parser, env, "(and false false)",  Value::False, true));
    TEST_CASE(parserTest(parser, env, "(or true true)",     Value::True,  true));
    TEST_CASE(parserTest(parser, env, "(or true false)",    Value::True,  true));
    TEST_CASE(parserTest(parser, env, "(or false true)",    Value::True,  true));
    TEST_CASE(parserTest(parser, env, "(or false false)",   Value::False, true));

    TEST_CASE(parserTest(parser, env, "(and true true true)",    Value::True,  true));
    TEST_CASE(parserTest(parser, env, "(and false true true)",   Value::False, true));
    TEST_CASE(parserTest(parser, env, "(and true false true)",   Value::False, true));
    TEST_CASE(parserTest(parser, env, "(and true true false)",   Value::False, true));
    TEST_CASE(parserTest(parser, env, "(and false false false)", Value::False, true));

    TEST_CASE(parserTest(parser, env, "(or true true true)",    Value::True,  true));
    TEST_CASE(parserTest(parser, env, "(or false true true)",   Value::True,  true));
    TEST_CASE(parserTest(parser, env, "(or true false true)",   Value::True,  true));
    TEST_CASE(parserTest(parser, env, "(or true true false)",   Value::True,  true));
    TEST_CASE(parserTest(parser, env, "(or false false false)", Value::False, true));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserNot) {
    auto env = Environment::make();
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(not true)",     Value::False, true));
    TEST_CASE(parserTest(parser, env, "(not false)",    Value::True,  true));
    TEST_CASE(parserTest(parser, env, "(not (== 1 1))", Value::False, true));
    TEST_CASE(parserTest(parser, env, "(not (== 1 2))", Value::True,  true));
    TEST_CASE(parserTest(parser, env, "(not 3)",        Value::False, false));

    TEST_CASE(parserTest(parser, env, "(not)",            Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(not True False)", Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserNegativeOf) {
    auto env = Environment::make();
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(neg 0)",    Value::Zero, true));
    TEST_CASE(parserTest(parser, env, "(neg 1)",    Value(-1ll), true));
    TEST_CASE(parserTest(parser, env, "(neg -1)",   Value(1ll),  true));
    TEST_CASE(parserTest(parser, env, "(neg 2.5)",  Value(-2.5), true));
    TEST_CASE(parserTest(parser, env, "(neg -2.5)", Value(2.5),  true));

    TEST_CASE(parserTest(parser, env, "(neg)",     Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(neg 'a')", Value::Null, false));
}
