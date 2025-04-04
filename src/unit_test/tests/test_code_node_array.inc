#include "unit_test_function.h"

#include "code_node.h"
#include "environment.h"
#include "exception.h"
#include "sequence.h"
#include "value.h"

#include <string>

using namespace Ishlang;

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeMakeArray) {
    auto env = Environment::make();

    Value value = CodeNode::make<MakeArray>()->eval(env);
    TEST_CASE_MSG(value.isArray(), "actual=" << value.typeToString());
    TEST_CASE_MSG(value.array().size() == 0lu, "actual=" << value.array().size());

    value =
        CodeNode::make<MakeArray>(
            CodeNode::SharedPtrList(
                { CodeNode::make<Literal>(Value('a')),
                  CodeNode::make<Literal>(Value('b')),
                  CodeNode::make<Literal>(Value('c')) }))
        ->eval(env);
    TEST_CASE_MSG(value.isArray(), "actual=" << value.typeToString());
    TEST_CASE_MSG(value.array().size() == 3lu, "actual=" << value.array().size());
    TEST_CASE_MSG(value.array().get(0lu) == Value('a'), "actual=" << value.array().get(0lu));
    TEST_CASE_MSG(value.array().get(1lu) == Value('b'), "actual=" << value.array().get(1lu));
    TEST_CASE_MSG(value.array().get(2lu) == Value('c'), "actual=" << value.array().get(2lu));
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeMakeArraySV) {
    auto env = Environment::make();

    Value value =
        CodeNode::make<MakeArraySV>(
            CodeNode::make<Literal>(Value(3ll)),
            CodeNode::make<Literal>(Value('0')))
        ->eval(env);

    TEST_CASE_MSG(value.isArray(), "actual=" << value.typeToString());
    TEST_CASE_MSG(value.array().size() == 3lu, "actual=" << value.array().size());
    TEST_CASE_MSG(value.array().get(0lu) == Value('0'), "actual=" << value.array().get(0lu));
    TEST_CASE_MSG(value.array().get(1lu) == Value('0'), "actual=" << value.array().get(1lu));
    TEST_CASE_MSG(value.array().get(2lu) == Value('0'), "actual=" << value.array().get(2lu));

    value =
        CodeNode::make<MakeArraySV>(
            CodeNode::make<Literal>(Value(2ll)))
        ->eval(env);

    TEST_CASE_MSG(value.isArray(), "actual=" << value.typeToString());
    TEST_CASE_MSG(value.array().size() == 2lu, "actual=" << value.array().size());
    TEST_CASE_MSG(value.array().get(0lu) == Value::Null, "actual=" << value.array().get(0lu));
    TEST_CASE_MSG(value.array().get(1lu) == Value::Null, "actual=" << value.array().get(1lu));
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeArrayLen) {
    auto env = Environment::make();

    Value value =
        CodeNode::make<ArrayLen>(
            CodeNode::make<Literal>(Value(Sequence())))
        ->eval(env);
    TEST_CASE_MSG(value == Value::Zero, "actual=" << value);

    value =
        CodeNode::make<ArrayLen>(
            CodeNode::make<Literal>(Value(Sequence({Value::Zero}))))
        ->eval(env);
    TEST_CASE_MSG(value == Value(1ll), "actual=" << value);

    value =
        CodeNode::make<ArrayLen>(
            CodeNode::make<Literal>(Value(Sequence({Value::Zero, Value::True, Value::False}))))
        ->eval(env);
    TEST_CASE_MSG(value == Value(3ll), "actual=" << value);

    try {
        value =
            CodeNode::make<ArrayLen>(
                CodeNode::make<Literal>(Value::Zero))
            ->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) { TEST_CASE(false); }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeArrayGet) {
    auto env = Environment::make();
    env->def("arr", Value(Sequence({Value('a'), Value('b'), Value('c')})));

    const CodeNode::SharedPtr var = CodeNode::make<Variable>("arr");

    auto ilit = [](Value::Long i) {
        return CodeNode::make<Literal>(Value(i));
    };

    Value value = CodeNode::make<ArrayGet>(var, ilit(0))->eval(env);
    TEST_CASE_MSG(value == Value('a'), "actual=" << value);

    value = CodeNode::make<ArrayGet>(var, ilit(1))->eval(env);
    TEST_CASE_MSG(value == Value('b'), "actual=" << value);

    value = CodeNode::make<ArrayGet>(var, ilit(2))->eval(env);
    TEST_CASE_MSG(value == Value('c'), "actual=" << value);

    try {
        CodeNode::make<ArrayGet>(var, ilit(3))->eval(env);
        TEST_CASE(false);
    }
    catch (OutOfRange const &) {}
    catch (...) { TEST_CASE(false); }

    try {
        CodeNode::make<ArrayGet>(var, ilit(-1))->eval(env);
        TEST_CASE(false);
    }
    catch (OutOfRange const &) {}
    catch (...) { TEST_CASE(false); }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeArraySet) {
    auto env = Environment::make();
    env->def("arr", Value(Sequence({Value('a'), Value('b'), Value('c')})));

    CodeNode::SharedPtr var = CodeNode::make<Variable>("arr");

    Value expectedValue(Sequence({Value('a'), Value('b'), Value('c')}));
    auto &expected = expectedValue.array();

    auto ilit = [](Value::Long i) { return CodeNode::make<Literal>(Value(i)); };
    auto clit = [](Value::Char c) { return CodeNode::make<Literal>(Value(c)); };

    Value value = CodeNode::make<ArraySet>(var, ilit(0), clit('A'))->eval(env);
    TEST_CASE_MSG(value == Value('A'), "actual=" << value);

    value = var->eval(env);
    expected.set(0, 'A');
    TEST_CASE_MSG(value == expectedValue, "actual=" << value);

    value = CodeNode::make<ArraySet>(var, ilit(1), clit('B'))->eval(env);
    TEST_CASE_MSG(value == Value('B'), "actual=" << value);

    value = var->eval(env);
    expected.set(1, 'B');
    TEST_CASE_MSG(value == expectedValue, "actual=" << value);

    value = CodeNode::make<ArraySet>(var, ilit(2), clit('C'))->eval(env);
    TEST_CASE_MSG(value == Value('C'), "actual=" << value);

    value = var->eval(env);
    expected.set(2, 'C');
    TEST_CASE_MSG(value == expectedValue, "actual=" << value);

    try {
        CodeNode::make<ArraySet>(var, ilit(-1), clit('X'))->eval(env);
        TEST_CASE(false);
    }
    catch (OutOfRange const &) {}
    catch (...) { TEST_CASE(false); }

    try {
        CodeNode::make<ArraySet>(var, ilit(3), clit('Y'))->eval(env);
        TEST_CASE(false);
    }
    catch (OutOfRange const &) {}
    catch (...) { TEST_CASE(false); }

    value = var->eval(env);
    TEST_CASE_MSG(value == expectedValue, "actual=" << value);
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeArrayPush) {
    auto env = Environment::make();
    env->def("arr", Value(Sequence()));

    CodeNode::SharedPtr var = CodeNode::make<Variable>("arr");

    auto ilit = [](Value::Long i) { return CodeNode::make<Literal>(Value(i)); };
    auto clit = [](Value::Char c) { return CodeNode::make<Literal>(Value(c)); };

    Value value = CodeNode::make<ArrayPush>(var, ilit(10))->eval(env);
    Value expected = Value(Sequence({Value(10ll)}));
    TEST_CASE_MSG(value == expected, "actual=" << value);

    value = CodeNode::make<ArrayPush>(var, clit('b'))->eval(env);
    expected = Value(Sequence({Value(10ll), Value('b')}));
    TEST_CASE_MSG(value == expected, "actual=" << value);

    value = CodeNode::make<ArrayPush>(var, ilit(25ll))->eval(env);
    expected = Value(Sequence({Value(10ll), Value('b'), Value(25ll)}));
    TEST_CASE_MSG(value == expected, "actual=" << value);
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeArrayPop) {
    auto env = Environment::make();
    env->def("arr", arrval(Value(0ll), Value(1ll)));

    CodeNode::SharedPtr var = CodeNode::make<Variable>("arr");

    Value value = CodeNode::make<ArrayPop>(var)->eval(env);
    Value arrValue = var->eval(env);
    TEST_CASE_MSG(value == Value(1ll), "actual" << value);
    TEST_CASE_MSG(arrValue == arrval(Value(0ll)), "actual" << arrValue);

    value = CodeNode::make<ArrayPop>(var)->eval(env);
    arrValue = var->eval(env);
    TEST_CASE_MSG(value == Value(0ll), "actual" << value);
    TEST_CASE_MSG(arrValue == arrval(), "actual" << arrValue);

    try {
        value = CodeNode::make<ArrayPop>(var)->eval(env);
        TEST_CASE(false);
    }
    catch (OutOfRange const & ex) {
        TEST_CASE_MSG(std::string(ex.what()) == "Out of range: Pop empty array", "actual=" << ex.what());
    }
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeArrayFind) {
    auto env = Environment::make();

    Value const a('a');
    Value const b('b');
    Value const c('c');
    Value const d('d');

    env->def("arr", Value(Sequence({a, b, c, a, b, c})));

    CodeNode::SharedPtr var = CodeNode::make<Variable>("arr");

    auto ilit = [](Value::Long pos) { return CodeNode::make<Literal>(Value(pos)); };
    auto clit = [](Value const & chr) { return CodeNode::make<Literal>(chr); };

    Value value = CodeNode::make<ArrayFind>(var, clit(a))->eval(env);
    TEST_CASE_MSG(value == Value(0ll), "actual=" << value);

    value = CodeNode::make<ArrayFind>(var, clit(b))->eval(env);
    TEST_CASE_MSG(value == Value(1ll), "actual=" << value);

    value = CodeNode::make<ArrayFind>(var, clit(c))->eval(env);
    TEST_CASE_MSG(value == Value(2ll), "actual=" << value);

    value = CodeNode::make<ArrayFind>(var, clit(d))->eval(env);
    TEST_CASE_MSG(value == Value(-1ll), "actual=" << value);

    value = CodeNode::make<ArrayFind>(var, clit(a), ilit(1))->eval(env);
    TEST_CASE_MSG(value == Value(3ll), "actual=" << value);

    value = CodeNode::make<ArrayFind>(var, clit(b), ilit(2))->eval(env);
    TEST_CASE_MSG(value == Value(4ll), "actual=" << value);

    value = CodeNode::make<ArrayFind>(var, clit(c), ilit(3))->eval(env);
    TEST_CASE_MSG(value == Value(5ll), "actual=" << value);

    value = CodeNode::make<ArrayFind>(var, clit(d), ilit(3))->eval(env);
    TEST_CASE_MSG(value == Value(-1ll), "actual=" << value);

    value = CodeNode::make<ArrayFind>(var, clit(a), ilit(4))->eval(env);
    TEST_CASE_MSG(value == Value(-1ll), "actual=" << value);

    value = CodeNode::make<ArrayFind>(var, clit(b), ilit(5))->eval(env);
    TEST_CASE_MSG(value == Value(-1ll), "actual=" << value);

    try {
        value = CodeNode::make<ArrayFind>(var, clit(c), ilit(6))->eval(env);
        TEST_CASE(false);
    }
    catch (OutOfRange const & ex) {
        TEST_CASE_MSG(std::string("Out of range: array find position access") == ex.what(),
                      "actual='" << ex.what() << "'");
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeArrayCount) {
    auto env = Environment::make();

    const Value v1(1ll);
    const Value v2(2ll);
    const Value v3(3ll);
    const Value v4(4ll);

    env->def("arr", Value(Sequence({v1, v2, v3, v2, v3, v4, v3, v4, v4, v4})));

    CodeNode::SharedPtr var = CodeNode::make<Variable>("arr");

    auto ilit = [](Value::Long i) { return CodeNode::make<Literal>(Value(i)); };
    auto count = [var, ilit](Value::Long i) { return CodeNode::make<ArrayCount>(var, ilit(i)); };

    Value value;

    value = count(1)->eval(env); TEST_CASE_MSG(value == Value(1ll), "actual=" << value);
    value = count(2)->eval(env); TEST_CASE_MSG(value == Value(2ll), "actual=" << value);
    value = count(3)->eval(env); TEST_CASE_MSG(value == Value(3ll), "actual=" << value);
    value = count(4)->eval(env); TEST_CASE_MSG(value == Value(4ll), "actual=" << value);
    value = count(5)->eval(env); TEST_CASE_MSG(value == Value(0ll), "actual=" << value);

    try {
        CodeNode::make<ArrayCount>(ilit(1), ilit(0))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeArraySort) {
    auto env = Environment::make();

    const Value v1(1ll);
    const Value v2(2ll);
    const Value v3(3ll);
    const Value v4(4ll);

    env->def("arr", Value::Null);
    env->def("num", Value::Zero);

    CodeNode::SharedPtr arr = CodeNode::make<Variable>("arr");
    CodeNode::SharedPtr num = CodeNode::make<Variable>("num");

    CodeNode::SharedPtr asc = CodeNode::make<Literal>(false);
    CodeNode::SharedPtr desc = CodeNode::make<Literal>(true);
    CodeNode::SharedPtr none = CodeNode::SharedPtr();

    auto mkarr =
        [&](std::initializer_list<Value> seq) {
            env->set("arr", Value(Sequence(seq)));
            return arr;
        };

    auto testcase =
        [this, &env](auto arr, auto desc, auto expect) {
            auto value = CodeNode::make<ArraySort>(arr, desc)->eval(env);
            TEST_CASE_MSG(value == expect, "value actual=" << value);
            TEST_CASE_MSG(arr->eval(env) == expect, "arr actual=" << arr);
        };

    testcase(mkarr({}),               none, arrval());
    testcase(mkarr({v1}),             none, arrval(v1));
    testcase(mkarr({v2, v1}),         none, arrval(v1, v2));
    testcase(mkarr({v3, v2, v1}),     none, arrval(v1, v2, v3));
    testcase(mkarr({v4, v3, v2, v1}), none, arrval(v1, v2, v3, v4));

    testcase(mkarr({}),               asc, arrval());
    testcase(mkarr({v1}),             asc, arrval(v1));
    testcase(mkarr({v2, v1}),         asc, arrval(v1, v2));
    testcase(mkarr({v3, v2, v1}),     asc, arrval(v1, v2, v3));
    testcase(mkarr({v4, v3, v2, v1}), asc, arrval(v1, v2, v3, v4));

    testcase(mkarr({}),               desc, arrval());
    testcase(mkarr({v1}),             desc, arrval(v1));
    testcase(mkarr({v1, v2}),         desc, arrval(v2, v1));
    testcase(mkarr({v1, v2, v3}),     desc, arrval(v3, v2, v1));
    testcase(mkarr({v1, v2, v3, v4}), desc, arrval(v4, v3, v2, v1));

    try {
        testcase(num, asc, Value::Null);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &ex) {
        TEST_CASE(std::string("Invalid operand type, expected=array actual=int") == ex.what());
    }
    catch (...) {
        TEST_CASE(false);
    }

    try {
        testcase(arr, num, Value::Null);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &ex) {
        TEST_CASE(std::string("Invalid operand type, expected=bool actual=int") == ex.what());
    }
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeArrayReverse) {
    auto env = Environment::make();

    const Value v1(1ll);
    const Value v2(2ll);
    const Value v3(3ll);
    const Value v4(4ll);

    env->def("arr", Value::Null);
    env->def("num", Value::Zero);

    CodeNode::SharedPtr arr = CodeNode::make<Variable>("arr");
    CodeNode::SharedPtr num = CodeNode::make<Variable>("num");

    auto mkarr =
        [&](std::initializer_list<Value> seq) {
            env->set("arr", Value(Sequence(seq)));
            return arr;
        };

    auto testcase =
        [this, &env](auto arr, auto expect) {
            auto value = CodeNode::make<ArrayReverse>(arr)->eval(env);
            TEST_CASE_MSG(value == expect, "value actual=" << value);
            TEST_CASE_MSG(arr->eval(env) == expect, "arr actual=" << arr);
        };

    testcase(mkarr({}),               arrval());
    testcase(mkarr({v1}),             arrval(v1));
    testcase(mkarr({v2, v1}),         arrval(v1, v2));
    testcase(mkarr({v3, v2, v1}),     arrval(v1, v2, v3));
    testcase(mkarr({v4, v3, v2, v1}), arrval(v1, v2, v3, v4));

    testcase(mkarr({}),               arrval());
    testcase(mkarr({v1}),             arrval(v1));
    testcase(mkarr({v1, v2}),         arrval(v2, v1));
    testcase(mkarr({v1, v2, v3}),     arrval(v3, v2, v1));
    testcase(mkarr({v1, v2, v3, v4}), arrval(v4, v3, v2, v1));

    try {
        testcase(num, Value::Null);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &ex) {
        TEST_CASE(std::string("Invalid operand type, expected=array actual=int") == ex.what());
    }
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeArrayClear) {
    auto env = Environment::make();

    const Value v1(1ll);
    const Value v2(2ll);
    const Value v3(3ll);

    env->def("arr", Value(Sequence({v1, v2, v3})));

    CodeNode::SharedPtr arr = CodeNode::make<Variable>("arr");

    auto arrVal = arr->eval(env);
    TEST_CASE_MSG(arrVal == Value(Sequence({v1, v2, v3})), "array actual=" << arrVal);

    auto result = CodeNode::make<ArrayClear>(arr)->eval(env);
    TEST_CASE_MSG(result.isNull(), "result actual=" << result);

    arrVal = arr->eval(env);
    TEST_CASE_MSG(arrVal == Value(Sequence()), "array actual=" << arrVal);

    try {
        CodeNode::make<ArrayClear>(
            CodeNode::make<Literal>(Value(5ll)))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &ex) {
        TEST_CASE(std::string("Invalid operand type, expected=array actual=int") == ex.what());
    }
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeArrayInsert) {
    auto env = Environment::make();

    CodeNode::SharedPtr arr = CodeNode::make<Variable>("arr");
    CodeNode::SharedPtr num = CodeNode::make<Variable>("num");

    const Value v0 = Value::Zero;
    const Value v1 = Value(1ll);
    const Value v2 = Value(2ll);
    const Value v3 = Value(3ll);
    const Value v4 = Value(4ll);
    const Value v5 = Value(5ll);

    env->def("arr", Value(Sequence()));
    env->def("num", Value::Zero);

    auto testcase =
        [this, &env](CodeNode::SharedPtr arrCode, const Value &pos, const Value &item, const Value &expect) {
            auto posCode = CodeNode::make<Literal>(pos);
            auto itemCode = CodeNode::make<Literal>(item);

            auto result = CodeNode::make<ArrayInsert>(arrCode, posCode, itemCode)->eval(env);
            TEST_CASE_MSG(result == item, "result actual=" << result);

            auto arr = arrCode->eval(env);
            TEST_CASE_MSG(arr == expect, "arr actual=" << arr);
        };

    testcase(arr, v0, v3, arrval(v3));
    testcase(arr, v0, v1, arrval(v1, v3));
    testcase(arr, v2, v5, arrval(v1, v3, v5));
    testcase(arr, v0, v0, arrval(v0, v1, v3, v5));
    testcase(arr, v2, v2, arrval(v0, v1, v2, v3, v5));
    testcase(arr, v4, v4, arrval(v0, v1, v2, v3, v4, v5));

    try {
        testcase(num, v0, v5, Value::Null);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &ex) {
        TEST_CASE(std::string("Invalid operand type, expected=array actual=int") == ex.what());
    }
    catch (...) {
        TEST_CASE(false);
    }

    try {
        testcase(arr, Value('a'), v5, Value::Null);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &ex) {
        TEST_CASE(std::string("Invalid operand type, expected=int actual=char") == ex.what());
    }
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeArrayRemove) {
    auto env = Environment::make();

    CodeNode::SharedPtr arr = CodeNode::make<Variable>("arr");
    CodeNode::SharedPtr num = CodeNode::make<Variable>("num");

    const Value v0 = Value::Zero;
    const Value v1 = Value(1ll);
    const Value v2 = Value(2ll);
    const Value v3 = Value(3ll);
    const Value v4 = Value(4ll);
    const Value v5 = Value(5ll);

    env->def("arr", Value(Sequence({v0, v1, v2, v3, v4, v5})));
    env->def("num", Value::Zero);

    auto testcase =
        [this, &env](CodeNode::SharedPtr arrCode, const Value &pos, const Value &expect) {
            auto posCode = CodeNode::make<Literal>(pos);
            auto result = CodeNode::make<ArrayRemove>(arrCode, posCode)->eval(env);
            TEST_CASE_MSG(result.isNull(), "value actual=" << result);

            auto arr = arrCode->eval(env);
            TEST_CASE_MSG(arr == expect, "arr actual=" << arr);
        };

    testcase(arr, v4, arrval(v0, v1, v2, v3, v5));
    testcase(arr, v2, arrval(v0, v1, v3, v5));
    testcase(arr, v0, arrval(v1, v3, v5));
    testcase(arr, v2, arrval(v1, v3));
    testcase(arr, v0, arrval(v3));
    testcase(arr, v0, arrval());

    try {
        testcase(arr, v3, Value::Null);
        TEST_CASE(false);
    }
    catch (const OutOfRange &ex) {
        TEST_CASE(std::string("Out of range: Array remove access") == ex.what());
    }
    catch (...) {
        TEST_CASE(false);
    }

    try {
        testcase(num, v0, Value::Null);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &ex) {
        TEST_CASE(std::string("Invalid operand type, expected=array actual=int") == ex.what());
    }
    catch (...) {
        TEST_CASE(false);
    }

    try {
        testcase(arr, Value('a'), Value::Null);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &ex) {
        TEST_CASE(std::string("Invalid operand type, expected=int actual=char") == ex.what());
    }
    catch (...) {
        TEST_CASE(false);
    }
}
