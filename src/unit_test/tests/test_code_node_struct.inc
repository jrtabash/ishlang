#include "code_node.h"
#include "environment.h"
#include "instance.h"
#include "struct.h"
#include "value.h"

using namespace Ishlang;

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeStruct) {
    auto env = Environment::make();

    CodeNode::SharedPtr structExpr(
        CodeNode::make<StructExpr>("Person", Struct::MemberList({"name", "age"})));
    Value structVal = structExpr->eval(env);
    TEST_CASE_MSG(structVal.isUserType(), "actual=" << structVal.typeToString());

    const Struct &s = structVal.userType();
    TEST_CASE_MSG(s.name() == "Person", "actual=" << s.name());
    TEST_CASE_MSG(s.members().size() == 2, "actual=" << s.members().size());
    TEST_CASE_MSG(s.members()[0] == "name", "actual=" << s.members()[0]);
    TEST_CASE_MSG(s.members()[1] == "age", "actual=" << s.members()[1]);
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeIsStructName) {
    auto env = Environment::make();

    CodeNode::SharedPtr isStructName(
        CodeNode::make<IsStructName>(
            CodeNode::make<StructExpr>("Person", Struct::MemberList({"name", "age"})),
            "Person"));

    Value result = isStructName->eval(env);
    TEST_CASE_MSG(result.isBool(), "actual=" << result.typeToString());
    TEST_CASE_MSG(result == Value::True, "actual=" << result << " (" << result.typeToString() << ')');
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeMakeInstance) {
    auto env = Environment::make();

    CodeNode::SharedPtr structExpr(
        CodeNode::make<StructExpr>("Person", Struct::MemberList({"name", "age"})));

    Value structValue = structExpr->eval(env);
    TEST_CASE_MSG(structValue.isUserType(), "actual=" << structValue.typeToString());
    TEST_CASE_MSG(structValue.userType().name() == "Person", "actual=" << structValue.userType().name());

    CodeNode::SharedPtr makeInstance(
        CodeNode::make<MakeInstance>("Person"));

    Value instValue = makeInstance->eval(env);
    TEST_CASE_MSG(instValue.isUserObject(), "actual=" << instValue.typeToString());

    const Struct &type = instValue.userObject().type();
    TEST_CASE_MSG(type.name() == "Person", "actual=" << type.name());

    const Instance &inst = instValue.userObject();
    TEST_CASE_MSG(inst.get("name") == Value::Null, "actual=" << inst.get("name"));
    TEST_CASE_MSG(inst.get("age") == Value::Null, "actual=" << inst.get("age"));
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeMakeInstanceWithInitArgs) {
    auto env = Environment::make();

    CodeNode::SharedPtr structExpr(
        CodeNode::make<StructExpr>("Person", Struct::MemberList({"name", "age"})));

    Value structValue = structExpr->eval(env);
    TEST_CASE_MSG(structValue.isUserType(), "actual=" << structValue.typeToString());
    TEST_CASE_MSG(structValue.userType().name() == "Person", "actual=" << structValue.userType().name());

    CodeNode::SharedPtr nameInit = CodeNode::make<Literal>(Value("John"));
    CodeNode::SharedPtr ageInit = CodeNode::make<Literal>(Value(30ll));

    CodeNode::SharedPtr makeInstance(
        CodeNode::make<MakeInstance>("Person", CodeNode::NameSharedPtrs{{"name", nameInit}, {"age", ageInit}}));

    Value instValue = makeInstance->eval(env);
    TEST_CASE_MSG(instValue.isUserObject(), "actual=" << instValue.typeToString());

    const Struct &type = instValue.userObject().type();
    TEST_CASE_MSG(type.name() == "Person", "actual=" << type.name());

    const Instance &inst = instValue.userObject();
    TEST_CASE_MSG(inst.get("name") == Value("John"), "actual=" << inst.get("name"));
    TEST_CASE_MSG(inst.get("age") == Value(30ll), "actual=" << inst.get("age"));

    // Partial init args
    makeInstance = CodeNode::make<MakeInstance>("Person", CodeNode::NameSharedPtrs{{"name", nameInit}});

    instValue = makeInstance->eval(env);
    TEST_CASE_MSG(instValue.isUserObject(), "actual=" << instValue.typeToString());

    const Instance &inst2 = instValue.userObject();
    TEST_CASE_MSG(inst2.get("name") == Value("John"), "actual=" << inst2.get("name"));
    TEST_CASE_MSG(inst2.get("age") == Value::Null, "actual=" << inst2.get("age"));
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeIsInstanceOf) {
    auto env = Environment::make();

    CodeNode::make<StructExpr>("Person", Struct::MemberList({"name", "age"}))
        ->eval(env);

    Value value =
        CodeNode::make<IsInstanceOf>(
            CodeNode::make<MakeInstance>("Person"),
            "Person")
        ->eval(env);
    TEST_CASE_MSG(value == Value::True, "actual=" << value);
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeStructName) {
    auto env = Environment::make();

    CodeNode::make<StructExpr>("Person", Struct::MemberList({"name", "age"}))
        ->eval(env);

    Value value =
        CodeNode::make<StructName>(
            CodeNode::make<MakeInstance>("Person"))
        ->eval(env);
    TEST_CASE_MSG(value == Value("Person"), "actual=" << value);

    value =
        CodeNode::make<StructName>(
            CodeNode::make<Variable>("Person"))
        ->eval(env);
    TEST_CASE_MSG(value == Value("Person"), "actual" << value);

    try {
        value =
            CodeNode::make<StructName>(
                CodeNode::make<Literal>(Value::Zero))
            ->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidExpressionType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeGetSetMember) {
    auto env = Environment::make();

    CodeNode::make<StructExpr>("Person", Struct::MemberList({"name", "age"}))
        ->eval(env);

    CodeNode::make<Define>(
        "inst",
        CodeNode::make<MakeInstance>("Person"))
        ->eval(env);

    Value value =
        CodeNode::make<IsInstanceOf>(
            CodeNode::make<Variable>("inst"),
            "Person")
        ->eval(env);
    TEST_CASE_MSG(value == Value::True, "actual=" << value);

    value =
        CodeNode::make<GetMember>(
            CodeNode::make<Variable>("inst"),
            "name")
        ->eval(env);
    TEST_CASE_MSG(value == Value::Null, "actual=" << value);

    value =
        CodeNode::make<SetMember>(
            CodeNode::make<Variable>("inst"),
            "name",
            CodeNode::make<Literal>(Value("John")))
        ->eval(env);
    TEST_CASE_MSG(value == Value("John"), "actual=" << value);

    value =
        CodeNode::make<GetMember>(
            CodeNode::make<Variable>("inst"),
            "name")
        ->eval(env);
    TEST_CASE_MSG(value == Value("John"), "actual=" << value);
}
