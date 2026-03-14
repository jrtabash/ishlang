#include "unit_test_function.h"

#include "code_node.h"
#include "environment.h"
#include "exception.h"
#include "generic_table.h"
#include "sequence.h"
#include "value.h"

#include <optional>
#include <string>

using namespace Ishlang;

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeMakeOrderedMap) {
    auto env = Environment::make();

    auto ilit = [](Value::Long value) { return CodeNode::make<Literal>(Value(value)); };
    auto slit = [](const Value::Text &value) { return CodeNode::make<Literal>(Value(value)); };

    Value value = CodeNode::make<MakeOrderedMap>()->eval(env);
    TEST_CASE_MSG(value.isOrderedMap(), "actual=" << value.typeToString());
    TEST_CASE_MSG(value.orderedMap().size() == 0lu, "actual=" << value.orderedMap().size());

    value =
        CodeNode::make<MakeOrderedMap>(
            CodeNode::SharedPtrList(
                { CodeNode::make<MakeArray>(CodeNode::SharedPtrList({slit("one"), ilit(1)})),
                  CodeNode::make<MakeArray>(CodeNode::SharedPtrList({slit("two"), ilit(2)})),
                  CodeNode::make<MakeArray>(CodeNode::SharedPtrList({slit("three"), ilit(3)}))
                }))
        ->eval(env);
    TEST_CASE_MSG(value.isOrderedMap(), "actual=" << value.typeToString());
    TEST_CASE_MSG(value.orderedMap().size() == 3lu, "actual=" << value.orderedMap().size());
    TEST_CASE_MSG(value.orderedMap().get(Value("one")) == Value(1ll), "actual=" << value.orderedMap().get(Value("one")));
    TEST_CASE_MSG(value.orderedMap().get(Value("two")) == Value(2ll), "actual=" << value.orderedMap().get(Value("two")));
    TEST_CASE_MSG(value.orderedMap().get(Value("three")) == Value(3ll), "actual=" << value.orderedMap().get(Value("three")));

    value =
        CodeNode::make<MakeOrderedMap>(
            CodeNode::SharedPtrList(
                { CodeNode::make<MakePair>(slit("a"), ilit(10)),
                  CodeNode::make<MakePair>(slit("b"), ilit(20)),
                  CodeNode::make<MakePair>(slit("c"), ilit(30))
                }))
        ->eval(env);
    TEST_CASE_MSG(value.isOrderedMap(), "actual=" << value.typeToString());
    TEST_CASE_MSG(value.orderedMap().size() == 3lu, "actual=" << value.orderedMap().size());
    TEST_CASE_MSG(value.orderedMap().get(Value("a")) == Value(10ll), "actual=" << value.orderedMap().get(Value("a")));
    TEST_CASE_MSG(value.orderedMap().get(Value("b")) == Value(20ll), "actual=" << value.orderedMap().get(Value("b")));
    TEST_CASE_MSG(value.orderedMap().get(Value("c")) == Value(30ll), "actual=" << value.orderedMap().get(Value("c")));

    try {
        CodeNode::make<MakeOrderedMap>(CodeNode::SharedPtrList({ilit(5)}))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &ex) {
        TEST_CASE_MSG(std::string("Invalid operand type, expected=pair|array actual=int") == ex.what(), ex.what());
    }
    catch (...) {
        TEST_CASE(false);
    }

    try {
        CodeNode::make<MakeOrderedMap>(
            CodeNode::SharedPtrList(
                { CodeNode::make<MakeArray>(CodeNode::SharedPtrList({slit("one")})) }))
            ->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidExpression &ex) {
        TEST_CASE_MSG(std::string("Invalid expression - Wrong array size, expecting 2") == ex.what(), ex.what());
    }
    catch (...) {
        TEST_CASE(false);
    }

    try {
        CodeNode::make<MakeOrderedMap>(
            CodeNode::SharedPtrList(
                { CodeNode::make<MakeArray>(CodeNode::SharedPtrList({slit("one"), ilit(1), ilit(2)})) }))
            ->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidExpression &ex) {
        TEST_CASE_MSG(std::string("Invalid expression - Wrong array size, expecting 2") == ex.what(), ex.what());
    }
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeOrderedMapLen) {
    auto env = Environment::make();

    auto ot = [](int count) {
                  OrderedTable table;
                  if (count >= 1) { table.set(Value(1ll), Value(100ll)); }
                  if (count >= 2) { table.set(Value(2ll), Value(200ll)); }
                  if (count >= 3) { table.set(Value(3ll), Value(300ll)); }
                  return table;
              };

    Value value =
        CodeNode::make<OrderedMapLen>(
            CodeNode::make<Literal>(Value(ot(0))))
        ->eval(env);
    TEST_CASE_MSG(value == Value::Zero, "actual=" << value);

    value =
        CodeNode::make<OrderedMapLen>(
            CodeNode::make<Literal>(Value(ot(1))))
        ->eval(env);
    TEST_CASE_MSG(value == Value(1ll), "actual=" << value);

    value =
        CodeNode::make<OrderedMapLen>(
            CodeNode::make<Literal>(Value(ot(3))))
        ->eval(env);
    TEST_CASE_MSG(value == Value(3ll), "actual=" << value);

    try {
        value =
            CodeNode::make<OrderedMapLen>(
                CodeNode::make<Literal>(Value::Zero))
            ->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) { TEST_CASE(false); }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeOrderedMapContains) {
    auto env = Environment::make();

    const Value one("one");
    const Value two("two");
    const Value three("three");
    const Value four("four");

    auto otlit = [one, two, three](int count) {
                     OrderedTable table;
                     if (count >= 1) { table.set(one, Value(1ll)); }
                     if (count >= 2) { table.set(two, Value(2ll)); }
                     if (count >= 3) { table.set(three, Value(3ll)); }
                     return CodeNode::make<Literal>(Value(table));
                 };

    auto exists = [otlit, &env](int count, const Value &key) {
                      return CodeNode::make<OrderedMapContains>(otlit(count), CodeNode::make<Literal>(key))->eval(env);
                  };

    Value result;

    result = exists(0, one);   TEST_CASE_MSG(result == Value::False, "actual=" << result);
    result = exists(1, one);   TEST_CASE_MSG(result == Value::True,  "actual=" << result);
    result = exists(1, two);   TEST_CASE_MSG(result == Value::False, "actual=" << result);
    result = exists(2, one);   TEST_CASE_MSG(result == Value::True,  "actual=" << result);
    result = exists(2, two);   TEST_CASE_MSG(result == Value::True,  "actual=" << result);
    result = exists(2, three); TEST_CASE_MSG(result == Value::False, "actual=" << result);
    result = exists(3, one);   TEST_CASE_MSG(result == Value::True,  "actual=" << result);
    result = exists(3, two);   TEST_CASE_MSG(result == Value::True,  "actual=" << result);
    result = exists(3, three); TEST_CASE_MSG(result == Value::True,  "actual=" << result);
    result = exists(3, four);  TEST_CASE_MSG(result == Value::False, "actual=" << result);

    try {
        CodeNode::make<OrderedMapContains>(
            CodeNode::make<Literal>(one),
            CodeNode::make<Literal>(two))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}
