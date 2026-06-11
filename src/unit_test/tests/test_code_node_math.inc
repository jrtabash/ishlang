#include "unit_test_function.h"

#include "code_node.h"
#include "environment.h"
#include "exception.h"
#include "value.h"

using namespace Ishlang;

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeMathAbs) {
    auto env = Environment::make();

    auto abs = [](Value const & num) {
        return CodeNode::make<MathFunction>(
            MathFunction::Abs,
            CodeNode::SharedPtrList{ CodeNode::make<Literal>(num) });
    };

    auto val = abs(Value::PlusOne)->eval(env);
    TEST_CASE_MSG(val.isInt(), "actual=" << val.typeToString());
    TEST_CASE_MSG(val.integer() == 1ll, "actual=" << val);

    val = abs(Value::MinusOne)->eval(env);
    TEST_CASE_MSG(val.isInt(), "actual=" << val.typeToString());
    TEST_CASE_MSG(val.integer() == 1ll, "actual=" << val);

    val = abs(Value::Zero)->eval(env);
    TEST_CASE_MSG(val.isInt(), "actual=" << val.typeToString());
    TEST_CASE_MSG(val.integer() == 0ll, "actual=" << val);

    val = abs(Value(2.0))->eval(env);
    TEST_CASE_MSG(val.isReal(), "actual=" << val.typeToString());
    TEST_CASE_MSG(val.real() == 2.0, "actual=" << val);

    val = abs(Value(-2.0))->eval(env);
    TEST_CASE_MSG(val.isReal(), "actual=" << val.typeToString());
    TEST_CASE_MSG(val.real() == 2.0, "actual=" << val);

    try {
        abs(Value('x'))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeMathMin) {
    auto env = Environment::make();

    auto lit = [](const Value & num) {
        return CodeNode::make<Literal>(num);
    };

    auto min = [lit](const Value &num1, const Value &num2, const Value &num3 = Value::Null) {
        return CodeNode::make<MathFunction>(
            MathFunction::Min,
            (num3.isNull()
             ? CodeNode::SharedPtrList{ lit(num1), lit(num2) }
             : CodeNode::SharedPtrList{ lit(num1), lit(num2), lit(num3) }));
    };

    auto val = min(Value::PlusOne, Value::MinusOne)->eval(env);
    TEST_CASE_MSG(val.isInt(), "actual=" << val.typeToString());
    TEST_CASE_MSG(val.integer() == -1ll, "actual=" << val);

    val = min(Value::MinusOne, Value::PlusOne)->eval(env);
    TEST_CASE_MSG(val.isInt(), "actual=" << val.typeToString());
    TEST_CASE_MSG(val.integer() == -1ll, "actual=" << val);

    val = min(Value::Zero, Value::MinusOne, Value::PlusOne)->eval(env);
    TEST_CASE_MSG(val.isInt(), "actual=" << val.typeToString());
    TEST_CASE_MSG(val.integer() == -1ll, "actual=" << val);

    val = min(Value(1.0), Value(-2.0), Value(-3.0))->eval(env);
    TEST_CASE_MSG(val.isReal(), "actual=" << val.typeToString());
    TEST_CASE_MSG(val.real() == -3.0, "actual=" << val);

    try {
        min(Value('x'), Value::Zero)->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }

    try {
        min(Value::PlusOne, Value::MinusOne, Value('x'))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeMathMax) {
    auto env = Environment::make();

    auto lit = [](const Value & num) {
        return CodeNode::make<Literal>(num);
    };

    auto max = [lit](const Value &num1, const Value &num2, const Value &num3 = Value::Null) {
        return CodeNode::make<MathFunction>(
            MathFunction::Max,
            (num3.isNull()
             ? CodeNode::SharedPtrList{ lit(num1), lit(num2) }
             : CodeNode::SharedPtrList{ lit(num1), lit(num2), lit(num3) }));
    };

    auto val = max(Value::PlusOne, Value::MinusOne)->eval(env);
    TEST_CASE_MSG(val.isInt(), "actual=" << val.typeToString());
    TEST_CASE_MSG(val.integer() == 1ll, "actual=" << val);

    val = max(Value::MinusOne, Value::PlusOne)->eval(env);
    TEST_CASE_MSG(val.isInt(), "actual=" << val.typeToString());
    TEST_CASE_MSG(val.integer() == 1ll, "actual=" << val);

    val = max(Value::Zero, Value::MinusOne, Value::PlusOne)->eval(env);
    TEST_CASE_MSG(val.isInt(), "actual=" << val.typeToString());
    TEST_CASE_MSG(val.integer() == 1ll, "actual=" << val);

    val = max(Value(1.0), Value(-3.0), Value(4.0))->eval(env);
    TEST_CASE_MSG(val.isReal(), "actual=" << val.typeToString());
    TEST_CASE_MSG(val.real() == 4.0, "actual=" << val);

    try {
        max(Value('x'), Value::Zero)->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }

    try {
        max(Value::PlusOne, Value::MinusOne, Value('x'))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeMathSign) {
    auto env = Environment::make();

    auto sign = [](Value const & num) {
        return CodeNode::make<MathFunction>(
            MathFunction::Sign,
            CodeNode::SharedPtrList{ CodeNode::make<Literal>(num) });
    };

    auto val = sign(Value::PlusOne)->eval(env);
    TEST_CASE_MSG(val.isInt(), "actual=" << val.typeToString());
    TEST_CASE_MSG(val.integer() == 1ll, "actual=" << val);

    val = sign(Value::MinusOne)->eval(env);
    TEST_CASE_MSG(val.isInt(), "actual=" << val.typeToString());
    TEST_CASE_MSG(val.integer() == -1ll, "actual=" << val);

    val = sign(Value::Zero)->eval(env);
    TEST_CASE_MSG(val.isInt(), "actual=" << val.typeToString());
    TEST_CASE_MSG(val.integer() == 0ll, "actual=" << val);

    val = sign(Value(2.0))->eval(env);
    TEST_CASE_MSG(val.isInt(), "actual=" << val.typeToString());
    TEST_CASE_MSG(val.integer() == 1ll, "actual=" << val);

    val = sign(Value(-2.0))->eval(env);
    TEST_CASE_MSG(val.isInt(), "actual=" << val.typeToString());
    TEST_CASE_MSG(val.integer() == -1ll, "actual=" << val);

    try {
        sign(Value('x'))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeMathSqrt) {
    auto env = Environment::make();

    auto sqrt = [](Value const & num) {
        return CodeNode::make<MathFunction>(
            MathFunction::Sqrt,
            CodeNode::SharedPtrList{ CodeNode::make<Literal>(num) });
    };

    auto val = sqrt(Value::PlusOne)->eval(env);
    TEST_CASE_MSG(val.isReal(), "actual=" << val.typeToString());
    TEST_CASE_MSG(val.real() == 1.0, "actual=" << val);

    val = sqrt(Value::Zero)->eval(env);
    TEST_CASE_MSG(val.isReal(), "actual=" << val.typeToString());
    TEST_CASE_MSG(val.real() == 0.0, "actual=" << val);

    val = sqrt(Value(25.0))->eval(env);
    TEST_CASE_MSG(val.isReal(), "actual=" << val.typeToString());
    TEST_CASE_MSG(val.real() == 5.0, "actual=" << val);

    val = sqrt(Value(-1.0))->eval(env);
    TEST_CASE_MSG(val.isReal(), "actual=" << val.typeToString());
    TEST_CASE_MSG(std::isnan(val.real()), "actual=" << val);

    try {
        sqrt(Value('x'))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeMathCeil) {
    auto env = Environment::make();

    auto ceil = [](Value const & num) {
        return CodeNode::make<MathFunction>(
            MathFunction::Ceil,
            CodeNode::SharedPtrList{ CodeNode::make<Literal>(num) });
    };

    auto val = ceil(Value(1.2))->eval(env);
    TEST_CASE_MSG(val.isReal(), "actual=" << val.typeToString());
    TEST_CASE_MSG(val.real() == 2.0, "actual=" << val);

    val = ceil(Value(-1.2))->eval(env);
    TEST_CASE_MSG(val.isReal(), "actual=" << val.typeToString());
    TEST_CASE_MSG(val.real() == -1.0, "actual=" << val);

    val = ceil(Value(1.0))->eval(env);
    TEST_CASE_MSG(val.isReal(), "actual=" << val.typeToString());
    TEST_CASE_MSG(val.real() == 1.0, "actual=" << val);

    val = ceil(Value(-1.0))->eval(env);
    TEST_CASE_MSG(val.isReal(), "actual=" << val.typeToString());
    TEST_CASE_MSG(val.real() == -1.0, "actual=" << val);

    val = ceil(Value(0.0))->eval(env);
    TEST_CASE_MSG(val.isReal(), "actual=" << val.typeToString());
    TEST_CASE_MSG(val.real() == 0.0, "actual=" << val);

    try {
        ceil(Value('x'))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeMathFloor) {
    auto env = Environment::make();

    auto floor = [](Value const & num) {
        return CodeNode::make<MathFunction>(
            MathFunction::Floor,
            CodeNode::SharedPtrList{ CodeNode::make<Literal>(num) });
    };

    auto val = floor(Value(1.2))->eval(env);
    TEST_CASE_MSG(val.isReal(), "actual=" << val.typeToString());
    TEST_CASE_MSG(val.real() == 1.0, "actual=" << val);

    val = floor(Value(-1.2))->eval(env);
    TEST_CASE_MSG(val.isReal(), "actual=" << val.typeToString());
    TEST_CASE_MSG(val.real() == -2.0, "actual=" << val);

    val = floor(Value(1.0))->eval(env);
    TEST_CASE_MSG(val.isReal(), "actual=" << val.typeToString());
    TEST_CASE_MSG(val.real() == 1.0, "actual=" << val);

    val = floor(Value(-1.0))->eval(env);
    TEST_CASE_MSG(val.isReal(), "actual=" << val.typeToString());
    TEST_CASE_MSG(val.real() == -1.0, "actual=" << val);

    val = floor(Value(0.0))->eval(env);
    TEST_CASE_MSG(val.isReal(), "actual=" << val.typeToString());
    TEST_CASE_MSG(val.real() == 0.0, "actual=" << val);

    try {
        floor(Value('x'))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}
