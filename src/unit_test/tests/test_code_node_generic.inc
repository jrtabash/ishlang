#include "unit_test_function.h"

#include "code_node.h"
#include "environment.h"
#include "exception.h"
#include "hashtable.h"
#include "instance.h"
#include "integer_range.h"
#include "lambda.h"
#include "sequence.h"
#include "struct.h"
#include "value.h"
#include "value_pair.h"

#include <optional>
#include <string>
#include <vector>

using namespace Ishlang;

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeGenericLen) {
    auto env = Environment::make();

    auto len = [](auto rawVal) {
        return CodeNode::make<GenericLen>(CodeNode::make<Literal>(Value(rawVal)));
    };

    auto val = len("hello")->eval(env);
    TEST_CASE_MSG(val.isInt(), "actual=" << Value::typeToString(val.type()));
    TEST_CASE_MSG(val.integer() == 5, "actual=" << val);

    val = len(Sequence(std::vector{Value(1ll), Value(2ll), Value(3ll)}))->eval(env);
    TEST_CASE_MSG(val.isInt(), "actual=" << Value::typeToString(val.type()));
    TEST_CASE_MSG(val.integer() == 3, "actual=" << val);

    val = len(Hashtable())->eval(env);
    TEST_CASE_MSG(val.isInt(), "actual=" << Value::typeToString(val.type()));
    TEST_CASE_MSG(val.integer() == 0, "actual=" << val);

    val = len(IntegerRange(10))->eval(env);
    TEST_CASE_MSG(val.isInt(), "actual=" << Value::typeToString(val.type()));
    TEST_CASE_MSG(val.integer() == 10, "actual=" << val);

    val = len(ValuePair(Value(2ll), Value(3ll)))->eval(env);
    TEST_CASE_MSG(val.isInt(), "actual=" << Value::typeToString(val.type()));
    TEST_CASE_MSG(val.integer() == 2, "actual=" << val);

    try {
        len(5ll)->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeGenericEmpty) {
    auto env = Environment::make();

    auto isEmpty = [](auto rawVal) {
        return CodeNode::make<GenericEmpty>(CodeNode::make<Literal>(Value(rawVal)));
    };

    auto val = isEmpty("")->eval(env);
    TEST_CASE_MSG(val.isBool(), "actual=" << Value::typeToString(val.type()));
    TEST_CASE_MSG(val.boolean(), "actual=" << val);

    val = isEmpty(Sequence())->eval(env);
    TEST_CASE_MSG(val.isBool(), "actual=" << Value::typeToString(val.type()));
    TEST_CASE_MSG(val.boolean(), "actual=" << val);

    val = isEmpty(Hashtable())->eval(env);
    TEST_CASE_MSG(val.isBool(), "actual=" << Value::typeToString(val.type()));
    TEST_CASE_MSG(val.boolean(), "actual=" << val);

    val = isEmpty(IntegerRange(0, 0, 1))->eval(env);
    TEST_CASE_MSG(val.isBool(), "actual=" << Value::typeToString(val.type()));
    TEST_CASE_MSG(val.boolean(), "actual=" << val);

    val = isEmpty(ValuePair(Value(2ll), Value(3ll)))->eval(env);
    TEST_CASE_MSG(val.isBool(), "actual=" << Value::typeToString(val.type()));
    TEST_CASE_MSG(!val.boolean(), "actual=" << val);

    try {
        isEmpty(5ll)->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeGenericGet) {
    auto env = Environment::make();

    auto get = [](auto rawObj, auto rawKey, std::optional<Value> defaultVal = std::nullopt) {
        return CodeNode::make<GenericGet>(
            CodeNode::make<Literal>(Value(rawObj)),
            CodeNode::make<Literal>(Value(rawKey)),
            defaultVal ? CodeNode::make<Literal>(*defaultVal) : CodeNode::SharedPtr());
    };

    { // String
        const char *rawStr = "hello";
        for (Value::Long i : {0, 1, 2, 3, 4}) {
            auto val = get(rawStr, i)->eval(env);
            TEST_CASE_MSG(val.isChar(), "actual=" << Value::typeToString(val.type()) << " index=" << i);
            TEST_CASE_MSG(val.character() == rawStr[i], "actual=" << val << " index=" << i);
        }

        try {
            get(rawStr, "0")->eval(env);
            TEST_CASE(false);
        }
        catch (const InvalidOperandType &) {}
        catch (...) {
            TEST_CASE(false);
        }

        try {
            get(rawStr, 5ll)->eval(env);
            TEST_CASE(false);
        }
        catch (const OutOfRange &) {}
        catch (...) {
            TEST_CASE(false);
        }
    }

    { // Array
        const auto rawSeq = Sequence(std::vector{Value(1ll), Value(2ll), Value(3ll)});
        for (Value::Long i : {0, 1, 2}) {
            auto val = get(rawSeq, i)->eval(env);
            TEST_CASE_MSG(val.isInt(), "actual=" << Value::typeToString(val.type()) << " index=" << i);
            TEST_CASE_MSG(val == rawSeq.get(i), "actual=" << val << " index=" << i);
        }

        try {
            get(rawSeq, "0")->eval(env);
            TEST_CASE(false);
        }
        catch (const InvalidOperandType &) {}
        catch (...) {
            TEST_CASE(false);
        }

        try {
            get(rawSeq, 5ll)->eval(env);
            TEST_CASE(false);
        }
        catch (const OutOfRange &) {}
        catch (...) {
            TEST_CASE(false);
        }
    }

    { // HashMap
        auto rawTab = Hashtable();
        rawTab.set(Value(1ll), Value(100ll));
        rawTab.set(Value(2ll), Value(200ll));
        for (Value::Long i : {1, 2}) {
            auto val = get(rawTab, i)->eval(env);
            TEST_CASE_MSG(val.isInt(), "actual=" << Value::typeToString(val.type()) << " key=" << i);
            TEST_CASE_MSG(val == rawTab.get(Value(i)), "actual=" << val << " key=" << i);
        }

        auto val = get(rawTab, Value::Long(3))->eval(env);
        TEST_CASE_MSG(val.isNull(), "actual=" << Value::typeToString(val.type()) << " key=3");

        val = get(rawTab, Value::Long(3), Value(300ll))->eval(env);
        TEST_CASE_MSG(val.isInt(), "actual=" << Value::typeToString(val.type()) << " key=3");
        TEST_CASE_MSG(val.integer() == 300, "actual=" << val << " key=3");
    }

    { // UserObject
        auto rawType = Struct("Person", Struct::MemberList{"name", "age"});
        auto rawInst = Instance(rawType, Instance::InitArgs{{"name", Value("Jon")}, {"age", Value(25ll)}});
        for (const char * mem : {"name", "age"}) {
            auto val = get(rawInst, mem)->eval(env);
            TEST_CASE_MSG(val == rawInst.get(mem), "actual=" << val << " mem=" << mem);
        }

        for (const char * mem : {"name", "age"}) {
            auto val = CodeNode::make<GenericGet>(
                CodeNode::make<Literal>(Value(rawInst)),
                CodeNode::make<Variable>(mem))->eval(env);
            TEST_CASE_MSG(val == rawInst.get(mem), "actual=" << val << " mem=" << mem);
        }

        try {
            get(rawInst, 0ll)->eval(env);
            TEST_CASE(false);
        }
        catch (const InvalidOperandType &) {}
        catch (...) {
            TEST_CASE(false);
        }

        try {
            get(rawInst, "address")->eval(env);
            TEST_CASE(false);
        }
        catch (const UnknownMember &) {}
        catch (...) {
            TEST_CASE(false);
        }
    }

    { // Pair
        const auto rawPair = ValuePair(Value(2ll), Value(3ll));

        auto val = get(rawPair, 0ll)->eval(env);
        TEST_CASE_MSG(val.isInt(), "actual=" << Value::typeToString(val.type()) << " index=0");
        TEST_CASE_MSG(val == rawPair.first(), "actual=" << val << " index=0");

        val = get(rawPair, 1ll)->eval(env);
        TEST_CASE_MSG(val.isInt(), "actual=" << Value::typeToString(val.type()) << " index=1");
        TEST_CASE_MSG(val == rawPair.second(), "actual=" << val << " index=1");

        try {
            get(rawPair, "0")->eval(env);
            TEST_CASE(false);
        }
        catch (const InvalidOperandType &) {}
        catch (...) {
            TEST_CASE(false);
        }

        try {
            get(rawPair, 2ll)->eval(env);
            TEST_CASE(false);
        }
        catch (const OutOfRange &) {}
        catch (...) {
            TEST_CASE(false);
        }
    }

    try {
        get(5ll, 0ll)->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeGenericSet) {
    auto env = Environment::make();

    auto set = [](const char *name, auto rawKey, auto rawValue) {
        return CodeNode::make<GenericSet>(
            CodeNode::make<Variable>(name),
            CodeNode::make<Literal>(Value(rawKey)),
            CodeNode::make<Literal>(Value(rawValue)));
    };

    { // String
        env->def("txt", Value("hello"));
        const auto finalTxt = Value("HELLO");

        for (auto [i, c] : std::vector<std::pair<Value::Long, char>>{{0ll, 'H'}, {1ll, 'E'}, {2ll, 'L'}, {3ll, 'L'}, {4ll, 'O'}}) {
            auto val = set("txt", i, c)->eval(env);
            TEST_CASE_MSG(val.isChar(), "actual=" << Value::typeToString(val.type()) << " index=" << i);
            TEST_CASE_MSG(val.character() == c, "actual=" << val << " index=" << i);
        }

        TEST_CASE_MSG(env->get("txt") == finalTxt, "actual=" << env->get("txt"));

        try {
            set("txt", "0", 'T')->eval(env);
            TEST_CASE(false);
        }
        catch (const InvalidOperandType &) {}
        catch (...) {
            TEST_CASE(false);
        }

        TEST_CASE_MSG(env->get("txt") == finalTxt, "actual=" << env->get("txt"));

        try {
            set("txt", 5ll, 'T')->eval(env);
            TEST_CASE(false);
        }
        catch (const OutOfRange &) {}
        catch (...) {
            TEST_CASE(false);
        }

        TEST_CASE_MSG(env->get("txt") == finalTxt, "actual=" << env->get("txt"));
    }

    { // Array
        env->def("seq", Value(Sequence(std::vector{Value(1ll), Value(2ll), Value(3ll)})));
        const auto finalSeq = Value(Sequence(std::vector{Value(10ll), Value(20ll), Value(30ll)}));

        for (Value::Long i : {0, 1, 2}) {
            auto val = set("seq", i, 10 * i + 10)->eval(env);
            TEST_CASE_MSG(val.isInt(), "actual=" << Value::typeToString(val.type()) << " index=" << i);
            TEST_CASE_MSG(val.integer() == 10 * i + 10, "actual=" << val << " index=" << i);
        }

        TEST_CASE_MSG(env->get("seq") == finalSeq, "actual=" << env->get("seq"));

        try {
            set("seq", "0", 25ll)->eval(env);
            TEST_CASE(false);
        }
        catch (const InvalidOperandType &) {}
        catch (...) {
            TEST_CASE(false);
        }

        TEST_CASE_MSG(env->get("seq") == finalSeq, "actual=" << env->get("seq"));

        try {
            set("seq", 5ll, 25ll)->eval(env);
            TEST_CASE(false);
        }
        catch (const OutOfRange &) {}
        catch (...) {
            TEST_CASE(false);
        }

        TEST_CASE_MSG(env->get("seq") == finalSeq, "actual=" << env->get("seq"));
    }

    { // HashMap
        auto rawTab = Hashtable();
        rawTab.set(Value(1ll), Value(100ll));
        rawTab.set(Value(2ll), Value(200ll));
        env->def("tab", Value(rawTab));

        auto ft = Hashtable();
        ft.set(Value(1ll), Value(1000ll));
        ft.set(Value(2ll), Value(2000ll));
        const auto finalTab = Value(ft);

        for (auto [i, v] : std::vector<std::pair<Value::Long, Value::Long>>{{1ll, 1000ll}, {2ll, 2000ll}}) {
            auto val = set("tab", i, v)->eval(env);
            TEST_CASE_MSG(val.isInt(), "actual=" << Value::typeToString(val.type()) << " key=" << i);
            TEST_CASE_MSG(val.integer() == v, "actual=" << val << " key=" << i);
        }

        TEST_CASE_MSG(env->get("tab") == finalTab, "actual=" << env->get("tab"));
    }

    { // UserObject
        auto rawType = Struct("Person", Struct::MemberList{"name", "age"});
        env->def("obj", Value(Instance(rawType, Instance::InitArgs{{"name", Value("J")}, {"age", Value(20ll)}})));

        auto finalInst = Value(Instance(rawType, Instance::InitArgs{{"name", Value("Jon")}, {"age", Value(25ll)}}));

        auto val = set("obj", "name", "JJ")->eval(env);
        TEST_CASE_MSG(val == Value("JJ"), "actual=" << val);

        val = set("obj", "age", 23ll)->eval(env);
        TEST_CASE_MSG(val == Value(23ll), "actual=" << val);

        val = CodeNode::make<GenericSet>(
            CodeNode::make<Variable>("obj"),
            CodeNode::make<Variable>("name"),
            CodeNode::make<Literal>(Value("Jon")))->eval(env);
        TEST_CASE_MSG(val == Value("Jon"), "actual=" << val);

        val = CodeNode::make<GenericSet>(
            CodeNode::make<Variable>("obj"),
            CodeNode::make<Variable>("age"),
            CodeNode::make<Literal>(Value(25ll)))->eval(env);
        TEST_CASE_MSG(val == Value(25ll), "actual=" << val);

        TEST_CASE_MSG(env->get("obj") == finalInst, "actual=" << env->get("obj"));

        try {
            set("obj", 0ll, "Test")->eval(env);
            TEST_CASE(false);
        }
        catch (const InvalidOperandType &) {}
        catch (...) {
            TEST_CASE(false);
        }

        TEST_CASE_MSG(env->get("obj") == finalInst, "actual=" << env->get("obj"));

        try {
            set("obj", "address", "Test")->eval(env);
            TEST_CASE(false);
        }
        catch (const UnknownMember &) {}
        catch (...) {
            TEST_CASE(false);
        }

        TEST_CASE_MSG(env->get("obj") == finalInst, "actual=" << env->get("obj"));
    }

    try {
        env->def("dummy", Value(5ll));
        set("dummy", 0ll, 100ll)->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeGenericClear) {
    auto env = Environment::make();
    env->def("a", Value(Sequence(std::vector{Value::Zero, Value::Null})));
    env->def("s", Value("1234"));
    env->def("i", Value(5ll));

    auto rawTab = Hashtable();
    rawTab.set(Value(1ll), Value(100ll));
    env->def("h", rawTab);

    auto clr = [](char const * name) {
        return CodeNode::make<GenericClear>(CodeNode::make<Variable>(name));
    };

    auto val = clr("a")->eval(env);
    TEST_CASE_MSG(val.isInt(), "actual=" << Value::typeToString(val.type()));
    TEST_CASE_MSG(val.integer() == 2, "actual=" << val);
    TEST_CASE_MSG(env->get("a") == Value(Sequence()), "actual=" << env->get("a"));

    val = clr("s")->eval(env);
    TEST_CASE_MSG(val.isInt(), "actual=" << Value::typeToString(val.type()));
    TEST_CASE_MSG(val.integer() == 4, "actual=" << val);
    TEST_CASE_MSG(env->get("s") == Value(""), "actual=" << env->get("s"));

    val = clr("h")->eval(env);
    TEST_CASE_MSG(val.isInt(), "actual=" << Value::typeToString(val.type()));
    TEST_CASE_MSG(val.integer() == 1, "actual=" << val);
    TEST_CASE_MSG(env->get("h") == Value(Hashtable()), "actual=" << env->get("h"));

    try {
        clr("i")->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeGenericFind) {
    auto env = Environment::make();

    auto find = [](auto rawObj, auto rawItem, std::optional<Value> rawPos = std::nullopt) {
        return CodeNode::make<GenericFind>(
            CodeNode::make<Literal>(Value(rawObj)),
            CodeNode::make<Literal>(Value(rawItem)),
            rawPos ? CodeNode::make<Literal>(*rawPos) : CodeNode::SharedPtr());
    };

    { // String
        const char *rawStr = "hello";
        auto val = find(rawStr, 'l')->eval(env);
        TEST_CASE_MSG(val.isInt(), "actual=" << val.typeToString());
        TEST_CASE_MSG(val.integer() == 2ll, "actual=" << val);

        val = find(rawStr, 'o', 2ll)->eval(env);
        TEST_CASE_MSG(val.isInt(), "actual=" << val.typeToString());
        TEST_CASE_MSG(val.integer() == 4ll, "actual=" << val);

        val = find(rawStr, 'w')->eval(env);
        TEST_CASE_MSG(val.isInt(), "actual=" << val.typeToString());
        TEST_CASE_MSG(val.integer() == -1ll, "actual=" << val);

        try {
            find(rawStr, "h")->eval(env);
            TEST_CASE(false);
        }
        catch (const InvalidOperandType &ex) {
            TEST_CASE_MSG(std::string("Invalid operand type, expected=char actual=string") == ex.what(), "actual='" << ex.what() << "'");
        }
        catch (...) {
            TEST_CASE(false);
        }

        try {
            find(rawStr, 'o', 5ll)->eval(env);
            TEST_CASE(false);
        }
        catch (const OutOfRange &ex) {
            TEST_CASE_MSG(std::string("Out of range: string find position access") == ex.what(), "actual='" << ex.what() << "'");
        }
        catch (...) {
            TEST_CASE(false);
        }
    }

    { // Array
        const auto rawSeq = Sequence(std::vector{Value(1ll), Value(2ll), Value(3ll), Value(4ll)});
        auto val = find(rawSeq, 3ll)->eval(env);
        TEST_CASE_MSG(val.isInt(), "actual=" << val.typeToString());
        TEST_CASE_MSG(val.integer() == 2ll, "actual=" << val);

        val = find(rawSeq, 3ll, 1ll)->eval(env);
        TEST_CASE_MSG(val.isInt(), "actual=" << val.typeToString());
        TEST_CASE_MSG(val.integer() == 2ll, "actual=" << val);

        val = find(rawSeq, 5ll)->eval(env);
        TEST_CASE_MSG(val.isInt(), "actual=" << val.typeToString());
        TEST_CASE_MSG(val.integer() == -1ll, "actual=" << val);

        try {
            find(rawSeq, 3ll, 5ll)->eval(env);
            TEST_CASE(false);
        }
        catch (const OutOfRange &ex) {
            TEST_CASE_MSG(std::string("Out of range: array find position access") == ex.what(), "actual='" << ex.what() << "'");
        }
        catch (...) {
            TEST_CASE(false);
        }

        try {
            find(rawSeq, 3ll, -1ll)->eval(env);
            TEST_CASE(false);
        }
        catch (const OutOfRange &ex) {
            TEST_CASE_MSG(std::string("Out of range: array find position access") == ex.what(), "actual='" << ex.what() << "'");
        }
        catch (...) {
            TEST_CASE(false);
        }
    }

    { // HashMap
        auto rawTab = Hashtable();
        rawTab.set(Value(1ll), Value(100ll));
        rawTab.set(Value(2ll), Value(200ll));
        rawTab.set(Value(3ll), Value(300ll));

        auto val = find(rawTab, 200ll)->eval(env);
        TEST_CASE_MSG(val.isInt(), "actual=" << val.typeToString());
        TEST_CASE_MSG(val.integer() == 2ll, "actual=" << val);

        val = find(rawTab, 400ll)->eval(env);
        TEST_CASE_MSG(val.isNull(), "actual=" << val.typeToString());
    }

    { // Pair
        const auto rawPair = ValuePair(Value(2ll), Value(3ll));

        auto val = find(rawPair, 2ll)->eval(env);
        TEST_CASE_MSG(val.isInt(), "actual=" << val.typeToString());
        TEST_CASE_MSG(val.integer() == 0ll, "actual=" << val);

        val = find(rawPair, 3ll)->eval(env);
        TEST_CASE_MSG(val.isInt(), "actual=" << val.typeToString());
        TEST_CASE_MSG(val.integer() == 1ll, "actual=" << val);

        val = find(rawPair, 5ll)->eval(env);
        TEST_CASE_MSG(val.isInt(), "actual=" << val.typeToString());
        TEST_CASE_MSG(val.integer() == -1ll, "actual=" << val);

        try {
            find(rawPair, 3ll, 5ll)->eval(env);
            TEST_CASE(false);
        }
        catch (const OutOfRange &ex) {
            TEST_CASE_MSG(std::string("Out of range: pair find position access") == ex.what(), "actual='" << ex.what() << "'");
        }
        catch (...) {
            TEST_CASE(false);
        }

        try {
            find(rawPair, 3ll, -1ll)->eval(env);
            TEST_CASE(false);
        }
        catch (const OutOfRange &ex) {
            TEST_CASE_MSG(std::string("Out of range: pair find position access") == ex.what(), "actual='" << ex.what() << "'");
        }
        catch (...) {
            TEST_CASE(false);
        }
    }

    try {
        find(5ll, 0ll)->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeGenericCount) {
    auto env = Environment::make();

    auto count = [](auto rawObj, auto rawItem) {
        return CodeNode::make<GenericCount>(
            CodeNode::make<Literal>(Value(rawObj)),
            CodeNode::make<Literal>(Value(rawItem)));
    };

    { // String
        const char *rawStr = "hello";
        auto val = count(rawStr, 'l')->eval(env);
        TEST_CASE_MSG(val.isInt(), "actual=" << val.typeToString());
        TEST_CASE_MSG(val.integer() == 2ll, "actual=" << val);

        val = count(rawStr, 'o')->eval(env);
        TEST_CASE_MSG(val.isInt(), "actual=" << val.typeToString());
        TEST_CASE_MSG(val.integer() == 1ll, "actual=" << val);

        val = count(rawStr, 'w')->eval(env);
        TEST_CASE_MSG(val.isInt(), "actual=" << val.typeToString());
        TEST_CASE_MSG(val.integer() == 0ll, "actual=" << val);

        try {
            count(rawStr, "h")->eval(env);
            TEST_CASE(false);
        }
        catch (const InvalidOperandType &ex) {
            TEST_CASE_MSG(std::string("Invalid operand type, expected=char actual=string") == ex.what(), "actual='" << ex.what() << "'");
        }
        catch (...) {
            TEST_CASE(false);
        }
    }

    { // Array
        const auto rawSeq = Sequence(std::vector{Value(1ll), Value(2ll), Value(3ll), Value(3ll), Value(4ll)});
        auto val = count(rawSeq, 3ll)->eval(env);
        TEST_CASE_MSG(val.isInt(), "actual=" << val.typeToString());
        TEST_CASE_MSG(val.integer() == 2ll, "actual=" << val);

        val = count(rawSeq, 2ll)->eval(env);
        TEST_CASE_MSG(val.isInt(), "actual=" << val.typeToString());
        TEST_CASE_MSG(val.integer() == 1ll, "actual=" << val);

        val = count(rawSeq, 5ll)->eval(env);
        TEST_CASE_MSG(val.isInt(), "actual=" << val.typeToString());
        TEST_CASE_MSG(val.integer() == 0ll, "actual=" << val);
    }

    { // HashMap
        auto rawTab = Hashtable();
        rawTab.set(Value(1ll), Value(100ll));
        rawTab.set(Value(2ll), Value(200ll));
        rawTab.set(Value(20ll), Value(200ll));
        rawTab.set(Value(3ll), Value(300ll));

        auto val = count(rawTab, 200ll)->eval(env);
        TEST_CASE_MSG(val.isInt(), "actual=" << val.typeToString());
        TEST_CASE_MSG(val.integer() == 2ll, "actual=" << val);

        val = count(rawTab, 100ll)->eval(env);
        TEST_CASE_MSG(val.isInt(), "actual=" << val.typeToString());
        TEST_CASE_MSG(val.integer() == 1ll, "actual=" << val);

        val = count(rawTab, 400ll)->eval(env);
        TEST_CASE_MSG(val.isInt(), "actual=" << val.typeToString());
        TEST_CASE_MSG(val.integer() == 0ll, "actual=" << val);
    }

    { // Pair
        const auto rawPair1 = ValuePair(Value(2ll), Value(3ll));
        const auto rawPair2 = ValuePair(Value(5ll), Value(5ll));

        auto val = count(rawPair1, 3ll)->eval(env);
        TEST_CASE_MSG(val.isInt(), "actual=" << val.typeToString());
        TEST_CASE_MSG(val.integer() == 1ll, "actual=" << val);

        val = count(rawPair1, 2ll)->eval(env);
        TEST_CASE_MSG(val.isInt(), "actual=" << val.typeToString());
        TEST_CASE_MSG(val.integer() == 1ll, "actual=" << val);

        val = count(rawPair2, 5ll)->eval(env);
        TEST_CASE_MSG(val.isInt(), "actual=" << val.typeToString());
        TEST_CASE_MSG(val.integer() == 2ll, "actual=" << val);

        val = count(rawPair1, 5ll)->eval(env);
        TEST_CASE_MSG(val.isInt(), "actual=" << val.typeToString());
        TEST_CASE_MSG(val.integer() == 0ll, "actual=" << val);
    }

    try {
        count(5ll, 0ll)->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeGenericSort) {
    auto env = Environment::make();

    const auto v1 = Value(1ll);
    const auto v2 = Value(2ll);
    const auto v3 = Value(3ll);
    const auto v4 = Value(4ll);
    const auto v5 = Value(5ll);
    const auto v6 = Value(6ll);
    const auto v7 = Value(7ll);

    env->def("num", Value(5ll));
    env->def("str", Value("3215674"));
    env->def("arr", arrval(v3, v2, v1, v5, v6, v7, v4));

    const auto sort = [](const char *obj, const Value &desc = Value::Null) {
        return CodeNode::make<GenericSort>(
            CodeNode::make<Variable>(obj),
            desc != Value::Null ? CodeNode::make<Literal>(desc) : CodeNode::SharedPtr());
    };

    const auto str = [&env]() { return env->get("str"); };
    const auto arr = [&env]() { return env->get("arr"); };

    { // String
        auto val = sort("str")->eval(env);
        TEST_CASE_MSG(val.isString(), "actual=" << val.typeToString());
        TEST_CASE_MSG(val.text() == "1234567", "actual=" << val);
        TEST_CASE_MSG(val == str(), "actual=" << val);

        val = sort("str", Value::True)->eval(env);
        TEST_CASE_MSG(val.isString(), "actual=" << val.typeToString());
        TEST_CASE_MSG(val.text() == "7654321", "actual=" << val);
        TEST_CASE_MSG(val == str(), "actual=" << val);

        val = sort("str", Value::False)->eval(env);
        TEST_CASE_MSG(val.isString(), "actual=" << val.typeToString());
        TEST_CASE_MSG(val.text() == "1234567", "actual=" << val);
        TEST_CASE_MSG(val == str(), "actual=" << val);

        try {
            sort("str", Value("h"))->eval(env);
            TEST_CASE(false);
        }
        catch (const InvalidOperandType &ex) {
            TEST_CASE_MSG(std::string("Invalid operand type, expected=bool actual=string") == ex.what(), "actual='" << ex.what() << "'");
        }
        catch (...) {
            TEST_CASE(false);
        }
    }

    { // Array
        auto val = sort("arr")->eval(env);
        TEST_CASE_MSG(val.isArray(), "actual=" << val.typeToString());
        TEST_CASE_MSG(val == arrval(v1, v2, v3, v4, v5, v6, v7), "actual=" << val);
        TEST_CASE_MSG(val == arr(), "actual=" << val);

        val = sort("arr", Value::True)->eval(env);
        TEST_CASE_MSG(val.isArray(), "actual=" << val.typeToString());
        TEST_CASE_MSG(val == arrval(v7, v6, v5, v4, v3, v2, v1), "actual=" << val);
        TEST_CASE_MSG(val == arr(), "actual=" << val);

        val = sort("arr", Value::False)->eval(env);
        TEST_CASE_MSG(val.isArray(), "actual=" << val.typeToString());
        TEST_CASE_MSG(val == arrval(v1, v2, v3, v4, v5, v6, v7), "actual=" << val);
        TEST_CASE_MSG(val == arr(), "actual=" << val);

        try {
            sort("arr", Value("h"))->eval(env);
            TEST_CASE(false);
        }
        catch (const InvalidOperandType &ex) {
            TEST_CASE_MSG(std::string("Invalid operand type, expected=bool actual=string") == ex.what(), "actual='" << ex.what() << "'");
        }
        catch (...) {
            TEST_CASE(false);
        }
    }

    try {
        sort("num")->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &ex) {
        TEST_CASE_MSG(std::string("Invalid operand type, expected=string|array actual=int") == ex.what(), "actual='" << ex.what() << "'");
    }
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeGenericReverse) {
    auto env = Environment::make();

    const auto v1 = Value(1ll);
    const auto v2 = Value(2ll);
    const auto v3 = Value(3ll);
    const auto v4 = Value(4ll);
    const auto v5 = Value(5ll);

    env->def("num", Value(5ll));
    env->def("str", Value("12345"));
    env->def("arr", arrval(v1, v2, v3, v4, v5));

    const auto reverse = [](const char *obj) {
        return CodeNode::make<GenericReverse>(CodeNode::make<Variable>(obj));
    };

    const auto str = [&env]() { return env->get("str"); };
    const auto arr = [&env]() { return env->get("arr"); };

    { // String
        auto val = reverse("str")->eval(env);
        TEST_CASE_MSG(val.isString(), "actual=" << val.typeToString());
        TEST_CASE_MSG(val.text() == "54321", "actual=" << val);
        TEST_CASE_MSG(val == str(), "actual=" << val);

        val = reverse("str")->eval(env);
        TEST_CASE_MSG(val.isString(), "actual=" << val.typeToString());
        TEST_CASE_MSG(val.text() == "12345", "actual=" << val);
        TEST_CASE_MSG(val == str(), "actual=" << val);
    }

    { // Array
        auto val = reverse("arr")->eval(env);
        TEST_CASE_MSG(val.isArray(), "actual=" << val.typeToString());
        TEST_CASE_MSG(val == arrval(v5, v4, v3, v2, v1), "actual=" << val);
        TEST_CASE_MSG(val == arr(), "actual=" << val);

        val = reverse("arr")->eval(env);
        TEST_CASE_MSG(val.isArray(), "actual=" << val.typeToString());
        TEST_CASE_MSG(val == arrval(v1, v2, v3, v4, v5), "actual=" << val);
        TEST_CASE_MSG(val == arr(), "actual=" << val);
    }

    try {
        reverse("num")->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &ex) {
        TEST_CASE_MSG(std::string("Invalid operand type, expected=string|array actual=int") == ex.what(), "actual='" << ex.what() << "'");
    }
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeGenericSum) {
    auto env = Environment::make();

    const auto v1 = Value(1ll);
    const auto v2 = Value(2ll);
    const auto v3 = Value(3ll);
    const auto v4 = Value(4ll);
    const auto v5 = Value(5ll);

    const auto f1 = Value(1.1);
    const auto f2 = Value(2.2);
    const auto f3 = Value(3.3);

    const auto c1 = Value('c');

    env->def("num", Value(5ll));
    env->def("rng0", Value(IntegerRange(0)));
    env->def("rng1", Value(IntegerRange(10)));
    env->def("arr0", arrval());
    env->def("arr1", arrval(v1, v2, v3, v4, v5));
    env->def("arr2", arrval(f1, f2, f3));
    env->def("arr3", arrval(v1, f2, v3));
    env->def("mix", arrval(v1, c1, f1));
    env->def("p0", ValuePair(v4, v5));
    env->def("p1", ValuePair(f2, f3));
    env->def("p2", ValuePair(v2, f3));
    env->def("p3", ValuePair(v2, c1));

    const auto sum = [](const char *obj) {
        return CodeNode::make<GenericSum>(CodeNode::make<Variable>(obj));
    };

    { // range
        auto val = sum("rng0")->eval(env);
        TEST_CASE_MSG(val.isInt(), "actual=" << val.typeToString());
        TEST_CASE_MSG(val.integer() == 0ll, "actual=" << val);

        val = sum("rng1")->eval(env);
        TEST_CASE_MSG(val.isInt(), "actual=" << val.typeToString());
        TEST_CASE_MSG(val.integer() == 45ll, "actual=" << val);
    }

    { // array
        auto val = sum("arr0")->eval(env);
        TEST_CASE_MSG(val.isInt(), "actual=" << val.typeToString());
        TEST_CASE_MSG(val.integer() == 0ll, "actual=" << val);

        val = sum("arr1")->eval(env);
        TEST_CASE_MSG(val.isInt(), "actual=" << val.typeToString());
        TEST_CASE_MSG(val.integer() == 15ll, "actual=" << val);

        val = sum("arr2")->eval(env);
        TEST_CASE_MSG(val.isReal(), "actual=" << val.typeToString());
        TEST_CASE_MSG(val.real() == 6.6, "actual=" << val);

        val = sum("arr3")->eval(env);
        TEST_CASE_MSG(val.isReal(), "actual=" << val.typeToString());
        TEST_CASE_MSG(val.real() == 6.2, "actual=" << val);
    }

    { // pair
        auto val = sum("p0")->eval(env);
        TEST_CASE_MSG(val.isInt(), "actual=" << val.typeToString());
        TEST_CASE_MSG(val.integer() == 9ll, "actual=" << val);

        val = sum("p1")->eval(env);
        TEST_CASE_MSG(val.isReal(), "actual=" << val.typeToString());
        TEST_CASE_MSG(val.real() == 5.5, "actual=" << val);

        val = sum("p2")->eval(env);
        TEST_CASE_MSG(val.isReal(), "actual=" << val.typeToString());
        TEST_CASE_MSG(val.real() == 5.3, "actual=" << val);
    }

    try {
        sum("mix")->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidExpression &ex) {
        TEST_CASE_MSG(std::string("Invalid expression - Unexpected non-numeric sum argument") == ex.what(), "actual='" << ex.what() << "'");
    }
    catch (...) {
        TEST_CASE(false);
    }

    try {
        sum("p3")->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidExpression &ex) {
        TEST_CASE_MSG(std::string("Invalid expression - Unexpected non-numeric sum argument") == ex.what(), "actual='" << ex.what() << "'");
    }
    catch (...) {
        TEST_CASE(false);
    }

    try {
        sum("num")->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &ex) {
        TEST_CASE_MSG(std::string("Invalid operand type, expected=array|range|pair actual=int") == ex.what(), "actual='" << ex.what() << "'");
    }
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeGenericApply) {
    auto env = Environment::make();

    const auto v1 = Value(1ll);
    const auto v2 = Value(2ll);
    const auto v3 = Value(3ll);

    env->def("num", Value(5ll));
    env->def("rng0", Value(IntegerRange(1, 1)));
    env->def("rng1", Value(IntegerRange(1, 2)));
    env->def("rng2", Value(IntegerRange(1, 3)));
    env->def("rng3", Value(IntegerRange(1, 4)));
    env->def("arr0", arrval());
    env->def("arr1", arrval(v1));
    env->def("arr2", arrval(v1, v2));
    env->def("arr3", arrval(v1, v2, v3));
    env->def("p", ValuePair(v1, v2));

    const auto adder = [](size_t paramsSize) {
        Lambda::ParamList params;
        CodeNode::SharedPtrList addArgs;
        for (size_t i = 0; i < paramsSize; ++i) {
            const auto name = std::string("p") + std::to_string(i); 
            params.push_back(name);
            addArgs.push_back(CodeNode::make<Variable>(name));
        }

        if (paramsSize == 0) {
            return CodeNode::make<LambdaExpr>(params, CodeNode::make<Literal>(Value::Zero));
        }
        else if (paramsSize == 1) {
            return CodeNode::make<LambdaExpr>(params, CodeNode::make<Variable>(params[0]));
        }
        else {
            return CodeNode::make<LambdaExpr>(params, CodeNode::make<ArithOp>(ArithOp::Add, addArgs));
        }
    };

    const auto apply = [](CodeNode::SharedPtr ftn, const char *obj) {
        return CodeNode::make<GenericApply>(ftn, CodeNode::make<Variable>(obj));
    };

    { // range
        auto val = apply(adder(0), "rng0")->eval(env);
        TEST_CASE_MSG(val.isInt(), "actual=" << val.typeToString());
        TEST_CASE_MSG(val.integer() == 0ll, "actual=" << val);

        val = apply(adder(1), "rng1")->eval(env);
        TEST_CASE_MSG(val.isInt(), "actual=" << val.typeToString());
        TEST_CASE_MSG(val.integer() == 1ll, "actual=" << val);

        val = apply(adder(2), "rng2")->eval(env);
        TEST_CASE_MSG(val.isInt(), "actual=" << val.typeToString());
        TEST_CASE_MSG(val.integer() == 3ll, "actual=" << val);

        val = apply(adder(3), "rng3")->eval(env);
        TEST_CASE_MSG(val.isInt(), "actual=" << val.typeToString());
        TEST_CASE_MSG(val.integer() == 6ll, "actual=" << val);
    }

    { // array
        auto val = apply(adder(0), "arr0")->eval(env);
        TEST_CASE_MSG(val.isInt(), "actual=" << val.typeToString());
        TEST_CASE_MSG(val.integer() == 0ll, "actual=" << val);

        val = apply(adder(1), "arr1")->eval(env);
        TEST_CASE_MSG(val.isInt(), "actual=" << val.typeToString());
        TEST_CASE_MSG(val.integer() == 1ll, "actual=" << val);

        val = apply(adder(2), "arr2")->eval(env);
        TEST_CASE_MSG(val.isInt(), "actual=" << val.typeToString());
        TEST_CASE_MSG(val.integer() == 3ll, "actual=" << val);

        val = apply(adder(3), "arr3")->eval(env);
        TEST_CASE_MSG(val.isInt(), "actual=" << val.typeToString());
        TEST_CASE_MSG(val.integer() == 6ll, "actual=" << val);
    }

    { // pair
        auto val = apply(adder(2), "p")->eval(env);
        TEST_CASE_MSG(val.isInt(), "actual=" << val.typeToString());
        TEST_CASE_MSG(val.integer() == 3ll, "actual=" << val);
    }

    try {
        apply(adder(3), "arr2")->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidArgsSize &ex) {
        TEST_CASE_MSG(std::string("Invalid apply arguments list - params size(3) is not equal to args size (2)") == ex.what(),
                      "actual='" << ex.what() << "'");
    }
    catch (...) {
        TEST_CASE(false);
    }

    try {
        apply(adder(1), "num")->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &ex) {
        TEST_CASE_MSG(std::string("Invalid operand type, expected=array|pair|range actual=int") == ex.what(),
                      "actual='" << ex.what() << "'");
    }
    catch (...) {
        TEST_CASE(false);
    }
}
