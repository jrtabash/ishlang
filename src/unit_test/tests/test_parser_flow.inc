#include "environment.h"
#include "parser.h"
#include "value.h"

using namespace Ishlang;

// -------------------------------------------------------------
DEFINE_TEST(testParserBlock) {
    auto env = Environment::make();
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(progn (var c 'a') (= c 'b') c)",   Value('b'),  true));
    TEST_CASE(parserTest(parser, env, "(progn (var x 2) (= x (+ x 1)))",   Value(3ll),  true));
    TEST_CASE(parserTest(parser, env, "(block (var f false) (= f true))",  Value(true), true));
    TEST_CASE(parserTest(parser, env, "(block (var y 5) (= y (* 2 y)) y)", Value(10ll), true));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserCond) {
    auto env = Environment::make();
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(if true 1)",                   Value(1ll),  true));
    TEST_CASE(parserTest(parser, env, "(if false 1)",                  Value::Null, true));
    TEST_CASE(parserTest(parser, env, "(if true 1 2)",                 Value(1ll),  true));
    TEST_CASE(parserTest(parser, env, "(if false 1 2)",                Value(2ll),  true));
    TEST_CASE(parserTest(parser, env, "(if (== 1 1) (+ 1 1) (+ 2 2))", Value(2ll),  true));
    TEST_CASE(parserTest(parser, env, "(if (== 1 2) (+ 1 1) (+ 2 2))", Value(4ll),  true));
    TEST_CASE(parserTest(parser, env, "(if (>= -2 0) 1 -1)",           Value(-1ll), true));

    TEST_CASE(parserTest(parser, env, "(cond (true 1))",                           Value(1ll),  true));
    TEST_CASE(parserTest(parser, env, "(cond (false 1))",                          Value::Null, true));
    TEST_CASE(parserTest(parser, env, "(cond (true 1) (false 2) (true 3))",        Value(1ll),  true));
    TEST_CASE(parserTest(parser, env, "(cond ((== 1 2) 1) ((== 1 1) 2) (true 3))", Value(2ll),  true));
    TEST_CASE(parserTest(parser, env, "(cond ((== 1 2) 1) ((== 2 3) 2) (true 3))", Value(3ll),  true));
    TEST_CASE(parserTest(parser, env, "(cond)",                                    Value::Null, false));

    TEST_CASE(parserTest(parser, env, "(when true 1)",   Value(1ll),  true));
    TEST_CASE(parserTest(parser, env, "(when false 1)",  Value::Null, true));
    TEST_CASE(parserTest(parser, env, "(when true)",     Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(when true 1 2)", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(when)",          Value::Null, false));

    TEST_CASE(parserTest(parser, env, "(unless true 1)",    Value::Null, true));
    TEST_CASE(parserTest(parser, env, "(unless false 1)",   Value(1ll),  true));
    TEST_CASE(parserTest(parser, env, "(unless false)",     Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(unless false 1 2)", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(unless)",           Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserLoop) {
    auto env = Environment::make();
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(loop false 10)",                                                              Value::Null, true));
    TEST_CASE(parserTest(parser, env, "(loop (var i 1) (< i 4) (= i (+ i 1)) i)",                                     Value(3ll),  true));
    TEST_CASE(parserTest(parser, env, "(loop (progn (var i 1) (var sum 0)) (< i 4) (= i (+ i 1)) (= sum (+ sum i)))", Value(6ll),  true));
    TEST_CASE(parserTest(parser, env, "(block (var sum 0) (loop (< sum 5) (= sum (+ sum 2))))",                       Value(6ll),  true));
    TEST_CASE(parserTest(parser, env, "(loop true (break))",                                                          Value::Null, true));
    TEST_CASE(parserTest(parser, env, "(block (var sum 0) (loop true (progn (= sum 3) (break))))",                    Value::Null, true));
    TEST_CASE(parserTest(parser, env, "(block (var sum 0) (loop true (progn (= sum 3) (break))) sum)",                Value(3ll),  true));
    TEST_CASE(parserTest(parser, env, "(loop (var i 1) (< i 4) (= i (+ i 1)))",                                       Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(loop (var i 1))",                                                             Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(loop)",                                                                       Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserForeach) {
    auto env = Environment::make();
    Parser parser;

    auto add_var = [&](const char *name, const char *val) {
        const auto expr = std::string("(var ") + name + " " + val + ")";
        parser.read(expr)->eval(env);
    };
    add_var("str", "\"abcdef\"");
    add_var("arr", "(array 1 2 3 4 5)");
    add_var("ht", "(hashmap (pair 1 10) (pair 2 20))");
    add_var("rng", "(range 10)");
    add_var("strCnt", "0");
    add_var("arrSum", "0");
    add_var("htSum", "0");
    add_var("keySum", "0");
    add_var("valSum", "0");
    add_var("rngSum", "0");
    add_var("dummy", "0");

    TEST_CASE(parserTest(parser, env, "(foreach c str (= strCnt (+ strCnt 1)))",                        Value(6ll),  true));
    TEST_CASE(parserTest(parser, env, "(foreach i arr (= arrSum (+ arrSum i)))",                        Value(15ll), true));
    TEST_CASE(parserTest(parser, env, "(foreach kv ht (= htSum (+ htSum (* (first kv) (second kv)))))", Value(50ll), true));
    TEST_CASE(parserTest(parser, env, "(foreach kv ht (progn (= keySum (+ keySum (first kv))) (= valSum (+ valSum (second kv)))))", Value(30ll), true));
    TEST_CASE(parserTest(parser, env, "(foreach i rng (= rngSum (+ rngSum i)))",                        Value(45ll), true));

    add_var("arrCnt", "0");
    TEST_CASE(parserTest(parser, env, "(foreach i arr (progn (when (> i 3) (break)) (= arrCnt (+ arrCnt 1))))", Value::Null, true));
    TEST_CASE(env->get("arrCnt") == Value(3ll));

    TEST_CASE(parserTest(parser, env, "(foreach)",                 Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(foreach x)",               Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(foreach x str)",           Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(foreach x 5 (+ dummy 1))", Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserLambda) {
    auto env = Environment::make();
    Parser parser;

    TEST_CASE(parserTest(parser, env, "((lambda (x) (+ x 1)) 3)",           Value(4ll),  true));
    TEST_CASE(parserTest(parser, env, "((lambda (x y) (+ (* x 2) y)) 2 1)", Value(5ll),  true));
    TEST_CASE(parserTest(parser, env, "((var x 1) 3)",                      Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserFtn) {
    auto env = Environment::make();
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(block (defun add (x y) (+ x y)) (add 1 2))",                                              Value(3ll),  true));
    TEST_CASE(parserTest(parser, env, "(block (var x 3) (var y 2) (defun sumxy () (+ x y)) (block (var x 4) (var y 5) (sumxy)))", Value(5ll),  true));
    TEST_CASE(parserTest(parser, env, "(block (defun inc (x) (+ x 1)) (defun add3 (x) (inc (inc (inc x)))) (add3 5))",            Value(8ll),  true));

    TEST_CASE(parserTest(parser, env, "(progn (defun add4 (x) (defun inc2 (y) (+ y 2)) (inc2 (inc2 x))) (add4 4))", Value(8ll),  true));
    TEST_CASE(parserTest(parser, env, "(add4 1)",                                                                   Value(5ll),  true));
    TEST_CASE(parserTest(parser, env, "(inc 2)",                                                                    Value::Null, false));
}
