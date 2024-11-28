#include "code_node.h"
#include "environment.h"
#include "exception.h"
#include "integer_range.h"
#include "value.h"

using namespace Ishlang;

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeMakeRange) {
    auto env = Environment::make();

    auto ilit = [](Value::Long value) { return CodeNode::make<Literal>(Value(value)); };
    auto clit = [](Value::Char value) { return CodeNode::make<Literal>(Value(value)); };

    Value value = CodeNode::make<MakeRange>(ilit(10))->eval(env);
    TEST_CASE_MSG(value.isRange(), "actual=" << value.isRange());
    TEST_CASE_MSG(value.range().begin() == 0ll, "actual=" << value.range().begin());
    TEST_CASE_MSG(value.range().end() == 10ll, "actual=" << value.range().end());
    TEST_CASE_MSG(value.range().step() == 1ll, "actual=" << value.range().step());

    value = CodeNode::make<MakeRange>(ilit(5), ilit(10), CodeNode::SharedPtr())->eval(env);
    TEST_CASE_MSG(value.isRange(), "actual=" << value.isRange());
    TEST_CASE_MSG(value.range().begin() == 5ll, "actual=" << value.range().begin());
    TEST_CASE_MSG(value.range().end() == 10ll, "actual=" << value.range().end());
    TEST_CASE_MSG(value.range().step() == 1ll, "actual=" << value.range().step());

    value = CodeNode::make<MakeRange>(ilit(0), ilit(10), ilit(2))->eval(env);
    TEST_CASE_MSG(value.isRange(), "actual=" << value.isRange());
    TEST_CASE_MSG(value.range().begin() == 0ll, "actual=" << value.range().begin());
    TEST_CASE_MSG(value.range().end() == 10ll, "actual=" << value.range().end());
    TEST_CASE_MSG(value.range().step() == 2ll, "actual=" << value.range().step());

    try {
        CodeNode::make<MakeRange>(CodeNode::SharedPtr())->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidExpression &) {}
    catch (...) {
        TEST_CASE(false);
    }

    try {
        CodeNode::make<MakeRange>(clit('a'))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }

    try {
        CodeNode::make<MakeRange>(ilit(0), clit('a'), CodeNode::SharedPtr())->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }

    try {
        CodeNode::make<MakeRange>(ilit(0), ilit(10), clit('a'))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeRangeGetters) {
    auto env = Environment::make();
    env->def("rng", Value(IntegerRange(10)));

    auto ilit = [](Value::Long value) { return CodeNode::make<Literal>(Value(value)); };
    auto rVar = []() { return CodeNode::make<Variable>("rng"); };

    auto val = CodeNode::make<RangeBegin>(rVar())->eval(env);
    TEST_CASE_MSG(val.isInt(), "actual=" << Value::typeToString(val.type()));
    TEST_CASE_MSG(val == Value(0ll), "actual=" << val);

    val = CodeNode::make<RangeEnd>(rVar())->eval(env);
    TEST_CASE_MSG(val.isInt(), "actual=" << Value::typeToString(val.type()));
    TEST_CASE_MSG(val == Value(10ll), "actual=" << val);

    val = CodeNode::make<RangeStep>(rVar())->eval(env);
    TEST_CASE_MSG(val.isInt(), "actual=" << Value::typeToString(val.type()));
    TEST_CASE_MSG(val == Value(1ll), "actual=" << val);

    val = CodeNode::make<RangeLen>(rVar())->eval(env);
    TEST_CASE_MSG(val.isInt(), "actual=" << Value::typeToString(val.type()));
    TEST_CASE_MSG(val == Value(10ll), "actual=" << val);

    try {
        CodeNode::make<RangeBegin>(ilit(5))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }

    try {
        CodeNode::make<RangeEnd>(ilit(5))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }

    try {
        CodeNode::make<RangeStep>(ilit(5))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }

    try {
        CodeNode::make<RangeLen>(ilit(5))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeExpand) {
    auto env = Environment::make();
    env->def("rng1", Value(IntegerRange(4)));
    env->def("rng2", Value(IntegerRange(4, 8, 2)));

    auto ilit = [](Value::Long value) { return CodeNode::make<Literal>(Value(value)); };
    auto rVar = [](const char * name) { return CodeNode::make<Variable>(name); };

    auto val = CodeNode::make<Expand>(CodeNode::SharedPtrList{ rVar("rng1") })->eval(env);
    TEST_CASE_MSG(val.isArray(), "actual=" << Value::typeToString(val.type()));
    TEST_CASE_MSG(val.array().size() == 4lu, "actual=" << val.array().size());
    TEST_CASE_MSG(val.array().get(0lu) == Value(0ll), "actual=" << val.array().get(0lu));
    TEST_CASE_MSG(val.array().get(1lu) == Value(1ll), "actual=" << val.array().get(1lu));
    TEST_CASE_MSG(val.array().get(2lu) == Value(2ll), "actual=" << val.array().get(2lu));
    TEST_CASE_MSG(val.array().get(3lu) == Value(3ll), "actual=" << val.array().get(3lu));

    val = CodeNode::make<Expand>(CodeNode::SharedPtrList{ rVar("rng1"), rVar("rng2") })->eval(env);
    TEST_CASE_MSG(val.isArray(), "actual=" << Value::typeToString(val.type()));
    TEST_CASE_MSG(val.array().size() == 6lu, "actual=" << val.array().size());
    TEST_CASE_MSG(val.array().get(0lu) == Value(0ll), "actual=" << val.array().get(0lu));
    TEST_CASE_MSG(val.array().get(1lu) == Value(1ll), "actual=" << val.array().get(1lu));
    TEST_CASE_MSG(val.array().get(2lu) == Value(2ll), "actual=" << val.array().get(2lu));
    TEST_CASE_MSG(val.array().get(3lu) == Value(3ll), "actual=" << val.array().get(3lu));
    TEST_CASE_MSG(val.array().get(4lu) == Value(4ll), "actual=" << val.array().get(4lu));
    TEST_CASE_MSG(val.array().get(5lu) == Value(6ll), "actual=" << val.array().get(5lu));

    try {
        CodeNode::make<Expand>(CodeNode::SharedPtrList{ ilit(5) })->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}
