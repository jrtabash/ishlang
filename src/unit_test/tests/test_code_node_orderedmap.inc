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

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeOrderedMapGet) {
    auto env = Environment::make();

    const Value kOne("one");
    const Value kTwo("two");
    const Value kThree("three");
    const Value kFour("four");

    const Value vOne(1ll);
    const Value vTwo(2ll);
    const Value vThree(3ll);

    OrderedTable ot;
    ot.set(kOne, vOne);
    ot.set(kTwo, vTwo);
    ot.set(kThree, vThree);
    env->def("ot", Value(ot));

    auto get = [&env](const Value &key, const std::optional<Value> &defaultValue = std::nullopt) {
        return CodeNode::make<OrderedMapGet>(
            CodeNode::make<Variable>("ot"),
                CodeNode::make<Literal>(key),
                (defaultValue
                 ? CodeNode::make<Literal>(*defaultValue)
                 : CodeNode::SharedPtr()))->eval(env);
    };

    Value result;

    result = get(kOne);   TEST_CASE_MSG(result == vOne,        "actual=" << result);
    result = get(kTwo);   TEST_CASE_MSG(result == vTwo,        "actual=" << result);
    result = get(kThree); TEST_CASE_MSG(result == vThree,      "actual=" << result);
    result = get(kFour);  TEST_CASE_MSG(result == Value::Null, "actual=" << result);

    result = get(kOne,   Value::Zero); TEST_CASE_MSG(result == vOne,        "actual=" << result);
    result = get(kTwo,   Value::Zero); TEST_CASE_MSG(result == vTwo,        "actual=" << result);
    result = get(kThree, Value::Zero); TEST_CASE_MSG(result == vThree,      "actual=" << result);
    result = get(kFour,  Value::Zero); TEST_CASE_MSG(result == Value::Zero, "actual=" << result);

    try {
        CodeNode::make<OrderedMapGet>(CodeNode::make<Literal>(Value::Zero),
                                      CodeNode::make<Literal>(kOne))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeOrderedMapSet) {
    auto env = Environment::make();
    env->def("ot", Value(OrderedTable()));

    CodeNode::SharedPtr ot = CodeNode::make<Variable>("ot");

    auto clit = [](Value::Char c) { return CodeNode::make<Literal>(Value(c)); };
    auto ilit = [](Value::Long i) { return CodeNode::make<Literal>(Value(i)); };

    auto set = [&env, &ot, clit, ilit](Value::Char key, Value::Long val) {
        return CodeNode::make<OrderedMapSet>(ot, clit(key), ilit(val))->eval(env);
    };

    auto otget = [&env](Value::Char key) { return env->get("ot").orderedMap().get(Value(key)); };
    auto otlen = [&env]() { return env->get("ot").orderedMap().size(); };

    Value result = set('a', 1);
    TEST_CASE_MSG(result == Value(1ll), "actual=" << result);
    TEST_CASE_MSG(otget('a') == result, "actual=" << otget('a'));
    TEST_CASE_MSG(otlen() == 1, "actual=" << otlen());

    result = set('b', 2);
    TEST_CASE_MSG(result == Value(2ll), "actual=" << result);
    TEST_CASE_MSG(otget('b') == result, "actual=" << otget('b'));
    TEST_CASE_MSG(otlen() == 2, "actual=" << otlen());

    result = set('c', 3);
    TEST_CASE_MSG(result == Value(3ll), "actual=" << result);
    TEST_CASE_MSG(otget('c') == result, "actual=" << otget('c'));
    TEST_CASE_MSG(otlen() == 3, "actual=" << otlen());

    result = set('a', 100);
    TEST_CASE_MSG(result == Value(100ll), "actual=" << result);
    TEST_CASE_MSG(otget('a') == result, "actual=" << otget('a'));
    TEST_CASE_MSG(otlen() == 3, "actual=" << otlen());

    try {
        CodeNode::make<OrderedMapSet>(ilit(0), ilit(1), ilit(2))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeOrderedMapRemove) {
    auto env = Environment::make();

    {
        OrderedTable ot;
        ot.set(Value('a'), Value(1ll));
        ot.set(Value('b'), Value(2ll));
        ot.set(Value('c'), Value(3ll));
        env->def("ot", Value(ot));
    }

    CodeNode::SharedPtr ot = CodeNode::make<Variable>("ot");

    auto clit = [](Value::Char c) { return CodeNode::make<Literal>(Value(c)); };

    auto remove = [&env, &ot, clit](Value::Char key) {
        return CodeNode::make<OrderedMapRemove>(ot, clit(key))->eval(env);
    };

    auto otlen = [&env]() { return env->get("ot").orderedMap().size(); };
    auto othas = [&env](Value::Char key) { return env->get("ot").orderedMap().exists(Value(key)); };

    TEST_CASE_MSG(otlen() == 3, "actual=" << otlen());
    TEST_CASE(othas('a'));
    remove('a');
    TEST_CASE(!othas('a'));

    TEST_CASE_MSG(otlen() == 2, "actual=" << otlen());
    TEST_CASE(othas('b'));
    remove('b');
    TEST_CASE(!othas('b'));

    TEST_CASE_MSG(otlen() == 1, "actual=" << otlen());
    TEST_CASE(othas('c'));
    remove('c');
    TEST_CASE(!othas('c'));

    TEST_CASE_MSG(otlen() == 0, "actual=" << otlen());
    TEST_CASE(!othas('d'));
    remove('d');
    TEST_CASE(!othas('d'));

    TEST_CASE_MSG(otlen() == 0, "actual=" << otlen());

    try {
        CodeNode::make<OrderedMapRemove>(clit('a'), clit('b'))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeOrderedMapClear) {
    auto env = Environment::make();

    {
        OrderedTable ot;
        ot.set(Value('a'), Value(1ll));
        ot.set(Value('b'), Value(2ll));
        ot.set(Value('c'), Value(3ll));
        env->def("ot", Value(ot));
    }

    CodeNode::SharedPtr ot = CodeNode::make<Variable>("ot");

    auto clear = [&env, &ot]() {
        return CodeNode::make<OrderedMapClear>(ot)->eval(env);
    };

    auto otlen = [&env]() { return env->get("ot").orderedMap().size(); };
    auto othas = [&env](Value::Char key) { return env->get("ot").orderedMap().exists(Value(key)); };

    TEST_CASE(otlen() == 3);
    TEST_CASE(othas('a'));
    TEST_CASE(othas('b'));
    TEST_CASE(othas('c'));

    clear();

    TEST_CASE(otlen() == 0);
    TEST_CASE(!othas('a'));
    TEST_CASE(!othas('b'));
    TEST_CASE(!othas('c'));

    try {
        CodeNode::make<OrderedMapClear>(CodeNode::make<Literal>(Value('a')))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeOrderedMapFind) {
    auto env = Environment::make();

    {
        OrderedTable ot;
        ot.set(Value('a'), Value(1ll));
        ot.set(Value('b'), Value(2ll));
        ot.set(Value('c'), Value(3ll));
        env->def("ot", Value(ot));
    }

    CodeNode::SharedPtr ot = CodeNode::make<Variable>("ot");

    auto ilit = [](Value::Long i) { return CodeNode::make<Literal>(i); };

    auto find = [&env, &ot, ilit](Value::Long value) {
        return CodeNode::make<OrderedMapFind>(ot, ilit(value))->eval(env);
    };

    Value result;
    result = find(1); TEST_CASE_MSG(result == Value('a'),  "actual=" << result);
    result = find(2); TEST_CASE_MSG(result == Value('b'),  "actual=" << result);
    result = find(3); TEST_CASE_MSG(result == Value('c'),  "actual=" << result);
    result = find(4); TEST_CASE_MSG(result == Value::Null, "actual=" << result);

    try {
        CodeNode::make<OrderedMapFind>(ilit(1), ilit(2))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeOrderedMapCount) {
    auto env = Environment::make();

    {
        OrderedTable ot;
        ot.set(Value('a'), Value(1ll));
        ot.set(Value('b'), Value(2ll));
        ot.set(Value('c'), Value(3ll));
        ot.set(Value('2'), Value(2ll));
        env->def("ot", Value(ot));
    }

    CodeNode::SharedPtr ot = CodeNode::make<Variable>("ot");

    auto ilit = [](Value::Long i) { return CodeNode::make<Literal>(i); };

    auto count = [&env, &ot, ilit](Value::Long value) {
        return CodeNode::make<OrderedMapCount>(ot, ilit(value))->eval(env);
    };

    Value result;
    result = count(1); TEST_CASE_MSG(result == Value(1ll),  "actual=" << result);
    result = count(2); TEST_CASE_MSG(result == Value(2ll),  "actual=" << result);
    result = count(3); TEST_CASE_MSG(result == Value(1ll),  "actual=" << result);
    result = count(4); TEST_CASE_MSG(result == Value::Zero, "actual=" << result);

    try {
        CodeNode::make<OrderedMapCount>(ilit(1), ilit(2))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeOrderedMapKeys) {
    auto env = Environment::make();

    {
        OrderedTable ot;
        ot.set(Value('b'), Value(2ll));
        ot.set(Value('c'), Value(3ll));
        ot.set(Value('a'), Value(1ll));
        env->def("ot", Value(ot));
    }

    CodeNode::SharedPtr ot = CodeNode::make<Variable>("ot");

    auto keys = [&env, &ot]() {
        return CodeNode::make<OrderedMapKeys>(ot)->eval(env);
    };

    const Value ks = keys();
    TEST_CASE(ks.isArray());

    const Sequence &seq = ks.array();
    TEST_CASE_MSG(seq.size() == 3, "actual=" << seq.size());

    TEST_CASE_MSG(seq.get(0) == Value('a'), "actual=" << seq.get(0));
    TEST_CASE_MSG(seq.get(1) == Value('b'), "actual=" << seq.get(1));
    TEST_CASE_MSG(seq.get(2) == Value('c'), "actual=" << seq.get(2));

    try {
        CodeNode::make<OrderedMapKeys>(CodeNode::make<Literal>(Value('a')))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeOrderedMapValues) {
    auto env = Environment::make();

    {
        OrderedTable ot;
        ot.set(Value('c'), Value(3ll));
        ot.set(Value('a'), Value(1ll));
        ot.set(Value('2'), Value(2ll));
        ot.set(Value('b'), Value(2ll));
        env->def("ot", Value(ot));
    }

    CodeNode::SharedPtr ot = CodeNode::make<Variable>("ot");

    auto values = [&env, &ot]() {
        return CodeNode::make<OrderedMapValues>(ot)->eval(env);
    };

    const Value vs = values();
    TEST_CASE(vs.isArray());

    const Sequence &vals = vs.array();
    TEST_CASE_MSG(vals.size() == 4, "actual=" << vals.size());

    TEST_CASE_MSG(vals.get(0) == Value(2ll), "actual=" << vals.get(0));
    TEST_CASE_MSG(vals.get(1) == Value(1ll), "actual=" << vals.get(1));
    TEST_CASE_MSG(vals.get(2) == Value(2ll), "actual=" << vals.get(2));
    TEST_CASE_MSG(vals.get(3) == Value(3ll), "actual=" << vals.get(3));

    try {
        CodeNode::make<OrderedMapValues>(CodeNode::make<Literal>(Value(1ll)))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeOrderedMapItems) {
    auto env = Environment::make();

    {
        OrderedTable ot;
        ot.set(Value('c'), Value(3ll));
        ot.set(Value('a'), Value(1ll));
        ot.set(Value('2'), Value(2ll));
        ot.set(Value('b'), Value(2ll));
        env->def("ot", Value(ot));
    }

    CodeNode::SharedPtr ot = CodeNode::make<Variable>("ot");

    auto items = [&env, &ot]() {
        return CodeNode::make<OrderedMapItems>(ot)->eval(env);
    };

    const Value itms = items();
    TEST_CASE(itms.isArray());

    const Sequence &pairs = itms.array();
    TEST_CASE_MSG(pairs.size() == 4, "actual=" << pairs.size());

    // Keys
    TEST_CASE_MSG(pairs.get(0).pair().first() == Value('2'), "actual=" << pairs.get(0).pair().first());
    TEST_CASE_MSG(pairs.get(1).pair().first() == Value('a'), "actual=" << pairs.get(1).pair().first());
    TEST_CASE_MSG(pairs.get(2).pair().first() == Value('b'), "actual=" << pairs.get(2).pair().first());
    TEST_CASE_MSG(pairs.get(3).pair().first() == Value('c'), "actual=" << pairs.get(3).pair().first());

    // Values
    TEST_CASE_MSG(pairs.get(0).pair().second() == Value(2ll), "actual=" << pairs.get(0).pair().second());
    TEST_CASE_MSG(pairs.get(1).pair().second() == Value(1ll), "actual=" << pairs.get(1).pair().second());
    TEST_CASE_MSG(pairs.get(2).pair().second() == Value(2ll), "actual=" << pairs.get(2).pair().second());
    TEST_CASE_MSG(pairs.get(3).pair().second() == Value(3ll), "actual=" << pairs.get(3).pair().second());

    try {
        CodeNode::make<OrderedMapItems>(CodeNode::make<Literal>(Value(1ll)))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeOrderedMapReverseKeys) {
    auto env = Environment::make();

    {
        OrderedTable ot;
        ot.set(Value('b'), Value(2ll));
        ot.set(Value('c'), Value(3ll));
        ot.set(Value('a'), Value(1ll));
        env->def("ot", Value(ot));
    }

    CodeNode::SharedPtr ot = CodeNode::make<Variable>("ot");

    auto keys = [&env, &ot]() {
        return CodeNode::make<OrderedMapReverseKeys>(ot)->eval(env);
    };

    const Value ks = keys();
    TEST_CASE(ks.isArray());

    const Sequence &seq = ks.array();
    TEST_CASE_MSG(seq.size() == 3, "actual=" << seq.size());

    TEST_CASE_MSG(seq.get(0) == Value('c'), "actual=" << seq.get(0));
    TEST_CASE_MSG(seq.get(1) == Value('b'), "actual=" << seq.get(1));
    TEST_CASE_MSG(seq.get(2) == Value('a'), "actual=" << seq.get(2));

    try {
        CodeNode::make<OrderedMapReverseKeys>(CodeNode::make<Literal>(Value('a')))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeOrderedMapReverseValues) {
    auto env = Environment::make();

    {
        OrderedTable ot;
        ot.set(Value('c'), Value(3ll));
        ot.set(Value('a'), Value(1ll));
        ot.set(Value('2'), Value(2ll));
        ot.set(Value('b'), Value(2ll));
        env->def("ot", Value(ot));
    }

    CodeNode::SharedPtr ot = CodeNode::make<Variable>("ot");

    auto values = [&env, &ot]() {
        return CodeNode::make<OrderedMapReverseValues>(ot)->eval(env);
    };

    const Value vs = values();
    TEST_CASE(vs.isArray());

    const Sequence &vals = vs.array();
    TEST_CASE_MSG(vals.size() == 4, "actual=" << vals.size());

    TEST_CASE_MSG(vals.get(0) == Value(3ll), "actual=" << vals.get(0));
    TEST_CASE_MSG(vals.get(1) == Value(2ll), "actual=" << vals.get(1));
    TEST_CASE_MSG(vals.get(2) == Value(1ll), "actual=" << vals.get(2));
    TEST_CASE_MSG(vals.get(3) == Value(2ll), "actual=" << vals.get(3));

    try {
        CodeNode::make<OrderedMapReverseValues>(CodeNode::make<Literal>(Value(1ll)))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeOrderedMapReverseItems) {
    auto env = Environment::make();

    {
        OrderedTable ot;
        ot.set(Value('c'), Value(3ll));
        ot.set(Value('a'), Value(1ll));
        ot.set(Value('2'), Value(2ll));
        ot.set(Value('b'), Value(2ll));
        env->def("ot", Value(ot));
    }

    CodeNode::SharedPtr ot = CodeNode::make<Variable>("ot");

    auto items = [&env, &ot]() {
        return CodeNode::make<OrderedMapReverseItems>(ot)->eval(env);
    };

    const Value itms = items();
    TEST_CASE(itms.isArray());

    const Sequence &pairs = itms.array();
    TEST_CASE_MSG(pairs.size() == 4, "actual=" << pairs.size());

    // Keys
    TEST_CASE_MSG(pairs.get(0).pair().first() == Value('c'), "actual=" << pairs.get(0).pair().first());
    TEST_CASE_MSG(pairs.get(1).pair().first() == Value('b'), "actual=" << pairs.get(1).pair().first());
    TEST_CASE_MSG(pairs.get(2).pair().first() == Value('a'), "actual=" << pairs.get(2).pair().first());
    TEST_CASE_MSG(pairs.get(3).pair().first() == Value('2'), "actual=" << pairs.get(3).pair().first());

    // Values
    TEST_CASE_MSG(pairs.get(0).pair().second() == Value(3ll), "actual=" << pairs.get(0).pair().second());
    TEST_CASE_MSG(pairs.get(1).pair().second() == Value(2ll), "actual=" << pairs.get(1).pair().second());
    TEST_CASE_MSG(pairs.get(2).pair().second() == Value(1ll), "actual=" << pairs.get(2).pair().second());
    TEST_CASE_MSG(pairs.get(3).pair().second() == Value(2ll), "actual=" << pairs.get(3).pair().second());

    try {
        CodeNode::make<OrderedMapReverseItems>(CodeNode::make<Literal>(Value(1ll)))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}
