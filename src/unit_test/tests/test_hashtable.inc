#include "hashtable.h"
#include "value.h"

using namespace Ishlang;

// -------------------------------------------------------------
DEFINE_TEST(testHashtable) {
    Hashtable ht;

    const auto one = Value("one");
    const auto two = Value("two");
    const auto three = Value("three");
    const auto four = Value("four");

    TEST_CASE_MSG(ht.size() == 0lu, "actual=" << ht.size());

    ht.set(one, Value(1ll));
    TEST_CASE_MSG(ht.size() == 1lu, "actual=" << ht.size());
    TEST_CASE_MSG(ht.get(one) == Value(1ll), "actual=" << ht.get(one));

    ht.set(two, Value(2ll));
    TEST_CASE_MSG(ht.size() == 2lu, "actual=" << ht.size());
    TEST_CASE_MSG(ht.get(two) == Value(2ll), "actual=" << ht.get(two));

    ht.set(three, Value(3ll));
    TEST_CASE_MSG(ht.size() == 3lu, "actual=" << ht.size());
    TEST_CASE_MSG(ht.get(three) == Value(3ll), "actual=" << ht.get(three));

    TEST_CASE_MSG(ht.get(four) == Value::Null, "actual=" << ht.get(four));
    TEST_CASE_MSG(ht.get(four, Value::Zero) == Value::Zero, "actual=" << ht.get(four, Value::Zero));

    TEST_CASE_MSG(ht.exists(one), "actual=" << ht.exists(one));
    TEST_CASE_MSG(ht.exists(two), "actual=" << ht.exists(two));
    TEST_CASE_MSG(ht.exists(three), "actual=" << ht.exists(three));
    TEST_CASE_MSG(!ht.exists(four), "actual=" << ht.exists(four));

    ht.remove(two);
    TEST_CASE_MSG(ht.size() == 2lu, "actual=" << ht.size());
    TEST_CASE_MSG(!ht.exists(two), "actual=" << ht.exists(two));

    ht.clear();
    TEST_CASE_MSG(ht.size() == 0lu, "actual=" << ht.size());
    TEST_CASE_MSG(!ht.exists(one), "actual=" << ht.exists(one));
    TEST_CASE_MSG(!ht.exists(three), "actual=" << ht.exists(three));
}

// -------------------------------------------------------------
DEFINE_TEST(testHashtableFind) {
    const auto kOne = Value("one");
    const auto kTwo = Value("two");
    const auto kThree = Value("three");

    const auto vOne = Value(1ll);
    const auto vTwo = Value(2ll);
    const auto vThree = Value(3ll);
    const auto vFour = Value(4ll);

    Hashtable ht;
    ht.set(kOne, vOne);
    ht.set(kTwo, vTwo);
    ht.set(kThree, vThree);

    Value key;

    key = ht.find(vOne); TEST_CASE_MSG(key == kOne, "actual=" << key);
    key = ht.find(vTwo); TEST_CASE_MSG(key == kTwo, "actual=" << key);
    key = ht.find(vThree); TEST_CASE_MSG(key == kThree, "actual=" << key);
    key = ht.find(vFour); TEST_CASE_MSG(key == Value::Null, "actual=" << key);
}

// -------------------------------------------------------------
DEFINE_TEST(testHashtableCount) {
    const auto vOne = Value(1ll);
    const auto vTwo = Value(2ll);
    const auto vThree = Value(3ll);
    const auto vFour = Value(4ll);

    Hashtable ht;
    ht.set(Value("one"), vOne);
    ht.set(Value("two"), vTwo);
    ht.set(Value("two2"), vTwo);
    ht.set(Value("three"), vThree);

    TEST_CASE_MSG(ht.count(vOne) == 1, "actual=" << ht.count(vOne));
    TEST_CASE_MSG(ht.count(vTwo) == 2, "actual=" << ht.count(vOne));
    TEST_CASE_MSG(ht.count(vThree) == 1, "actual=" << ht.count(vOne));
    TEST_CASE_MSG(ht.count(vFour) == 0, "actual=" << ht.count(vOne));
}

// -------------------------------------------------------------
DEFINE_TEST(testHashtableKeysValues) {
    const auto kOne = Value("one");
    const auto kTwo = Value("two");
    const auto kThree = Value("three");

    const auto vOne = Value(1ll);
    const auto vTwo = Value(2ll);
    const auto vThree = Value(3ll);

    const auto pOne = Value(Value::Pair(kOne, vOne));
    const auto pTwo = Value(Value::Pair(kTwo, vTwo));
    const auto pThree = Value(Value::Pair(kThree, vThree));

    Hashtable ht;
    TEST_CASE(ht.keys() == Sequence());
    TEST_CASE(ht.values() == Sequence());
    TEST_CASE(ht.items() == Sequence());

    ht.set(kOne, vOne);
    ht.set(kTwo, vTwo);
    ht.set(kThree, vThree);

    const Sequence keys = ht.keys();
    const Sequence values = ht.values();
    const Sequence items = ht.items();
    TEST_CASE_MSG(keys.size() == 3, "actual=" << keys.size());
    TEST_CASE_MSG(values.size() == 3, "actual=" << values.size());
    TEST_CASE_MSG(items.size() == 3, "actual=" << items.size());

    std::size_t result = 0;
    result = keys.count(kOne);   TEST_CASE_MSG(result == 1, "actual=" << result);
    result = keys.count(kTwo);   TEST_CASE_MSG(result == 1, "actual=" << result);
    result = keys.count(kThree); TEST_CASE_MSG(result == 1, "actual=" << result);

    result = values.count(vOne);   TEST_CASE_MSG(result == 1, "actual=" << result);
    result = values.count(vTwo);   TEST_CASE_MSG(result == 1, "actual=" << result);
    result = values.count(vThree); TEST_CASE_MSG(result == 1, "actual=" << result);

    result = items.count(pOne);   TEST_CASE_MSG(result == 1, "actual=" << result);
    result = items.count(pTwo);   TEST_CASE_MSG(result == 1, "actual=" << result);
    result = items.count(pThree); TEST_CASE_MSG(result == 1, "actual=" << result);

    for (std::size_t i = 0; i < keys.size(); ++i) {
        const auto & key = keys.get(i);
        const auto & value = values.get(i);
        const auto & pair = items.get(i).pair();

        TEST_CASE_MSG(ht.get(key) == value, "index=" << i << " key=" << key << " value=" << value);
        TEST_CASE_MSG(ht.get(pair.first()) == pair.second(), "index=" << i << " key=" << pair.first() << " value=" << pair.second());
    }
}
