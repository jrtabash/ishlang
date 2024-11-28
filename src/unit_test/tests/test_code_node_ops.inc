#include "code_node.h"
#include "environment.h"
#include "exception.h"
#include "value.h"

#include <string>

using namespace Ishlang;

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeArithOp) {
    auto env = Environment::make();

    CodeNode::SharedPtr iLit1(new Literal(Value(1ll)));
    CodeNode::SharedPtr iLit2(new Literal(Value(2ll)));
    CodeNode::SharedPtr iLit3(new Literal(Value(0ll)));
    CodeNode::SharedPtr iLit4(new Literal(Value(4ll)));
    CodeNode::SharedPtr iLit5(new Literal(Value(5ll)));
    CodeNode::SharedPtr rLit1(new Literal(Value(1.2)));
    CodeNode::SharedPtr rLit2(new Literal(Value(1.3)));
    CodeNode::SharedPtr rLit3(new Literal(Value(0.0)));
    CodeNode::SharedPtr rLit4(new Literal(Value(9.0)));
    CodeNode::SharedPtr rLit5(new Literal(Value(0.5)));
    CodeNode::SharedPtr cLit(new Literal(Value('c')));
    CodeNode::SharedPtr bLit(new Literal(Value(true)));

    CodeNode::SharedPtr arith(new ArithOp(ArithOp::Add, {iLit1, iLit2}));
    Value result = arith->eval(env);
    TEST_CASE_MSG(result.isInt(), "actual=" << result.typeToString());
    TEST_CASE_MSG(result.integer() == 3ll, "actual=" << result);

    arith.reset(new ArithOp(ArithOp::Add, {iLit1, rLit1}));
    result = arith->eval(env);
    TEST_CASE_MSG(result.isReal(), "actual=" << result.typeToString());
    TEST_CASE_MSG(Util::isEqual(result.real(), 2.2), "actual=" << result);

    arith.reset(new ArithOp(ArithOp::Sub, {rLit2, rLit1}));
    result = arith->eval(env);
    TEST_CASE_MSG(result.isReal(), "actual=" << result.typeToString());
    TEST_CASE_MSG(Util::isEqual(result.real(), 0.1), "actual=" << result);

    arith.reset(new ArithOp(ArithOp::Mul, {rLit1, iLit2}));
    result = arith->eval(env);
    TEST_CASE_MSG(result.isReal(), "actual=" << result.typeToString());
    TEST_CASE_MSG(Util::isEqual(result.real(), 2.4), "actual=" << result);

    arith.reset(new ArithOp(ArithOp::Div, {iLit1, iLit2}));
    result = arith->eval(env);
    TEST_CASE_MSG(result.isInt(), "actual=" << result.typeToString());
    TEST_CASE_MSG(result.integer() == 0, "actual=" << result);

    arith.reset(new ArithOp(ArithOp::Div, {rLit1, iLit2}));
    result = arith->eval(env);
    TEST_CASE_MSG(result.isReal(), "actual=" << result.typeToString());
    TEST_CASE_MSG(Util::isEqual(result.real(), 0.6), "actual=" << result);

    arith.reset(new ArithOp(ArithOp::Mod, {iLit4, iLit2}));
    result = arith->eval(env);
    TEST_CASE_MSG(result.isInt(), "actual=" << result.typeToString());
    TEST_CASE_MSG(Util::isEqual(result.integer(), 0), "actual=" << result);

    arith.reset(new ArithOp(ArithOp::Mod, {iLit5, iLit2}));
    result = arith->eval(env);
    TEST_CASE_MSG(result.isInt(), "actual=" << result.typeToString());
    TEST_CASE_MSG(Util::isEqual(result.integer(), 1), "actual=" << result);

    arith.reset(new ArithOp(ArithOp::Pow, {iLit4, iLit2}));
    result = arith->eval(env);
    TEST_CASE_MSG(result.isReal(), "actual=" << result.typeToString());
    TEST_CASE_MSG(Util::isEqual(result.real(), 16.0), "actual=" << result);

    arith.reset(new ArithOp(ArithOp::Pow, {iLit4, iLit3}));
    result = arith->eval(env);
    TEST_CASE_MSG(result.isReal(), "actual=" << result.typeToString());
    TEST_CASE_MSG(Util::isEqual(result.real(), 1.0), "actual=" << result);

    arith.reset(new ArithOp(ArithOp::Pow, {iLit4, iLit1}));
    result = arith->eval(env);
    TEST_CASE_MSG(result.isReal(), "actual=" << result.typeToString());
    TEST_CASE_MSG(Util::isEqual(result.real(), 4.0), "actual=" << result);

    arith.reset(new ArithOp(ArithOp::Pow, {rLit4, rLit5}));
    result = arith->eval(env);
    TEST_CASE_MSG(result.isReal(), "actual=" << result.typeToString());
    TEST_CASE_MSG(Util::isEqual(result.real(), 3.0), "actual=" << result);

    try {
        arith.reset(new ArithOp(ArithOp::Div, {iLit1, iLit3}));
        arith->eval(env);
        TEST_CASE(false);
    }
    catch (const DivByZero &ex) {}
    catch (...) { TEST_CASE(false); }

    try {
        arith.reset(new ArithOp(ArithOp::Div, {rLit1, rLit3}));
        arith->eval(env);
        TEST_CASE(false);
    }
    catch (const DivByZero &ex) {}
    catch (...) { TEST_CASE(false); }

    try {
        arith.reset(new ArithOp(ArithOp::Mod, {iLit1, iLit3}));
        arith->eval(env);
        TEST_CASE(false);
    }
    catch (const DivByZero &ex) {}
    catch (...) { TEST_CASE(false); }

    try {
        arith.reset(new ArithOp(ArithOp::Add, {iLit1, cLit}));
        arith->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &ex) {}
    catch (...) { TEST_CASE(false); }

    try {
        arith.reset(new ArithOp(ArithOp::Add, {bLit, rLit2}));
        arith->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &ex) {}
    catch (...) { TEST_CASE(false); }

    try {
        arith.reset(new ArithOp(ArithOp::Add, {bLit, cLit}));
        arith->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &ex) {}
    catch (...) { TEST_CASE(false); }

    try {
        arith.reset(new ArithOp(ArithOp::Mod, {rLit2, iLit1}));
        arith->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &ex) {}
    catch (...) { TEST_CASE(false); }

    try {
        arith.reset(new ArithOp(ArithOp::Mod, {iLit1, rLit2}));
        arith->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &ex) {}
    catch (...) { TEST_CASE(false); }

    try {
        arith.reset(new ArithOp(ArithOp::Mod, {rLit1, rLit2}));
        arith->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &ex) {}
    catch (...) { TEST_CASE(false); }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeArithOpVariadic) {
    auto env = Environment::make();

    const auto i0 = CodeNode::make<Literal>(Value(0ll));
    const auto i1 = CodeNode::make<Literal>(Value(1ll));
    const auto i2 = CodeNode::make<Literal>(Value(2ll));
    const auto i3 = CodeNode::make<Literal>(Value(3ll));
    const auto i4 = CodeNode::make<Literal>(Value(4ll));
    const auto i5 = CodeNode::make<Literal>(Value(5ll));
    const auto i11 = CodeNode::make<Literal>(Value(11ll));
    const auto r0 = CodeNode::make<Literal>(Value(0.0));
    const auto r1 = CodeNode::make<Literal>(Value(1.0));
    const auto r2 = CodeNode::make<Literal>(Value(2.0));
    const auto r3 = CodeNode::make<Literal>(Value(3.0));
    const auto r4 = CodeNode::make<Literal>(Value(4.0));
    const auto r5 = CodeNode::make<Literal>(Value(5.0));

    const auto arith = [](ArithOp::Type op, CodeNode::SharedPtrList operands) {
        return CodeNode::make<ArithOp>(op, operands);
    };

    Value result = arith(ArithOp::Add, {i1, i2, i3, i4, i5})->eval(env);
    TEST_CASE_MSG(result.isInt(), "actual=" << result.typeToString());
    TEST_CASE_MSG(result.integer() == 15ll, "actual=" << result);

    result = arith(ArithOp::Add, {r1, r2, r3, r4, r5})->eval(env);
    TEST_CASE_MSG(result.isReal(), "actual=" << result.typeToString());
    TEST_CASE_MSG(result.real() == 15.0, "actual=" << result);

    result = arith(ArithOp::Add, {i1, i2, r3, r4, r5})->eval(env);
    TEST_CASE_MSG(result.isReal(), "actual=" << result.typeToString());
    TEST_CASE_MSG(result.real() == 15.0, "actual=" << result);

    result = arith(ArithOp::Sub, {i5, i2, i1})->eval(env);
    TEST_CASE_MSG(result.isInt(), "actual=" << result.typeToString());
    TEST_CASE_MSG(result.integer() == 2ll, "actual=" << result);

    result = arith(ArithOp::Mul, {r5, r2, r3})->eval(env);
    TEST_CASE_MSG(result.isReal(), "actual=" << result.typeToString());
    TEST_CASE_MSG(result.real() == 30.0, "actual=" << result);

    result = arith(ArithOp::Div, {r4, r2, r4})->eval(env);
    TEST_CASE_MSG(result.isReal(), "actual=" << result.typeToString());
    TEST_CASE_MSG(result.real() == 0.5, "actual=" << result);

    result = arith(ArithOp::Mod, {i11, i4, i2})->eval(env);
    TEST_CASE_MSG(result.isInt(), "actual=" << result.typeToString());
    TEST_CASE_MSG(result.integer() == 1ll, "actual=" << result);

    result = arith(ArithOp::Pow, {i2, i3, i2})->eval(env);
    TEST_CASE_MSG(result.isReal(), "actual=" << result.typeToString());
    TEST_CASE_MSG(result.real() == 64.0, "actual=" << result);

    result = arith(ArithOp::Pow, {r2, r3, r2})->eval(env);
    TEST_CASE_MSG(result.isReal(), "actual=" << result.typeToString());
    TEST_CASE_MSG(result.real() == 64.0, "actual=" << result);
}

// -------------------------------------------------------------
#define TEST_COMP(TYPE, LHS, RHS, RVALUE) \
    comp.reset(new CompOp(CompOp::TYPE, LHS, RHS)); \
    result = comp->eval(env); \
    TEST_CASE_MSG(result.isBool(), "actual=" << result.typeToString()); \
    TEST_CASE_MSG(result.boolean() == RVALUE, "actual=" << result);
DEFINE_TEST(testCodeNodeCompOp) {
    auto env = Environment::make();

    CodeNode::SharedPtr iLit0(new Literal(Value(0ll)));
    CodeNode::SharedPtr iLit1(new Literal(Value(1ll)));
    CodeNode::SharedPtr rLit0(new Literal(Value(0.0)));
    CodeNode::SharedPtr rLit1(new Literal(Value(1.0)));
    CodeNode::SharedPtr cLitA(new Literal(Value('a')));
    CodeNode::SharedPtr cLitB(new Literal(Value('b')));
    CodeNode::SharedPtr bLitT(new Literal(Value(true)));
    CodeNode::SharedPtr bLitF(new Literal(Value(false)));

    CodeNode::SharedPtr comp;
    Value result;

    TEST_COMP(EQ, iLit0, iLit0, true);
    TEST_COMP(EQ, iLit0, iLit1, false);
    TEST_COMP(EQ, rLit1, rLit1, true);
    TEST_COMP(EQ, rLit0, rLit1, false);
    TEST_COMP(EQ, cLitA, cLitA, true);
    TEST_COMP(EQ, cLitA, cLitB, false);
    TEST_COMP(EQ, bLitT, bLitT, true);
    TEST_COMP(EQ, bLitT, bLitF, false);

    TEST_COMP(NE, iLit0, iLit0, false);
    TEST_COMP(NE, iLit0, iLit1, true);
    TEST_COMP(NE, rLit1, rLit1, false);
    TEST_COMP(NE, rLit0, rLit1, true);
    TEST_COMP(NE, cLitA, cLitA, false);
    TEST_COMP(NE, cLitA, cLitB, true);
    TEST_COMP(NE, bLitT, bLitT, false);
    TEST_COMP(NE, bLitT, bLitF, true);

    TEST_COMP(LT, iLit0, iLit0, false);
    TEST_COMP(LT, iLit0, iLit1, true);
    TEST_COMP(LT, rLit1, rLit1, false);
    TEST_COMP(LT, rLit0, rLit1, true);
    TEST_COMP(LT, cLitA, cLitA, false);
    TEST_COMP(LT, cLitA, cLitB, true);
    TEST_COMP(LT, bLitT, bLitT, false);
    TEST_COMP(LT, bLitF, bLitT, true);

    TEST_COMP(GT, iLit0, iLit0, false);
    TEST_COMP(GT, iLit1, iLit0, true);
    TEST_COMP(GT, rLit1, rLit1, false);
    TEST_COMP(GT, rLit1, rLit0, true);
    TEST_COMP(GT, cLitA, cLitA, false);
    TEST_COMP(GT, cLitB, cLitA, true);
    TEST_COMP(GT, bLitT, bLitT, false);
    TEST_COMP(GT, bLitT, bLitF, true);

    TEST_COMP(LE, iLit0, iLit0, true);
    TEST_COMP(LE, iLit0, iLit1, true);
    TEST_COMP(LE, rLit1, rLit1, true);
    TEST_COMP(LE, rLit0, rLit1, true);
    TEST_COMP(LE, cLitA, cLitA, true);
    TEST_COMP(LE, cLitA, cLitB, true);
    TEST_COMP(LE, bLitT, bLitT, true);
    TEST_COMP(LE, bLitF, bLitT, true);

    TEST_COMP(GE, iLit0, iLit0, true);
    TEST_COMP(GE, iLit1, iLit0, true);
    TEST_COMP(GE, rLit1, rLit1, true);
    TEST_COMP(GE, rLit1, rLit0, true);
    TEST_COMP(GE, cLitA, cLitA, true);
    TEST_COMP(GE, cLitB, cLitA, true);
    TEST_COMP(GE, bLitT, bLitT, true);
    TEST_COMP(GE, bLitT, bLitF, true);

    TEST_COMP(EQ, iLit1, rLit1, true);
    TEST_COMP(NE, iLit1, rLit0, true);
    TEST_COMP(LT, iLit0, rLit1, true);
    TEST_COMP(GT, iLit1, rLit0, true);
    TEST_COMP(LE, iLit1, rLit1, true);
    TEST_COMP(GE, iLit1, rLit1, true);

    try {
        comp.reset(new CompOp(CompOp::EQ, iLit0, cLitA));
        result = comp->eval(env);
        TEST_CASE(false);
    }
    catch (const IncompatibleTypes &ex) {}
    catch (...) { TEST_CASE(false); }

    try {
        comp.reset(new CompOp(CompOp::NE, iLit0, cLitA));
        result = comp->eval(env);
        TEST_CASE(false);
    }
    catch (const IncompatibleTypes &ex) {}
    catch (...) { TEST_CASE(false); }

    try {
        comp.reset(new CompOp(CompOp::LT, iLit0, cLitA));
        result = comp->eval(env);
        TEST_CASE(false);
    }
    catch (const IncompatibleTypes &ex) {}
    catch (...) { TEST_CASE(false); }

    try {
        comp.reset(new CompOp(CompOp::GT, iLit0, cLitA));
        result = comp->eval(env);
        TEST_CASE(false);
    }
    catch (const IncompatibleTypes &ex) {}
    catch (...) { TEST_CASE(false); }

    try {
        comp.reset(new CompOp(CompOp::LE, iLit0, cLitA));
        result = comp->eval(env);
        TEST_CASE(false);
    }
    catch (const IncompatibleTypes &ex) {}
    catch (...) { TEST_CASE(false); }

    try {
        comp.reset(new CompOp(CompOp::GE, iLit0, cLitA));
        result = comp->eval(env);
        TEST_CASE(false);
    }
    catch (const IncompatibleTypes &ex) {}
    catch (...) { TEST_CASE(false); }
}
#undef TEST_COMP

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeLogicOp) {
    auto env = Environment::make();

    struct Case {
        LogicOp::Type type;
        bool lhs;
        bool rhs;
        bool result;
        bool other;
        bool error;

        std::string toString() const {
            std::string str;
            str += static_cast<char>(type);
            str += (lhs ? " true" : " false");
            str += (rhs ? " true" : " false");
            str += (result ? " true" : " false");
            str += (other ? " true" : " false");
            str += (error ? " true" : " false");
            return str;
        }
    } cases[] = {
        { LogicOp::Conjunction, true,  true,  true,  false, false },
        { LogicOp::Conjunction, true,  false, false, false, false },
        { LogicOp::Conjunction, false, true,  false, false, false },
        { LogicOp::Conjunction, false, false, false, false, false },
        { LogicOp::Conjunction, true,  true,  false, true,  true  },
        { LogicOp::Disjunction, true,  true,  true,  false, false },
        { LogicOp::Disjunction, true,  false, true,  false, false },
        { LogicOp::Disjunction, false, true,  true,  false, false },
        { LogicOp::Disjunction, false, false, false, false, false },
        { LogicOp::Disjunction, true,  true,  true,  true,  false }
    };
    const size_t size = sizeof(cases)/sizeof(cases[0]);
    for (size_t i = 0; i < size; ++i) {
        const Case &c = cases[i];

        try {
            CodeNode::SharedPtr lhsLit(new Literal(Value(c.lhs)));
            CodeNode::SharedPtr rhsLit(new Literal(!c.other ? Value(c.rhs) : Value('a')));
            CodeNode::SharedPtr logic(new LogicOp(c.type, CodeNode::SharedPtrList{lhsLit, rhsLit}));
            Value result = logic->eval(env);
            if (!c.error) {
                TEST_CASE_MSG(result.isBool(), "actual=" << result.typeToString() << " (" << c.toString() << ')');
                TEST_CASE_MSG(result.boolean() == c.result, "actual=" << result << " (" << c.toString() << ')');
            }
            else {
                TEST_CASE_MSG(false, c.toString());
            }
        }
        catch (const InvalidOperandType &ex) { if (!c.error) { TEST_CASE_MSG(false, c.toString()); } }
        catch (...) { TEST_CASE_MSG(false, c.toString()); }
    }

    auto value = CodeNode::make<LogicOp>(
        LogicOp::Conjunction,
        CodeNode::SharedPtrList{
            CodeNode::make<Literal>(Value::True),
            CodeNode::make<Literal>(Value::True),
            CodeNode::make<Literal>(Value::False)})->eval(env);
    TEST_CASE_MSG(value.isBool(), "actual=" << value.typeToString());
    TEST_CASE_MSG(value == Value::False, "actual=" << value);

    value = CodeNode::make<LogicOp>(
        LogicOp::Disjunction,
        CodeNode::SharedPtrList{
            CodeNode::make<Literal>(Value::False),
            CodeNode::make<Literal>(Value::False),
            CodeNode::make<Literal>(Value::True)})->eval(env);
    TEST_CASE_MSG(value.isBool(), "actual=" << value.typeToString());
    TEST_CASE_MSG(value == Value::True, "actual=" << value);
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeNot) {
    auto env = Environment::make();

    Value value =
        CodeNode::make<Not>(CodeNode::make<Literal>(Value::True))
        ->eval(env);
    TEST_CASE_MSG(value.isBool(), "actual=" << value.typeToString());
    TEST_CASE_MSG(value == Value::False, "actual=" << value);

    value =
        CodeNode::make<Not>(CodeNode::make<Literal>(Value::False))
        ->eval(env);
    TEST_CASE_MSG(value.isBool(), "actual=" << value.typeToString());
    TEST_CASE_MSG(value == Value::True, "actual=" << value);

    try {
        value =
            CodeNode::make<Not>(CodeNode::make<Literal>(Value::Zero))
            ->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeNegativeOf) {
    auto env = Environment::make();

    auto iVal = CodeNode::make<Literal>(Value(5ll));
    auto fVal = CodeNode::make<Literal>(Value(3.5));

    auto inv = [env](CodeNode::SharedPtr operand) { return CodeNode::make<NegativeOf>(operand)->eval(env); };

    Value value = inv(iVal);
    TEST_CASE_MSG(value == Value(-5ll), "actual=" << value);

    value = inv(fVal);
    TEST_CASE_MSG(value == Value(-3.5), "actual=" << value);

    try {
        value = inv(CodeNode::make<Literal>(Value('a')));
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}
