#include "unit_test_function.h"

#include "environment.h"
#include "hashtable.h"
#include "instance.h"
#include "parser.h"
#include "sequence.h"
#include "struct.h"
#include "value.h"
#include "value_pair.h"

#include <vector>

using namespace Ishlang;

// -------------------------------------------------------------
DEFINE_TEST(testParserGenericLen) {
    auto env = Environment::make();
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(len \"\")",                 Value(0ll), true));
    TEST_CASE(parserTest(parser, env, "(len \"text\")",             Value(4ll), true));
    TEST_CASE(parserTest(parser, env, "(len (array))",              Value(0ll), true));
    TEST_CASE(parserTest(parser, env, "(len (array 2 4 6))",        Value(3ll), true));
    TEST_CASE(parserTest(parser, env, "(len (hashmap))",            Value(0ll), true));
    TEST_CASE(parserTest(parser, env, "(len (hashmap (pair 2 4)))", Value(1ll), true));
    TEST_CASE(parserTest(parser, env, "(len (range 1 1))",          Value(0ll), true));
    TEST_CASE(parserTest(parser, env, "(len (range 5))",            Value(5ll), true));
    TEST_CASE(parserTest(parser, env, "(len (pair 1 2))",           Value(2ll), true));

    TEST_CASE(parserTest(parser, env, "(len)",   Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(len 5)", Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserGenericEmpty) {
    auto env = Environment::make();
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(empty \"\")",                 Value::True,  true));
    TEST_CASE(parserTest(parser, env, "(empty \"text\")",             Value::False, true));
    TEST_CASE(parserTest(parser, env, "(empty (array))",              Value::True,  true));
    TEST_CASE(parserTest(parser, env, "(empty (array 1))",            Value::False, true));
    TEST_CASE(parserTest(parser, env, "(empty (hashmap))",            Value::True,  true));
    TEST_CASE(parserTest(parser, env, "(empty (hashmap (pair 2 4)))", Value::False, true));
    TEST_CASE(parserTest(parser, env, "(empty (range 1 1))",          Value::True,  true));
    TEST_CASE(parserTest(parser, env, "(empty (range 5))",            Value::False, true));
    TEST_CASE(parserTest(parser, env, "(empty (pair 1 2))",           Value::False, true));

    TEST_CASE(parserTest(parser, env, "(empty)",   Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(empty 5)", Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserGenericGet) {
    auto env = Environment::make();
    Parser parser;

    env->def("txt", Value("abc"));
    env->def("seq", Value(Sequence(std::vector{Value(1ll), Value(2ll), Value(3ll)})));
    env->def("cpl", ValuePair(Value(2ll), Value(3ll)));

    Hashtable ht;
    ht.set(Value(1ll), Value(10ll));
    ht.set(Value(2ll), Value(20ll));
    env->def("ht", Value(ht));

    auto rawStruct = Struct("Person", Struct::MemberList{"name", "age"});
    env->def("inst", Value(Instance(rawStruct, Instance::InitArgs{{"name", Value("Jon")}, {"age", Value(25ll)}})));

    TEST_CASE(parserTest(parser, env, "(get txt 0)",   Value('a'),  true));
    TEST_CASE(parserTest(parser, env, "(get txt 1)",   Value('b'),  true));
    TEST_CASE(parserTest(parser, env, "(get txt 2)",   Value('c'),  true));

    TEST_CASE(parserTest(parser, env, "(get seq 0)",   Value(1ll),  true));
    TEST_CASE(parserTest(parser, env, "(get seq 1)",   Value(2ll),  true));
    TEST_CASE(parserTest(parser, env, "(get seq 2)",   Value(3ll),  true));

    TEST_CASE(parserTest(parser, env, "(get ht 1)",    Value(10ll), true));
    TEST_CASE(parserTest(parser, env, "(get ht 2)",    Value(20ll), true));
    TEST_CASE(parserTest(parser, env, "(get ht 3)",    Value::Null, true));
    TEST_CASE(parserTest(parser, env, "(get ht 3 30)", Value(30ll), true));

    TEST_CASE(parserTest(parser, env, "(get inst \"name\")", Value("Jon"), true));
    TEST_CASE(parserTest(parser, env, "(get inst \"age\")",  Value(25ll), true));
    TEST_CASE(parserTest(parser, env, "(get inst name)",     Value("Jon"), true));
    TEST_CASE(parserTest(parser, env, "(get inst age)",      Value(25ll), true));

    TEST_CASE(parserTest(parser, env, "(get cpl 0)", Value(2ll), true));
    TEST_CASE(parserTest(parser, env, "(get cpl 1)", Value(3ll), true));

    TEST_CASE(parserTest(parser, env, "(get txt 'a')",  Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(get txt 5)",    Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(get seq 'a')",  Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(get seq 5)",    Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(get inst 5)",   Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(get inst mem)", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(get cpl 2)",    Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(get cpl 'a')",  Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(get 5 0)",      Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserGenericSet) {
    auto env = Environment::make();
    Parser parser;

    env->def("txt", Value("abc"));
    env->def("seq", Value(Sequence(std::vector{Value(1ll), Value(2ll), Value(3ll)})));

    Hashtable ht;
    ht.set(Value(1ll), Value(10ll));
    ht.set(Value(2ll), Value(20ll));
    env->def("ht", Value(ht));

    auto rawStruct = Struct("Person", Struct::MemberList{"name", "age"});
    env->def("inst", Value(Instance(rawStruct, Instance::InitArgs{{"name", Value("J")}, {"age", Value(20ll)}})));

    TEST_CASE(parserTest(parser, env, "(set txt 0 'A')", Value('A'),  true));
    TEST_CASE(parserTest(parser, env, "(set txt 1 'B')", Value('B'),  true));
    TEST_CASE(parserTest(parser, env, "(set txt 2 'C')", Value('C'),  true));

    TEST_CASE_MSG(env->get("txt") == Value("ABC"), "actual=" << env->get("txt"));

    TEST_CASE(parserTest(parser, env, "(set seq 0 10)", Value(10ll),  true));
    TEST_CASE(parserTest(parser, env, "(set seq 1 20)", Value(20ll),  true));
    TEST_CASE(parserTest(parser, env, "(set seq 2 30)", Value(30ll),  true));

    TEST_CASE_MSG(env->get("seq") == Value(Sequence(std::vector{Value(10ll), Value(20ll), Value(30ll)})),
                  "actual=" << env->get("seq"));

    TEST_CASE(parserTest(parser, env, "(set ht 1 10)", Value(10ll), true));
    TEST_CASE(parserTest(parser, env, "(set ht 2 20)", Value(20ll), true));
    TEST_CASE(parserTest(parser, env, "(set ht 3 30)", Value(30ll), true));

    auto ht2 = Hashtable();
    ht2.set(Value(1ll), Value(10ll));
    ht2.set(Value(2ll), Value(20ll));
    ht2.set(Value(3ll), Value(30ll));
    TEST_CASE_MSG(env->get("ht") == Value(ht2), "actual=" << env->get("ht"));

    TEST_CASE(parserTest(parser, env, "(set inst \"name\" \"JJ\")", Value("JJ"),  true));
    TEST_CASE(parserTest(parser, env, "(set inst \"age\" 23)",      Value(23ll),  true));
    TEST_CASE(parserTest(parser, env, "(set inst name \"Jon\")",    Value("Jon"), true));
    TEST_CASE(parserTest(parser, env, "(set inst age 25)",          Value(25ll),  true));

    TEST_CASE_MSG(env->get("inst") == Value(Instance(rawStruct, Instance::InitArgs{{"name", Value("Jon")}, {"age", Value(25ll)}})),
                  "actual=" << env->get("inst"));

    TEST_CASE(parserTest(parser, env, "(set txt 0)",     Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(set seq 0)",     Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(set tab 1)",     Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(set inst name)", Value::Null, false));

    TEST_CASE(parserTest(parser, env, "(set txt 'a' 'x')", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(set txt 5 'x')",   Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(set seq 'a' 225)", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(set seq 5 225)",   Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(set inst 5 10)",   Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(set inst mem 10)", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(set 5 0 10)",      Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserGenericClear) {
    auto env = Environment::make();
    Parser parser;

    env->def("a", Value(Sequence(std::vector{Value::Zero, Value::Null})));
    env->def("s", Value("1234"));

    auto rawTab = Hashtable();
    rawTab.set(Value(1ll), Value(100ll));
    env->def("h", rawTab);

    TEST_CASE(parserTest(parser, env, "(clear a)", Value(2ll), true));
    TEST_CASE(parserTest(parser, env, "(clear s)", Value(4ll), true));
    TEST_CASE(parserTest(parser, env, "(clear h)", Value(1ll), true));

    TEST_CASE(parserTest(parser, env, "(len a)", Value::Zero, true));
    TEST_CASE(parserTest(parser, env, "(len s)", Value::Zero, true));
    TEST_CASE(parserTest(parser, env, "(len h)", Value::Zero, true));

    TEST_CASE(parserTest(parser, env, "(clear)",   Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(clear 5)", Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserGenericFind) {
    auto env = Environment::make();
    Parser parser;
    env->def("txt", Value("abcdef"));
    env->def("seq", Value(Sequence(std::vector{Value(1ll), Value(2ll), Value(3ll), Value(4ll), Value(5ll)})));
    env->def("cpl", ValuePair(Value(2ll), Value(3ll)));

    Hashtable ht;
    ht.set(Value(1ll), Value(10ll));
    ht.set(Value(2ll), Value(20ll));
    ht.set(Value(3ll), Value(30ll));
    env->def("ht", Value(ht));

    TEST_CASE(parserTest(parser, env, "(find txt 'a')", Value(0ll),  true));
    TEST_CASE(parserTest(parser, env, "(find txt 'd')", Value(3ll),  true));
    TEST_CASE(parserTest(parser, env, "(find txt 'f')", Value(5ll),  true));
    TEST_CASE(parserTest(parser, env, "(find txt 'g')", Value(-1ll), true));

    TEST_CASE(parserTest(parser, env, "(find txt 'a' 1)", Value(-1ll), true));
    TEST_CASE(parserTest(parser, env, "(find txt 'd' 2)", Value(3ll),  true));
    TEST_CASE(parserTest(parser, env, "(find txt 'f' 4)", Value(5ll),  true));
    TEST_CASE(parserTest(parser, env, "(find txt 'g' 3)", Value(-1ll), true));

    TEST_CASE(parserTest(parser, env, "(find seq 1)", Value(0ll),  true));
    TEST_CASE(parserTest(parser, env, "(find seq 3)", Value(2ll),  true));
    TEST_CASE(parserTest(parser, env, "(find seq 5)", Value(4ll),  true));
    TEST_CASE(parserTest(parser, env, "(find seq 6)", Value(-1ll), true));

    TEST_CASE(parserTest(parser, env, "(find seq 1 1)", Value(-1ll), true));
    TEST_CASE(parserTest(parser, env, "(find seq 3 1)", Value(2ll),  true));
    TEST_CASE(parserTest(parser, env, "(find seq 5 3)", Value(4ll),  true));
    TEST_CASE(parserTest(parser, env, "(find seq 6 3)", Value(-1ll), true));

    TEST_CASE(parserTest(parser, env, "(find ht 10)", Value(1ll),  true));
    TEST_CASE(parserTest(parser, env, "(find ht 20)", Value(2ll),  true));
    TEST_CASE(parserTest(parser, env, "(find ht 30)", Value(3ll),  true));
    TEST_CASE(parserTest(parser, env, "(find ht 40)", Value::Null, true));

    TEST_CASE(parserTest(parser, env, "(find cpl 2)",   Value(0ll),  true));
    TEST_CASE(parserTest(parser, env, "(find cpl 3)",   Value(1ll),  true));
    TEST_CASE(parserTest(parser, env, "(find cpl 2 1)", Value(-1ll), true));
    TEST_CASE(parserTest(parser, env, "(find cpl 3 1)", Value(1ll),  true));

    TEST_CASE(parserTest(parser, env, "(find txt 1)",      Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(find txt 'a' 7)",  Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(find txt 'a' -1)", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(find seq 3 6)",    Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(find seq 3 -1)",   Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(find cpl 2 2)",    Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(find cpl 2 -1)",   Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(find 5 0)",        Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserGenericCount) {
    auto env = Environment::make();
    Parser parser;
    env->def("txt", Value("abacbadef"));
    env->def("seq", Value(Sequence(std::vector{Value(1ll), Value(2ll), Value(1ll), Value(2ll), Value(3ll), Value(1ll), Value(4ll), Value(5ll)})));
    env->def("cpl", ValuePair(Value(2ll), Value(3ll)));
    env->def("rpt", ValuePair(Value(5ll), Value(5ll)));

    Hashtable ht;
    ht.set(Value(1ll), Value(10ll));
    ht.set(Value(2ll), Value(20ll));
    ht.set(Value(3ll), Value(30ll));
    ht.set(Value(4ll), Value(20ll));
    env->def("ht", Value(ht));

    TEST_CASE(parserTest(parser, env, "(count txt 'a')", Value(3ll), true));
    TEST_CASE(parserTest(parser, env, "(count txt 'b')", Value(2ll), true));
    TEST_CASE(parserTest(parser, env, "(count txt 'c')", Value(1ll), true));
    TEST_CASE(parserTest(parser, env, "(count txt 'f')", Value(1ll), true));
    TEST_CASE(parserTest(parser, env, "(count txt 'g')", Value(0ll), true));

    TEST_CASE(parserTest(parser, env, "(count seq 1)", Value(3ll), true));
    TEST_CASE(parserTest(parser, env, "(count seq 2)", Value(2ll), true));
    TEST_CASE(parserTest(parser, env, "(count seq 3)", Value(1ll), true));
    TEST_CASE(parserTest(parser, env, "(count seq 5)", Value(1ll), true));
    TEST_CASE(parserTest(parser, env, "(count seq 6)", Value(0ll), true));

    TEST_CASE(parserTest(parser, env, "(count ht 10)", Value(1ll), true));
    TEST_CASE(parserTest(parser, env, "(count ht 20)", Value(2ll), true));
    TEST_CASE(parserTest(parser, env, "(count ht 30)", Value(1ll), true));
    TEST_CASE(parserTest(parser, env, "(count ht 40)", Value(0ll), true));

    TEST_CASE(parserTest(parser, env, "(count cpl 2)", Value(1ll), true));
    TEST_CASE(parserTest(parser, env, "(count cpl 3)", Value(1ll), true));
    TEST_CASE(parserTest(parser, env, "(count cpl 5)", Value(0ll), true));
    TEST_CASE(parserTest(parser, env, "(count rpt 5)", Value(2ll), true));

    TEST_CASE(parserTest(parser, env, "(count txt 1)", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(count 5 0)",   Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserGenericSort) {
    auto env = Environment::make();
    Parser parser;

    const auto v1 = Value(1ll);
    const auto v2 = Value(2ll);
    const auto v3 = Value(3ll);
    const auto v4 = Value(4ll);
    const auto v5 = Value(5ll);
    const auto v6 = Value(6ll);
    const auto v7 = Value(7ll);

    env->def("txt", Value("3216574"));
    env->def("seq", arrval(v3, v2, v1, v6, v5, v7, v4));

    TEST_CASE(parserTest(parser, env, "(sort txt)",       Value("1234567"), true));
    TEST_CASE(parserTest(parser, env, "(sort txt true)",  Value("7654321"), true));
    TEST_CASE(parserTest(parser, env, "(sort txt false)", Value("1234567"), true));

    TEST_CASE(parserTest(parser, env, "(sort \"\")",     Value(""),     true));
    TEST_CASE(parserTest(parser, env, "(sort \"1\")",    Value("1"),    true));
    TEST_CASE(parserTest(parser, env, "(sort \"21\")",   Value("12"),   true));
    TEST_CASE(parserTest(parser, env, "(sort \"2121\")", Value("1122"), true));

    TEST_CASE(parserTest(parser, env, "(sort seq)",       arrval(v1, v2, v3, v4, v5, v6, v7), true));
    TEST_CASE(parserTest(parser, env, "(sort seq true)",  arrval(v7, v6, v5, v4, v3, v2, v1), true));
    TEST_CASE(parserTest(parser, env, "(sort seq false)", arrval(v1, v2, v3, v4, v5, v6, v7), true));

    TEST_CASE(parserTest(parser, env, "(sort (array))",         arrval(),               true));
    TEST_CASE(parserTest(parser, env, "(sort (array 1))",       arrval(v1),             true));
    TEST_CASE(parserTest(parser, env, "(sort (array 2 1))",     arrval(v1, v2),         true));
    TEST_CASE(parserTest(parser, env, "(sort (array 2 1 2 1))", arrval(v1, v1, v2, v2), true));

    TEST_CASE(parserTest(parser, env, "(sort txt 1)", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(sort 5)",     Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserGenericReverse) {
    auto env = Environment::make();
    Parser parser;

    const auto v1 = Value(1ll);
    const auto v2 = Value(2ll);
    const auto v3 = Value(3ll);
    const auto v4 = Value(4ll);
    const auto v5 = Value(5ll);

    env->def("txt", Value("12345"));
    env->def("seq", arrval(v1, v2, v3, v4, v5));

    TEST_CASE(parserTest(parser, env, "(reverse txt)", Value("54321"), true));
    TEST_CASE(parserTest(parser, env, "(reverse txt)", Value("12345"), true));

    TEST_CASE(parserTest(parser, env, "(reverse \"\")",     Value(""),     true));
    TEST_CASE(parserTest(parser, env, "(reverse \"1\")",    Value("1"),    true));
    TEST_CASE(parserTest(parser, env, "(reverse \"21\")",   Value("12"),   true));
    TEST_CASE(parserTest(parser, env, "(reverse \"2121\")", Value("1212"), true));

    TEST_CASE(parserTest(parser, env, "(reverse seq)", arrval(v5, v4, v3, v2, v1), true));
    TEST_CASE(parserTest(parser, env, "(reverse seq)", arrval(v1, v2, v3, v4, v5), true));

    TEST_CASE(parserTest(parser, env, "(reverse (array))",         arrval(),               true));
    TEST_CASE(parserTest(parser, env, "(reverse (array 1))",       arrval(v1),             true));
    TEST_CASE(parserTest(parser, env, "(reverse (array 2 1))",     arrval(v1, v2),         true));
    TEST_CASE(parserTest(parser, env, "(reverse (array 2 1 2 1))", arrval(v1, v2, v1, v2), true));

    TEST_CASE(parserTest(parser, env, "(sort 5)", Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserGenericSum) {
    auto env = Environment::make();
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(sum (range 0))",      Value(0ll),   true));
    TEST_CASE(parserTest(parser, env, "(sum (range 10))",     Value(45ll),  true));
    TEST_CASE(parserTest(parser, env, "(sum (range -10 0))",  Value(-55ll), true));
    TEST_CASE(parserTest(parser, env, "(sum (range -10 10))", Value(-10ll), true));

    TEST_CASE(parserTest(parser, env, "(sum (array))",                 Value(0ll),  true));
    TEST_CASE(parserTest(parser, env, "(sum (array 1 3 5 9))",         Value(18ll), true));
    TEST_CASE(parserTest(parser, env, "(sum (array -1 -3 -5))",        Value(-9ll), true));
    TEST_CASE(parserTest(parser, env, "(sum (array 1.1 3.1 5.2 9.2))", Value(18.6), true));
    TEST_CASE(parserTest(parser, env, "(sum (array 1 3.1 5 9.0))",     Value(18.1), true));

    TEST_CASE(parserTest(parser, env, "(sum (pair 2 3))",     Value(5ll), true));
    TEST_CASE(parserTest(parser, env, "(sum (pair 2.5 3.0))", Value(5.5), true));
    TEST_CASE(parserTest(parser, env, "(sum (pair 2.5 3))",   Value(5.5), true));

    TEST_CASE(parserTest(parser, env, "(sum 5)",             Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(sum (array 1 'c'))", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(sum (pair 2 'c'))",  Value::Null, false));
}
