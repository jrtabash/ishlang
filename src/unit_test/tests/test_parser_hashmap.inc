#include "environment.h"
#include "hashtable.h"
#include "parser.h"
#include "value.h"

using namespace Ishlang;

// -------------------------------------------------------------
DEFINE_TEST(testParserMakeHashMap) {
    auto env = Environment::make();
    Parser parser;

    auto ht = [](int count) {
                  Hashtable ht;
                  if (count >= 1) { ht.set(Value('a'), Value(0ll)); }
                  if (count >= 2) { ht.set(Value('b'), Value(1ll)); }
                  if (count >= 3) { ht.set(Value('c'), Value(2ll)); }
                  return ht;
              };

    TEST_CASE(parserTest(parser, env, "(hashmap)",                                           Value(ht(0)), true));
    TEST_CASE(parserTest(parser, env, "(hashmap (pair 'a' 0))",                              Value(ht(1)), true));
    TEST_CASE(parserTest(parser, env, "(hashmap (pair 'a' 0) (pair 'b' 1))",                 Value(ht(2)), true));
    TEST_CASE(parserTest(parser, env, "(hashmap (pair 'a' 0) (pair 'b' 1) (pair 'c' 2))",    Value(ht(3)), true));
    TEST_CASE(parserTest(parser, env, "(hashmap (array 'a' 0))",                             Value(ht(1)), true));
    TEST_CASE(parserTest(parser, env, "(hashmap (array 'a' 0) (array 'b' 1))",               Value(ht(2)), true));
    TEST_CASE(parserTest(parser, env, "(hashmap (array 'a' 0) (array 'b' 1) (array 'c' 2))", Value(ht(3)), true));

    TEST_CASE(parserTest(parser, env, "(hashmap (pair))",        Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hashmap (pair 1))",      Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hashmap (pair 1 2 3))",  Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hashmap (array))",       Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hashmap (array 1))",     Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hashmap (array 1 2 3))", Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserHashMapLen) {
    auto env = Environment::make();
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(hmlen (hashmap))",                                           Value::Zero, true));
    TEST_CASE(parserTest(parser, env, "(hmlen (hashmap (array 'a' 1)))",                             Value(1ll),  true));
    TEST_CASE(parserTest(parser, env, "(hmlen (hashmap (array 'a' 1) (array 'b' 2) (array 'c' 3)))", Value(3ll),  true));

    TEST_CASE(parserTest(parser, env, "(hmlen 10)",                  Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmlen)",                     Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmlen (hashamp) (hashmap))", Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserHashMapContains) {
    auto env = Environment::make();
    Parser parser;

    Hashtable ht;
    ht.set(Value('a'), Value(1ll));
    ht.set(Value('b'), Value(2ll));
    ht.set(Value('c'), Value(3ll));
    env->def("ht", ht);

    TEST_CASE(parserTest(parser, env, "(hmhas (hashmap) 'a')",  Value::False, true));
    TEST_CASE(parserTest(parser, env, "(hmhas ht 'a')",         Value::True,  true));
    TEST_CASE(parserTest(parser, env, "(hmhas ht 'b')",         Value::True,  true));
    TEST_CASE(parserTest(parser, env, "(hmhas ht 'c')",         Value::True,  true));
    TEST_CASE(parserTest(parser, env, "(hmhas ht 'd')",         Value::False, true));

    TEST_CASE(parserTest(parser, env, "(hmhas)",            Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmhas ht)",         Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmhas ht 'a' 'b')", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmhas 'a')",        Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserHashMapGet) {
    auto env = Environment::make();
    Parser parser;

    Hashtable ht;
    ht.set(Value('a'), Value(1ll));
    ht.set(Value('b'), Value(2ll));
    ht.set(Value('c'), Value(3ll));
    env->def("ht", ht);

    TEST_CASE(parserTest(parser, env, "(hmget (hashmap) 'a')", Value::Null, true));
    TEST_CASE(parserTest(parser, env, "(hmget ht 'a')",        Value(1ll),  true));
    TEST_CASE(parserTest(parser, env, "(hmget ht 'b')",        Value(2ll),  true));
    TEST_CASE(parserTest(parser, env, "(hmget ht 'c')",        Value(3ll),  true));
    TEST_CASE(parserTest(parser, env, "(hmget ht 'd')",        Value::Null, true));

    TEST_CASE(parserTest(parser, env, "(hmget (hashmap) 'a' 0)", Value::Zero, true));
    TEST_CASE(parserTest(parser, env, "(hmget ht 'a' 0)",        Value(1ll),  true));
    TEST_CASE(parserTest(parser, env, "(hmget ht 'b' 0)",        Value(2ll),  true));
    TEST_CASE(parserTest(parser, env, "(hmget ht 'c' 0)",        Value(3ll),  true));
    TEST_CASE(parserTest(parser, env, "(hmget ht 'd' 0)",        Value::Zero, true));

    TEST_CASE(parserTest(parser, env, "(hmget)",            Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmget ht)",         Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmget ht 'a' 0 1)", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmget 'a' 0)",      Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserHashMapSet) {
    auto env = Environment::make();
    Parser parser;

    Hashtable ht;
    env->def("ht", Value(ht));

    auto htget = [&env](Value::Char key) { return env->get("ht").hashMap().get(Value(key)); };
    auto htlen = [&env]() { return env->get("ht").hashMap().size(); };

    TEST_CASE(parserTest(parser, env, "(hmset ht 'a' 1)", Value(1ll), true));
    TEST_CASE(parserTest(parser, env, "(hmset ht 'b' 2)", Value(2ll), true));
    TEST_CASE(parserTest(parser, env, "(hmset ht 'c' 3)", Value(3ll), true));

    TEST_CASE(htlen() == 3);
    TEST_CASE(htget('a') == Value(1ll));
    TEST_CASE(htget('b') == Value(2ll));
    TEST_CASE(htget('c') == Value(3ll));

    TEST_CASE(parserTest(parser, env, "(hmset ht 'a' 100)", Value(100ll), true));
    TEST_CASE(parserTest(parser, env, "(hmset ht 'b' 200)", Value(200ll), true));
    TEST_CASE(parserTest(parser, env, "(hmset ht 'c' 300)", Value(300ll), true));

    TEST_CASE(htlen() == 3);
    TEST_CASE(htget('a') == Value(100ll));
    TEST_CASE(htget('b') == Value(200ll));
    TEST_CASE(htget('c') == Value(300ll));

    TEST_CASE(parserTest(parser, env, "(hmset)",            Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmset ht)",         Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmset ht 'a')",     Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmset ht 'a' 1 2)", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmset 'a' 1 2)",    Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserHashMapRemove) {
    auto env = Environment::make();
    Parser parser;

    Hashtable ht;
    ht.set(Value('a'), Value(1ll));
    ht.set(Value('b'), Value(2ll));
    ht.set(Value('c'), Value(3ll));
    env->def("ht", Value(ht));

    auto hthas = [&env](Value::Char key) { return env->get("ht").hashMap().exists(Value(key)); };
    auto htlen = [&env]() { return env->get("ht").hashMap().size(); };

    TEST_CASE(parserTest(parser, env, "(hmrem ht 'd')", Value::Null, true));
    TEST_CASE(htlen() == 3);
    TEST_CASE(hthas('a'));
    TEST_CASE(hthas('b'));
    TEST_CASE(hthas('c'));

    TEST_CASE(parserTest(parser, env, "(hmrem ht 'c')", Value::Null, true));
    TEST_CASE(htlen() == 2);
    TEST_CASE(hthas('a'));
    TEST_CASE(hthas('b'));
    TEST_CASE(!hthas('c'));

    TEST_CASE(parserTest(parser, env, "(hmrem ht 'b')", Value::Null, true));
    TEST_CASE(htlen() == 1);
    TEST_CASE(hthas('a'));
    TEST_CASE(!hthas('b'));
    TEST_CASE(!hthas('c'));

    TEST_CASE(parserTest(parser, env, "(hmrem ht 'a')", Value::Null, true));
    TEST_CASE(htlen() == 0);
    TEST_CASE(!hthas('a'));
    TEST_CASE(!hthas('b'));
    TEST_CASE(!hthas('c'));

    TEST_CASE(parserTest(parser, env, "(hmrem)",          Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmrem ht)",       Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmrem ht 'a' 1)", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmrem 'a' 1)",    Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserHashMapClear) {
    auto env = Environment::make();
    Parser parser;

    Hashtable ht;
    ht.set(Value('a'), Value(1ll));
    ht.set(Value('b'), Value(2ll));
    ht.set(Value('c'), Value(3ll));
    env->def("ht", Value(ht));

    auto hthas = [&env](Value::Char key) { return env->get("ht").hashMap().exists(Value(key)); };
    auto htlen = [&env]() { return env->get("ht").hashMap().size(); };

    TEST_CASE(htlen() == 3);
    TEST_CASE(hthas('a'));
    TEST_CASE(hthas('b'));
    TEST_CASE(hthas('c'));

    TEST_CASE(parserTest(parser, env, "(hmclr ht)", Value::Null, true));

    TEST_CASE(htlen() == 0);
    TEST_CASE(!hthas('a'));
    TEST_CASE(!hthas('b'));
    TEST_CASE(!hthas('c'));

    TEST_CASE(parserTest(parser, env, "(hmclr ht)", Value::Null, true));

    TEST_CASE(parserTest(parser, env, "(hmclr)",        Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmclr ht 'a')", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmclr 'a')",    Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserHashMapFind) {
    auto env = Environment::make();
    Parser parser;

    Hashtable ht;
    ht.set(Value('a'), Value(1ll));
    ht.set(Value('b'), Value(2ll));
    ht.set(Value('c'), Value(3ll));
    env->def("ht", Value(ht));

    TEST_CASE(parserTest(parser, env, "(hmfind ht 1)", Value('a'),  true));
    TEST_CASE(parserTest(parser, env, "(hmfind ht 2)", Value('b'),  true));
    TEST_CASE(parserTest(parser, env, "(hmfind ht 3)", Value('c'),  true));
    TEST_CASE(parserTest(parser, env, "(hmfind ht 4)", Value::Null, true));

    TEST_CASE(parserTest(parser, env, "(hmfind)",        Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmfind ht)",     Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmfind ht 1 2)", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmfind 1 2)",    Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserHashMapCount) {
    auto env = Environment::make();
    Parser parser;

    Hashtable ht;
    ht.set(Value('a'), Value(1ll));
    ht.set(Value('b'), Value(2ll));
    ht.set(Value('c'), Value(3ll));
    ht.set(Value('2'), Value(2ll));
    env->def("ht", Value(ht));

    TEST_CASE(parserTest(parser, env, "(hmcount ht 1)", Value(1ll),  true));
    TEST_CASE(parserTest(parser, env, "(hmcount ht 2)", Value(2ll),  true));
    TEST_CASE(parserTest(parser, env, "(hmcount ht 3)", Value(1ll),  true));
    TEST_CASE(parserTest(parser, env, "(hmcount ht 4)", Value::Zero, true));

    TEST_CASE(parserTest(parser, env, "(hmcount)",        Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmcount ht)",     Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmcount ht 1 2)", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmcount 1 2)",    Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserHashMapKeys) {
    auto env = Environment::make();
    Parser parser;

    Hashtable ht;
    ht.set(Value('a'), Value(1ll));
    ht.set(Value('b'), Value(2ll));
    ht.set(Value('c'), Value(3ll));
    env->def("ht", Value(ht));

    TEST_CASE(parserTest(parser, env, "(hmkeys (hashmap))",   Value(Sequence()), true));
    TEST_CASE(parserTest(parser, env, "(arrlen (hmkeys ht))", Value(3ll),        true));

    TEST_CASE(parserTest(parser, env, "(hmkeys)",         Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmkeys ht 'a')",  Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmkeys 'a' 'b')", Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserHashMapVals) {
    auto env = Environment::make();
    Parser parser;

    Hashtable ht;
    ht.set(Value('a'), Value(1ll));
    ht.set(Value('b'), Value(2ll));
    ht.set(Value('c'), Value(3ll));
    ht.set(Value('2'), Value(2ll));
    env->def("ht", Value(ht));

    TEST_CASE(parserTest(parser, env, "(hmvals (hashmap))",   Value(Sequence()), true));
    TEST_CASE(parserTest(parser, env, "(arrlen (hmvals ht))", Value(4ll),        true));

    TEST_CASE(parserTest(parser, env, "(hmvals)",         Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmvals ht 'a')",  Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmvals 'a' 'b')", Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserHashMapItems) {
    auto env = Environment::make();
    Parser parser;

    Hashtable ht;
    ht.set(Value('a'), Value(1ll));
    ht.set(Value('b'), Value(2ll));
    ht.set(Value('c'), Value(3ll));
    ht.set(Value('2'), Value(2ll));
    env->def("ht", Value(ht));

    TEST_CASE(parserTest(parser, env, "(hmitems (hashmap))",   Value(Sequence()), true));
    TEST_CASE(parserTest(parser, env, "(arrlen (hmitems ht))", Value(4ll),        true));

    TEST_CASE(parserTest(parser, env, "(hmitems)",         Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmitems ht 'a')",  Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmitems 'a' 'b')", Value::Null, false));
}
