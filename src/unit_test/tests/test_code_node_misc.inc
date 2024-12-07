#include "code_node.h"
#include "environment.h"
#include "exception.h"
#include "module.h"
#include "lambda.h"
#include "sequence.h"
#include "value.h"

#include <array>
#include <map>
#include <string>
#include <vector>

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeBasic) {
    auto env = Environment::make();

    CodeNode::SharedPtr literal(new Literal(Value('c')));
    Value result = literal->eval(env);
    TEST_CASE_MSG(result == Value('c'), "actual=" << result << " (" << result.typeToString() << ')');

    CodeNode::SharedPtr definition(new Define("var", literal));
    result = definition->eval(env);
    TEST_CASE_MSG(result == Value('c'), "actual=" << result << " (" << result.typeToString() << ')');

    CodeNode::SharedPtr definition2(new Define("var2", CodeNode::SharedPtr(new Literal(Value(2ll)))));
    result = definition2->eval(env);
    TEST_CASE_MSG(result == Value(2ll), "actual=" << result << " (" << result.typeToString() << ')');

    CodeNode::SharedPtr variable(new Variable("var"));
    result = variable->eval(env);
    TEST_CASE_MSG(result == Value('c'), "actual=" << result << " (" << result.typeToString() << ')');

    CodeNode::SharedPtr assignment(new Assign("var", CodeNode::SharedPtr(new Variable("var2"))));
    result = assignment->eval(env);
    TEST_CASE_MSG(result == Value(2ll), "actual=" << result << " (" << result.typeToString() << ')');

    CodeNode::SharedPtr exists(new Exists("var"));
    result = exists->eval(env);
    TEST_CASE_MSG(result == Value::True, "actual=" << result << " (" << result.typeToString() << ')');

    CodeNode::SharedPtr doesNotExist(new Exists("novar"));
    result = doesNotExist->eval(env);
    TEST_CASE_MSG(result == Value::False, "actual=" << result << " (" << result.typeToString() << ')');

    variable.reset(new Variable("var"));
    result = variable->eval(env);
    TEST_CASE_MSG(result == Value(2ll), "actual=" << result << " (" << result.typeToString() << ')');
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeClone) {
    auto env = Environment::make();

    CodeNode::SharedPtr definition(new Define("var", CodeNode::SharedPtr(new Literal(Value("foobar")))));
    Value result1 = definition->eval(env);

    CodeNode::SharedPtr cloneExpr(new Clone(CodeNode::SharedPtr(new Variable("var"))));
    Value result2 = cloneExpr->eval(env);

    TEST_CASE(result1 == result2);

    auto & text1 = result1.text();
    auto & text2 = result2.text();

    TEST_CASE(text1 == text2);
    TEST_CASE(&text1 != &text2);
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeIsType) {
    auto env = Environment::make();

    Value lambdaValue(
        Lambda(CodeNode::ParamList(),
               CodeNode::SharedPtr(new Literal(Value::Zero)),
               env));

    Value structValue(Struct("Person", {"name", "age"}));

    Value instanceValue(Instance(structValue.userType()));

    Value arrayValue(Sequence(1, Value::Zero));

    const std::array<Value::Type, 10> types = {
        Value::eNone,
        Value::eInteger,
        Value::eReal,
        Value::eCharacter,
        Value::eBoolean,
        Value::eString,
        Value::eClosure,
        Value::eUserType,
        Value::eUserObject,
        Value::eArray
    };

    std::map<CodeNode::SharedPtr, Value::Type> valueTypes; {
        valueTypes.insert(std::make_pair(CodeNode::SharedPtr(new Literal(Value::Null)), Value::eNone));
        valueTypes.insert(std::make_pair(CodeNode::SharedPtr(new Literal(Value(1ll))), Value::eInteger));
        valueTypes.insert(std::make_pair(CodeNode::SharedPtr(new Literal(Value(1.0))), Value::eReal));
        valueTypes.insert(std::make_pair(CodeNode::SharedPtr(new Literal(Value('c'))),  Value::eCharacter));
        valueTypes.insert(std::make_pair(CodeNode::SharedPtr(new Literal(Value::True)), Value::eBoolean));
        valueTypes.insert(std::make_pair(CodeNode::SharedPtr(new Literal(Value("txt"))), Value::eString));
        valueTypes.insert(std::make_pair(CodeNode::SharedPtr(new Literal(lambdaValue)), Value::eClosure));
        valueTypes.insert(std::make_pair(CodeNode::SharedPtr(new Literal(structValue)), Value::eUserType));
        valueTypes.insert(std::make_pair(CodeNode::SharedPtr(new Literal(instanceValue)), Value::eUserObject));
        valueTypes.insert(std::make_pair(CodeNode::SharedPtr(new Literal(arrayValue)), Value::eArray));
    }

    for (auto valueType : valueTypes) {
        CodeNode::SharedPtr expr = valueType.first;
        Value::Type exprType = valueType.second;

        for (auto type : types) {
            CodeNode::SharedPtr isType(new IsType(expr, { type }));
            Value result = isType->eval(env);
            bool expected = exprType == type ? true : false;
            TEST_CASE_MSG(result.isBool() && result.boolean() == expected,
                          "actual=" << Value::typeToString(exprType) << " check=" << Value::typeToString(type));

            if (exprType != Value::eUserType && exprType != Value::eUserObject) {
                CodeNode::SharedPtr isTypeMulti(new IsType(expr, { Value::eUserType, type, Value::eUserObject }));
                Value result = isTypeMulti->eval(env);
                bool expected = exprType == type ? true : false;
                TEST_CASE_MSG(result.isBool() && result.boolean() == expected,
                              "actual=" << Value::typeToString(exprType) << " check=usertype|" << Value::typeToString(type) << "|userobject");
            }

            if (exprType == Value::eInteger) {
                CodeNode::SharedPtr isTypeMulti(new IsType(expr, { Value::eReal, type }));
                Value result = isTypeMulti->eval(env);
                bool expected = exprType == type ? true : false;
                TEST_CASE_MSG(result.isBool() && result.boolean() == expected,
                              "actual=" << Value::typeToString(exprType) << " check=" << typesToString(Value::eReal, type));
            }
            else if (exprType == Value::eReal) {
                CodeNode::SharedPtr isTypeMulti(new IsType(expr, { Value::eInteger, type }));
                Value result = isTypeMulti->eval(env);
                bool expected = exprType == type ? true : false;
                TEST_CASE_MSG(result.isBool() && result.boolean() == expected,
                              "actual=" << Value::typeToString(exprType) << " check=" << typesToString(Value::eInteger, type));
            }
        }
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeTypeName) {
    auto env = Environment::make();

    Value lambdaValue(
        Lambda(CodeNode::ParamList(),
               CodeNode::SharedPtr(new Literal(Value::Zero)),
               env));

    Value structValue(Struct("Person", {"name", "age"}));

    Value instanceValue(Instance(structValue.userType()));

    Value arrayValue(Sequence(1, Value::Zero));

    std::vector<std::pair<CodeNode::SharedPtr, std::string>> valueTypePairs; {
        valueTypePairs.push_back(std::make_pair(CodeNode::SharedPtr(new Literal(Value::Null)),   "none"));
        valueTypePairs.push_back(std::make_pair(CodeNode::SharedPtr(new Literal(Value(1ll))),    "int"));
        valueTypePairs.push_back(std::make_pair(CodeNode::SharedPtr(new Literal(Value(1.0))),    "real"));
        valueTypePairs.push_back(std::make_pair(CodeNode::SharedPtr(new Literal(Value('c'))),    "char"));
        valueTypePairs.push_back(std::make_pair(CodeNode::SharedPtr(new Literal(Value::True)),   "bool"));
        valueTypePairs.push_back(std::make_pair(CodeNode::SharedPtr(new Literal(Value("txt"))),  "string"));
        valueTypePairs.push_back(std::make_pair(CodeNode::SharedPtr(new Literal(lambdaValue)),   "closure"));
        valueTypePairs.push_back(std::make_pair(CodeNode::SharedPtr(new Literal(structValue)),   "usertype"));
        valueTypePairs.push_back(std::make_pair(CodeNode::SharedPtr(new Literal(instanceValue)), "userobject"));
        valueTypePairs.push_back(std::make_pair(CodeNode::SharedPtr(new Literal(arrayValue)),    "array"));
    }

    for (auto const & valueType : valueTypePairs) {
        auto expr = CodeNode::SharedPtr(new TypeName(valueType.first));
        auto const & expectedName = valueType.second;

        Value typeName = expr->eval(env);
        TEST_CASE_MSG(typeName.isString() && typeName.text() == expectedName,
                      "actual=" << Value::typeToString(typeName.type()) << " expected=" << expectedName);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeAsType) {
    auto env = Environment::make();

    const Value int1(1ll);
    const Value int98(98ll);
    const Value real1(1.0);
    const Value real1p5(1.5);
    const Value real98(98.0);
    const Value charB('b');
    const Value charT('t');
    const Value boolTrue(Value::True);
    const Value string98("98");
    const Value string1p5("1.500000");
    const Value stringTrue("true");
    const Value stringB("b");

    std::vector<std::pair<Value, Value>> testCases = {
        { int1,  real1 },
        { int98, charB },
        { int1,  boolTrue },
        { int98,  string98 },
        { real1p5, int1 },
        { real98,  charB },
        { real1,   boolTrue },
        { real1p5, string1p5 },
        { charB, int98 },
        { charB, real98 },
        { charT, boolTrue },
        { charB, stringB },
        { boolTrue, int1 },
        { boolTrue, real1 },
        { boolTrue, charT },
        { boolTrue, stringTrue },
        { string98, int98 },
        { string98, real98 },
        { stringB, charB },
        { stringTrue, boolTrue }
    };

    for (auto const & fromAndTo : testCases) {
        CodeNode::SharedPtr asType =
            CodeNode::make<AsType>(
                CodeNode::make<Literal>(fromAndTo.first), fromAndTo.second.type());
        Value value = asType->eval(env);
        TEST_CASE_MSG(value == fromAndTo.second, "actual=" << value << " expected=" << fromAndTo.second);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeAssert) {
    auto env = Environment::make();

    auto const aeval = [env](const std::string &tag, Value value) {
        return CodeNode::make<Assert>(
            tag,
            CodeNode::make<Literal>(value))->eval(env);
    };

    try {
        aeval("A1", Value::True);
    }
    catch (const AssertFailed &) {
        TEST_CASE(false);
    }
    catch (...) {
        TEST_CASE(false);
    }

    try {
        aeval("A2", Value::False);
        TEST_CASE(false);
    }
    catch (const AssertFailed &ex) {
        TEST_CASE_MSG(std::string("Assert A2 failed") == ex.what(), "actual='" << ex.what() << "'");
    }
    catch (...) {
        TEST_CASE(false);
    }

    try {
        aeval("A3", Value::Zero);
        TEST_CASE(false);
    }
    catch (const AssertFailed &ex) {
        TEST_CASE_MSG(std::string("Assert A3 failed (unable to check non-boolean expression)") == ex.what(), "actual='" << ex.what() << "'");
    }
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeImportModule) {
    const std::string moduleName = "importtest";
    auto tempFile(unitTest().createTempModuleFile(moduleName));

    {
        auto env = Environment::make();

        auto varName = [&moduleName](const char *name) { return moduleName + '.' + name; };

        auto import = CodeNode::make<ImportModule>(moduleName);
        TEST_CASE(import->eval(env) == Value::True);

        TEST_CASE_MSG(env->size() == 3, "actual=" << env->size());
        TEST_CASE(env->exists(varName("PI")));
        TEST_CASE(env->exists(varName("add")));
        TEST_CASE(env->exists(varName("sub")));

        TEST_CASE(env->get(varName("PI")).isReal());
        TEST_CASE(env->get(varName("add")).isClosure());
        TEST_CASE(env->get(varName("sub")).isClosure());
    }

    {
        auto env = Environment::make();

        const std::string asName = "testmod";
        auto varAsName = [&asName](const char *name) { return asName + '.' + name; };

        auto import = CodeNode::make<ImportModule>(moduleName, asName);
        TEST_CASE(import->eval(env) == Value::True);

        TEST_CASE_MSG(env->size() == 3, "actual=" << env->size());
        TEST_CASE(env->exists(varAsName("PI")));
        TEST_CASE(env->exists(varAsName("add")));
        TEST_CASE(env->exists(varAsName("sub")));

        TEST_CASE(env->get(varAsName("PI")).isReal());
        TEST_CASE(env->get(varAsName("add")).isClosure());
        TEST_CASE(env->get(varAsName("sub")).isClosure());
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeFromModuleImport) {
    const std::string moduleName = "fromtest";
    auto tempFile(unitTest().createTempModuleFile(moduleName));

    {
        auto env = Environment::make();

        auto fromImport = CodeNode::make<FromModuleImport>(moduleName, "add");
        TEST_CASE(fromImport->eval(env) == Value::True);

        TEST_CASE_MSG(env->size() == 1, "actual=" << env->size());
        TEST_CASE(env->exists("add"));

        fromImport = CodeNode::make<FromModuleImport>(moduleName, "add", "plus");
        TEST_CASE(fromImport->eval(env) == Value::True);

        TEST_CASE_MSG(env->size() == 2, "actual=" << env->size());
        TEST_CASE(env->exists("add"));
        TEST_CASE(env->exists("plus"));

        TEST_CASE(env->get("add").isClosure());
        TEST_CASE(env->get("plus").isClosure());
        TEST_CASE(env->get("add") == env->get("plus"));
    }

    {
        auto env = Environment::make();

        auto fromImport = CodeNode::make<FromModuleImport>(
            moduleName,
            Module::AliasList({ { "add", Module::OptionalName() }, { "sub", "minus" } }));

        TEST_CASE(fromImport->eval(env) == Value::True);

        TEST_CASE_MSG(env->size() == 2, "actual=" << env->size());
        TEST_CASE(env->exists("add"));
        TEST_CASE(env->exists("minus"));

        TEST_CASE(env->get("add").isClosure());
        TEST_CASE(env->get("minus").isClosure());
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeRandom) {
    auto env = Environment::make();

    auto lit = [](const Value &val) { return CodeNode::make<Literal>(val); };

    auto test = [&env, lit](Value::Long mod, std::unordered_set<Value::Long> expectedSet) {
                    auto randCode = CodeNode::make<Random>(lit(Value(mod)));
                    for (int i = 0; i < 1000; ++i) {
                        if (expectedSet.find(randCode->eval(env).integer()) == expectedSet.end()) {
                            return false;
                        }
                    }
                    return true;
                };

    TEST_CASE_MSG(test(0, { 0 }), "rand(0)");
    TEST_CASE_MSG(test(1, { 0, 1 }), "rand(1)");
    TEST_CASE_MSG(test(2, { 0, 1, 2 }), "rand(2)");
    TEST_CASE_MSG(test(10, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 }), "rand(10)");

    try {
        CodeNode::make<Random>(lit(Value(-1ll)))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidExpression &ex) {
        TEST_CASE(std::string("Invalid expression - max negative") == ex.what());
    }
    catch (...) {
        TEST_CASE(false);
    }

    try {
        CodeNode::make<Random>(lit(Value('a')))->eval(env);
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
DEFINE_TEST(testCodeNodeHash) {
    auto env = Environment::make();

    auto hash = [&env](const Value &value) {
                    auto lit = CodeNode::make<Literal>(value);
                    return CodeNode::make<Hash>(lit)->eval(env);
                };

    auto test = [hash](const Value &value) {
                    auto hashValue = hash(value);
                    return hashValue.isInt() && hashValue.integer() >= 0;
                };

    TEST_CASE(hash(Value::Null) == Value::Zero);

    TEST_CASE_MSG(test(Value(5ll)), "hash(5)");
    TEST_CASE_MSG(test(Value(2.5)), "hash(2.5)");
    TEST_CASE_MSG(test(Value::True), "hash(true)");
    TEST_CASE_MSG(test(Value::False), "hash(false");
    TEST_CASE_MSG(test(Value('a')), "hash('a')");
    TEST_CASE_MSG(test(Value("str")), "hash(\"str\")");
    TEST_CASE_MSG(test(Value(Value::Pair(Value(1ll), Value(2ll)))), "hash(pair)");
    TEST_CASE_MSG(test(Value(Sequence())), "hash(array");
    TEST_CASE_MSG(test(Value(Hashtable())), "hash(hashmap)");
    TEST_CASE_MSG(test(Value(IntegerRange())), "hash(range)");
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodePairOperations) {
    auto env = Environment::make();

    auto ilit = [](Value::Long value) { return CodeNode::make<Literal>(Value(value)); };

    Value value = CodeNode::make<MakePair>(ilit(1), ilit(2))->eval(env);
    TEST_CASE_MSG(value.isPair(), "actual=" << value.isPair());
    TEST_CASE_MSG(value.pair().first() == Value(1ll), "actual=" << value.pair().first());
    TEST_CASE_MSG(value.pair().second() == Value(2ll), "actual=" << value.pair().second());

    env->def("p", value);

    Value fValue = CodeNode::make<PairFirst>(CodeNode::make<Variable>("p"))->eval(env);
    TEST_CASE_MSG(fValue == Value(1ll), "actual=" << fValue);

    Value sValue = CodeNode::make<PairSecond>(CodeNode::make<Variable>("p"))->eval(env);
    TEST_CASE_MSG(sValue == Value(2ll), "actual=" << sValue);

    try {
        CodeNode::make<PairFirst>(CodeNode::make<Literal>(Value(1ll)))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }

    try {
        CodeNode::make<PairSecond>(CodeNode::make<Literal>(Value(1ll)))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}
