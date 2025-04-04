#include "unit_test_function.h"

#include "exception.h"
#include "instance.h"
#include "struct.h"

using namespace Ishlang;

// -------------------------------------------------------------
DEFINE_TEST(testStruct) {
    Struct s("Foo", {"mem1", "mem2", "mem3"});

    TEST_CASE_MSG(s.name() == "Foo", "actual=" << s.name());
    TEST_CASE_MSG(s.members().size() == 3, "actual=" << s.members().size());
    TEST_CASE_MSG(s.members()[0] == "mem1", "actual=" << s.members()[0]);
    TEST_CASE_MSG(s.members()[1] == "mem2", "actual=" << s.members()[1]);
    TEST_CASE_MSG(s.members()[2] == "mem3", "actual=" << s.members()[2]);
}

// -------------------------------------------------------------
DEFINE_TEST(testStructValue) {
    Value sValue(Struct("Person", {"name", "age"}));
    TEST_CASE_MSG(sValue.isUserType(), "actual=" << sValue.typeToString());

    {
        const Struct &s = sValue.userType();
        TEST_CASE_MSG(s.name() == "Person", "actual=" << s.name());
        TEST_CASE_MSG(s.members().size() == 2, "actual=" << s.members().size());
        TEST_CASE_MSG(s.members()[0] == "name", "actual=" << s.members()[0]);
        TEST_CASE_MSG(s.members()[1] == "age", "actual=" << s.members()[1]);
    }

    sValue = Value(Struct("Container", {"length", "width", "height"}));
    TEST_CASE_MSG(sValue.isUserType(), "actual=" << sValue.typeToString());

    {
        const Struct &s = sValue.userType();
        TEST_CASE_MSG(s.name() == "Container", "actual=" << s.name());
        TEST_CASE_MSG(s.members().size() == 3, "actual=" << s.members().size());
        TEST_CASE_MSG(s.members()[0] == "length", "actual=" << s.members()[0]);
        TEST_CASE_MSG(s.members()[1] == "width", "actual=" << s.members()[1]);
        TEST_CASE_MSG(s.members()[2] == "height", "actual=" << s.members()[2]);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testInstance) {
    Struct s("Person", {"name", "age"});
    Instance i(s);

    TEST_CASE_MSG(i.type() == s, "actual=" << i.type().name());
    TEST_CASE_MSG(i.get("name") == Value::Null, "actual=" << i.get("name"));
    TEST_CASE_MSG(i.get("age") == Value::Null, "actual=" << i.get("age"));

    i.set("name", Value("Jack"));
    i.set("age", Value(32ll));

    TEST_CASE_MSG(i.get("name") == Value("Jack"), "actual=" << i.get("name"));
    TEST_CASE_MSG(i.get("age") == Value(32ll), "actual=" << i.get("age"));

    Instance i2(s, {{"name", Value("Stan")}, {"age", Value(25ll)}});
    TEST_CASE_MSG(i2.get("name") == Value("Stan"), "actual=" << i2.get("name"));
    TEST_CASE_MSG(i2.get("age") == Value(25ll), "actual=" << i2.get("age"));

    Instance i3(s, {{"name", Value("Sara")}});
    TEST_CASE_MSG(i3.get("name") == Value("Sara"), "actual=" << i3.get("name"));
    TEST_CASE_MSG(i3.get("age") == Value::Null, "actual=" << i3.get("age"));
}

// -------------------------------------------------------------
DEFINE_TEST(testInstanceValue) {
    Struct s("Person", {"name", "age"});
    Instance is(s);
    Value iValue(is);
    TEST_CASE_MSG(iValue.isUserObject(), "actual=" << iValue.typeToString());

    try {
        const Instance &i = iValue.userObject();
        TEST_CASE_MSG(i.type() == s, "actual=" << i.type().name());
        TEST_CASE_MSG(i.type().name() == "Person", "actual=" << i.type().name());
        TEST_CASE_MSG(i.get("name") == Value::Null, "actual=" << i.get("name"));
        TEST_CASE_MSG(i.get("age") == Value::Null, "actual=" << i.get("age"));
    }
    catch (const Exception &ex) {
        TEST_CASE_MSG(false, "Caught exception: " << ex.what());
    }

    Struct s2("Container", {"length", "width", "height"});
    Instance is2(s2);
    iValue = Value(is2);
    TEST_CASE_MSG(iValue.isUserObject(), "actual=" << iValue.typeToString());

    try {
        const Instance &i = iValue.userObject();
        TEST_CASE_MSG(i.type() == s2, "actual=" << i.type().name());
        TEST_CASE_MSG(i.type().name() == "Container", "actual=" << i.type().name());
        TEST_CASE_MSG(i.get("length") == Value::Null, "actual=" << i.get("length"));
        TEST_CASE_MSG(i.get("width") == Value::Null, "actual=" << i.get("width"));
        TEST_CASE_MSG(i.get("height") == Value::Null, "actual=" << i.get("height"));
    }
    catch (const Exception &ex) {
        TEST_CASE_MSG(false, "Caught exception: " << ex.what());
    }
}
