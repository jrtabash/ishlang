#include "environment.h"
#include "parser.h"
#include "value.h"

using namespace Ishlang;

// -------------------------------------------------------------
DEFINE_TEST(testParserBasic) {
    auto env = Environment::make();
    Parser parser;

    TEST_CASE(parserTest(parser, env, "'a'",      Value('a'),     true));
    TEST_CASE(parserTest(parser, env, "\"text\"", Value("text"),  true));
    TEST_CASE(parserTest(parser, env, "12",       Value(12ll),    true));
    TEST_CASE(parserTest(parser, env, "-20",      Value(-20ll),   true));
    TEST_CASE(parserTest(parser, env, "24.56",    Value(24.56),   true));
    TEST_CASE(parserTest(parser, env, "-34.125",  Value(-34.125), true));
    TEST_CASE(parserTest(parser, env, "true",     Value(true),    true));
    TEST_CASE(parserTest(parser, env, "false",    Value(false),   true));
    TEST_CASE(parserTest(parser, env, "null",     Value::Null,    true));

    TEST_CASE(parserTest(parser, env, "'q",     Value::Null, false));
    TEST_CASE(parserTest(parser, env, "\"",     Value::Null, false));
    TEST_CASE(parserTest(parser, env, "\"text", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "2-0",    Value::Null, false));
    TEST_CASE(parserTest(parser, env, "2+0",    Value::Null, false));
    TEST_CASE(parserTest(parser, env, "23.3.4", Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserClone) {
    auto env = Environment::make();
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(var str1 \"hello\")",    Value("hello"), true));
    TEST_CASE(parserTest(parser, env, "(var str2 (clone str1))", Value("hello"), true));
    TEST_CASE(parserTest(parser, env, "(== str1 str2)",          Value::True,    true));
    TEST_CASE(parserTest(parser, env, "(== str1 \"hello\")",     Value::True,    true));
    TEST_CASE(parserTest(parser, env, "(== str2 \"hello\")",     Value::True,    true));

    TEST_CASE(parserTest(parser, env, "(= str1 \"world\")",  Value("world"), true));
    TEST_CASE(parserTest(parser, env, "(!= str1 str2)",      Value::True,    true));
    TEST_CASE(parserTest(parser, env, "(== str1 \"world\")", Value::True,    true));
    TEST_CASE(parserTest(parser, env, "(== str2 \"hello\")", Value::True,    true));

    TEST_CASE(parserTest(parser, env, "(clone)",           Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(clone str1 str2)", Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserIsType) {
    auto env = Environment::make();
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(istypeof null none)",                     Value::True, true));
    TEST_CASE(parserTest(parser, env, "(istypeof 3 int)",                         Value::True, true));
    TEST_CASE(parserTest(parser, env, "(istypeof 1.0 real)",                      Value::True, true));
    TEST_CASE(parserTest(parser, env, "(istypeof 'c' char)",                      Value::True, true));
    TEST_CASE(parserTest(parser, env, "(istypeof true bool)",                     Value::True, true));
    TEST_CASE(parserTest(parser, env, "(istypeof \"txt\" string)",                Value::True, true));
    TEST_CASE(parserTest(parser, env, "(istypeof (lambda () 1) closure)",         Value::True, true));
    TEST_CASE(parserTest(parser, env, "(istypeof (struct Foo (x y)) usertype)",   Value::True, true));
    TEST_CASE(parserTest(parser, env, "(istypeof (makeinstance Foo) userobject)", Value::True, true));
    TEST_CASE(parserTest(parser, env, "(istypeof (array 1 1) array)",             Value::True, true));

    TEST_CASE(parserTest(parser, env, "(istypeof null int)",                   Value::False, true));
    TEST_CASE(parserTest(parser, env, "(istypeof 3 char)",                     Value::False, true));
    TEST_CASE(parserTest(parser, env, "(istypeof 1.0 int)",                    Value::False, true));
    TEST_CASE(parserTest(parser, env, "(istypeof 'c' string)",                 Value::False, true));
    TEST_CASE(parserTest(parser, env, "(istypeof \"txt\" char)",               Value::False, true));
    TEST_CASE(parserTest(parser, env, "(istypeof (lambda () 1) none)",         Value::False, true));
    TEST_CASE(parserTest(parser, env, "(istypeof (struct Bar (y z)) closure)", Value::False, true));

    TEST_CASE(parserTest(parser, env, "(istypeof 3 real int)",          Value::True, true));
    TEST_CASE(parserTest(parser, env, "(istypeof 1.5 int real)",        Value::True, true));
    TEST_CASE(parserTest(parser, env, "(istypeof true int bool real)",  Value::True, true));
    TEST_CASE(parserTest(parser, env, "(istypeof 'c' char string)",     Value::True, true));
    TEST_CASE(parserTest(parser, env, "(istypeof \"txt\" char string)", Value::True, true));

    TEST_CASE(parserTest(parser, env, "(istypeof 3 real bool)",         Value::False, true));
    TEST_CASE(parserTest(parser, env, "(istypeof 1.5 bool int)",        Value::False, true));
    TEST_CASE(parserTest(parser, env, "(istypeof true int real char)",  Value::False, true));
    TEST_CASE(parserTest(parser, env, "(istypeof 'c' int string)",      Value::False, true));
    TEST_CASE(parserTest(parser, env, "(istypeof \"txt\" char bool)",   Value::False, true));

    TEST_CASE(parserTest(parser, env, "(istypeof 3)",           Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(istypeof 3 3)",         Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(istypeof 3 three)",     Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(istypeof 3 int three)", Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserTypeName) {
    auto env = Environment::make();
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(typename null)",                   Value("none"),       true));
    TEST_CASE(parserTest(parser, env, "(typename 3)",                      Value("int"),        true));
    TEST_CASE(parserTest(parser, env, "(typename 1.0)",                    Value("real"),       true));
    TEST_CASE(parserTest(parser, env, "(typename 'c')",                    Value("char"),       true));
    TEST_CASE(parserTest(parser, env, "(typename true)",                   Value("bool"),       true));
    TEST_CASE(parserTest(parser, env, "(typename \"txt\")",                Value("string"),     true));
    TEST_CASE(parserTest(parser, env, "(typename (lambda () 1))",          Value("closure"),    true));
    TEST_CASE(parserTest(parser, env, "(typename (struct Foo (x y)))",     Value("usertype"),   true));
    TEST_CASE(parserTest(parser, env, "(typename (makeinstance Foo))",     Value("userobject"), true));
    TEST_CASE(parserTest(parser, env, "(typename (array 1 1))",            Value("array"),      true));

    TEST_CASE(parserTest(parser, env, "(typename)",       Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(typename 1 int)", Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserAsType) {
    auto env = Environment::make();
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(astype 1 real)",        Value(1.0),        true));
    TEST_CASE(parserTest(parser, env, "(astype 98 char)",       Value('b'),        true));
    TEST_CASE(parserTest(parser, env, "(astype 1 bool)",        Value::True,       true));
    TEST_CASE(parserTest(parser, env, "(astype 98 string)",     Value("98"),       true));
    TEST_CASE(parserTest(parser, env, "(astype 1.5 int)",       Value(1ll),        true));
    TEST_CASE(parserTest(parser, env, "(astype 98.0 char)",     Value('b'),        true));
    TEST_CASE(parserTest(parser, env, "(astype 1.0 bool)",      Value::True,       true));
    TEST_CASE(parserTest(parser, env, "(astype 1.5 string)",    Value("1.500000"), true));
    TEST_CASE(parserTest(parser, env, "(astype 'b' int)",       Value(98ll),       true));
    TEST_CASE(parserTest(parser, env, "(astype 'b' real)",      Value(98.0),       true));
    TEST_CASE(parserTest(parser, env, "(astype 't' bool)",      Value::True,       true));
    TEST_CASE(parserTest(parser, env, "(astype 'b' string)",    Value("b"),        true));
    TEST_CASE(parserTest(parser, env, "(astype true int)",      Value(1ll),        true));
    TEST_CASE(parserTest(parser, env, "(astype true real)",     Value(1.0),        true));
    TEST_CASE(parserTest(parser, env, "(astype true char)",     Value('t'),        true));
    TEST_CASE(parserTest(parser, env, "(astype true string)",   Value("true"),     true));
    TEST_CASE(parserTest(parser, env, "(astype \"98\" int)",    Value(98ll),       true));
    TEST_CASE(parserTest(parser, env, "(astype \"98\" real)",   Value(98.0),       true));
    TEST_CASE(parserTest(parser, env, "(astype \"b\" char)",    Value('b'),        true));
    TEST_CASE(parserTest(parser, env, "(astype \"true\" bool)", Value::True,       true));

    TEST_CASE(parserTest(parser, env, "(astype null int)", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(astype 'b' bool)", Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserAssert) {
    auto env = Environment::make();
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(assert C1 true)",     Value::True, true));
    TEST_CASE(parserTest(parser, env, "(assert C2 (== 1 1))", Value::True, true));

    TEST_CASE(parserTest(parser, env, "(assert C3 false)", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(assert C4 3)",     Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(assert C5)",       Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(assert)",          Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserVar) {
    auto env = Environment::make();
    Parser parser;

    TEST_CASE(parserTest(parser, env, "foo",         Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(var foo 5)", Value(5ll),  true));
    TEST_CASE(parserTest(parser, env, "foo",         Value(5ll),  true));
    TEST_CASE(parserTest(parser, env, "(= foo 12)",  Value(12ll), true));
    TEST_CASE(parserTest(parser, env, "foo",         Value(12ll), true));

    TEST_CASE(parserTest(parser, env, "(var x)",     Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(var x 1 2)", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(= x)",       Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(= x 1 2)",   Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserImportModule) {
    const std::string moduleName = "pimporttest";
    auto tempFile(unitTest().createTempModuleFile(moduleName));

    Parser parser;

    {
        auto env = Environment::make();
        const std::string asName = "test";

        auto varName = [&moduleName](const char * var) { return moduleName + '.' + var; };
        auto varAsName = [&asName](const char *var) { return asName + '.' + var; };

        TEST_CASE(parserTest(parser, env, "(import pimporttest)", Value::True, true));

        TEST_CASE_MSG(env->size() == 3, "actual=" << env->size());

        TEST_CASE(parserTest(parser, env, "(import pimporttest as test)", Value::True, true));

        TEST_CASE_MSG(env->size() == 6, "actual=" << env->size());

        TEST_CASE(env->get(varName("PI")) == env->get(varAsName("PI")));
        TEST_CASE(env->get(varName("add")) == env->get(varAsName("add")));
        TEST_CASE(env->get(varName("sub")) == env->get(varAsName("sub")));

        TEST_CASE(parserTest(parser, env, "(import pimporttest)",         Value::Null, false));
        TEST_CASE(parserTest(parser, env, "(import pimporttest as test)", Value::Null, false));
    }

    {
        auto env = Environment::make();
        TEST_CASE(parserTest(parser, env, "(import)",                Value::Null, false));
        TEST_CASE(parserTest(parser, env, "(import as)",             Value::Null, false));
        TEST_CASE(parserTest(parser, env, "(import pimporttest as)", Value::Null, false));

        TEST_CASE_MSG(env->size() == 0, "actual=" << env->size());
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testParserFromModuleImport) {
    const std::string moduleName = "pfromtest";
    auto tempFile(unitTest().createTempModuleFile(moduleName));

    Parser parser;

    {
        auto env = Environment::make();

        TEST_CASE(parserTest(parser, env, "(from pfromtest import add)", Value::True, true));
        TEST_CASE(parserTest(parser, env, "(from pfromtest import sub)", Value::True, true));

        TEST_CASE_MSG(env->size() == 2, "actual=" << env->size());

        TEST_CASE(parserTest(parser, env, "(from pfromtest import add as plus)",  Value::True, true));
        TEST_CASE(parserTest(parser, env, "(from pfromtest import sub as minus)", Value::True, true));

        TEST_CASE_MSG(env->size() == 4, "actual=" << env->size());

        TEST_CASE(env->get("add") == env->get("plus"));
        TEST_CASE(env->get("sub") == env->get("minus"));

        TEST_CASE(parserTest(parser, env, "(from pfromtest import add)", Value::Null, false));
        TEST_CASE(parserTest(parser, env, "(from pfromtest import sub)", Value::Null, false));
        TEST_CASE(parserTest(parser, env, "(from pfromtest import add as plus)",  Value::Null, false));
        TEST_CASE(parserTest(parser, env, "(from pfromtest import sub as minus)", Value::Null, false));
    }

    {
        auto env = Environment::make();

        TEST_CASE(parserTest(parser, env, "(from pfromtest import add sub)", Value::True, true));

        TEST_CASE_MSG(env->size() == 2, "actual=" << env->size());

        TEST_CASE(parserTest(parser, env, "(from pfromtest import add as plus sub as minus)",  Value::True, true));

        TEST_CASE_MSG(env->size() == 4, "actual=" << env->size());

        TEST_CASE(env->get("add") == env->get("plus"));
        TEST_CASE(env->get("sub") == env->get("minus"));
    }

    {
        auto env = Environment::make();

        TEST_CASE(parserTest(parser, env, "(from)",                                     Value::Null, false));
        TEST_CASE(parserTest(parser, env, "(from pfromtest)",                           Value::Null, false));
        TEST_CASE(parserTest(parser, env, "(from pfromtest import)",                    Value::Null, false));
        TEST_CASE(parserTest(parser, env, "(from pfromtest import as)",                 Value::Null, false));
        TEST_CASE(parserTest(parser, env, "(from pfromtest import add as)",             Value::Null, false));
        TEST_CASE(parserTest(parser, env, "(from pfromtest import add as plus as)",     Value::Null, false));
        TEST_CASE(parserTest(parser, env, "(from pfromtest import add as plus sub as)", Value::Null, false));

        TEST_CASE_MSG(env->size() == 0, "actual=" << env->size());
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testParserRandom) {
    auto env = Environment::make();
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(rand 0)",   Value::Zero, true));
    TEST_CASE(parserTest(parser, env, "(rand -1)",  Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(rand 'a')", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(rand 1 2)", Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserHash) {
    auto env = Environment::make();
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(hash null)", Value::Zero, true));
    TEST_CASE(parserTest(parser, env, "(hash 0)",    Value::Zero, true));
    TEST_CASE(parserTest(parser, env, "(hash)",      Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hash 1 2)",  Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserPairOperations) {
    auto env = Environment::make();
    Parser parser;

    env->def("p", Value(Value::Pair(Value('a'), Value('b'))));

    TEST_CASE(parserTest(parser, env, "(pair 'a' 'b')", Value(Value::Pair(Value('a'), Value('b'))), true));

    TEST_CASE(parserTest(parser, env, "(first p)",  Value('a'), true));
    TEST_CASE(parserTest(parser, env, "(second p)", Value('b'), true));

    TEST_CASE(parserTest(parser, env, "(pair)",             Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(pair 'a')",         Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(pair 'a' 'b' 'c')", Value::Null, false));

    TEST_CASE(parserTest(parser, env, "(first)",         Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(first p 'a')",   Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(second)",        Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(second p 'a')",  Value::Null, false));
}
