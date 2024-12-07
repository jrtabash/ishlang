#include "code_node_util.h"

using namespace Ishlang;

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeUtil_anyOfType) {
    TEST_CASE(anyOfType(Value::eInteger, Value::eInteger));
    TEST_CASE(anyOfType(Value::eInteger, Value::eReal, Value::eInteger));
    TEST_CASE(anyOfType(Value::eInteger, Value::eReal, Value::eInteger, Value::eBoolean));

    TEST_CASE(!anyOfType(Value::eInteger, Value::eReal));
    TEST_CASE(!anyOfType(Value::eInteger, Value::eReal, Value::eBoolean));
    TEST_CASE(!anyOfType(Value::eInteger, Value::eReal, Value::eBoolean, Value::eString));
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeUtil_typesToString) {
    TEST_CASE(typesToString(Value::eInteger) == "int");
    TEST_CASE(typesToString(Value::eInteger, Value::eReal) == "int|real");
    TEST_CASE(typesToString(Value::eInteger, Value::eReal, Value::eBoolean) == "int|real|bool");
}
