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
        env->defByName("x", Value::Null);
        env->defByName("y", Value::Null);
    }
    catch (const DuplicateDef &ex) { TEST_CASE(false); }
    catch (...) { TEST_CASE(false); }

    TEST_CASE(!env->empty());
    TEST_CASE(env->size() == 2);
    TEST_CASE(env->exists("x"));
    TEST_CASE(env->exists("y"));
    TEST_CASE(!env->exists("z"));

    try {
        env->defByName("x", Value::Null);
        TEST_CASE(false);
    }
    catch (const DuplicateDef &ex) { TEST_CASE(true); }
    catch (...) { TEST_CASE(false); }

    try {
        auto childEnv = Environment::make(env);
        childEnv->defByName("x", Value::True);
    }
    catch (const DuplicateDef &ex) { TEST_CASE(false); }
    catch (...) { TEST_CASE(false); }

    try {
        env->getByName("nothing");
        TEST_CASE(false);
    }
    catch (const UnknownSymbol &ex) {}
    catch (...) { TEST_CASE(false); }

    try {
        TEST_CASE_MSG(env->getByName("x").isNull(), "actual=" << env->getByName("x"));
        TEST_CASE_MSG(env->getByName("y").isNull(), "actual=" << env->getByName("y"));
    }
    catch (const UnknownSymbol &ex) { TEST_CASE(false); }
    catch (...) { TEST_CASE(false); }

    try {
        env->setByName("x", Value(2ll));
        env->setByName("y", Value('c'));

        TEST_CASE_MSG(env->getByName("x").integer() == 2ll, "actual=" << env->getByName("x"));
        TEST_CASE_MSG(env->getByName("y").character() == 'c', "actual=" << env->getByName("y"));
    }
    catch (const UnknownSymbol &ex) { TEST_CASE(false); }
    catch (...) { TEST_CASE(false); }

    try {
        env->setByName("x", Value(24ll));

        TEST_CASE_MSG(env->getByName("x").integer() == 24ll, "actual=" << env->getByName("x"));
        TEST_CASE_MSG(env->getByName("y").character() == 'c', "actual=" << env->getByName("y"));
    }
    catch (const UnknownSymbol &ex) { TEST_CASE(false); }
    catch (...) { TEST_CASE(false); }

    auto child = Environment::make(env);

    try {
        child->getByName("nothing");
        TEST_CASE(false);
    }
    catch (const UnknownSymbol &ex) {}
    catch (...) { TEST_CASE(false); }

    try {
        child->defByName("z", Value(true));

        TEST_CASE_MSG(child->getByName("x").integer() == 24ll, "actual=" << child->getByName("x"));
        TEST_CASE_MSG(child->getByName("y").character() == 'c', "actual=" << child->getByName("y"));
        TEST_CASE_MSG(child->getByName("z").boolean() == true, "actual=" << child->getByName("z"));
    }
    catch (const DuplicateDef &ex) { TEST_CASE(false); }
    catch (...) { TEST_CASE(false); }

    try {
        child->setByName("z", Value(false));
        child->setByName("x", Value(30ll));

        TEST_CASE_MSG(child->getByName("x").integer() == 30ll, "actual=" << child->getByName("x"));
        TEST_CASE_MSG(child->getByName("y").character() == 'c', "actual=" << child->getByName("y"));
        TEST_CASE_MSG(child->getByName("z").boolean() == false, "actual=" << child->getByName("z"));
    }
    catch (const UnknownSymbol &ex) { TEST_CASE(false); }
    catch (...) { TEST_CASE(false); }

    try {
        TEST_CASE_MSG(env->getByName("x").integer() == 30ll, "actual=" << env->getByName("x"));
        TEST_CASE_MSG(env->getByName("y").character() == 'c', "actual=" << env->getByName("y"));
    }
    catch (const UnknownSymbol &ex) { TEST_CASE(false); }
    catch (...) { TEST_CASE(false); }

    try {
        child->defByName("x", Value('a'));

        TEST_CASE_MSG(child->getByName("x").character() == 'a', "actual=" << child->getByName("x"));
        TEST_CASE_MSG(child->getByName("x").integer() == 0ll, "actual=" << child->getByName("x"));
        TEST_CASE_MSG(child->getByName("y").character() == 'c', "actual=" << child->getByName("y"));
        TEST_CASE_MSG(child->getByName("z").boolean() == false, "actual=" << child->getByName("z"));
    }
    catch (const DuplicateDef &ex) { TEST_CASE(false); }
    catch (...) { TEST_CASE(false); }

    try {
        TEST_CASE_MSG(env->getByName("x").integer() == 30ll, "actual=" << env->getByName("x"));
        TEST_CASE_MSG(env->getByName("y").character() == 'c', "actual=" << env->getByName("y"));
    }
    catch (const UnknownSymbol &ex) { TEST_CASE(false); }
    catch (...) { TEST_CASE(false); }

    try {
        child->clear();
        TEST_CASE(child->size() == 0);
        TEST_CASE(env->size() > 0);
    }
    catch (...) { TEST_CASE(false); }
}

// -------------------------------------------------------------
DEFINE_TEST(testEnvironmentForeach) {
    auto env = Environment::make();
    env->defByName("one", Value(1ll));
    env->defByName("two", Value(2ll));
    env->defByName("three", Value(3ll));

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
