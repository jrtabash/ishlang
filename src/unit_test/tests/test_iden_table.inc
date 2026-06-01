#include "unit_test_function.h"

#include "iden_table.h"

using namespace Ishlang;

// -------------------------------------------------------------
DEFINE_TEST(testIdenTable) {
    auto table = IdenTable();

    TEST_CASE(table.empty());
    TEST_CASE(table.size() == 0);
    TEST_CASE(!table.exists("tst"));
    TEST_CASE(table.getName(1) == "");

    TEST_CASE(table.mapName("foo") == 1);
    TEST_CASE(table.mapName("foo") == 1);
    TEST_CASE(table.exists("foo"));
    TEST_CASE(table.getName(1) == "foo");

    TEST_CASE(!table.empty());
    TEST_CASE(table.size() == 1);

    TEST_CASE(table.mapName("bar") == 2);
    TEST_CASE(table.mapName("bar") == 2);
    TEST_CASE(table.exists("bar"));
    TEST_CASE(table.getName(2) == "bar");

    TEST_CASE(!table.empty());
    TEST_CASE(table.size() == 2);

    TEST_CASE(table.mapName("wahoo") == 3);
    TEST_CASE(table.mapName("wahoo") == 3);
    TEST_CASE(table.exists("wahoo"));
    TEST_CASE(table.getName(3) == "wahoo");

    TEST_CASE(!table.empty());
    TEST_CASE(table.size() == 3);

    table.clear();
    TEST_CASE(!table.exists("foo"));
    TEST_CASE(!table.exists("bar"));
    TEST_CASE(!table.exists("wahoo"));

    TEST_CASE(table.empty());
    TEST_CASE(table.size() == 0);
}
