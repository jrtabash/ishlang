#include "unit_test_function.h"

#include "environment.h"
#include "generic_table.h"
#include "parser.h"
#include "value.h"

using namespace Ishlang;

// -------------------------------------------------------------
DEFINE_TEST(testParserMakeOrderedMap) {
    auto env = Environment::make();
    Parser parser;

    auto ot = [](int count) {
        OrderedTable ot;
        if (count >= 1) { ot.set(Value('a'), Value(0ll)); }
        if (count >= 2) { ot.set(Value('b'), Value(1ll)); }
        if (count >= 3) { ot.set(Value('c'), Value(2ll)); }
        return ot;
    };

    TEST_CASE(parserTest(parser, env, "(orderedmap)",                                           Value(ot(0)), true));
    TEST_CASE(parserTest(parser, env, "(orderedmap (pair 'a' 0))",                              Value(ot(1)), true));
    TEST_CASE(parserTest(parser, env, "(orderedmap (pair 'a' 0) (pair 'b' 1))",                 Value(ot(2)), true));
    TEST_CASE(parserTest(parser, env, "(orderedmap (pair 'a' 0) (pair 'b' 1) (pair 'c' 2))",    Value(ot(3)), true));
    TEST_CASE(parserTest(parser, env, "(orderedmap (array 'a' 0))",                             Value(ot(1)), true));
    TEST_CASE(parserTest(parser, env, "(orderedmap (array 'a' 0) (array 'b' 1))",               Value(ot(2)), true));
    TEST_CASE(parserTest(parser, env, "(orderedmap (array 'a' 0) (array 'b' 1) (array 'c' 2))", Value(ot(3)), true));

    TEST_CASE(parserTest(parser, env, "(orderedmap (pair))",        Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(orderedmap (pair 1))",      Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(orderedmap (pair 1 2 3))",  Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(orderedmap (array))",       Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(orderedmap (array 1))",     Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(orderedmap (array 1 2 3))", Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserOrderedMapLen) {
    auto env = Environment::make();
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(omlen (orderedmap))",                                           Value::Zero, true));
    TEST_CASE(parserTest(parser, env, "(omlen (orderedmap (array 'a' 1)))",                             Value(1ll),  true));
    TEST_CASE(parserTest(parser, env, "(omlen (orderedmap (array 'a' 1) (array 'b' 2) (array 'c' 3)))", Value(3ll),  true));

    TEST_CASE(parserTest(parser, env, "(omlen 10)",                        Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(omlen)",                           Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(omlen (orderedmap) (orderedmap))", Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserOrderedMapContains) {
    auto env = Environment::make();
    Parser parser;

    OrderedTable ot;
    ot.set(Value('a'), Value(1ll));
    ot.set(Value('b'), Value(2ll));
    ot.set(Value('c'), Value(3ll));
    env->def("ot", ot);

    TEST_CASE(parserTest(parser, env, "(omhas (orderedmap) 'a')", Value::False, true));
    TEST_CASE(parserTest(parser, env, "(omhas ot 'a')",           Value::True,  true));
    TEST_CASE(parserTest(parser, env, "(omhas ot 'b')",           Value::True,  true));
    TEST_CASE(parserTest(parser, env, "(omhas ot 'c')",           Value::True,  true));
    TEST_CASE(parserTest(parser, env, "(omhas ot 'd')",           Value::False, true));

    TEST_CASE(parserTest(parser, env, "(omhas)",            Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(omhas ot)",         Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(omhas ot 'a' 'b')", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(omhas 'a')",        Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserOrderedMapGet) {
    auto env = Environment::make();
    Parser parser;

    OrderedTable ot;
    ot.set(Value('a'), Value(1ll));
    ot.set(Value('b'), Value(2ll));
    ot.set(Value('c'), Value(3ll));
    env->def("ot", ot);

    TEST_CASE(parserTest(parser, env, "(omget (orderedmap) 'a')", Value::Null, true));
    TEST_CASE(parserTest(parser, env, "(omget ot 'a')",           Value(1ll),  true));
    TEST_CASE(parserTest(parser, env, "(omget ot 'b')",           Value(2ll),  true));
    TEST_CASE(parserTest(parser, env, "(omget ot 'c')",           Value(3ll),  true));
    TEST_CASE(parserTest(parser, env, "(omget ot 'd')",           Value::Null, true));

    TEST_CASE(parserTest(parser, env, "(omget (orderedmap) 'a' 0)", Value::Zero, true));
    TEST_CASE(parserTest(parser, env, "(omget ot 'a' 0)",           Value(1ll),  true));
    TEST_CASE(parserTest(parser, env, "(omget ot 'b' 0)",           Value(2ll),  true));
    TEST_CASE(parserTest(parser, env, "(omget ot 'c' 0)",           Value(3ll),  true));
    TEST_CASE(parserTest(parser, env, "(omget ot 'd' 0)",           Value::Zero, true));

    TEST_CASE(parserTest(parser, env, "(omget)",            Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(omget ot)",         Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(omget ot 'a' 0 1)", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(omget 'a' 0)",      Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserOrderedMapSet) {
    auto env = Environment::make();
    Parser parser;

OrderedTable ot;
    env->def("ot", Value(ot));

    auto otget = [&env](Value::Char key) { return env->get("ot").orderedMap().get(Value(key)); };
    auto otlen = [&env]() { return env->get("ot").orderedMap().size(); };

    TEST_CASE(parserTest(parser, env, "(omset ot 'a' 1)", Value(1ll), true));
    TEST_CASE(parserTest(parser, env, "(omset ot 'b' 2)", Value(2ll), true));
    TEST_CASE(parserTest(parser, env, "(omset ot 'c' 3)", Value(3ll), true));

    TEST_CASE(otlen() == 3);
    TEST_CASE(otget('a') == Value(1ll));
    TEST_CASE(otget('b') == Value(2ll));
    TEST_CASE(otget('c') == Value(3ll));

    TEST_CASE(parserTest(parser, env, "(omset ot 'a' 100)", Value(100ll), true));
    TEST_CASE(parserTest(parser, env, "(omset ot 'b' 200)", Value(200ll), true));
    TEST_CASE(parserTest(parser, env, "(omset ot 'c' 300)", Value(300ll), true));

    TEST_CASE(otlen() == 3);
    TEST_CASE(otget('a') == Value(100ll));
    TEST_CASE(otget('b') == Value(200ll));
    TEST_CASE(otget('c') == Value(300ll));

    TEST_CASE(parserTest(parser, env, "(omset)",            Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(omset ot)",         Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(omset ot 'a')",     Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(omset ot 'a' 1 2)", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(omset 'a' 1 2)",    Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserOrderedMapRemove) {
    auto env = Environment::make();
    Parser parser;

    OrderedTable ot;
    ot.set(Value('a'), Value(1ll));
    ot.set(Value('b'), Value(2ll));
    ot.set(Value('c'), Value(3ll));
    env->def("ot", Value(ot));

    auto othas = [&env](Value::Char key) { return env->get("ot").orderedMap().exists(Value(key)); };
    auto otlen = [&env]() { return env->get("ot").orderedMap().size(); };

    TEST_CASE(parserTest(parser, env, "(omrem ot 'd')", Value::Null, true));
    TEST_CASE(otlen() == 3);
    TEST_CASE(othas('a'));
    TEST_CASE(othas('b'));
    TEST_CASE(othas('c'));

    TEST_CASE(parserTest(parser, env, "(omrem ot 'c')", Value::Null, true));
    TEST_CASE(otlen() == 2);
    TEST_CASE(othas('a'));
    TEST_CASE(othas('b'));
    TEST_CASE(!othas('c'));

    TEST_CASE(parserTest(parser, env, "(omrem ot 'b')", Value::Null, true));
    TEST_CASE(otlen() == 1);
    TEST_CASE(othas('a'));
    TEST_CASE(!othas('b'));
    TEST_CASE(!othas('c'));

    TEST_CASE(parserTest(parser, env, "(omrem ot 'a')", Value::Null, true));
    TEST_CASE(otlen() == 0);
    TEST_CASE(!othas('a'));
    TEST_CASE(!othas('b'));
    TEST_CASE(!othas('c'));

    TEST_CASE(parserTest(parser, env, "(omrem)",          Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(omrem ot)",       Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(omrem ot 'a' 1)", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(omrem 'a' 1)",    Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserOrderedMapClear) {
    auto env = Environment::make();
    Parser parser;

    OrderedTable ot;
    ot.set(Value('a'), Value(1ll));
    ot.set(Value('b'), Value(2ll));
    ot.set(Value('c'), Value(3ll));
    env->def("ot", Value(ot));

    auto othas = [&env](Value::Char key) { return env->get("ot").orderedMap().exists(Value(key)); };
    auto otlen = [&env]() { return env->get("ot").orderedMap().size(); };

    TEST_CASE(otlen() == 3);
    TEST_CASE(othas('a'));
    TEST_CASE(othas('b'));
    TEST_CASE(othas('c'));

    TEST_CASE(parserTest(parser, env, "(omclr ot)", Value::Null, true));

    TEST_CASE(otlen() == 0);
    TEST_CASE(!othas('a'));
    TEST_CASE(!othas('b'));
    TEST_CASE(!othas('c'));

    TEST_CASE(parserTest(parser, env, "(omclr ot)", Value::Null, true));

    TEST_CASE(parserTest(parser, env, "(omclr)",        Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(omclr ot 'a')", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(omclr 'a')",    Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserOrderedMapFind) {
    auto env = Environment::make();
    Parser parser;

    OrderedTable ot;
    ot.set(Value('a'), Value(1ll));
    ot.set(Value('b'), Value(2ll));
    ot.set(Value('c'), Value(3ll));
    env->def("ot", Value(ot));

    TEST_CASE(parserTest(parser, env, "(omfind ot 1)", Value('a'),  true));
    TEST_CASE(parserTest(parser, env, "(omfind ot 2)", Value('b'),  true));
    TEST_CASE(parserTest(parser, env, "(omfind ot 3)", Value('c'),  true));
    TEST_CASE(parserTest(parser, env, "(omfind ot 4)", Value::Null, true));

    TEST_CASE(parserTest(parser, env, "(omfind)",        Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(omfind ot)",     Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(omfind ot 1 2)", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(omfind 1 2)",    Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserOrderedMapCount) {
    auto env = Environment::make();
    Parser parser;

    OrderedTable ot;
    ot.set(Value('a'), Value(1ll));
    ot.set(Value('b'), Value(2ll));
    ot.set(Value('c'), Value(3ll));
    ot.set(Value('2'), Value(2ll));
    env->def("ot", Value(ot));

    TEST_CASE(parserTest(parser, env, "(omcount ot 1)", Value(1ll),  true));
    TEST_CASE(parserTest(parser, env, "(omcount ot 2)", Value(2ll),  true));
    TEST_CASE(parserTest(parser, env, "(omcount ot 3)", Value(1ll),  true));
    TEST_CASE(parserTest(parser, env, "(omcount ot 4)", Value::Zero, true));

    TEST_CASE(parserTest(parser, env, "(omcount)",        Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(omcount ot)",     Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(omcount ot 1 2)", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(omcount 1 2)",    Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserOrderedMapKeys) {
    auto env = Environment::make();
    Parser parser;

    OrderedTable ot;
    ot.set(Value('a'), Value(1ll));
    ot.set(Value('b'), Value(2ll));
    ot.set(Value('c'), Value(3ll));
    env->def("ot", Value(ot));

    TEST_CASE(parserTest(parser, env, "(omkeys (orderedmap))", Value(Sequence()), true));
    TEST_CASE(parserTest(parser, env, "(arrlen (omkeys ot))",  Value(3ll),        true));

    TEST_CASE(parserTest(parser, env, "(omkeys)",         Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(omkeys ot 'a')",  Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(omkeys 'a' 'b')", Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserOrderedMapVals) {
    auto env = Environment::make();
    Parser parser;

    OrderedTable ot;
    ot.set(Value('a'), Value(1ll));
    ot.set(Value('b'), Value(2ll));
    ot.set(Value('c'), Value(3ll));
    ot.set(Value('2'), Value(2ll));
    env->def("ot", Value(ot));

    TEST_CASE(parserTest(parser, env, "(omvals (orderedmap))", Value(Sequence()), true));
    TEST_CASE(parserTest(parser, env, "(arrlen (omvals ot))",  Value(4ll),        true));

    TEST_CASE(parserTest(parser, env, "(omvals)",         Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(omvals ot 'a')",  Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(omvals 'a' 'b')", Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserOrderedMapItems) {
    auto env = Environment::make();
    Parser parser;

    OrderedTable ot;
    ot.set(Value('a'), Value(1ll));
    ot.set(Value('b'), Value(2ll));
    ot.set(Value('c'), Value(3ll));
    ot.set(Value('2'), Value(2ll));
    env->def("ot", Value(ot));

    TEST_CASE(parserTest(parser, env, "(omitems (orderedmap))", Value(Sequence()), true));
    TEST_CASE(parserTest(parser, env, "(arrlen (omitems ot))",  Value(4ll),        true));

    TEST_CASE(parserTest(parser, env, "(omitems)",         Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(omitems ot 'a')",  Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(omitems 'a' 'b')", Value::Null, false));
}
