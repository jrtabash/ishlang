#include "unit_test_function.h"

#include "exception.h"
#include "file_io.h"
#include "hashtable.h"
#include "integer_range.h"
#include "sequence.h"
#include "util.h"
#include "value.h"
#include "value_pair.h"

#include <string>

using namespace Ishlang;

// -------------------------------------------------------------
DEFINE_TEST(testValue) {
    {
        struct Case {
            Value v;
            bool nFlag, iFlag, rFlag, cFlag, bFlag, numFlag;
            Value::Long l;
            Value::Double d;
            Value::Char c;
            Value::Bool b;
        } cases[] = { { Value(),     true,  false, false, false, false, false, 0, 0.0, '\0', false },
                      { Value(5ll),  false, true,  false, false, false, true,  5, 5.0, '\0', false },
                      { Value(1.2),  false, false, true,  false, false, true,  0, 1.2, '\0', false },
                      { Value('a'),  false, false, false, true,  false, false, 0, 0.0, 'a',  false },
                      { Value(true), false, false, false, false, true,  false, 0, 0.0, '\0', true  } };
        const size_t numCases = sizeof(cases)/sizeof(cases[0]);
        for (size_t i = 0; i < numCases; ++i) {
            Case &c = cases[i];
            TEST_CASE_MSG(c.v.isNull() == c.nFlag, c.v << " isNull? expect=" << c.nFlag << " actual=" << c.v.isNull());
            TEST_CASE_MSG(c.v.isInt()  == c.iFlag, c.v << " isInt? expect="  << c.iFlag << " actual=" << c.v.isInt());
            TEST_CASE_MSG(c.v.isReal() == c.rFlag, c.v << " isReal? expect=" << c.rFlag << " actual=" << c.v.isReal());
            TEST_CASE_MSG(c.v.isChar() == c.cFlag, c.v << " isChar? expect=" << c.cFlag << " actual=" << c.v.isChar());
            TEST_CASE_MSG(c.v.isBool() == c.bFlag, c.v << " isBool? expect=" << c.bFlag << " actual=" << c.v.isBool());

            TEST_CASE_MSG(c.v.isNumber() == c.numFlag, c.v << " isNum? expect=" << c.numFlag << " actual=" << c.v.isNumber());

            TEST_CASE_MSG(c.v.integer() == c.l,   c.v << " expect=" << c.l);
            TEST_CASE_MSG(c.v.real() == c.d,      c.v << " expect=" << c.d);
            TEST_CASE_MSG(c.v.character() == c.c, c.v << " expect=" << c.c);
            TEST_CASE_MSG(c.v.boolean() == c.b,   c.v << " expect=" << c.b);
        }
    }

    {
        const Value nVal1;
        const Value nVal2;
        const Value iVal1(2ll);
        const Value iVal2(2ll);
        const Value iVal3(3ll);
        const Value rVal1(2.5);
        const Value rVal2(2.5);
        const Value rVal3(5.5);
        const Value cVal1('a');
        const Value cVal2('a');
        const Value cVal3('b');
        const Value bVal1(true);
        const Value bVal2(true);
        const Value bVal3(false);

        TEST_CASE(nVal1 == nVal2);
        TEST_CASE(nVal1 != iVal1);
        TEST_CASE(nVal1 != rVal1);
        TEST_CASE(nVal1 != cVal1);
        TEST_CASE(nVal1 != bVal1);

        TEST_CASE(iVal1 != nVal1);
        TEST_CASE(iVal1 == iVal2);
        TEST_CASE(iVal1 != iVal3);
        TEST_CASE(iVal1 != rVal1);
        TEST_CASE(iVal1 != cVal1);
        TEST_CASE(iVal1 != bVal1);
        TEST_CASE(iVal1 <= iVal2);
        TEST_CASE(iVal1 < iVal3);
        TEST_CASE(iVal2 >= iVal1);
        TEST_CASE(iVal3 > iVal1);

        TEST_CASE(rVal1 != nVal1);
        TEST_CASE(rVal1 != iVal1);
        TEST_CASE(rVal1 == rVal2);
        TEST_CASE(rVal1 != rVal3);
        TEST_CASE(rVal1 != cVal1);
        TEST_CASE(rVal1 != bVal1);
        TEST_CASE(rVal1 <= rVal2);
        TEST_CASE(rVal1 < rVal3);
        TEST_CASE(rVal2 >= rVal1);
        TEST_CASE(rVal3 > rVal1);

        TEST_CASE(cVal1 != nVal1);
        TEST_CASE(cVal1 != iVal1);
        TEST_CASE(cVal1 != rVal1);
        TEST_CASE(cVal1 == cVal2);
        TEST_CASE(cVal1 != cVal3);
        TEST_CASE(cVal1 != bVal1);
        TEST_CASE(cVal1 <= cVal2);
        TEST_CASE(cVal1 < cVal3);
        TEST_CASE(cVal2 >= cVal1);
        TEST_CASE(cVal3 > cVal1);

        TEST_CASE(bVal1 != nVal1);
        TEST_CASE(bVal1 != iVal1);
        TEST_CASE(bVal1 != rVal1);
        TEST_CASE(bVal1 != cVal1);
        TEST_CASE(bVal1 == bVal2);
        TEST_CASE(bVal1 != bVal3);
        TEST_CASE(bVal1 <= bVal2);
        TEST_CASE(bVal3 < bVal1);
        TEST_CASE(bVal2 >= bVal1);
        TEST_CASE(bVal1 > bVal3);
    }

    {
        Value iVal1(1ll);
        Value iVal2(2ll);
        Value rVal1(1.0);
        Value rVal2(2.0);

        TEST_CASE(iVal1 == rVal1);
        TEST_CASE(iVal1 != rVal2);
        TEST_CASE(iVal1 < rVal2);
        TEST_CASE(iVal2 > rVal1);
        TEST_CASE(iVal1 <= rVal1);
        TEST_CASE(iVal1 >= rVal1);
    }

    {
        Value iVal1(1ll);
        Value iVal2(2ll);
        Value iVal4(4ll);
        Value p1(Value::Pair(iVal1, iVal2));
        Value p2(Value::Pair(iVal1, iVal2));
        Value p3(Value::Pair(iVal1, iVal4));

        TEST_CASE(p1.isPair());
        TEST_CASE(p2.isPair());
        TEST_CASE(p3.isPair());

        TEST_CASE(p1.pair().first() == iVal1);
        TEST_CASE(p1.pair().second() == iVal2);
        TEST_CASE(p2.pair().first() == iVal1);
        TEST_CASE(p2.pair().second() == iVal2);
        TEST_CASE(p3.pair().first() == iVal1);
        TEST_CASE(p3.pair().second() == iVal4);

        TEST_CASE(p1 == p2);
        TEST_CASE(p1 != p3);
        TEST_CASE(p1 < p3);
        TEST_CASE(p3 > p1);
        TEST_CASE(p1 <= p2);
        TEST_CASE(p1 >= p2);
    }

    {
        Value r1(IntegerRange(10));
        Value r2(IntegerRange(10));
        Value r3(IntegerRange(100));

        TEST_CASE(r1.isRange());
        TEST_CASE(r2.isRange());
        TEST_CASE(r3.isRange());

        TEST_CASE(r1 == r2);
        TEST_CASE(r1 != r3);
        TEST_CASE(r1 < r3);
        TEST_CASE(r2 <= r3);
        TEST_CASE(r3 > r2);
        TEST_CASE(r3 >= r1);
    }

    {
        Util::TemporaryFile tempFile1("test_file_value1.txt");
        Util::TemporaryFile tempFile2("test_file_value2.txt");

        Value f1({.filename=tempFile1.path(), .mode=FileMode::Read});
        Value f2({.filename=tempFile1.path(), .mode=FileMode::Read});
        Value f3({.filename=tempFile2.path(), .mode=FileMode::Read});

        TEST_CASE(f1.isFile());
        TEST_CASE(f2.isFile());
        TEST_CASE(f3.isFile());

        TEST_CASE(f1 == f2);
        TEST_CASE(f1 != f3);
    }

    {
        TEST_CASE(Value::stringToType("none")       == Value::eNone);
        TEST_CASE(Value::stringToType("int")        == Value::eInteger);
        TEST_CASE(Value::stringToType("real")       == Value::eReal);
        TEST_CASE(Value::stringToType("char")       == Value::eCharacter);
        TEST_CASE(Value::stringToType("bool")       == Value::eBoolean);
        TEST_CASE(Value::stringToType("pair")       == Value::ePair);
        TEST_CASE(Value::stringToType("string")     == Value::eString);
        TEST_CASE(Value::stringToType("closure")    == Value::eClosure);
        TEST_CASE(Value::stringToType("usertype")   == Value::eUserType);
        TEST_CASE(Value::stringToType("userobject") == Value::eUserObject);
        TEST_CASE(Value::stringToType("array")      == Value::eArray);
        TEST_CASE(Value::stringToType("hashmap")    == Value::eHashMap);
        TEST_CASE(Value::stringToType("range")      == Value::eRange);
        TEST_CASE(Value::stringToType("file")       == Value::eFile);

        try {
            Value::stringToType("foobar");
            TEST_CASE(false);
        }
        catch (const InvalidExpression &) {}
        catch (...) { TEST_CASE(false); }
    }

    {
        Value str1("foo");
        Value str2 = str1.clone();
        TEST_CASE(str1 == str2);

        auto & text1 = str1.text();
        auto & text2 = str2.text();
        TEST_CASE(text1 == text2);
        TEST_CASE(&text1 != &text2);
    }
}

// -------------------------------------------------------------
#define TEST_VALUE_ASTYPE(VALUE, ASFTN, ASVALUE)                        \
    TEST_CASE_MSG(VALUE.ASFTN == ASVALUE, "actual=" << VALUE.ASFTN)

#define TEST_ASTYPE_EXCEPT(VALUE, ASFTN, ASTYPE)                        \
    try {                                                               \
        VALUE.ASFTN;                                                    \
        TEST_CASE(false);                                               \
    }                                                                   \
    catch (InvalidAsType const & ex) {                                  \
        TEST_CASE_MSG(                                                  \
            ex.what() == (std::string("Invalid astype from '") + VALUE.typeToString() + "' to '" + ASTYPE + "'"), \
            ex.what());                                                 \
    }                                                                   \
    catch (...) { TEST_CASE(false); }

DEFINE_TEST(testValueAsType) {
    Value const null = Value::Null;
    Value const int0 = Value::Zero;
    Value const int1 = Value(1ll);
    Value const int2 = Value(2ll);
    Value const int25 = Value(25ll);
    Value const int99 = Value(99ll);
    Value const real0 = Value(0.0);
    Value const real1 = Value(1.0);
    Value const real2p5 = Value(2.5);
    Value const real25 = Value(25.0);
    Value const real25p72 = Value(25.72);
    Value const real99 = Value(99.0);
    Value const charC = Value('c');
    Value const charT = Value('t');
    Value const charF = Value('f');
    Value const char2 = Value('2');
    Value const charNull = Value('\0');
    Value const boolTrue = Value::True;
    Value const boolFalse = Value::False;
    Value const string25 = Value("25");
    Value const string25p72 = Value("25.72");
    Value const stringEmpty = Value("");
    Value const stringTrue = Value("true");
    Value const stringFalse = Value("false");
    Value const array1 = Value(Sequence(1, int1));
    Value const ht = Value(Hashtable());
    Value const rng = Value(IntegerRange());

    { // asInt
        TEST_VALUE_ASTYPE(int1,        asInt(), int1);
        TEST_VALUE_ASTYPE(real2p5,     asInt(), int2);
        TEST_VALUE_ASTYPE(charC,       asInt(), int99);
        TEST_VALUE_ASTYPE(boolTrue,    asInt(), int1);
        TEST_VALUE_ASTYPE(boolFalse,   asInt(), int0);
        TEST_VALUE_ASTYPE(string25,    asInt(), int25);
        TEST_VALUE_ASTYPE(string25p72, asInt(), int25);

        TEST_ASTYPE_EXCEPT(null,   asInt(), "int");
        TEST_ASTYPE_EXCEPT(array1, asInt(), "int");
        TEST_ASTYPE_EXCEPT(ht,     asInt(), "int");
        TEST_ASTYPE_EXCEPT(rng,    asInt(), "int");
    }

    { // asReal
        TEST_VALUE_ASTYPE(int1,        asReal(), real1);
        TEST_VALUE_ASTYPE(real2p5,     asReal(), real2p5);
        TEST_VALUE_ASTYPE(charC,       asReal(), real99);
        TEST_VALUE_ASTYPE(boolTrue,    asReal(), real1);
        TEST_VALUE_ASTYPE(boolFalse,   asReal(), real0);
        TEST_VALUE_ASTYPE(string25,    asReal(), real25);
        TEST_VALUE_ASTYPE(string25p72, asReal(), real25p72);

        TEST_ASTYPE_EXCEPT(null,   asReal(), "real");
        TEST_ASTYPE_EXCEPT(array1, asReal(), "real");
        TEST_ASTYPE_EXCEPT(ht,     asReal(), "real");
        TEST_ASTYPE_EXCEPT(rng,    asReal(), "real");
    }

    { // asChar
        TEST_VALUE_ASTYPE(int99,       asChar(), charC);
        TEST_VALUE_ASTYPE(real99,      asChar(), charC);
        TEST_VALUE_ASTYPE(charC,       asChar(), charC);
        TEST_VALUE_ASTYPE(boolTrue,    asChar(), charT);
        TEST_VALUE_ASTYPE(boolFalse,   asChar(), charF);
        TEST_VALUE_ASTYPE(string25,    asChar(), char2);
        TEST_VALUE_ASTYPE(stringEmpty, asChar(), charNull);

        TEST_ASTYPE_EXCEPT(null,   asChar(), "char");
        TEST_ASTYPE_EXCEPT(array1, asChar(), "char");
        TEST_ASTYPE_EXCEPT(ht,     asChar(), "char");
        TEST_ASTYPE_EXCEPT(rng,    asChar(), "char");
    }

    { // asBool
        TEST_VALUE_ASTYPE(int0,        asBool(), boolFalse);
        TEST_VALUE_ASTYPE(int1,        asBool(), boolTrue);
        TEST_VALUE_ASTYPE(real0,       asBool(), boolFalse);
        TEST_VALUE_ASTYPE(real1,       asBool(), boolTrue);
        TEST_VALUE_ASTYPE(charT,       asBool(), boolTrue);
        TEST_VALUE_ASTYPE(charF,       asBool(), boolFalse);
        TEST_VALUE_ASTYPE(boolTrue,    asBool(), boolTrue);
        TEST_VALUE_ASTYPE(boolFalse,   asBool(), boolFalse);
        TEST_VALUE_ASTYPE(stringTrue,  asBool(), boolTrue);
        TEST_VALUE_ASTYPE(stringFalse, asBool(), boolFalse);

        TEST_ASTYPE_EXCEPT(null,     asBool(), "bool");
        TEST_ASTYPE_EXCEPT(charC,    asBool(), "bool");
        TEST_ASTYPE_EXCEPT(string25, asBool(), "bool");
        TEST_ASTYPE_EXCEPT(array1,   asBool(), "bool");
        TEST_ASTYPE_EXCEPT(ht,       asBool(), "bool");
        TEST_ASTYPE_EXCEPT(rng,      asBool(), "bool");
    }

    { // asString
        TEST_VALUE_ASTYPE(int25,     asString(), string25);
        TEST_VALUE_ASTYPE(real25p72, asString(), Value("25.720000"));
        TEST_VALUE_ASTYPE(charC,     asString(), Value("c"));
        TEST_VALUE_ASTYPE(boolTrue,  asString(), stringTrue);
        TEST_VALUE_ASTYPE(boolFalse, asString(), stringFalse);
        TEST_VALUE_ASTYPE(string25,  asString(), string25);

        TEST_ASTYPE_EXCEPT(null,   asString(), "string");
        TEST_ASTYPE_EXCEPT(array1, asString(), "string");
        TEST_ASTYPE_EXCEPT(ht,     asString(), "string");
        TEST_ASTYPE_EXCEPT(rng,    asString(), "string");
    }

    { // asType
        TEST_VALUE_ASTYPE(int1,        asType(Value::eInteger), int1);
        TEST_VALUE_ASTYPE(real2p5,     asType(Value::eInteger), int2);
        TEST_VALUE_ASTYPE(charC,       asType(Value::eInteger), int99);
        TEST_VALUE_ASTYPE(boolTrue,    asType(Value::eInteger), int1);
        TEST_VALUE_ASTYPE(boolFalse,   asType(Value::eInteger), int0);
        TEST_VALUE_ASTYPE(string25,    asType(Value::eInteger), int25);
        TEST_VALUE_ASTYPE(string25p72, asType(Value::eInteger), int25);

        TEST_VALUE_ASTYPE(int1,        asType(Value::eReal), real1);
        TEST_VALUE_ASTYPE(real2p5,     asType(Value::eReal), real2p5);
        TEST_VALUE_ASTYPE(charC,       asType(Value::eReal), real99);
        TEST_VALUE_ASTYPE(boolTrue,    asType(Value::eReal), real1);
        TEST_VALUE_ASTYPE(boolFalse,   asType(Value::eReal), real0);
        TEST_VALUE_ASTYPE(string25,    asType(Value::eReal), real25);
        TEST_VALUE_ASTYPE(string25p72, asType(Value::eReal), real25p72);

        TEST_VALUE_ASTYPE(int99,       asType(Value::eCharacter), charC);
        TEST_VALUE_ASTYPE(real99,      asType(Value::eCharacter), charC);
        TEST_VALUE_ASTYPE(charC,       asType(Value::eCharacter), charC);
        TEST_VALUE_ASTYPE(boolTrue,    asType(Value::eCharacter), charT);
        TEST_VALUE_ASTYPE(boolFalse,   asType(Value::eCharacter), charF);
        TEST_VALUE_ASTYPE(string25,    asType(Value::eCharacter), char2);
        TEST_VALUE_ASTYPE(stringEmpty, asType(Value::eCharacter), charNull);

        TEST_VALUE_ASTYPE(int0,        asType(Value::eBoolean), boolFalse);
        TEST_VALUE_ASTYPE(int1,        asType(Value::eBoolean), boolTrue);
        TEST_VALUE_ASTYPE(real0,       asType(Value::eBoolean), boolFalse);
        TEST_VALUE_ASTYPE(real1,       asType(Value::eBoolean), boolTrue);
        TEST_VALUE_ASTYPE(charF,       asType(Value::eBoolean), boolFalse);
        TEST_VALUE_ASTYPE(charT,       asType(Value::eBoolean), boolTrue);
        TEST_VALUE_ASTYPE(boolFalse,   asType(Value::eBoolean), boolFalse);
        TEST_VALUE_ASTYPE(boolTrue,    asType(Value::eBoolean), boolTrue);
        TEST_VALUE_ASTYPE(stringFalse, asType(Value::eBoolean), boolFalse);
        TEST_VALUE_ASTYPE(stringTrue,  asType(Value::eBoolean), boolTrue);

        TEST_VALUE_ASTYPE(int25,     asType(Value::eString), string25);
        TEST_VALUE_ASTYPE(real25p72, asType(Value::eString), Value("25.720000"));
        TEST_VALUE_ASTYPE(charC,     asType(Value::eString), Value("c"));
        TEST_VALUE_ASTYPE(boolFalse, asType(Value::eString), stringFalse);
        TEST_VALUE_ASTYPE(boolTrue,  asType(Value::eString), stringTrue);
        TEST_VALUE_ASTYPE(string25,  asType(Value::eString), string25);

        TEST_ASTYPE_EXCEPT(null,        asType(Value::eInteger),   "int");
        TEST_ASTYPE_EXCEPT(null,        asType(Value::eReal),      "real");
        TEST_ASTYPE_EXCEPT(null,        asType(Value::eCharacter), "char");
        TEST_ASTYPE_EXCEPT(null,        asType(Value::eBoolean),   "bool");
        TEST_ASTYPE_EXCEPT(null,        asType(Value::eString),    "string");
        TEST_ASTYPE_EXCEPT(charC,       asType(Value::eBoolean),   "bool");
        TEST_ASTYPE_EXCEPT(stringEmpty, asType(Value::eBoolean),   "bool");
    }
}
#undef TEST_ASTYPE_EXCEPT
#undef TEST_VALUE_ASTYPE

// -------------------------------------------------------------
#define TEST_PAIR_EQUAL(VP, F, S) TEST_CASE(VP.first() == F && VP.second() == S)
DEFINE_TEST(testValuePair) {
    TEST_PAIR_EQUAL(ValuePair(), Value::Null, Value::Null);
    TEST_PAIR_EQUAL(ValuePair(Value('a'), Value('b')), Value('a'), Value('b'));
    TEST_PAIR_EQUAL(ValuePair(ValuePair::Pair(Value("a"), Value("b"))), Value("a"), Value("b"));

    const Value one(1ll);
    const Value two(2ll);
    const Value three(3ll);
    const Value four(4ll);

    TEST_CASE( (ValuePair(one, two) == ValuePair(one, two)) );
    TEST_CASE(!(ValuePair(one, two) == ValuePair(one, three)) );

    TEST_CASE( (ValuePair(one, two) != ValuePair(one, three)) );
    TEST_CASE(!(ValuePair(one, two) != ValuePair(one, two)) );

    TEST_CASE( (ValuePair(one, two) < ValuePair(two, one)) );
    TEST_CASE( (ValuePair(one, two) < ValuePair(one, three)) );
    TEST_CASE(!(ValuePair(two, four) < ValuePair(one, four)) );
    TEST_CASE(!(ValuePair(two, four) < ValuePair(two, three)) );

    TEST_CASE( (ValuePair(two, one) > ValuePair(one, two)) );
    TEST_CASE( (ValuePair(one, two) > ValuePair(one, one)) );
    TEST_CASE(!(ValuePair(two, four) > ValuePair(three, four)) );
    TEST_CASE(!(ValuePair(two, three) > ValuePair(two, four)) );

    TEST_CASE( (ValuePair(one, two) <= ValuePair(two, one)) );
    TEST_CASE( (ValuePair(one, two) <= ValuePair(one, three)) );
    TEST_CASE( (ValuePair(one, two) <= ValuePair(one, two)) );
    TEST_CASE(!(ValuePair(two, four) <= ValuePair(one, four)) );
    TEST_CASE(!(ValuePair(two, four) <= ValuePair(two, three)) );

    TEST_CASE( (ValuePair(two, one) >= ValuePair(one, two)) );
    TEST_CASE( (ValuePair(one, two) >= ValuePair(one, one)) );
    TEST_CASE( (ValuePair(one, two) >= ValuePair(one, two)) );
    TEST_CASE(!(ValuePair(two, four) >= ValuePair(three, four)) );
    TEST_CASE(!(ValuePair(two, three) >= ValuePair(two, four)) );
}
#undef TEST_PAIR_EQUAL

// -------------------------------------------------------------
#define TEST_RAW_TYPE_TO_CSTR(RAWTYPE, RAWCSTR) \
    TEST_CASE(std::string(RAWCSTR) == Value::rawTypeToCStr<RAWTYPE>())
DEFINE_TEST(testValueRawTypeToCStr) {
    TEST_RAW_TYPE_TO_CSTR(Value::Long, "int");
    TEST_RAW_TYPE_TO_CSTR(Value::Double, "real");
    TEST_RAW_TYPE_TO_CSTR(Value::Char, "char");
    TEST_RAW_TYPE_TO_CSTR(Value::Bool, "bool");
    TEST_RAW_TYPE_TO_CSTR(Value::Pair, "pair");
    TEST_RAW_TYPE_TO_CSTR(Value::Text, "string");
    TEST_RAW_TYPE_TO_CSTR(Value::Func, "closure");
    TEST_RAW_TYPE_TO_CSTR(Value::UserType, "usertype");
    TEST_RAW_TYPE_TO_CSTR(Value::UserObject, "userobject");
    TEST_RAW_TYPE_TO_CSTR(Value::Array, "array");
    TEST_RAW_TYPE_TO_CSTR(Value::HashMap, "hashmap");
    TEST_RAW_TYPE_TO_CSTR(Value::Range, "range");
    TEST_RAW_TYPE_TO_CSTR(Value::File, "file");
}
#undef TEST_RAW_TYPE_TO_CSTR
