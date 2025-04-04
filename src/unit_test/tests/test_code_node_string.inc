#include "unit_test_function.h"

#include "code_node.h"
#include "environment.h"
#include "exception.h"
#include "value.h"

#include <optional>
#include <string>

using namespace Ishlang;

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeStringLen) {
    auto env = Environment::make();

    Value value =
        CodeNode::make<StringLen>(
            CodeNode::make<Literal>(Value("")))
        ->eval(env);
    TEST_CASE_MSG(value == Value::Zero, "actual=" << value);

    value =
        CodeNode::make<StringLen>(
            CodeNode::make<Literal>("a"))
        ->eval(env);
    TEST_CASE_MSG(value == Value(1ll), "actual=" << value);

    value =
        CodeNode::make<StringLen>(
            CodeNode::make<Literal>("abcde"))
        ->eval(env);
    TEST_CASE_MSG(value == Value(5ll), "actual=" << value);

    try {
        value =
            CodeNode::make<StringLen>(
                CodeNode::make<Literal>(Value::Zero))
            ->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) { TEST_CASE(false); }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeStringGet) {
    auto env = Environment::make();

    Value value =
        CodeNode::make<StringGet>(
            CodeNode::make<Literal>(Value("abc")),
            CodeNode::make<Literal>(Value(0ll)))
        ->eval(env);
    TEST_CASE_MSG(value == Value('a'), "actual=" << value);

    value =
        CodeNode::make<StringGet>(
            CodeNode::make<Literal>(Value("abc")),
            CodeNode::make<Literal>(Value(1ll)))
        ->eval(env);
    TEST_CASE_MSG(value == Value('b'), "actual=" << value);

    value =
        CodeNode::make<StringGet>(
            CodeNode::make<Literal>(Value("abc")),
            CodeNode::make<Literal>(Value(2ll)))
        ->eval(env);
    TEST_CASE_MSG(value == Value('c'), "actual=" << value);

    try {
        value =
            CodeNode::make<StringGet>(
                CodeNode::make<Literal>(Value("abc")),
                CodeNode::make<Literal>(Value(3ll)))
            ->eval(env);
        TEST_CASE(false);
    }
    catch (OutOfRange const &) {}
    catch (...) { TEST_CASE(false); }

    try {
        value =
            CodeNode::make<StringGet>(
                CodeNode::make<Literal>(Value("abc")),
                CodeNode::make<Literal>(Value(-1ll)))
            ->eval(env);
        TEST_CASE(false);
    }
    catch (OutOfRange const &) {}
    catch (...) { TEST_CASE(false); }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeStringSet) {
    auto env = Environment::make();
    env->def("str", Value("abc"));

    CodeNode::SharedPtr var = CodeNode::make<Variable>("str");

    Value value =
        CodeNode::make<StringSet>(
            var,
            CodeNode::make<Literal>(Value(0ll)),
            CodeNode::make<Literal>(Value('A')))
        ->eval(env);
    TEST_CASE_MSG(value == Value('A'), "actual=" << value);

    value = var->eval(env);
    TEST_CASE_MSG(value == Value("Abc"), "actual=" << value);

    value =
        CodeNode::make<StringSet>(
            var,
            CodeNode::make<Literal>(Value(1ll)),
            CodeNode::make<Literal>(Value('B')))
        ->eval(env);
    TEST_CASE_MSG(value == Value('B'), "actual=" << value);

    value = var->eval(env);
    TEST_CASE_MSG(value == Value("ABc"), "actual=" << value);

    value =
        CodeNode::make<StringSet>(
            var,
            CodeNode::make<Literal>(Value(2ll)),
            CodeNode::make<Literal>(Value('C')))
        ->eval(env);
    TEST_CASE_MSG(value == Value('C'), "actual=" << value);

    value = var->eval(env);
    TEST_CASE_MSG(value == Value("ABC"), "actual=" << value);

    try {
        value =
            CodeNode::make<StringSet>(
                var,
                CodeNode::make<Literal>(Value(-1ll)),
                CodeNode::make<Literal>(Value('X')))
            ->eval(env);
        TEST_CASE(false);
    }
    catch (OutOfRange const &) {}
    catch (...) { TEST_CASE(false); }

    try {
        value =
            CodeNode::make<StringSet>(
                var,
                CodeNode::make<Literal>(Value(3ll)),
                CodeNode::make<Literal>(Value('Y')))
            ->eval(env);
        TEST_CASE(false);
    }
    catch (OutOfRange const &) {}
    catch (...) { TEST_CASE(false); }

    value = var->eval(env);
    TEST_CASE_MSG(value == Value("ABC"), "actual=" << value);
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeStringCat) {
    auto env = Environment::make();
    env->def("str", Value("abc"));

    CodeNode::SharedPtr var = CodeNode::make<Variable>("str");

    Value value =
        CodeNode::make<StringCat>(
            var,
            CodeNode::make<Literal>(Value("de")))
        ->eval(env);
    TEST_CASE_MSG(value == Value("abcde"), "actual=" << value);

    value = var->eval(env);
    TEST_CASE_MSG(value == Value("abcde"), "actual=" << value);

    value =
        CodeNode::make<StringCat>(
            var,
            CodeNode::make<Literal>(Value('f')))
        ->eval(env);
    TEST_CASE_MSG(value == Value("abcdef"), "actual=" << value);

    value = var->eval(env);
    TEST_CASE_MSG(value == Value("abcdef"), "actual=" << value);

    try {
        value =
            CodeNode::make<StringCat>(
                var,
                CodeNode::make<Literal>(Value::Zero))
            ->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) { TEST_CASE(false); }

    value = var->eval(env);
    TEST_CASE_MSG(value == Value("abcdef"), "actual=" << value);
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeSubString) {
    auto env = Environment::make();
    env->def("str", Value("0123456789"));

    CodeNode::SharedPtr var = CodeNode::make<Variable>("str");
    CodeNode::SharedPtr minus1 = CodeNode::make<Literal>(Value(-1ll));
    CodeNode::SharedPtr zero = CodeNode::make<Literal>(Value::Zero);
    CodeNode::SharedPtr three = CodeNode::make<Literal>(Value(3ll));
    CodeNode::SharedPtr four = CodeNode::make<Literal>(Value(4ll));
    CodeNode::SharedPtr five = CodeNode::make<Literal>(Value(5ll));
    CodeNode::SharedPtr ten = CodeNode::make<Literal>(Value(10ll));
    CodeNode::SharedPtr eleven = CodeNode::make<Literal>(Value(11ll));

    Value value = CodeNode::make<SubString>(var, zero)->eval(env);
    TEST_CASE_MSG(value == Value("0123456789"), "actual=" << value);

    value = CodeNode::make<SubString>(var, five)->eval(env);
    TEST_CASE_MSG(value == Value("56789"), "actual=" << value);

    value = CodeNode::make<SubString>(var, zero, ten)->eval(env);
    TEST_CASE_MSG(value == Value("0123456789"), "actual=" << value);

    value = CodeNode::make<SubString>(var, zero, five)->eval(env);
    TEST_CASE_MSG(value == Value("01234"), "actual=" << value);

    value = CodeNode::make<SubString>(var, four, three)->eval(env);
    TEST_CASE_MSG(value == Value("456"), "actual=" << value);

    try {
        CodeNode::make<SubString>(var, minus1)->eval(env);
        TEST_CASE(false);
    }
    catch (OutOfRange const &) {}
    catch (...) {
        TEST_CASE(false);
    }

    try {
        CodeNode::make<SubString>(var, ten)->eval(env);
        TEST_CASE(false);
    }
    catch (OutOfRange const &) {}
    catch (...) {
        TEST_CASE(false);
    }

    try {
        CodeNode::make<SubString>(var, zero, minus1)->eval(env);
        TEST_CASE(false);
    }
    catch (OutOfRange const &) {}
    catch (...) {
        TEST_CASE(false);
    }

    try {
        CodeNode::make<SubString>(var, zero, eleven)->eval(env);
        TEST_CASE(false);
    }
    catch (OutOfRange const &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeStringFind) {
    auto env = Environment::make();
    env->def("str", Value("0123456789"));

    CodeNode::SharedPtr var = CodeNode::make<Variable>("str");

    auto clit = [](Value::Char c) { return CodeNode::make<Literal>(Value(c)); };
    auto ilit = [](Value::Long i) { return CodeNode::make<Literal>(Value(i)); };
    auto find = [env, var, clit, ilit](Value::Char c, std::optional<Value::Long> i = std::nullopt) {
        return (i
                ? CodeNode::make<StringFind>(var, clit(c), ilit(*i))
                : CodeNode::make<StringFind>(var, clit(c)));
    };

    Value value;

    value = find('0')->eval(env); TEST_CASE_MSG(value == Value::Zero, "actual=" << value);
    value = find('5')->eval(env); TEST_CASE_MSG(value == Value(5ll),  "actual=" << value);
    value = find('9')->eval(env); TEST_CASE_MSG(value == Value(9ll),  "actual=" << value);

    value = find('0', 0)->eval(env); TEST_CASE_MSG(value == Value::Zero, "actual=" << value);
    value = find('5', 0)->eval(env); TEST_CASE_MSG(value == Value(5ll),  "actual=" << value);
    value = find('9', 0)->eval(env); TEST_CASE_MSG(value == Value(9ll),  "actual=" << value);

    value = find('5', 3)->eval(env); TEST_CASE_MSG(value == Value(5ll), "actual=" << value);
    value = find('8', 6)->eval(env); TEST_CASE_MSG(value == Value(8ll), "actual=" << value);
    value = find('9', 9)->eval(env); TEST_CASE_MSG(value == Value(9ll), "actual=" << value);

    value = find('0', 1)->eval(env); TEST_CASE_MSG(value == Value(-1ll), "actual=" << value);
    value = find('5', 6)->eval(env); TEST_CASE_MSG(value == Value(-1ll), "actual=" << value);

    value = find('A', 0)->eval(env); TEST_CASE_MSG(value == Value(-1ll), "actual=" << value);
    value = find('B', 5)->eval(env); TEST_CASE_MSG(value == Value(-1ll), "actual=" << value);

    try {
        CodeNode::make<StringFind>(var, ilit(0))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }

    try {
        CodeNode::make<StringFind>(var, clit('0'), clit('0'))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }

    try {
        CodeNode::make<StringFind>(var, clit('0'), ilit(10))->eval(env);
        TEST_CASE(false);
    }
    catch (const OutOfRange &ex) {
        TEST_CASE_MSG(std::string("Out of range: string find position access") == ex.what(),
                      "actual='" << ex.what() << "'");
    }
    catch (...) {
        TEST_CASE(false);
    }

    try {
        CodeNode::make<StringFind>(var, clit('0'), ilit(-1))->eval(env);
        TEST_CASE(false);
    }
    catch (const OutOfRange &ex) {
        TEST_CASE_MSG(std::string("Out of range: string find position access") == ex.what(),
                      "actual='" << ex.what() << "'");
    }
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeStringCount) {
    auto env = Environment::make();
    env->def("str", Value("0120344501223678910"));

    CodeNode::SharedPtr var = CodeNode::make<Variable>("str");

    auto clit = [](Value::Char c) { return CodeNode::make<Literal>(Value(c)); };
    auto count = [var, clit](Value::Char c) { return CodeNode::make<StringCount>(var, clit(c)); };

    Value value;

    value = count('9')->eval(env); TEST_CASE_MSG(value == Value(1ll),  "actual=" << value);
    value = count('8')->eval(env); TEST_CASE_MSG(value == Value(1ll),  "actual=" << value);
    value = count('7')->eval(env); TEST_CASE_MSG(value == Value(1ll),  "actual=" << value);
    value = count('6')->eval(env); TEST_CASE_MSG(value == Value(1ll),  "actual=" << value);
    value = count('5')->eval(env); TEST_CASE_MSG(value == Value(1ll),  "actual=" << value);
    value = count('4')->eval(env); TEST_CASE_MSG(value == Value(2ll),  "actual=" << value);
    value = count('3')->eval(env); TEST_CASE_MSG(value == Value(2ll),  "actual=" << value);
    value = count('2')->eval(env); TEST_CASE_MSG(value == Value(3ll),  "actual=" << value);
    value = count('1')->eval(env); TEST_CASE_MSG(value == Value(3ll),  "actual=" << value);
    value = count('0')->eval(env); TEST_CASE_MSG(value == Value(4ll),  "actual=" << value);
    value = count('a')->eval(env); TEST_CASE_MSG(value == Value::Zero, "actual=" << value);

    try {
        CodeNode::make<StringCount>(clit('b'), clit('0'))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }

    try {
        CodeNode::make<StringCount>(var, CodeNode::make<Literal>(Value::Zero))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeStringCompare) {
    auto env = Environment::make();
    env->def("str1", Value("abcde"));
    env->def("str2", Value("bcdef"));
    env->def("str3", Value("cdefg"));
    env->def("str4", Value("cdefg"));
    env->def("num", Value::Zero);

    CodeNode::SharedPtr str1 = CodeNode::make<Variable>("str1");
    CodeNode::SharedPtr str2 = CodeNode::make<Variable>("str2");
    CodeNode::SharedPtr str3 = CodeNode::make<Variable>("str3");
    CodeNode::SharedPtr str4 = CodeNode::make<Variable>("str4");
    CodeNode::SharedPtr num = CodeNode::make<Variable>("num");

    auto compare = [](auto lhs, auto rhs) { return CodeNode::make<StringCompare>(lhs, rhs); };

    Value value;

    value = compare(str1, str2)->eval(env); TEST_CASE_MSG(value == Value(-1ll), "actual=" << value);
    value = compare(str2, str1)->eval(env); TEST_CASE_MSG(value == Value(1ll),  "actual=" << value);
    value = compare(str1, str3)->eval(env); TEST_CASE_MSG(value == Value(-2ll), "actual=" << value);
    value = compare(str3, str1)->eval(env); TEST_CASE_MSG(value == Value(2ll),  "actual=" << value);
    value = compare(str2, str3)->eval(env); TEST_CASE_MSG(value == Value(-1ll), "actual=" << value);
    value = compare(str3, str2)->eval(env); TEST_CASE_MSG(value == Value(1ll),  "actual=" << value);
    value = compare(str3, str4)->eval(env); TEST_CASE_MSG(value == Value::Zero, "actual=" << value);

    try {
        value = compare(str1, num)->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &ex) {
        TEST_CASE(std::string("Invalid operand type, expected=string actual=int") == ex.what());
    }
    catch (...) {
        TEST_CASE(false);
    }

    try {
        value = compare(num, str1)->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &ex) {
        TEST_CASE(std::string("Invalid operand type, expected=string actual=int") == ex.what());
    }
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeStringSort) {
    auto env = Environment::make();
    env->def("str0", Value(""));
    env->def("str1", Value("a"));
    env->def("str2", Value("ba"));
    env->def("str3", Value("bca"));
    env->def("str4", Value("dbca"));
    env->def("str5", Value("bdcabdcdcd"));
    env->def("num", Value::Zero);

    CodeNode::SharedPtr str0 = CodeNode::make<Variable>("str0");
    CodeNode::SharedPtr str1 = CodeNode::make<Variable>("str1");
    CodeNode::SharedPtr str2 = CodeNode::make<Variable>("str2");
    CodeNode::SharedPtr str3 = CodeNode::make<Variable>("str3");
    CodeNode::SharedPtr str4 = CodeNode::make<Variable>("str4");
    CodeNode::SharedPtr str5 = CodeNode::make<Variable>("str5");
    CodeNode::SharedPtr num = CodeNode::make<Variable>("num");

    CodeNode::SharedPtr asc = CodeNode::make<Literal>(false);
    CodeNode::SharedPtr desc = CodeNode::make<Literal>(true);
    CodeNode::SharedPtr none = CodeNode::SharedPtr();

    auto testcase =
        [this, &env](auto str, auto desc, auto expect) {
            auto value = CodeNode::make<StringSort>(str, desc)->eval(env);
            TEST_CASE_MSG(value == expect, "value actual=" << value);
            TEST_CASE_MSG(str->eval(env) == expect, "str actual=" << str);
        };

    testcase(str0, asc, Value(""));
    testcase(str1, asc, Value("a"));
    testcase(str2, asc, Value("ab"));
    testcase(str3, asc, Value("abc"));
    testcase(str4, asc, Value("abcd"));
    testcase(str5, asc, Value("abbcccdddd"));

    testcase(str0, desc, Value(""));
    testcase(str1, desc, Value("a"));
    testcase(str2, desc, Value("ba"));
    testcase(str3, desc, Value("cba"));
    testcase(str4, desc, Value("dcba"));
    testcase(str5, desc, Value("ddddcccbba"));

    testcase(str0, none, Value(""));
    testcase(str1, none, Value("a"));
    testcase(str2, none, Value("ab"));
    testcase(str3, none, Value("abc"));
    testcase(str4, none, Value("abcd"));
    testcase(str5, none, Value("abbcccdddd"));

    try {
        testcase(num, asc, Value::Null);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &ex) {
        TEST_CASE(std::string("Invalid operand type, expected=string actual=int") == ex.what());
    }
    catch (...) {
        TEST_CASE(false);
    }

    try {
        testcase(str3, num, Value::Null);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &ex) {
        TEST_CASE(std::string("Invalid operand type, expected=bool actual=int") == ex.what());
    }
    catch (...) {
        TEST_CASE(false);
    }

    Value check;
    check = str0->eval(env); TEST_CASE_MSG(check == Value(""),           "actual=" << check);
    check = str1->eval(env); TEST_CASE_MSG(check == Value("a"),          "actual=" << check);
    check = str2->eval(env); TEST_CASE_MSG(check == Value("ab"),         "actual=" << check);
    check = str3->eval(env); TEST_CASE_MSG(check == Value("abc"),        "actual=" << check);
    check = str4->eval(env); TEST_CASE_MSG(check == Value("abcd"),       "actual=" << check);
    check = str5->eval(env); TEST_CASE_MSG(check == Value("abbcccdddd"), "actual=" << check);
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeStringReverse) {
    auto env = Environment::make();
    env->def("str0", Value(""));
    env->def("str1", Value("a"));
    env->def("str2", Value("ab"));
    env->def("str3", Value("abc"));
    env->def("str4", Value("abcd"));
    env->def("num", Value::Zero);

    CodeNode::SharedPtr str0 = CodeNode::make<Variable>("str0");
    CodeNode::SharedPtr str1 = CodeNode::make<Variable>("str1");
    CodeNode::SharedPtr str2 = CodeNode::make<Variable>("str2");
    CodeNode::SharedPtr str3 = CodeNode::make<Variable>("str3");
    CodeNode::SharedPtr str4 = CodeNode::make<Variable>("str4");
    CodeNode::SharedPtr num = CodeNode::make<Variable>("num");

    auto testcase =
        [this, &env](auto str, auto expect) {
            auto value = CodeNode::make<StringReverse>(str)->eval(env);
            TEST_CASE_MSG(value == expect, "value actual=" << value);
            TEST_CASE_MSG(str->eval(env) == expect, "str actual=" << str);
        };

    testcase(str0, Value(""));
    testcase(str1, Value("a"));
    testcase(str2, Value("ba"));
    testcase(str3, Value("cba"));
    testcase(str4, Value("dcba"));

    testcase(str0, Value(""));
    testcase(str1, Value("a"));
    testcase(str2, Value("ab"));
    testcase(str3, Value("abc"));
    testcase(str4, Value("abcd"));

    try {
        testcase(num, Value::Null);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &ex) {
        TEST_CASE(std::string("Invalid operand type, expected=string actual=int") == ex.what());
    }
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeStringSplit) {
    auto env = Environment::make();

    auto splitv = [](const Value &str, const Value &delim) {
        return CodeNode::make<StringSplit>(CodeNode::make<Literal>(str), CodeNode::make<Literal>(delim));
    };

    auto split = [splitv](const char * str, char delim) {
        return splitv(Value(str), Value(delim));
    };

    auto testCase = [&](const char *str, char delim, const std::vector<const char *> &expect) {
        auto value = split(str, delim)->eval(env);
        TEST_CASE_MSG(value.isArray(), "type actual=" << value.typeToString());

        const auto &actual = value.array();
        TEST_CASE_MSG(actual.size() == expect.size(), "size actual=" << actual.size());
        for (std::size_t i = 0; i < expect.size(); ++i) {
            TEST_CASE_MSG(actual.get(i) == expect[i], "token " << i << " actual=" << actual.get(i));
        }
    };

    testCase("",          ' ', {});
    testCase(" ",         ' ', {"", ""});
    testCase("abc",       ' ', {"abc"});
    testCase("ab cd",     ' ', {"ab", "cd"});
    testCase("ab cd ef",  ' ', {"ab", "cd", "ef"});
    testCase("a,b,c",     ',', {"a", "b", "c"});
    testCase(" a,b , c",  ',', {" a", "b ", " c"});
    testCase(" a,,b , c", ',', {" a", "", "b ", " c"});

    try {
        splitv(Value(5ll), Value(' '))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &ex) {
        TEST_CASE(std::string("Invalid operand type, expected=string actual=int") == ex.what());
    }
    catch (...) {
        TEST_CASE(false);
    }

    try {
        splitv(Value("test"), Value(5ll))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &ex) {
        TEST_CASE(std::string("Invalid operand type, expected=char actual=int") == ex.what());
    }
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeStrCharCheck) {
    auto env = Environment::make();

    const Value cUp = Value('U');
    const Value cLo = Value('l');
    const Value cNm = Value('5');
    const Value cPt = Value('?');
    const Value cSp = Value(' ');

    const Value sUp = Value("UPP");
    const Value sLo = Value("low");
    const Value sNm = Value("123");
    const Value sPt = Value("@?!");
    const Value sSp = Value(" \t\n");
    const Value mix = Value("Ul5? ");

    auto lit = [](const Value &val) { return CodeNode::make<Literal>(val); };
    auto check = [lit](StrCharCheck::Type chkType, const Value &val) {
        return CodeNode::make<StrCharCheck>(chkType, lit(val));
    };

    Value value;

    value = check(StrCharCheck::Upper, cUp)->eval(env); TEST_CASE_MSG(value == Value::True, "actual=" << value);
    value = check(StrCharCheck::Lower, cLo)->eval(env); TEST_CASE_MSG(value == Value::True, "actual=" << value);
    value = check(StrCharCheck::Alpha, cLo)->eval(env); TEST_CASE_MSG(value == Value::True, "actual=" << value);
    value = check(StrCharCheck::Numer, cNm)->eval(env); TEST_CASE_MSG(value == Value::True, "actual=" << value);
    value = check(StrCharCheck::Alnum, cLo)->eval(env); TEST_CASE_MSG(value == Value::True, "actual=" << value);
    value = check(StrCharCheck::Alnum, cNm)->eval(env); TEST_CASE_MSG(value == Value::True, "actual=" << value);
    value = check(StrCharCheck::Punct, cPt)->eval(env); TEST_CASE_MSG(value == Value::True, "actual=" << value);
    value = check(StrCharCheck::Space, cSp)->eval(env); TEST_CASE_MSG(value == Value::True, "actual=" << value);

    value = check(StrCharCheck::Upper, cLo)->eval(env); TEST_CASE_MSG(value == Value::False, "actual=" << value);
    value = check(StrCharCheck::Lower, cUp)->eval(env); TEST_CASE_MSG(value == Value::False, "actual=" << value);
    value = check(StrCharCheck::Alpha, cNm)->eval(env); TEST_CASE_MSG(value == Value::False, "actual=" << value);
    value = check(StrCharCheck::Numer, cUp)->eval(env); TEST_CASE_MSG(value == Value::False, "actual=" << value);
    value = check(StrCharCheck::Alnum, cPt)->eval(env); TEST_CASE_MSG(value == Value::False, "actual=" << value);
    value = check(StrCharCheck::Punct, cSp)->eval(env); TEST_CASE_MSG(value == Value::False, "actual=" << value);
    value = check(StrCharCheck::Space, cPt)->eval(env); TEST_CASE_MSG(value == Value::False, "actual=" << value);

    value = check(StrCharCheck::Upper, sUp)->eval(env); TEST_CASE_MSG(value == Value::True, "actual=" << value);
    value = check(StrCharCheck::Lower, sLo)->eval(env); TEST_CASE_MSG(value == Value::True, "actual=" << value);
    value = check(StrCharCheck::Alpha, sLo)->eval(env); TEST_CASE_MSG(value == Value::True, "actual=" << value);
    value = check(StrCharCheck::Numer, sNm)->eval(env); TEST_CASE_MSG(value == Value::True, "actual=" << value);
    value = check(StrCharCheck::Alnum, sLo)->eval(env); TEST_CASE_MSG(value == Value::True, "actual=" << value);
    value = check(StrCharCheck::Alnum, sNm)->eval(env); TEST_CASE_MSG(value == Value::True, "actual=" << value);
    value = check(StrCharCheck::Punct, sPt)->eval(env); TEST_CASE_MSG(value == Value::True, "actual=" << value);
    value = check(StrCharCheck::Space, sSp)->eval(env); TEST_CASE_MSG(value == Value::True, "actual=" << value);

    value = check(StrCharCheck::Upper, sLo)->eval(env); TEST_CASE_MSG(value == Value::False, "actual=" << value);
    value = check(StrCharCheck::Lower, sUp)->eval(env); TEST_CASE_MSG(value == Value::False, "actual=" << value);
    value = check(StrCharCheck::Alpha, sNm)->eval(env); TEST_CASE_MSG(value == Value::False, "actual=" << value);
    value = check(StrCharCheck::Numer, sUp)->eval(env); TEST_CASE_MSG(value == Value::False, "actual=" << value);
    value = check(StrCharCheck::Alnum, sPt)->eval(env); TEST_CASE_MSG(value == Value::False, "actual=" << value);
    value = check(StrCharCheck::Punct, sSp)->eval(env); TEST_CASE_MSG(value == Value::False, "actual=" << value);
    value = check(StrCharCheck::Space, sPt)->eval(env); TEST_CASE_MSG(value == Value::False, "actual=" << value);

    value = check(StrCharCheck::Upper, mix)->eval(env); TEST_CASE_MSG(value == Value::False, "actual=" << value);
    value = check(StrCharCheck::Lower, mix)->eval(env); TEST_CASE_MSG(value == Value::False, "actual=" << value);
    value = check(StrCharCheck::Alpha, mix)->eval(env); TEST_CASE_MSG(value == Value::False, "actual=" << value);
    value = check(StrCharCheck::Numer, mix)->eval(env); TEST_CASE_MSG(value == Value::False, "actual=" << value);
    value = check(StrCharCheck::Alnum, mix)->eval(env); TEST_CASE_MSG(value == Value::False, "actual=" << value);
    value = check(StrCharCheck::Punct, mix)->eval(env); TEST_CASE_MSG(value == Value::False, "actual=" << value);
    value = check(StrCharCheck::Space, mix)->eval(env); TEST_CASE_MSG(value == Value::False, "actual=" << value);

    try {
        check(StrCharCheck::Upper, Value(5ll))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeStrCharTransform) {
    auto env = Environment::make();

    const Value a = Value('a');
    const Value A = Value('A');
    const Value n = Value('5');

    const Value str = Value("str");
    const Value STR = Value("STR");
    const Value num = Value("525");

    auto lit = [](const Value &val) { return CodeNode::make<Literal>(val); };
    auto trans = [lit](StrCharTransform::Type type, const Value &val) {
        return CodeNode::make<StrCharTransform>(type, lit(val));
    };

    Value value;

    value = trans(StrCharTransform::ToUpper, a)->eval(env); TEST_CASE_MSG(value == Value('A'), "actual=" << value);
    value = trans(StrCharTransform::ToUpper, A)->eval(env); TEST_CASE_MSG(value == Value('A'), "actual=" << value);
    value = trans(StrCharTransform::ToUpper, n)->eval(env); TEST_CASE_MSG(value == Value('5'), "actual=" << value);
    value = trans(StrCharTransform::ToLower, a)->eval(env); TEST_CASE_MSG(value == Value('a'), "actual=" << value);
    value = trans(StrCharTransform::ToLower, A)->eval(env); TEST_CASE_MSG(value == Value('a'), "actual=" << value);
    value = trans(StrCharTransform::ToLower, n)->eval(env); TEST_CASE_MSG(value == Value('5'), "actual=" << value);

    value = trans(StrCharTransform::ToUpper, str)->eval(env); TEST_CASE_MSG(value == Value("STR"), "actual=" << value);
    value = trans(StrCharTransform::ToUpper, STR)->eval(env); TEST_CASE_MSG(value == Value("STR"), "actual=" << value);
    value = trans(StrCharTransform::ToUpper, num)->eval(env); TEST_CASE_MSG(value == Value("525"), "actual=" << value);
    value = trans(StrCharTransform::ToLower, str)->eval(env); TEST_CASE_MSG(value == Value("str"), "actual=" << value);
    value = trans(StrCharTransform::ToLower, STR)->eval(env); TEST_CASE_MSG(value == Value("str"), "actual=" << value);
    value = trans(StrCharTransform::ToLower, num)->eval(env); TEST_CASE_MSG(value == Value("525"), "actual=" << value);

    try {
        trans(StrCharTransform::ToUpper, Value(5ll))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}
