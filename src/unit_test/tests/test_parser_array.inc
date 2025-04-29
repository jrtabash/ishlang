#include "unit_test_function.h"

#include "environment.h"
#include "parser.h"
#include "sequence.h"
#include "value.h"

using namespace Ishlang;

// -------------------------------------------------------------
DEFINE_TEST(testParserMakeArray) {
    auto env = Environment::make();
    Parser parser;

    const Value v1(1ll);
    const Value v2(2ll);
    const Value v3(3ll);

    TEST_CASE(parserTest(parser, env, "(array)",       Value(Sequence()),             true));
    TEST_CASE(parserTest(parser, env, "(array 1)",     Value(Sequence({v1})),         true));
    TEST_CASE(parserTest(parser, env, "(array 1 2)",   Value(Sequence({v1, v2})),     true));
    TEST_CASE(parserTest(parser, env, "(array 1 2 3)", Value(Sequence({v1, v2, v3})), true));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserMakeArraySV) {
    auto env = Environment::make();
    Parser parser;

    const Value v0(0ll);
    const Value v1(1ll);
    const Value v2(2ll);

    TEST_CASE(parserTest(parser, env, "(arraysv 1 0)",   Value(Sequence({v0})),          true));
    TEST_CASE(parserTest(parser, env, "(arraysv 2 1)",   Value(Sequence({v1, v1})),      true));
    TEST_CASE(parserTest(parser, env, "(arraysv 3 2)",   Value(Sequence({v2, v2, v2})),  true));
    TEST_CASE(parserTest(parser, env, "(arraysv 1)",     Value(Sequence({Value::Null})), true));
    TEST_CASE(parserTest(parser, env, "(arraysv)",       Value::Null,                    false));
    TEST_CASE(parserTest(parser, env, "(arraysv 'a' 0)", Value::Null,                    false));
    TEST_CASE(parserTest(parser, env, "(arraysv)",       Value::Null,                    false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserMakeArraySG) {
    auto env = Environment::make();
    Parser parser;

    const Value v1(1ll);
    const Value v2(2ll);
    const Value v3(3ll);
    const Value v4(4ll);

    env->def("i", Value::Zero);

    TEST_CASE(parserTest(parser, env, "(arraysg 0 (lambda () (= i (+ i 1))))",                         Value(Sequence()),             true));
    TEST_CASE(parserTest(parser, env, "(arraysg 3 (lambda () (= i (+ i 1))))",                         Value(Sequence({v1, v2, v3})), true));
    TEST_CASE(parserTest(parser, env, "(arraysg 2 ((lambda () (var g 0) (lambda () (= g (+ g 2))))))", Value(Sequence({v2, v4})),     true));

    TEST_CASE(parserTest(parser, env, "(arraysg)",                               Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(arraysg 5)",                             Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(arraysg 5 10)",                          Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(arraysg 'a' (lambda () (= i (+ i 1))))", Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserArrayLen) {
    auto env = Environment::make();
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(arrlen (array))",         Value::Zero, true));
    TEST_CASE(parserTest(parser, env, "(arrlen (array 1))",       Value(1ll),  true));
    TEST_CASE(parserTest(parser, env, "(arrlen (array 1 2 3 4))", Value(4ll),  true));
    TEST_CASE(parserTest(parser, env, "(arrlen 10)",              Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(arrlen)",                 Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(arrlen (array) (array))", Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserArrayGet) {
    auto env = Environment::make();
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(arrget (array 'B' 'y') 0)", Value('B'),  true));
    TEST_CASE(parserTest(parser, env, "(arrget (array 'B' 'y') 1)", Value('y'),  true));
    TEST_CASE(parserTest(parser, env, "(arrget (array 'B' 'y') 2)", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(arrget)",                   Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(arrget (array 1))",         Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserArraySet) {
    auto env = Environment::make();
    Parser parser;

    env->def("arr", Value(Sequence({Value(1ll), Value(2ll), Value(3ll), Value(4ll), Value(5ll)})));

    TEST_CASE(parserTest(parser, env, "(arrset arr 0 'a')", Value('a'),  true));
    TEST_CASE(parserTest(parser, env, "(arrset arr 2 'C')", Value('C'),  true));
    TEST_CASE(parserTest(parser, env, "(arrset arr 6 'b')", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(arrset)",           Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(arrset arr)",       Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(arrset arr 0)",     Value::Null, false));

    Value expected(Sequence({Value('a'), Value(2ll), Value('C'), Value(4ll), Value(5ll)}));
    TEST_CASE(parserTest(parser, env, "arr", expected, true));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserArrayPush) {
    auto env = Environment::make();
    Parser parser;

    env->def("arr", Value(Sequence()));

    Value expected = Value(Sequence({Value(1ll)}));
    TEST_CASE(parserTest(parser, env, "(arrpush arr 1)", expected, true));

    expected = Value(Sequence({Value(1ll), Value(2ll)}));
    TEST_CASE(parserTest(parser, env, "(arrpush arr 2)", expected, true));

    TEST_CASE(parserTest(parser, env, "(arrpush)",      Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(arrpush arr)",  Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserArrayPop) {
    auto env = Environment::make();
    Parser parser;

    env->def("arr", arrval(Value(0ll), Value(1ll)));

    TEST_CASE(parserTest(parser, env, "(arrpop arr)", Value(1ll),         true));
    TEST_CASE(parserTest(parser, env, "arr",          arrval(Value(0ll)), true));
    TEST_CASE(parserTest(parser, env, "(arrpop arr)", Value(0ll),         true));
    TEST_CASE(parserTest(parser, env, "arr",          arrval(),           true));

    TEST_CASE(parserTest(parser, env, "(arrpop arr)", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(arrpop)",     Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserArrayFind) {
    auto env = Environment::make();
    Parser parser;

    env->def("arr", Value(Sequence({Value('a'), Value('b'), Value('c'), Value('a')})));

    TEST_CASE(parserTest(parser, env, "(arrfind arr 'a')", Value(0ll),  true));
    TEST_CASE(parserTest(parser, env, "(arrfind arr 'b')", Value(1ll),  true));
    TEST_CASE(parserTest(parser, env, "(arrfind arr 'c')", Value(2ll),  true));
    TEST_CASE(parserTest(parser, env, "(arrfind arr 'd')", Value(-1ll), true));

    TEST_CASE(parserTest(parser, env, "(arrfind arr 'a' 1)", Value(3ll),  true));
    TEST_CASE(parserTest(parser, env, "(arrfind arr 'b' 1)", Value(1ll),  true));
    TEST_CASE(parserTest(parser, env, "(arrfind arr 'c' 1)", Value(2ll),  true));
    TEST_CASE(parserTest(parser, env, "(arrfind arr 'd' 1)", Value(-1ll), true));

    TEST_CASE(parserTest(parser, env, "(arrfind arr 'c' -1)", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(arrfind arr 'c' 4)",  Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(arrfind)",            Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(arrfind arr)",        Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserArrayCount) {
    auto env = Environment::make();
    Parser parser;

    const Value v1(1ll);
    const Value v2(2ll);
    const Value v3(3ll);
    const Value v4(4ll);

    env->def("arr", Value(Sequence({v1, v2, v3, v2, v3, v4, v3, v4, v4, v4})));

    TEST_CASE(parserTest(parser, env, "(arrcount arr 1)", Value(1ll), true));
    TEST_CASE(parserTest(parser, env, "(arrcount arr 2)", Value(2ll), true));
    TEST_CASE(parserTest(parser, env, "(arrcount arr 3)", Value(3ll), true));
    TEST_CASE(parserTest(parser, env, "(arrcount arr 4)", Value(4ll), true));
    TEST_CASE(parserTest(parser, env, "(arrcount arr 5)", Value(0ll), true));

    TEST_CASE(parserTest(parser, env, "(arrcount '5' 0)", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(arrcount)",       Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(arrcount arr)",   Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserArraySort) {
    auto env = Environment::make();
    Parser parser;

    env->def("arr1", Value(Sequence({Value('b'), Value('d'), Value('c'), Value('a')})));
    env->def("arr2", Value(Sequence({Value(2ll), Value(3ll), Value(4ll), Value(1ll)})));

    const Value arr1Asc(Sequence({Value('a'), Value('b'), Value('c'), Value('d')}));
    const Value arr2Asc(Sequence({Value(1ll), Value(2ll), Value(3ll), Value(4ll)}));
    const Value arr1Desc(Sequence({Value('d'), Value('c'), Value('b'), Value('a')}));
    const Value arr2Desc(Sequence({Value(4ll), Value(3ll), Value(2ll), Value(1ll)}));

    TEST_CASE(parserTest(parser, env, "(progn (arrsort arr1) arr1)",       arr1Asc,  true));
    TEST_CASE(parserTest(parser, env, "(progn (arrsort arr1 true) arr1)",  arr1Desc, true));
    TEST_CASE(parserTest(parser, env, "(progn (arrsort arr1 false) arr1)", arr1Asc,  true));

    TEST_CASE(parserTest(parser, env, "(progn (arrsort arr2) arr2)",       arr2Asc,  true));
    TEST_CASE(parserTest(parser, env, "(progn (arrsort arr2 true) arr2)",  arr2Desc, true));
    TEST_CASE(parserTest(parser, env, "(progn (arrsort arr2 false) arr2)", arr2Asc,  true));

    TEST_CASE(parserTest(parser, env, "(arrsort (array 3 1))",       Value(Sequence({Value(1ll), Value(3ll)})), true));
    TEST_CASE(parserTest(parser, env, "(arrsort (array 1 3) true)",  Value(Sequence({Value(3ll), Value(1ll)})), true));
    TEST_CASE(parserTest(parser, env, "(arrsort (array 1 3) false)", Value(Sequence({Value(1ll), Value(3ll)})), true));

    TEST_CASE(parserTest(parser, env, "(arrsort (array 3 1 1))",       Value(Sequence({Value(1ll), Value(1ll), Value(3ll)})), true));
    TEST_CASE(parserTest(parser, env, "(arrsort (array 1 1 3) true)",  Value(Sequence({Value(3ll), Value(1ll), Value(1ll)})), true));
    TEST_CASE(parserTest(parser, env, "(arrsort (array 1 1 3) false)", Value(Sequence({Value(1ll), Value(1ll), Value(3ll)})), true));

    TEST_CASE(parserTest(parser, env, "(arrsort 34251)",       Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(arrsort 34251 true)",  Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(arrsort 34251 false)", Value::Null, false));

    TEST_CASE(parserTest(parser, env, "(strsort arr1 3)", Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserArrayReverse) {
    auto env = Environment::make();
    Parser parser;

    env->def("arr1", Value(Sequence({Value('b'), Value('d'), Value('c'), Value('a')})));
    env->def("arr2", Value(Sequence({Value(2ll), Value(3ll), Value(4ll), Value(1ll)})));

    const Value arr1Orig(Sequence({Value('b'), Value('d'), Value('c'), Value('a')}));
    const Value arr2Orig(Sequence({Value(2ll), Value(3ll), Value(4ll), Value(1ll)}));
    const Value arr1Rev(Sequence({Value('a'), Value('c'), Value('d'), Value('b')}));
    const Value arr2Rev(Sequence({Value(1ll), Value(4ll), Value(3ll), Value(2ll)}));

    TEST_CASE(parserTest(parser, env, "(progn (arrrev arr1) arr1)",  arr1Rev,  true));
    TEST_CASE(parserTest(parser, env, "(progn (arrrev arr1) arr1)",  arr1Orig,  true));
    TEST_CASE(parserTest(parser, env, "(progn (arrrev arr2) arr2)",  arr2Rev,  true));
    TEST_CASE(parserTest(parser, env, "(progn (arrrev arr2) arr2)",  arr2Orig,  true));

    TEST_CASE(parserTest(parser, env, "(arrrev (array 3 1))",   Value(Sequence({Value(1ll), Value(3ll)})), true));
    TEST_CASE(parserTest(parser, env, "(arrrev (array 3 1 1))", Value(Sequence({Value(1ll), Value(1ll), Value(3ll)})), true));

    TEST_CASE(parserTest(parser, env, "(arrrev 34251)", Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserArrayClear) {
    auto env = Environment::make();
    Parser parser;

    env->def("a", Value(Sequence({Value('a'), Value('b'), Value('c')})));

    TEST_CASE(parserTest(parser, env, "(arrlen a)", Value(3ll),  true));
    TEST_CASE(parserTest(parser, env, "(arrclr a)", Value::Null, true));
    TEST_CASE(parserTest(parser, env, "(arrlen a)", Value(0ll),  true));

    TEST_CASE(parserTest(parser, env, "(arrlen)",     Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(arrlen a 2)", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(arrlen 5)",   Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserArrayInsert) {
    auto env = Environment::make();
    Parser parser;

    env->def("arr", Value(Sequence()));

    const Value v0 = Value::Zero;
    const Value v1 = Value(1ll);
    const Value v2 = Value(2ll);
    const Value v3 = Value(3ll);
    const Value v4 = Value(4ll);
    const Value v5 = Value(5ll);

    TEST_CASE(parserTest(parser, env, "(arrins arr 0 3)", v3,         true));
    TEST_CASE(parserTest(parser, env, "arr",              arrval(v3), true));

    TEST_CASE(parserTest(parser, env, "(arrins arr 0 1)", v1,             true));
    TEST_CASE(parserTest(parser, env, "arr",              arrval(v1, v3), true));

    TEST_CASE(parserTest(parser, env, "(arrins arr 2 5)", v5,                 true));
    TEST_CASE(parserTest(parser, env, "arr",              arrval(v1, v3, v5), true));

    TEST_CASE(parserTest(parser, env, "(arrins arr 0 0)", v0,                     true));
    TEST_CASE(parserTest(parser, env, "arr",              arrval(v0, v1, v3, v5), true));

    TEST_CASE(parserTest(parser, env, "(arrins arr 2 2)", v2,                         true));
    TEST_CASE(parserTest(parser, env, "arr",              arrval(v0, v1, v2, v3, v5), true));

    TEST_CASE(parserTest(parser, env, "(arrins arr 4 4)", v4,                             true));
    TEST_CASE(parserTest(parser, env, "arr",              arrval(v0, v1, v2, v3, v4, v5), true));

    TEST_CASE(parserTest(parser, env, "(arrins 3 5 5)",     Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(arrins arr 'a' 5)", Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserArrayRemove) {
    auto env = Environment::make();
    Parser parser;

    const Value v0 = Value::Zero;
    const Value v1 = Value(1ll);
    const Value v2 = Value(2ll);
    const Value v3 = Value(3ll);
    const Value v4 = Value(4ll);
    const Value v5 = Value(5ll);

    env->def("arr", arrval(v0, v1, v2, v3, v4, v5));

    TEST_CASE(parserTest(parser, env, "(arrrem arr 4)", Value::Null,                true));
    TEST_CASE(parserTest(parser, env, "arr",            arrval(v0, v1, v2, v3, v5), true));

    TEST_CASE(parserTest(parser, env, "(arrrem arr 2)", Value::Null,            true));
    TEST_CASE(parserTest(parser, env, "arr",            arrval(v0, v1, v3, v5), true));

    TEST_CASE(parserTest(parser, env, "(arrrem arr 0)", Value::Null,            true));
    TEST_CASE(parserTest(parser, env, "arr",            arrval(v1, v3, v5),     true));

    TEST_CASE(parserTest(parser, env, "(arrrem arr 2)", Value::Null,    true));
    TEST_CASE(parserTest(parser, env, "arr",            arrval(v1, v3), true));

    TEST_CASE(parserTest(parser, env, "(arrrem arr 0)", Value::Null, true));
    TEST_CASE(parserTest(parser, env, "arr",            arrval(v3),  true));

    TEST_CASE(parserTest(parser, env, "(arrrem arr 0)", Value::Null, true));
    TEST_CASE(parserTest(parser, env, "arr",            arrval(),    true));

    TEST_CASE(parserTest(parser, env, "(arrrem 3 5)",     Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(arrrem arr 'a')", Value::Null, false));
}
