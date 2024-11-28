#include "code_node.h"
#include "environment.h"
#include "lambda.h"

using namespace Ishlang;

// -------------------------------------------------------------
DEFINE_TEST(testLambda) {
    auto env = Environment::make();
    env->def("x", Value::Zero);

    const Lambda::ParamList params({"x", "y"});
    CodeNode::SharedPtr body(
        new ArithOp(ArithOp::Add,
                    { CodeNode::SharedPtr(new Variable("x")),
                      CodeNode::SharedPtr(new Variable("y")) }));

    Lambda lambda(params, body, env);

    Lambda::ArgList args;
    args.push_back(Value(3ll));
    args.push_back(Value(4ll));

    Value result = lambda.exec(args);
    TEST_CASE_MSG(result.isInt(), "actual=" << result.typeToString());
    TEST_CASE_MSG(result.integer() == 7ll, "actual=" << result);
}
