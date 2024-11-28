#include "code_node.h"
#include "environment.h"
#include "exception.h"
#include "hashtable.h"
#include "sequence.h"
#include "value.h"

#include <optional>
#include <string>

using namespace Ishlang;

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeMakeHashMap) {
    auto env = Environment::make();

    auto ilit = [](Value::Long value) { return CodeNode::make<Literal>(Value(value)); };
    auto slit = [](const Value::Text &value) { return CodeNode::make<Literal>(Value(value)); };

    Value value = CodeNode::make<MakeHashMap>()->eval(env);
    TEST_CASE_MSG(value.isHashMap(), "actual=" << value.typeToString());
    TEST_CASE_MSG(value.hashMap().size() == 0lu, "actual=" << value.hashMap().size());

    value =
        CodeNode::make<MakeHashMap>(
            CodeNode::SharedPtrList(
                { CodeNode::make<MakeArray>(CodeNode::SharedPtrList({slit("one"), ilit(1)})),
                  CodeNode::make<MakeArray>(CodeNode::SharedPtrList({slit("two"), ilit(2)})),
                  CodeNode::make<MakeArray>(CodeNode::SharedPtrList({slit("three"), ilit(3)}))
                }))
        ->eval(env);
    TEST_CASE_MSG(value.isHashMap(), "actual=" << value.typeToString());
    TEST_CASE_MSG(value.hashMap().size() == 3lu, "actual=" << value.hashMap().size());
    TEST_CASE_MSG(value.hashMap().get(Value("one")) == Value(1ll), "actual=" << value.hashMap().get(Value("one")));
    TEST_CASE_MSG(value.hashMap().get(Value("two")) == Value(2ll), "actual=" << value.hashMap().get(Value("two")));
    TEST_CASE_MSG(value.hashMap().get(Value("three")) == Value(3ll), "actual=" << value.hashMap().get(Value("three")));

    value =
        CodeNode::make<MakeHashMap>(
            CodeNode::SharedPtrList(
                { CodeNode::make<MakePair>(slit("a"), ilit(10)),
                  CodeNode::make<MakePair>(slit("b"), ilit(20)),
                  CodeNode::make<MakePair>(slit("c"), ilit(30))
                }))
        ->eval(env);
    TEST_CASE_MSG(value.isHashMap(), "actual=" << value.typeToString());
    TEST_CASE_MSG(value.hashMap().size() == 3lu, "actual=" << value.hashMap().size());
    TEST_CASE_MSG(value.hashMap().get(Value("a")) == Value(10ll), "actual=" << value.hashMap().get(Value("a")));
    TEST_CASE_MSG(value.hashMap().get(Value("b")) == Value(20ll), "actual=" << value.hashMap().get(Value("b")));
    TEST_CASE_MSG(value.hashMap().get(Value("c")) == Value(30ll), "actual=" << value.hashMap().get(Value("c")));

    try {
        CodeNode::make<MakeHashMap>(CodeNode::SharedPtrList({ilit(5)}))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &ex) {
        TEST_CASE_MSG(std::string("Invalid operand type, expected=pair|array actual=int") == ex.what(), ex.what());
    }
    catch (...) {
        TEST_CASE(false);
    }

    try {
        CodeNode::make<MakeHashMap>(
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
        CodeNode::make<MakeHashMap>(
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
DEFINE_TEST(testCodeNodeHashMapLen) {
    auto env = Environment::make();

    auto ht = [](int count) {
                  Hashtable table;
                  if (count >= 1) { table.set(Value(1ll), Value(100ll)); }
                  if (count >= 2) { table.set(Value(2ll), Value(200ll)); }
                  if (count >= 3) { table.set(Value(3ll), Value(300ll)); }
                  return table;
              };

    Value value =
        CodeNode::make<HashMapLen>(
            CodeNode::make<Literal>(Value(ht(0))))
        ->eval(env);
    TEST_CASE_MSG(value == Value::Zero, "actual=" << value);

    value =
        CodeNode::make<HashMapLen>(
            CodeNode::make<Literal>(Value(ht(1))))
        ->eval(env);
    TEST_CASE_MSG(value == Value(1ll), "actual=" << value);

    value =
        CodeNode::make<HashMapLen>(
            CodeNode::make<Literal>(Value(ht(3))))
        ->eval(env);
    TEST_CASE_MSG(value == Value(3ll), "actual=" << value);

    try {
        value =
            CodeNode::make<HashMapLen>(
                CodeNode::make<Literal>(Value::Zero))
            ->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) { TEST_CASE(false); }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeHashMapContains) {
    auto env = Environment::make();

    const Value one("one");
    const Value two("two");
    const Value three("three");
    const Value four("four");

    auto htlit = [one, two, three](int count) {
                     Hashtable table;
                     if (count >= 1) { table.set(one, Value(1ll)); }
                     if (count >= 2) { table.set(two, Value(2ll)); }
                     if (count >= 3) { table.set(three, Value(3ll)); }
                     return CodeNode::make<Literal>(Value(table));
                 };

    auto exists = [htlit, &env](int count, const Value &key) {
                      return CodeNode::make<HashMapContains>(htlit(count), CodeNode::make<Literal>(key))->eval(env);
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
        CodeNode::make<HashMapContains>(
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
DEFINE_TEST(testCodeNodeHashMapGet) {
    auto env = Environment::make();

    const Value kOne("one");
    const Value kTwo("two");
    const Value kThree("three");
    const Value kFour("four");

    const Value vOne(1ll);
    const Value vTwo(2ll);
    const Value vThree(3ll);

    Hashtable ht;
    ht.set(kOne, vOne);
    ht.set(kTwo, vTwo);
    ht.set(kThree, vThree);
    env->def("ht", Value(ht));

    auto get = [&env](const Value &key, const std::optional<Value> &defaultValue = std::nullopt) {
        return CodeNode::make<HashMapGet>(CodeNode::make<Variable>("ht"),
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
        CodeNode::make<HashMapGet>(CodeNode::make<Literal>(Value::Zero),
                                   CodeNode::make<Literal>(kOne))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeHashMapSet) {
    auto env = Environment::make();
    env->def("ht", Value(Hashtable()));

    CodeNode::SharedPtr ht = CodeNode::make<Variable>("ht");

    auto clit = [](Value::Char c) { return CodeNode::make<Literal>(Value(c)); };
    auto ilit = [](Value::Long i) { return CodeNode::make<Literal>(Value(i)); };

    auto set = [&env, &ht, clit, ilit](Value::Char key, Value::Long val) {
                   return CodeNode::make<HashMapSet>(ht, clit(key), ilit(val))->eval(env);
               };

    auto htget = [&env](Value::Char key) { return env->get("ht").hashMap().get(Value(key)); };
    auto htlen = [&env]() { return env->get("ht").hashMap().size(); };

    Value result = set('a', 1);
    TEST_CASE_MSG(result == Value(1ll), "actual=" << result);
    TEST_CASE_MSG(htget('a') == result, "actual=" << htget('a'));
    TEST_CASE_MSG(htlen() == 1, "actual=" << htlen());

    result = set('b', 2);
    TEST_CASE_MSG(result == Value(2ll), "actual=" << result);
    TEST_CASE_MSG(htget('b') == result, "actual=" << htget('b'));
    TEST_CASE_MSG(htlen() == 2, "actual=" << htlen());

    result = set('c', 3);
    TEST_CASE_MSG(result == Value(3ll), "actual=" << result);
    TEST_CASE_MSG(htget('c') == result, "actual=" << htget('c'));
    TEST_CASE_MSG(htlen() == 3, "actual=" << htlen());

    result = set('a', 100);
    TEST_CASE_MSG(result == Value(100ll), "actual=" << result);
    TEST_CASE_MSG(htget('a') == result, "actual=" << htget('a'));
    TEST_CASE_MSG(htlen() == 3, "actual=" << htlen());

    try {
        CodeNode::make<HashMapSet>(ilit(0), ilit(1), ilit(2))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeHashMapRemove) {
    auto env = Environment::make();

    {
        Hashtable ht;
        ht.set(Value('a'), Value(1ll));
        ht.set(Value('b'), Value(2ll));
        ht.set(Value('c'), Value(3ll));
        env->def("ht", Value(ht));
    }

    CodeNode::SharedPtr ht = CodeNode::make<Variable>("ht");

    auto clit = [](Value::Char c) { return CodeNode::make<Literal>(Value(c)); };

    auto remove = [&env, &ht, clit](Value::Char key) {
                      return CodeNode::make<HashMapRemove>(ht, clit(key))->eval(env);
                  };

    auto htlen = [&env]() { return env->get("ht").hashMap().size(); };
    auto hthas = [&env](Value::Char key) { return env->get("ht").hashMap().exists(Value(key)); };

    TEST_CASE_MSG(htlen() == 3, "actual=" << htlen());
    TEST_CASE(hthas('a'));
    remove('a');
    TEST_CASE(!hthas('a'));

    TEST_CASE_MSG(htlen() == 2, "actual=" << htlen());
    TEST_CASE(hthas('b'));
    remove('b');
    TEST_CASE(!hthas('b'));

    TEST_CASE_MSG(htlen() == 1, "actual=" << htlen());
    TEST_CASE(hthas('c'));
    remove('c');
    TEST_CASE(!hthas('c'));

    TEST_CASE_MSG(htlen() == 0, "actual=" << htlen());
    TEST_CASE(!hthas('d'));
    remove('d');
    TEST_CASE(!hthas('d'));

    TEST_CASE_MSG(htlen() == 0, "actual=" << htlen());

    try {
        CodeNode::make<HashMapRemove>(clit('a'), clit('b'))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeHashMapClear) {
    auto env = Environment::make();

    {
        Hashtable ht;
        ht.set(Value('a'), Value(1ll));
        ht.set(Value('b'), Value(2ll));
        ht.set(Value('c'), Value(3ll));
        env->def("ht", Value(ht));
    }

    CodeNode::SharedPtr ht = CodeNode::make<Variable>("ht");

    auto clear = [&env, &ht]() {
                     return CodeNode::make<HashMapClear>(ht)->eval(env);
                 };

    auto htlen = [&env]() { return env->get("ht").hashMap().size(); };
    auto hthas = [&env](Value::Char key) { return env->get("ht").hashMap().exists(Value(key)); };

    TEST_CASE(htlen() == 3);
    TEST_CASE(hthas('a'));
    TEST_CASE(hthas('b'));
    TEST_CASE(hthas('c'));

    clear();

    TEST_CASE(htlen() == 0);
    TEST_CASE(!hthas('a'));
    TEST_CASE(!hthas('b'));
    TEST_CASE(!hthas('c'));

    try {
        CodeNode::make<HashMapClear>(CodeNode::make<Literal>(Value('a')))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeHashMapFind) {
    auto env = Environment::make();

    {
        Hashtable ht;
        ht.set(Value('a'), Value(1ll));
        ht.set(Value('b'), Value(2ll));
        ht.set(Value('c'), Value(3ll));
        env->def("ht", Value(ht));
    }

    CodeNode::SharedPtr ht = CodeNode::make<Variable>("ht");

    auto ilit = [](Value::Long i) { return CodeNode::make<Literal>(i); };

    auto find = [&env, &ht, ilit](Value::Long value) {
                    return CodeNode::make<HashMapFind>(ht, ilit(value))->eval(env);
                };

    Value result;
    result = find(1); TEST_CASE_MSG(result == Value('a'),  "actual=" << result);
    result = find(2); TEST_CASE_MSG(result == Value('b'),  "actual=" << result);
    result = find(3); TEST_CASE_MSG(result == Value('c'),  "actual=" << result);
    result = find(4); TEST_CASE_MSG(result == Value::Null, "actual=" << result);

    try {
        CodeNode::make<HashMapFind>(ilit(1), ilit(2))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeHashMapCount) {
    auto env = Environment::make();

    {
        Hashtable ht;
        ht.set(Value('a'), Value(1ll));
        ht.set(Value('b'), Value(2ll));
        ht.set(Value('c'), Value(3ll));
        ht.set(Value('2'), Value(2ll));
        env->def("ht", Value(ht));
    }

    CodeNode::SharedPtr ht = CodeNode::make<Variable>("ht");

    auto ilit = [](Value::Long i) { return CodeNode::make<Literal>(i); };

    auto count = [&env, &ht, ilit](Value::Long value) {
                     return CodeNode::make<HashMapCount>(ht, ilit(value))->eval(env);
                 };

    Value result;
    result = count(1); TEST_CASE_MSG(result == Value(1ll),  "actual=" << result);
    result = count(2); TEST_CASE_MSG(result == Value(2ll),  "actual=" << result);
    result = count(3); TEST_CASE_MSG(result == Value(1ll),  "actual=" << result);
    result = count(4); TEST_CASE_MSG(result == Value::Zero, "actual=" << result);

    try {
        CodeNode::make<HashMapCount>(ilit(1), ilit(2))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeHashMapKeys) {
    auto env = Environment::make();

    {
        Hashtable ht;
        ht.set(Value('a'), Value(1ll));
        ht.set(Value('b'), Value(2ll));
        ht.set(Value('c'), Value(3ll));
        env->def("ht", Value(ht));
    }

    CodeNode::SharedPtr ht = CodeNode::make<Variable>("ht");

    auto keys = [&env, &ht]() {
                    return CodeNode::make<HashMapKeys>(ht)->eval(env);
                };

    const Value ks = keys();
    TEST_CASE(ks.isArray());

    const Sequence &seq = ks.array();
    TEST_CASE_MSG(seq.size() == 3, "actual=" << seq.size());

    std::size_t cnt = 0;
    cnt = seq.count(Value('a')); TEST_CASE_MSG(cnt == 1, "actual=" << cnt);
    cnt = seq.count(Value('b')); TEST_CASE_MSG(cnt == 1, "actual=" << cnt);
    cnt = seq.count(Value('c')); TEST_CASE_MSG(cnt == 1, "actual=" << cnt);

    try {
        CodeNode::make<HashMapKeys>(CodeNode::make<Literal>(Value('a')))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeHashMapValues) {
    auto env = Environment::make();

    {
        Hashtable ht;
        ht.set(Value('a'), Value(1ll));
        ht.set(Value('b'), Value(2ll));
        ht.set(Value('c'), Value(3ll));
        ht.set(Value('2'), Value(2ll));
        env->def("ht", Value(ht));
    }

    CodeNode::SharedPtr ht = CodeNode::make<Variable>("ht");

    auto values = [&env, &ht]() {
                      return CodeNode::make<HashMapValues>(ht)->eval(env);
                  };

    const Value vs = values();
    TEST_CASE(vs.isArray());

    const Sequence &vals = vs.array();
    TEST_CASE_MSG(vals.size() == 4, "actual=" << vals.size());

    std::size_t cnt = 0;
    cnt = vals.count(Value(1ll)); TEST_CASE_MSG(cnt == 1, "actual=" << cnt);
    cnt = vals.count(Value(2ll)); TEST_CASE_MSG(cnt == 2, "actual=" << cnt);
    cnt = vals.count(Value(3ll)); TEST_CASE_MSG(cnt == 1, "actual=" << cnt);

    try {
        CodeNode::make<HashMapValues>(CodeNode::make<Literal>(Value(1ll)))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeHashMapItems) {
    auto env = Environment::make();

    {
        Hashtable ht;
        ht.set(Value('a'), Value(1ll));
        ht.set(Value('b'), Value(2ll));
        ht.set(Value('c'), Value(3ll));
        ht.set(Value('2'), Value(2ll));
        env->def("ht", Value(ht));
    }

    CodeNode::SharedPtr ht = CodeNode::make<Variable>("ht");

    auto items = [&env, &ht]() {
                     return CodeNode::make<HashMapItems>(ht)->eval(env);
                 };

    const Value itms = items();
    TEST_CASE(itms.isArray());

    const Sequence &pairs = itms.array();
    TEST_CASE_MSG(pairs.size() == 4, "actual=" << pairs.size());

    std::size_t cnt = 0;
    cnt = pairs.count(Value(Value::Pair(Value('a'), Value(1ll)))); TEST_CASE_MSG(cnt == 1, "actual=" << cnt);
    cnt = pairs.count(Value(Value::Pair(Value('b'), Value(2ll)))); TEST_CASE_MSG(cnt == 1, "actual=" << cnt);
    cnt = pairs.count(Value(Value::Pair(Value('c'), Value(3ll)))); TEST_CASE_MSG(cnt == 1, "actual=" << cnt);
    cnt = pairs.count(Value(Value::Pair(Value('2'), Value(2ll)))); TEST_CASE_MSG(cnt == 1, "actual=" << cnt);

    try {
        CodeNode::make<HashMapItems>(CodeNode::make<Literal>(Value(1ll)))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}
