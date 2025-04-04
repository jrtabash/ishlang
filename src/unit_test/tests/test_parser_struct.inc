#include "unit_test_function.h"

#include "environment.h"
#include "parser.h"
#include "value.h"

using namespace Ishlang;

// -------------------------------------------------------------
DEFINE_TEST(testParserStruct) {
    auto env = Environment::make();
    Parser parser;

    const Struct stest("Person", {"name", "age"});
    TEST_CASE(parserTest(parser, env, "(struct Person (name age))", Value(stest), true));

    const Value &sValue = env->get("Person");
    TEST_CASE_MSG(sValue.isUserType(), "actual=" << sValue.typeToString());

    const Struct &s = sValue.userType();
    TEST_CASE_MSG(s.name() == "Person", "actual=" << s.name());
    TEST_CASE_MSG(s.members().size() == 2, "actual=" << s.members().size());
    TEST_CASE_MSG(s.members()[0] == "name", "actual=" << s.members()[0]);
    TEST_CASE_MSG(s.members()[1] == "age", "actual=" << s.members()[1]);
}

// -------------------------------------------------------------
DEFINE_TEST(testParserIsStructName) {
    auto env = Environment::make();
    Parser parser;

    const Struct stest("Person", {"name", "age"});
    TEST_CASE(parserTest(parser, env, "(var s (struct Person (name age)))", Value(stest), true));
    TEST_CASE(parserTest(parser, env, "(isstructname Person Person)",           Value::True,  true));
    TEST_CASE(parserTest(parser, env, "(isstructname s Person)",                Value::True,  true));

    TEST_CASE(parserTest(parser, env, "(isstructname Person Foobar)", Value::False,  true));
    TEST_CASE(parserTest(parser, env, "(isstructname s Foobar)",      Value::False,  true));
    TEST_CASE(parserTest(parser, env, "(isstructname 3 Person)",      Value::False,  true));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserMakeInstance) {
    auto env = Environment::make();
    Parser parser;

    const Struct stest("Person", {"name", "age"});
    const Instance itest(stest);
    TEST_CASE(parserTest(parser, env, "(var s (struct Person (name age)))", Value(stest), true));
    TEST_CASE(parserTest(parser, env, "(var p (makeinstance Person))",      Value(itest), true));
    TEST_CASE(parserTest(parser, env, "(istypeof p userobject)",            Value::True,  true));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserMakeInstanceWithInitArgs) {
    auto env = Environment::make();
    Parser parser;

    const Struct stest("Person", {"name", "age"});
    const Instance itest(stest, {{"name", Value("John")}, {"age", Value(25ll)}});
    TEST_CASE(parserTest(parser, env, "(var s (struct Person (name age)))",                     Value(stest), true));
    TEST_CASE(parserTest(parser, env, "(var p (makeinstance Person (name \"John\") (age 25)))", Value(itest), true));
    TEST_CASE(parserTest(parser, env, "(istypeof p userobject)",                                Value::True,  true));

    const Instance itest2(stest, {{"name", Value("Sara")}});
    TEST_CASE(parserTest(parser, env, "(var p2 (makeinstance Person (name \"Sara\")))", Value(itest2), true));
    TEST_CASE(parserTest(parser, env, "(istypeof p2 userobject)",                       Value::True,   true));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserIsInstanceOf) {
    auto env = Environment::make();
    Parser parser;

    const Struct stest("Person", {"name", "age"});
    const Instance itest(stest);
    TEST_CASE(parserTest(parser, env, "(struct Person (name age))",    Value(stest), true));
    TEST_CASE(parserTest(parser, env, "(var p (makeinstance Person))", Value(itest), true));
    TEST_CASE(parserTest(parser, env, "(isinstanceof p Person)",       Value::True,  true));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserStructName) {
    auto env = Environment::make();
    Parser parser;

    const Struct stest("Person", {"name", "age"});
    const Instance itest(stest);

    TEST_CASE(parserTest(parser, env, "(struct Person (name age))",    Value(stest),    true));
    TEST_CASE(parserTest(parser, env, "(var p (makeinstance Person))", Value(itest),    true));
    TEST_CASE(parserTest(parser, env, "(structname Person)",           Value("Person"), true));
    TEST_CASE(parserTest(parser, env, "(structname p)",                Value("Person"), true));
    TEST_CASE(parserTest(parser, env, "(structname 5)",                Value::Null,     false));
    TEST_CASE(parserTest(parser, env, "(structname)",                  Value::Null,     false));
    TEST_CASE(parserTest(parser, env, "(structname p Person)",         Value::Null,     false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserGetSetMember) {
    auto env = Environment::make();
    Parser parser;

    const Struct stest("Person", {"name", "age"});
    const Instance itest(stest);

    TEST_CASE(parserTest(parser, env, "(struct Person (name age))",    Value(stest),  true));
    TEST_CASE(parserTest(parser, env, "(var p (makeinstance Person))", Value(itest),  true));
    TEST_CASE(parserTest(parser, env, "(memget p name)",               Value::Null,   true));
    TEST_CASE(parserTest(parser, env, "(memget p age)",                Value::Null,   true));
    TEST_CASE(parserTest(parser, env, "(memset p name \"John\")",      Value("John"), true));
    TEST_CASE(parserTest(parser, env, "(memset p age 25)",             Value(25ll),   true));
    TEST_CASE(parserTest(parser, env, "(memget p name)",               Value("John"), true));
    TEST_CASE(parserTest(parser, env, "(memget p age)",                Value(25ll),   true));
}
