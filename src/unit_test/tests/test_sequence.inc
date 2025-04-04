#include "unit_test_function.h"

#include "sequence.h"
#include "value.h"

#include <sstream>
#include <vector>

using namespace Ishlang;

// -------------------------------------------------------------
DEFINE_TEST(testSequence) {
    Sequence seq;

    TEST_CASE_MSG(seq.size() == 0lu, "actual=" << seq.size());

    seq.push(Value(1ll));
    TEST_CASE_MSG(seq.size() == 1lu, "actual=" << seq.size());
    TEST_CASE_MSG(seq.get(0lu) == Value(1ll), "actual=" << seq.get(0lu));

    seq.set(0lu, Value(2ll));
    TEST_CASE_MSG(seq.size() == 1lu, "actual=" << seq.size());
    TEST_CASE_MSG(seq.get(0lu) == Value(2ll), "actual=" << seq.get(0lu));

    seq.push(Value(3ll));
    TEST_CASE_MSG(seq.size() == 2lu, "actual=" << seq.size());
    TEST_CASE_MSG(seq.get(0lu) == Value(2ll), "actual=" << seq.get(0lu));
    TEST_CASE_MSG(seq.get(1lu) == Value(3ll), "actual=" << seq.get(1lu));

    seq.pop();
    TEST_CASE_MSG(seq.size() == 1lu, "actual=" << seq.size());
    TEST_CASE_MSG(seq.get(0lu) == Value(2ll), "actual=" << seq.get(0lu));

    seq.pop();
    TEST_CASE_MSG(seq.size() == 0lu, "actual=" << seq.size());

    Sequence seq2(std::vector({Value('a'), Value('b'), Value('c')}));
    TEST_CASE_MSG(seq2.size() == 3lu, "actual=" << seq2.size());
    TEST_CASE_MSG(seq2.get(0lu) == Value('a'), "actual=" << seq2.get(0lu));
    TEST_CASE_MSG(seq2.get(1lu) == Value('b'), "actual=" << seq2.get(1lu));
    TEST_CASE_MSG(seq2.get(2lu) == Value('c'), "actual=" << seq2.get(2lu));

    Sequence seq3(3, Value::Zero);
    TEST_CASE_MSG(seq3.size() == 3lu, "actual=" << seq3.size());
    TEST_CASE_MSG(seq3.get(0lu) == Value::Zero, "actual=" << seq3.get(0lu));
    TEST_CASE_MSG(seq3.get(1lu) == Value::Zero, "actual=" << seq3.get(1lu));
    TEST_CASE_MSG(seq3.get(2lu) == Value::Zero, "actual=" << seq3.get(2lu));
}

// -------------------------------------------------------------
DEFINE_TEST(testSequenceFind) {
    const Value a = Value('a');
    const Value b = Value('b');
    const Value c = Value('c');
    const Value d = Value('d');
    const Sequence seq({a, b, c, a, b, c});

    auto index = seq.find(a); TEST_CASE(index && *index == 0);
    index = seq.find(b); TEST_CASE(index && *index == 1);
    index = seq.find(c); TEST_CASE(index && *index == 2);
    index = seq.find(d); TEST_CASE(!index);

    index = seq.find(a, 1); TEST_CASE(index && *index == 3);
    index = seq.find(b, 2); TEST_CASE(index && *index == 4);
    index = seq.find(c, 3); TEST_CASE(index && *index == 5);
    index = seq.find(d, 3); TEST_CASE(!index);

    index = seq.find(a, 4); TEST_CASE(!index);
    index = seq.find(b, 5); TEST_CASE(!index);
    index = seq.find(c, 6); TEST_CASE(!index);
}

// -------------------------------------------------------------
DEFINE_TEST(testSequenceCount) {
    const Value v0 = Value::Zero;
    const Value v1 = Value(1ll);
    const Value v2 = Value(2ll);
    const Value v3 = Value(3ll);
    const Value v4 = Value(4ll);
    const Value v5 = Value(5ll);
    const Sequence seq({v0, v1, v2, v3, v0, v2, v4, v3, v0, v4, v4, v0});

    TEST_CASE_MSG(seq.count(v0) == 4, "actual=" << seq.count(v0));
    TEST_CASE_MSG(seq.count(v1) == 1, "actual=" << seq.count(v1));
    TEST_CASE_MSG(seq.count(v2) == 2, "actual=" << seq.count(v2));
    TEST_CASE_MSG(seq.count(v3) == 2, "actual=" << seq.count(v3));
    TEST_CASE_MSG(seq.count(v4) == 3, "actual=" << seq.count(v4));
    TEST_CASE_MSG(seq.count(v5) == 0, "actual=" << seq.count(v5));
}

// -------------------------------------------------------------
DEFINE_TEST(testSequenceSort) {
    const Value v0 = Value::Zero;
    const Value v1 = Value(1ll);
    const Value v2 = Value(2ll);
    const Value v3 = Value(3ll);
    const Value v4 = Value(4ll);
    const Value v5 = Value(5ll);

    Sequence seq({v4, v1, v0, v3, v5, v2});

    seq.sort(true);  TEST_CASE_MSG(seq == Sequence({v5, v4, v3, v2, v1, v0}), "actual=" << seq);
    seq.sort(false); TEST_CASE_MSG(seq == Sequence({v0, v1, v2, v3, v4, v5}), "actual=" << seq);
}

// -------------------------------------------------------------
DEFINE_TEST(testSequenceReverse) {
    const Value v0 = Value::Zero;
    const Value v1 = Value(1ll);
    const Value v2 = Value(2ll);
    const Value v3 = Value(3ll);
    const Value v4 = Value(4ll);
    const Value v5 = Value(5ll);

    Sequence seq({v0, v1, v2, v3, v4, v5});

    seq.reverse(); TEST_CASE_MSG(seq == Sequence({v5, v4, v3, v2, v1, v0}), "actual=" << seq);
    seq.reverse(); TEST_CASE_MSG(seq == Sequence({v0, v1, v2, v3, v4, v5}), "actual=" << seq);
}

// -------------------------------------------------------------
DEFINE_TEST(testSequenceInsert) {
    const Value v0 = Value::Zero;
    const Value v1 = Value(1ll);
    const Value v2 = Value(2ll);
    const Value v3 = Value(3ll);
    const Value v4 = Value(4ll);
    const Value v5 = Value(5ll);

    Sequence seq;

    seq.insert(0, v3); TEST_CASE_MSG(seq == Sequence({v3}), "actual=" << seq);
    seq.insert(0, v1); TEST_CASE_MSG(seq == Sequence({v1, v3}), "actual=" << seq);
    seq.insert(2, v5); TEST_CASE_MSG(seq == Sequence({v1, v3, v5}), "actual=" << seq);

    seq.insert(0, v0); TEST_CASE_MSG(seq == Sequence({v0, v1, v3, v5}), "actual=" << seq);
    seq.insert(2, v2); TEST_CASE_MSG(seq == Sequence({v0, v1, v2, v3, v5}), "actual=" << seq);
    seq.insert(4, v4); TEST_CASE_MSG(seq == Sequence({v0, v1, v2, v3, v4, v5}), "actual=" << seq);
}

// -------------------------------------------------------------
DEFINE_TEST(testSequenceErase) {
    const Value v0 = Value::Zero;
    const Value v1 = Value(1ll);
    const Value v2 = Value(2ll);
    const Value v3 = Value(3ll);
    const Value v4 = Value(4ll);
    const Value v5 = Value(5ll);

    Sequence seq({v0, v1, v2, v3, v4, v5});

    seq.erase(4); TEST_CASE_MSG(seq == Sequence({v0, v1, v2, v3, v5}), "actual=" << seq);
    seq.erase(2); TEST_CASE_MSG(seq == Sequence({v0, v1, v3, v5}), "actual=" << seq);
    seq.erase(0); TEST_CASE_MSG(seq == Sequence({v1, v3, v5}), "actual=" << seq);

    seq.erase(2); TEST_CASE_MSG(seq == Sequence({v1, v3}), "actual=" << seq);
    seq.erase(0); TEST_CASE_MSG(seq == Sequence({v3}), "actual=" << seq);
    seq.erase(0); TEST_CASE_MSG(seq == Sequence(), "actual=" << seq);
}

// -------------------------------------------------------------
DEFINE_TEST(testSequenceClear) {
    const Value v0 = Value::Zero;
    const Value v1 = Value(1ll);
    const Value v2 = Value(2ll);

    Sequence seq({v0, v1, v2});

    TEST_CASE_MSG(seq.size() == 3, "size actual=" << seq.size());
    TEST_CASE_MSG(seq == Sequence({v0, v1, v2}), "elements actual=" << seq);

    seq.clear();

    TEST_CASE_MSG(seq.size() == 0, "size actual=" << seq.size());
    TEST_CASE_MSG(seq == Sequence(), "elements actual=" << seq);
}

// -------------------------------------------------------------
DEFINE_TEST(testSequenceValue) {
    Sequence seq;
    seq.push(1ll);
    seq.push(2ll);

    try {
        Value sValue(seq);
        TEST_CASE_MSG(sValue.isArray(), "actual=" << sValue.typeToString());

        const Sequence &s = sValue.array();
        TEST_CASE_MSG(s.size() == 2lu, "actual=" << s.size());
        TEST_CASE_MSG(s.get(0lu) == Value(1ll), "actual=" << s.get(0lu));
        TEST_CASE_MSG(s.get(1lu) == Value(2ll), "actual=" << s.get(1lu));
    }
    catch (const Ishlang::Exception &ex) {
        TEST_CASE_MSG(false, "Caught exception: " << ex.what());
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testSequencePrint) {
    const Value v1(1ll);
    const Value v2(2ll);
    const Value v3(3ll);
    const Value v4(4ll);
    const Value v5(5ll);
    const Value v6(6ll);
    const Value v7(7ll);
    const Value v8(8ll);
    const Value v9(9ll);
    const Value v10(10ll);
    const Value v11(11ll);
    const Value v12(12ll);

    {
        std::ostringstream oss;
        oss << Sequence();
        TEST_CASE_MSG(oss.str() == "[]", "actual=" << oss.str());
    }

    {
        std::ostringstream oss;
        oss << Sequence({v1, v2, v3});
        TEST_CASE_MSG(oss.str() == "[1 2 3]", "actual=" << oss.str());
    }

    {
        std::ostringstream oss;
        oss << Sequence({v1, v2, v3, v4, v5, v6, v7, v8, v9, v10});
        TEST_CASE_MSG(oss.str() == "[1 2 3 4 5 6 7 8 9 10]", "actual=" << oss.str());
    }

    {
        std::ostringstream oss;
        oss << Sequence({v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11});
        TEST_CASE_MSG(oss.str() == "[1 2 3 4 5 6 7 8 9 10 ...]", "actual=" << oss.str());
    }

    {
        std::ostringstream oss;
        oss << Sequence({v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12});
        TEST_CASE_MSG(oss.str() == "[1 2 3 4 5 6 7 8 9 10 ...]", "actual=" << oss.str());
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testSequenceGenerate) {
    auto seq = Sequence::generate(0, []() { return Value::Zero; });
    TEST_CASE_MSG(seq.size() == 0, "actual=" << seq.size());

    seq = Sequence::generate(1, []() { return Value::Zero; });
    TEST_CASE_MSG(seq.size() == 1, "actual=" << seq.size());
    TEST_CASE_MSG(seq.get(0ul) == Value::Zero, "actual=" << seq.get(0ul));

    seq = Sequence::generate(5, [i = 1ll]() mutable { return Value(i++); });
    TEST_CASE_MSG(seq.size() == 5, "actual=" << seq.size());
    TEST_CASE_MSG(seq.get(0ul) == Value(1ll), "actual=" << seq.get(0ul));
    TEST_CASE_MSG(seq.get(1ul) == Value(2ll), "actual=" << seq.get(1ul));
    TEST_CASE_MSG(seq.get(2ul) == Value(3ll), "actual=" << seq.get(2ul));
    TEST_CASE_MSG(seq.get(3ul) == Value(4ll), "actual=" << seq.get(3ul));
    TEST_CASE_MSG(seq.get(4ul) == Value(5ll), "actual=" << seq.get(4ul));
}
