#include "unit_test_function.h"

#include "generic_table.h"
#include "value.h"

using namespace Ishlang;

// -------------------------------------------------------------
DEFINE_TEST(testOrderedTable) {
    OrderedTable ot;

    const auto one = Value("one");
    const auto two = Value("two");
    const auto three = Value("three");
    const auto four = Value("four");

    TEST_CASE_MSG(ot.size() == 0lu, "actual=" << ot.size());

    ot.set(one, Value(1ll));
    TEST_CASE_MSG(ot.size() == 1lu, "actual=" << ot.size());
    TEST_CASE_MSG(ot.get(one) == Value(1ll), "actual=" << ot.get(one));

    ot.set(two, Value(2ll));
    TEST_CASE_MSG(ot.size() == 2lu, "actual=" << ot.size());
    TEST_CASE_MSG(ot.get(two) == Value(2ll), "actual=" << ot.get(two));

    ot.set(three, Value(3ll));
    TEST_CASE_MSG(ot.size() == 3lu, "actual=" << ot.size());
    TEST_CASE_MSG(ot.get(three) == Value(3ll), "actual=" << ot.get(three));

    TEST_CASE_MSG(ot.get(four) == Value::Null, "actual=" << ot.get(four));
    TEST_CASE_MSG(ot.get(four, Value::Zero) == Value::Zero, "actual=" << ot.get(four, Value::Zero));

    TEST_CASE_MSG(ot.exists(one), "actual=" << ot.exists(one));
    TEST_CASE_MSG(ot.exists(two), "actual=" << ot.exists(two));
    TEST_CASE_MSG(ot.exists(three), "actual=" << ot.exists(three));
    TEST_CASE_MSG(!ot.exists(four), "actual=" << ot.exists(four));

    ot.remove(two);
    TEST_CASE_MSG(ot.size() == 2lu, "actual=" << ot.size());
    TEST_CASE_MSG(!ot.exists(two), "actual=" << ot.exists(two));

    ot.clear();
    TEST_CASE_MSG(ot.size() == 0lu, "actual=" << ot.size());
    TEST_CASE_MSG(!ot.exists(one), "actual=" << ot.exists(one));
    TEST_CASE_MSG(!ot.exists(three), "actual=" << ot.exists(three));
}

// -------------------------------------------------------------
DEFINE_TEST(testOrderedTableFind) {
    const auto kOne = Value("one");
    const auto kTwo = Value("two");
    const auto kThree = Value("three");

    const auto vOne = Value(1ll);
    const auto vTwo = Value(2ll);
    const auto vThree = Value(3ll);
    const auto vFour = Value(4ll);

    OrderedTable ot;
    ot.set(kOne, vOne);
    ot.set(kTwo, vTwo);
    ot.set(kThree, vThree);

    Value key;

    key = ot.find(vOne); TEST_CASE_MSG(key == kOne, "actual=" << key);
    key = ot.find(vTwo); TEST_CASE_MSG(key == kTwo, "actual=" << key);
    key = ot.find(vThree); TEST_CASE_MSG(key == kThree, "actual=" << key);
    key = ot.find(vFour); TEST_CASE_MSG(key == Value::Null, "actual=" << key);
}

// -------------------------------------------------------------
DEFINE_TEST(testOrderedTableCount) {
    const auto vOne = Value(1ll);
    const auto vTwo = Value(2ll);
    const auto vThree = Value(3ll);
    const auto vFour = Value(4ll);

    OrderedTable ot;
    ot.set(Value("one"), vOne);
    ot.set(Value("two"), vTwo);
    ot.set(Value("two2"), vTwo);
    ot.set(Value("three"), vThree);

    TEST_CASE_MSG(ot.count(vOne) == 1, "actual=" << ot.count(vOne));
    TEST_CASE_MSG(ot.count(vTwo) == 2, "actual=" << ot.count(vOne));
    TEST_CASE_MSG(ot.count(vThree) == 1, "actual=" << ot.count(vOne));
    TEST_CASE_MSG(ot.count(vFour) == 0, "actual=" << ot.count(vOne));
}

// -------------------------------------------------------------
DEFINE_TEST(testOrderedTableKeysValues) {
    const auto kOne = Value("111");
    const auto kTwo = Value("222");
    const auto kThree = Value("333");

    const auto vOne = Value(1ll);
    const auto vTwo = Value(2ll);
    const auto vThree = Value(3ll);

    const auto pOne = Value(Value::Pair(kOne, vOne));
    const auto pTwo = Value(Value::Pair(kTwo, vTwo));
    const auto pThree = Value(Value::Pair(kThree, vThree));

    OrderedTable ot;
    TEST_CASE(ot.keys() == Sequence());
    TEST_CASE(ot.values() == Sequence());
    TEST_CASE(ot.items() == Sequence());

    ot.set(kOne, vOne);
    ot.set(kTwo, vTwo);
    ot.set(kThree, vThree);

    const Sequence keys = ot.keys();
    const Sequence values = ot.values();
    const Sequence items = ot.items();

    TEST_CASE_MSG(keys.size() == 3, "actual=" << keys.size());
    TEST_CASE_MSG(values.size() == 3, "actual=" << values.size());
    TEST_CASE_MSG(items.size() == 3, "actual=" << items.size());

    TEST_CASE_MSG(keys.get(0) == kOne, "actual=" << keys.get(0));
    TEST_CASE_MSG(keys.get(1) == kTwo, "actual=" << keys.get(1));
    TEST_CASE_MSG(keys.get(2) == kThree, "actual=" << keys.get(2));

    TEST_CASE_MSG(values.get(0) == vOne, "actual=" << values.get(0));
    TEST_CASE_MSG(values.get(1) == vTwo, "actual=" << values.get(1));
    TEST_CASE_MSG(values.get(2) == vThree, "actual=" << values.get(2));

    TEST_CASE_MSG(items.get(0) == pOne, "actual=" << items.get(0));
    TEST_CASE_MSG(items.get(1) == pTwo, "actual=" << items.get(1));
    TEST_CASE_MSG(items.get(2) == pThree, "actual=" << items.get(2));
}

// -------------------------------------------------------------
DEFINE_TEST(testOrderedTableReverseKeysValues) {
    const auto kOne = Value("1");
    const auto kTwo = Value("2");
    const auto kThree = Value("3");

    const auto vOne = Value(100ll);
    const auto vTwo = Value(200ll);
    const auto vThree = Value(300ll);

    const auto pOne = Value(Value::Pair(kOne, vOne));
    const auto pTwo = Value(Value::Pair(kTwo, vTwo));
    const auto pThree = Value(Value::Pair(kThree, vThree));

    OrderedTable ot;
    TEST_CASE(ot.reverseKeys() == Sequence());
    TEST_CASE(ot.reverseValues() == Sequence());
    TEST_CASE(ot.reverseItems() == Sequence());

    ot.set(kOne, vOne);
    ot.set(kTwo, vTwo);
    ot.set(kThree, vThree);

    const Sequence keys = ot.reverseKeys();
    const Sequence values = ot.reverseValues();
    const Sequence items = ot.reverseItems();

    TEST_CASE_MSG(keys.size() == 3, "actual=" << keys.size());
    TEST_CASE_MSG(values.size() == 3, "actual=" << values.size());
    TEST_CASE_MSG(items.size() == 3, "actual=" << items.size());

    TEST_CASE_MSG(keys.get(0) == kThree, "actual=" << keys.get(0));
    TEST_CASE_MSG(keys.get(1) == kTwo, "actual=" << keys.get(1));
    TEST_CASE_MSG(keys.get(2) == kOne, "actual=" << keys.get(2));

    TEST_CASE_MSG(values.get(0) == vThree, "actual=" << values.get(0));
    TEST_CASE_MSG(values.get(1) == vTwo, "actual=" << values.get(1));
    TEST_CASE_MSG(values.get(2) == vOne, "actual=" << values.get(2));

    TEST_CASE_MSG(items.get(0) == pThree, "actual=" << items.get(0));
    TEST_CASE_MSG(items.get(1) == pTwo, "actual=" << items.get(1));
    TEST_CASE_MSG(items.get(2) == pOne, "actual=" << items.get(2));
}

// -------------------------------------------------------------
DEFINE_TEST(testOrderedTableOrdering) {
    const auto kOne = Value(1ll);
    const auto kTwo = Value(2ll);
    const auto kThree = Value(3ll);
    const auto kFour = Value(4ll);
    const auto kFive = Value(5ll);

    const auto vOne = Value(100ll);
    const auto vTwo = Value(200ll);
    const auto vThree = Value(300ll);
    const auto vFour = Value(400ll);
    const auto vFive = Value(500ll);

    OrderedTable ot;
    ot.set(kTwo, vTwo);
    ot.set(kFour, vFour);
    ot.set(kFive, vFive);
    ot.set(kThree, vThree);
    ot.set(kOne, vOne);

    TEST_CASE_MSG(ot.size() == 5, "size=" << ot.size());

    auto iter = ot.begin();
    TEST_CASE_MSG(iter->first == kOne, " first key=" << iter->first);
    TEST_CASE_MSG(iter->second == vOne, " first value=" << iter->second);

    ++iter;
    TEST_CASE_MSG(iter->first == kTwo, " second key=" << iter->first);
    TEST_CASE_MSG(iter->second == vTwo, " second value=" << iter->second);

    ++iter;
    TEST_CASE_MSG(iter->first == kThree, " third key=" << iter->first);
    TEST_CASE_MSG(iter->second == vThree, " third value=" << iter->second);

    ++iter;
    TEST_CASE_MSG(iter->first == kFour, " fourth key=" << iter->first);
    TEST_CASE_MSG(iter->second == vFour, " fourth value=" << iter->second);

    ++iter;
    TEST_CASE_MSG(iter->first == kFive, " fifth key=" << iter->first);
    TEST_CASE_MSG(iter->second == vFive, " fifth value=" << iter->second);

    ++iter;
    TEST_CASE(iter == ot.end());
}
