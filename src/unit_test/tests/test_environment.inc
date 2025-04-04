#include "unit_test_function.h"

#include "environment.h"

#include <string>
#include <unordered_map>

using namespace Ishlang;

// -------------------------------------------------------------
DEFINE_TEST(testEnvironment) {
    auto env = Environment::make();
    TEST_CASE(env->empty());
    TEST_CASE(env->size() == 0);

    try {
        env->def("x", Value::Null);
        env->def("y", Value::Null);
    }
    catch (const DuplicateDef &ex) { TEST_CASE(false); }
    catch (...) { TEST_CASE(false); }

    TEST_CASE(!env->empty());
    TEST_CASE(env->size() == 2);
    TEST_CASE(env->exists("x"));
    TEST_CASE(env->exists("y"));
    TEST_CASE(!env->exists("z"));

    try {
        env->def("x", Value::Null);
        TEST_CASE(false);
    }
    catch (const DuplicateDef &ex) { TEST_CASE(true); }
    catch (...) { TEST_CASE(false); }

    try {
        auto childEnv = Environment::make(env);
        childEnv->def("x", Value::True);
    }
    catch (const DuplicateDef &ex) { TEST_CASE(false); }
    catch (...) { TEST_CASE(false); }

    try {
        env->get("nothing");
        TEST_CASE(false);
    }
    catch (const UnknownSymbol &ex) {}
    catch (...) { TEST_CASE(false); }

    try {
        TEST_CASE_MSG(env->get("x").isNull(), "actual=" << env->get("x"));
        TEST_CASE_MSG(env->get("y").isNull(), "actual=" << env->get("y"));
    }
    catch (const UnknownSymbol &ex) { TEST_CASE(false); }
    catch (...) { TEST_CASE(false); }

    try {
        env->set("x", Value(2ll));
        env->set("y", Value('c'));

        TEST_CASE_MSG(env->get("x").integer() == 2ll, "actual=" << env->get("x"));
        TEST_CASE_MSG(env->get("y").character() == 'c', "actual=" << env->get("y"));
    }
    catch (const UnknownSymbol &ex) { TEST_CASE(false); }
    catch (...) { TEST_CASE(false); }

    try {
        env->set("x", Value(24ll));

        TEST_CASE_MSG(env->get("x").integer() == 24ll, "actual=" << env->get("x"));
        TEST_CASE_MSG(env->get("y").character() == 'c', "actual=" << env->get("y"));
    }
    catch (const UnknownSymbol &ex) { TEST_CASE(false); }
    catch (...) { TEST_CASE(false); }

    auto child = Environment::make(env);

    try {
        child->get("nothing");
        TEST_CASE(false);
    }
    catch (const UnknownSymbol &ex) {}
    catch (...) { TEST_CASE(false); }

    try {
        child->def("z", Value(true));

        TEST_CASE_MSG(child->get("x").integer() == 24ll, "actual=" << child->get("x"));
        TEST_CASE_MSG(child->get("y").character() == 'c', "actual=" << child->get("y"));
        TEST_CASE_MSG(child->get("z").boolean() == true, "actual=" << child->get("z"));
    }
    catch (const DuplicateDef &ex) { TEST_CASE(false); }
    catch (...) { TEST_CASE(false); }

    try {
        child->set("z", Value(false));
        child->set("x", Value(30ll));

        TEST_CASE_MSG(child->get("x").integer() == 30ll, "actual=" << child->get("x"));
        TEST_CASE_MSG(child->get("y").character() == 'c', "actual=" << child->get("y"));
        TEST_CASE_MSG(child->get("z").boolean() == false, "actual=" << child->get("z"));
    }
    catch (const UnknownSymbol &ex) { TEST_CASE(false); }
    catch (...) { TEST_CASE(false); }

    try {
        TEST_CASE_MSG(env->get("x").integer() == 30ll, "actual=" << env->get("x"));
        TEST_CASE_MSG(env->get("y").character() == 'c', "actual=" << env->get("y"));
    }
    catch (const UnknownSymbol &ex) { TEST_CASE(false); }
    catch (...) { TEST_CASE(false); }

    try {
        child->def("x", Value('a'));

        TEST_CASE_MSG(child->get("x").character() == 'a', "actual=" << child->get("x"));
        TEST_CASE_MSG(child->get("x").integer() == 0ll, "actual=" << child->get("x"));
        TEST_CASE_MSG(child->get("y").character() == 'c', "actual=" << child->get("y"));
        TEST_CASE_MSG(child->get("z").boolean() == false, "actual=" << child->get("z"));
    }
    catch (const DuplicateDef &ex) { TEST_CASE(false); }
    catch (...) { TEST_CASE(false); }

    try {
        TEST_CASE_MSG(env->get("x").integer() == 30ll, "actual=" << env->get("x"));
        TEST_CASE_MSG(env->get("y").character() == 'c', "actual=" << env->get("y"));
    }
    catch (const UnknownSymbol &ex) { TEST_CASE(false); }
    catch (...) { TEST_CASE(false); }
}

// -------------------------------------------------------------
DEFINE_TEST(testEnvironmentForeach) {
    auto env = Environment::make();
    env->def("one", Value(1ll));
    env->def("two", Value(2ll));
    env->def("three", Value(3ll));

    unsigned count = 0;
    std::unordered_map<std::string, Value> nameValues{
        { "one", Value(1ll) },
        { "two", Value(2ll) },
        { "three", Value(3ll) } };

    env->foreach(
        [this, &count, &nameValues](const std::string &name, const Value &value) {
            ++count;

            const auto nameCount = nameValues.count(name);
            TEST_CASE_MSG(nameCount == 1, "actual=" << nameCount);

            if (nameCount > 0) {
                TEST_CASE_MSG(value == nameValues[name], "actual=" << value << " expected=" << nameValues[name]);
                nameValues.erase(name);
            }
        });

    TEST_CASE_MSG(nameValues.empty(), "actual size = " << nameValues.size());
    TEST_CASE_MSG(count == 3, "actual=" << count);
}
