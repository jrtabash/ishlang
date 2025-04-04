#include "unit_test_function.h"

#include "code_node.h"
#include "environment.h"
#include "exception.h"
#include "hashtable.h"
#include "integer_range.h"
#include "lambda.h"
#include "sequence.h"
#include "value.h"

using namespace Ishlang;

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeSequence) {
    auto env = Environment::make();

    env->def("x", Value(1ll));
    env->def("y", Value(2ll));

    { // Block
        CodeNode::SharedPtr def1(new Define("x", CodeNode::SharedPtr(new Literal(Value(1ll)))));
        CodeNode::SharedPtr def2(new Define("y", CodeNode::SharedPtr(new Literal(Value(1ll)))));
        CodeNode::SharedPtr assign1(new Assign("x", CodeNode::SharedPtr(new Variable("y"))));
        CodeNode::SharedPtr assign2(new Assign("y", CodeNode::SharedPtr(new Literal(Value(3ll)))));
        CodeNode::SharedPtr litT(new Literal(Value('T')));

        CodeNode::SharedPtrList exprs;
        exprs.push_back(def1);
        exprs.push_back(def2);
        exprs.push_back(assign1);
        exprs.push_back(assign2);
        exprs.push_back(litT);

        CodeNode::SharedPtr block(new Block(exprs));
        Value result = block->eval(env);
        TEST_CASE_MSG(result.isChar(), "actual=" << result.typeToString());
        TEST_CASE_MSG(result.character() == 'T', "actual=" << result);
        TEST_CASE_MSG(env->get("x") == Value(1ll), "actual=" << env->get("x"));
        TEST_CASE_MSG(env->get("y") == Value(2ll), "actual=" << env->get("y"));
    }

    { // ProgN
        CodeNode::SharedPtr assign1(new Assign("x", CodeNode::SharedPtr(new Variable("y"))));
        CodeNode::SharedPtr assign2(new Assign("y", CodeNode::SharedPtr(new Literal(Value(3ll)))));
        CodeNode::SharedPtr litT(new Literal(Value('T')));

        CodeNode::SharedPtrList exprs;
        exprs.push_back(assign1);
        exprs.push_back(assign2);
        exprs.push_back(litT);

        CodeNode::SharedPtr progN(new ProgN(exprs));
        Value result = progN->eval(env);
        TEST_CASE_MSG(result.isChar(), "actual=" << result.typeToString());
        TEST_CASE_MSG(result.character() == 'T', "actual=" << result);
        TEST_CASE_MSG(env->get("x") == Value(2ll), "actual=" << env->get("x"));
        TEST_CASE_MSG(env->get("y") == Value(3ll), "actual=" << env->get("y"));
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeCond) {
    { // If
        auto env = Environment::make();

        {
            CodeNode::SharedPtr bLitT(new Literal(Value::True));
            CodeNode::SharedPtr bLitF(new Literal(Value::False));
            CodeNode::SharedPtr iLit1(new Literal(Value(1ll)));
            CodeNode::SharedPtr iLit2(new Literal(Value(2ll)));

            CodeNode::SharedPtr cond(new If(bLitT, iLit1));
            Value result = cond->eval(env);
            TEST_CASE_MSG(result.isInt(), "actual=" << result.typeToString());
            TEST_CASE_MSG(result.integer() == 1ll, "actual=" << result);

            cond.reset(new If(bLitF, iLit1));
            result = cond->eval(env);
            TEST_CASE_MSG(result.isNull(), "actual=" << result.typeToString());

            cond.reset(new If(bLitT, iLit1, iLit2));
            result = cond->eval(env);
            TEST_CASE_MSG(result.isInt(), "actual=" << result.typeToString());
            TEST_CASE_MSG(result.integer() == 1ll, "actual=" << result);

            cond.reset(new If(bLitF, iLit1, iLit2));
            result = cond->eval(env);
            TEST_CASE_MSG(result.isInt(), "actual=" << result.typeToString());
            TEST_CASE_MSG(result.integer() == 2ll, "actual=" << result);
        }

        {
            env->def("x", Value(1ll));
            env->def("y", Value(2ll));

            CodeNode::SharedPtr tPred(new Literal(Value::True));
            CodeNode::SharedPtr fPred(new Literal(Value::False));
            CodeNode::SharedPtr tCode(new Define("x", CodeNode::SharedPtr(new Literal(Value(3ll)))));
            CodeNode::SharedPtr fCode(new Assign("y", CodeNode::SharedPtr(new Literal(Value(4ll)))));

            CodeNode::SharedPtr cond(new If(tPred, tCode, fCode));
            Value result = cond->eval(env);
            TEST_CASE_MSG(result.isInt(), "actual=" << result.typeToString());
            TEST_CASE_MSG(result.integer() == 3ll, "actual=" << result);
            TEST_CASE_MSG(env->get("x") == Value(1ll), "actual=" << env->get("x"));
            TEST_CASE_MSG(env->get("y") == Value(2ll), "actual=" << env->get("y"));

            cond.reset(new If(fPred, tCode, fCode));
            result = cond->eval(env);
            TEST_CASE_MSG(result.isInt(), "actual=" << result.typeToString());
            TEST_CASE_MSG(result.integer() == 4ll, "actual=" << result);
            TEST_CASE_MSG(env->get("x") == Value(1ll), "actual=" << env->get("x"));
            TEST_CASE_MSG(env->get("y") == Value(4ll), "actual=" << env->get("y"));
        }
    }

    { // Cond
        auto env = Environment::make();

        {
            CodeNode::SharedPtrPairs cases;
            cases.push_back(
                std::make_pair(
                    CodeNode::SharedPtr(new Literal(Value::False)),
                    CodeNode::SharedPtr(new Literal(Value(1ll)))));
            cases.push_back(
                std::make_pair(
                    CodeNode::SharedPtr(new Literal(Value::True)),
                    CodeNode::SharedPtr(new Literal(Value(2ll)))));
            CodeNode::SharedPtr cond(new Cond(cases));
            Value result = cond->eval(env);
            TEST_CASE_MSG(result.isInt(), "actual=" << result.typeToString());
            TEST_CASE_MSG(result.integer() == 2ll, "actual=" << result);
        }

        {
            CodeNode::SharedPtrPairs cases;
            cases.push_back(
                std::make_pair(
                    CodeNode::SharedPtr(new Literal(Value::False)),
                    CodeNode::SharedPtr(new Literal(Value(1ll)))));
            cases.push_back(
                std::make_pair(
                    CodeNode::SharedPtr(new Literal(Value::True)),
                    CodeNode::SharedPtr()));
            CodeNode::SharedPtr cond(new Cond(cases));
            Value result = cond->eval(env);
            TEST_CASE_MSG(result.isNull(), "actual=" << result.typeToString());
        }

        {
            env->def("x", Value(3ll));
            CodeNode::SharedPtr var(new Variable("x"));

            CodeNode::SharedPtrPairs cases;
            cases.push_back(
                std::make_pair(
                    CodeNode::SharedPtr(new CompOp(CompOp::EQ, var, CodeNode::SharedPtr(new Literal(Value(1ll))))),
                    CodeNode::SharedPtr(new Literal(Value('a')))));
            cases.push_back(
                std::make_pair(
                    CodeNode::SharedPtr(new CompOp(CompOp::EQ, var, CodeNode::SharedPtr(new Literal(Value(2ll))))),
                    CodeNode::SharedPtr(new Literal(Value('b')))));
            cases.push_back(
                std::make_pair(
                    CodeNode::SharedPtr(new CompOp(CompOp::EQ, var, CodeNode::SharedPtr(new Literal(Value(3ll))))),
                    CodeNode::SharedPtr(new Literal(Value('c')))));
            cases.push_back(
                std::make_pair(
                    CodeNode::SharedPtr(new CompOp(CompOp::EQ, var, CodeNode::SharedPtr(new Literal(Value(4ll))))),
                    CodeNode::SharedPtr(new Literal(Value('d')))));
            cases.push_back(
                std::make_pair(
                    CodeNode::SharedPtr(new Literal(Value::True)),
                    CodeNode::SharedPtr(new Literal(Value('e')))));
            CodeNode::SharedPtr cond(new Cond(cases));
            Value result = cond->eval(env);
            TEST_CASE_MSG(result.isChar(), "actual=" << result.typeToString());
            TEST_CASE_MSG(result.character() == 'c', "actual=" << result);
        }

        {
            try {
                CodeNode::SharedPtrPairs cases;
                cases.push_back(std::make_pair(CodeNode::SharedPtr(), CodeNode::SharedPtr()));
                CodeNode::SharedPtr cond(new Cond(cases));
                cond->eval(env);
                TEST_CASE(false);
            }
            catch (const InvalidExpression &ex) {}
            catch (...) { TEST_CASE(false); }
        }

        {
            try {
                CodeNode::SharedPtrPairs cases;
                cases.push_back(std::make_pair(CodeNode::SharedPtr(new Literal(Value::Zero)), CodeNode::SharedPtr()));
                CodeNode::SharedPtr cond(new Cond(cases));
                cond->eval(env);
                TEST_CASE(false);
            }
            catch (const InvalidExpressionType &ex) {}
            catch (...) { TEST_CASE(false); }
        }
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeLoop) {
    auto env = Environment::make();

    { // For
        env->def("sum", Value(0ll));

        CodeNode::SharedPtr loop(
            new Loop(
                CodeNode::SharedPtr(
                    new Define("i", CodeNode::SharedPtr(new Literal(Value(1ll))))),
                CodeNode::SharedPtr(
                    new CompOp(CompOp::LE,
                               CodeNode::SharedPtr(new Variable("i")),
                               CodeNode::SharedPtr(new Literal(Value(10ll))))),
                CodeNode::SharedPtr(
                    new Assign("i", CodeNode::SharedPtr(
                                        new ArithOp(ArithOp::Add,
                                                    { CodeNode::SharedPtr(new Variable("i")),
                                                      CodeNode::SharedPtr(new Literal(Value(1ll))) })))),
                CodeNode::SharedPtr(
                    new Assign("sum", CodeNode::SharedPtr(
                                        new ArithOp(ArithOp::Add,
                                                    { CodeNode::SharedPtr(new Variable("sum")),
                                                      CodeNode::SharedPtr(new Variable("i")) }))))));
        Value result = loop->eval(env);
        TEST_CASE_MSG(result.isInt(), "actual=" << result.typeToString());
        TEST_CASE_MSG(result.integer() == 55ll, "actual=" << result);

        try {
            env->get("i");
            TEST_CASE(false);
        }
        catch (const UnknownSymbol &ex) {}
        catch (...) { TEST_CASE(false); }
    }

    { // While
        env->def("count", Value(1ll));

        CodeNode::SharedPtr loop(
                new Loop(
                    CodeNode::SharedPtr(
                        new CompOp(CompOp::LT,
                                   CodeNode::SharedPtr(new Variable("count")),
                                   CodeNode::SharedPtr(new Literal(Value(32ll))))),
                    CodeNode::SharedPtr(
                        new Assign("count", CodeNode::SharedPtr(
                                                new ArithOp(ArithOp::Mul,
                                                            { CodeNode::SharedPtr(new Variable("count")),
                                                              CodeNode::SharedPtr(new Literal(Value(2ll))) }))))));
        Value result = loop->eval(env);
        TEST_CASE_MSG(result.isInt(), "actual=" << result.typeToString());
        TEST_CASE_MSG(result.integer() == 32ll, "actual=" << result);
    }

    { // Break
        env->def("x", Value(0ll));

        CodeNode::SharedPtrList exprs;
        exprs.push_back(
            CodeNode::SharedPtr(
                new Assign("x", CodeNode::SharedPtr(
                                    new ArithOp(ArithOp::Add,
                                                { CodeNode::SharedPtr(new Variable("x")),
                                                  CodeNode::SharedPtr(new Literal(Value(1ll))) })))));
        exprs.push_back(
            CodeNode::SharedPtr(
                new If(CodeNode::SharedPtr(
                           new CompOp(CompOp::EQ,
                                      CodeNode::SharedPtr(new Variable("x")),
                                      CodeNode::SharedPtr(new Literal(Value(2ll))))),
                       CodeNode::SharedPtr(new Break()),
                       CodeNode::SharedPtr())));

        CodeNode::SharedPtr loop(
                new Loop(
                    CodeNode::SharedPtr(new Literal(Value::True)),
                    CodeNode::SharedPtr(new ProgN(exprs))));

        Value result = loop->eval(env);
        TEST_CASE_MSG(result.isNull(), "actual=" << result.typeToString());
        TEST_CASE_MSG(env->get("x") == Value(2ll), "actual=" << env->get("x"));
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeForeach) {
    auto var = [](const char *name) {
        return CodeNode::make<Variable>(name);
    };
    auto lit = [](auto val) {
        return CodeNode::make<Literal>(Value(val));
    };
    auto add = [](auto lhs, auto rhs) {
        return CodeNode::make<ArithOp>(ArithOp::Add, CodeNode::SharedPtrList{lhs, rhs});
    };
    auto assign = [](const char *name, auto expr) {
        return CodeNode::make<Assign>(name, expr);
    };
    auto first = [](auto expr) {
        return CodeNode::make<PairFirst>(expr);
    };
    auto second = [](auto expr) {
        return CodeNode::make<PairSecond>(expr);
    };
    auto progn = [](auto e1, auto e2, auto e3) {
        CodeNode::SharedPtrList exprs;
        exprs.push_back(e1);
        exprs.push_back(e2);
        exprs.push_back(e3);
        return CodeNode::make<ProgN>(exprs);
    };

    { // String
        auto env = Environment::make();
        env->def("str", Value("1234567890"));
        env->def("cnt", Value(0ll));

        auto foreach = CodeNode::make<Foreach>(
            "c",
            var("str"),
            assign("cnt", add(var("cnt"), lit(1ll))));
        Value result = foreach->eval(env);
        TEST_CASE_MSG(result.isInt(), "actual=" << result.typeToString());
        TEST_CASE_MSG(result.integer() == 10ll, "actual=" << result);

        auto const & cnt = env->get("cnt");
        TEST_CASE_MSG(cnt.isInt(), "actual=" << cnt.typeToString());
        TEST_CASE_MSG(cnt.integer() == 10ll, "actual=" << cnt);
    }

    { // Array
        auto env = Environment::make();
        env->def("arr", Value(Sequence::generate(10, [i = 1ll]() mutable { return Value(i++); })));
        env->def("sum", Value(0ll));

        auto foreach = CodeNode::make<Foreach>(
            "i",
            var("arr"),
            assign("sum", add(var("sum"), var("i"))));
        Value result = foreach->eval(env);
        TEST_CASE_MSG(result.isInt(), "actual=" << result.typeToString());
        TEST_CASE_MSG(result.integer() == 55ll, "actual=" << result);

        auto const & sum = env->get("sum");
        TEST_CASE_MSG(sum.isInt(), "actual=" << sum.typeToString());
        TEST_CASE_MSG(sum.integer() == 55ll, "actual=" << sum);
    }

    { // Hashtable
        auto env = Environment::make();
        Hashtable ht;
        for (auto [k, v] : {
                std::pair{ 1ll, 10ll },
                std::pair{ 2ll, 20ll },
                std::pair{ 3ll, 30ll },
                std::pair{ 4ll, 40ll },
                std::pair{ 5ll, 50ll } }) {
            ht.set(Value(k), Value(v));
        }
        env->def("ht", Value(ht));
        env->def("cnt", Value(0ll));
        env->def("keySum", Value(0ll));
        env->def("valSum", Value(0ll));

        auto foreach = CodeNode::make<Foreach>(
            "kv",
            var("ht"),
            progn(
                assign("cnt", add(var("cnt"), lit(1ll))),
                assign("keySum", add(var("keySum"), first(var("kv")))),
                assign("valSum", add(var("valSum"), second(var("kv"))))));
        Value result = foreach->eval(env);
        TEST_CASE_MSG(result.isInt(), "actual=" << result.typeToString());
        TEST_CASE_MSG(result.integer() == 150ll, "actual=" << result);

        auto const & cnt = env->get("cnt");
        TEST_CASE_MSG(cnt.isInt(), "actual=" << cnt.typeToString());
        TEST_CASE_MSG(cnt.integer() == 5ll, "actual=" << cnt);

        auto const & keySum = env->get("keySum");
        TEST_CASE_MSG(keySum.isInt(), "actual=" << keySum.typeToString());
        TEST_CASE_MSG(keySum.integer() == 15ll, "actual=" << keySum);

        auto const & valSum = env->get("valSum");
        TEST_CASE_MSG(valSum.isInt(), "actual=" << valSum.typeToString());
        TEST_CASE_MSG(valSum.integer() == 150ll, "actual=" << valSum);
    }

    { // Range
        auto env = Environment::make();
        env->def("rng", Value(IntegerRange(0, 20, 2)));
        env->def("sum", Value(0ll));

        auto foreach = CodeNode::make<Foreach>(
            "i",
            var("rng"),
            assign("sum", add(var("sum"), var("i"))));
        Value result = foreach->eval(env);
        TEST_CASE_MSG(result.isInt(), "actual=" << result.typeToString());
        TEST_CASE_MSG(result.integer() == 90ll, "actual=" << result);

        auto const & sum = env->get("sum");
        TEST_CASE_MSG(sum.isInt(), "actual=" << sum.typeToString());
        TEST_CASE_MSG(sum.integer() == 90ll, "actual=" << sum);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeLambdaExpr) {
    auto env = Environment::make();
    env->def("foo", Value(1ll));

    const Lambda::ParamList params({"x", "y"});

    CodeNode::SharedPtr body(
        new ArithOp(ArithOp::Add,
                    { CodeNode::SharedPtr(
                            new ArithOp(ArithOp::Add,
                                        { CodeNode::SharedPtr(new Variable("x")),
                                          CodeNode::SharedPtr(new Variable("y")) })),
                      CodeNode::SharedPtr(new Variable("foo")) }));

    CodeNode::SharedPtr lambdaExpr(new LambdaExpr(params, body));
    Value lambdaVal = lambdaExpr->eval(env);
    TEST_CASE_MSG(lambdaVal.isClosure(), "actual=" << lambdaVal.typeToString());

    Lambda::ArgList args;
    args.push_back(Value(3ll));
    args.push_back(Value(4ll));

    const Lambda &func = lambdaVal.closure();
    Value result = func.exec(args);
    TEST_CASE_MSG(result.isInt(), "actual=" << result.typeToString());
    TEST_CASE_MSG(result.integer() == 8ll, "actual=" << result);
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeLambdaApp) {
    auto env = Environment::make();

    const Lambda::ParamList params({"x", "y"});

    CodeNode::SharedPtr body(
        new ArithOp(ArithOp::Add,
                    { CodeNode::SharedPtr(new Variable("x")),
                      CodeNode::SharedPtr(new Variable("y")) }));

    CodeNode::SharedPtr lambdaExpr(new LambdaExpr(params, body));

    CodeNode::SharedPtrList args;
    args.push_back(CodeNode::SharedPtr(new Literal(Value(3ll))));
    args.push_back(CodeNode::SharedPtr(new Literal(Value(4ll))));
    CodeNode::SharedPtr appExpr(new LambdaApp(lambdaExpr, args));

    Value result = appExpr->eval(env);
    TEST_CASE_MSG(result.isInt(), "actual=" << result.typeToString());
    TEST_CASE_MSG(result.integer() == 7ll, "actual=" << result);
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeFunctionExpr) {
    auto env = Environment::make();

    const Lambda::ParamList params({"x", "y"});

    CodeNode::SharedPtr body(
        new ArithOp(ArithOp::Add,
                    { CodeNode::SharedPtr(new Variable("x")),
                      CodeNode::SharedPtr(new Variable("y")) }));

    CodeNode::SharedPtr ftnExpr(new FunctionExpr("myfunc", params, body));
    Value ftnVal = ftnExpr->eval(env);
    TEST_CASE_MSG(ftnVal.isClosure(), "actual=" << ftnVal.typeToString());

    Lambda::ArgList args;
    args.push_back(Value(3ll));
    args.push_back(Value(4ll));

    const Value &ftn = env->get("myfunc");
    TEST_CASE_MSG(ftn.isClosure(), "actual=" << ftn.typeToString());

    Value result = ftn.closure().exec(args);
    TEST_CASE_MSG(result.isInt(), "actual=" << result.typeToString());
    TEST_CASE_MSG(result.integer() == 7ll, "actual=" << result);
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeFunctionApp) {
    auto env = Environment::make();

    CodeNode::SharedPtr ftnExpr(
        CodeNode::make<FunctionExpr>(
            "myfunc",
            Lambda::ParamList({"x", "y"}),
            CodeNode::make<ArithOp>(
                ArithOp::Add,
                CodeNode::SharedPtrList{
                    CodeNode::make<Variable>("x"),
                    CodeNode::make<Variable>("y") })));
    Value ftnVal = ftnExpr->eval(env);
    TEST_CASE_MSG(ftnVal.isClosure(), "actual=" << ftnVal.typeToString());

    CodeNode::SharedPtr ftnApp(
        CodeNode::make<FunctionApp>(
            "myfunc",
            CodeNode::SharedPtrList({
                    CodeNode::make<Literal>(Value(3ll)),
                        CodeNode::make<Literal>(Value(4ll))})));

    Value result = ftnApp->eval(env);
    TEST_CASE_MSG(result.isInt(), "actual=" << result.typeToString());
    TEST_CASE_MSG(result.integer() == 7ll, "actual=" << result);
}
