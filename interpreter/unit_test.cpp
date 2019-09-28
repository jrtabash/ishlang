#include "unit_test.h"
#include "util.h"

#include <map>
#include <iostream>
#include <functional>
#include <array>

using namespace Int;

#define TEST_CASE(EXPR) if (!(EXPR)) { std::cerr << '[' << __LINE__ << "] " << #EXPR << '\n'; success_ = false; }
#define TEST_CASE_MSG(EXPR, MSG) if (!(EXPR)) { std::cerr << '[' << __LINE__ << "] " << #EXPR << ": " << MSG << '\n'; success_ = false; }

// -------------------------------------------------------------
#define ADD_TEST(NAME) tests_.insert(Tests::value_type(#NAME, std::bind(&UnitTest::NAME, this)))
UnitTest::UnitTest()
    : success_(true)
    , verbose_(false)
    , tests_()
{
    ADD_TEST(testToken);
    ADD_TEST(testValue);
    ADD_TEST(testEnvironment);
    ADD_TEST(testLambda);
    ADD_TEST(testStruct);
    ADD_TEST(testStructValue);
    ADD_TEST(testInstance);
    ADD_TEST(testInstanceValue);
    ADD_TEST(testByteCodeBasic);
    ADD_TEST(testByteCodeClone);
    ADD_TEST(testByteCodeIsType);
    ADD_TEST(testByteCodeArithOp);
    ADD_TEST(testByteCodeCompOp);
    ADD_TEST(testByteCodeLogicOp);
    ADD_TEST(testByteCodeSequence);
    ADD_TEST(testByteCodeCond);
    ADD_TEST(testByteCodeLoop);
    ADD_TEST(testByteCodeLambdaExpr);
    ADD_TEST(testByteCodeLambdaApp);
    ADD_TEST(testByteCodeFunctionExpr);
    ADD_TEST(testByteCodeFunctionApp);
    ADD_TEST(testByteCodeStruct);
    ADD_TEST(testByteCodeIsStructName);
    ADD_TEST(testByteCodeMakeInstance);
    ADD_TEST(testByteCodeIsInstanceOf);
    ADD_TEST(testByteCodeGetSetMember);
    ADD_TEST(testTokenType);
    ADD_TEST(testByteCodeStringLen);
    ADD_TEST(testByteCodeCharAt);
    ADD_TEST(testByteCodeSetCharAt);
    ADD_TEST(testParserBasic);
    ADD_TEST(testParserIsType);
    ADD_TEST(testParserVar);
    ADD_TEST(testParserArith);
    ADD_TEST(testParserComp);
    ADD_TEST(testParserLogic);
    ADD_TEST(testParserBlock);
    ADD_TEST(testParserCond);
    ADD_TEST(testParserLoop);
    ADD_TEST(testParserLambda);
    ADD_TEST(testParserFtn);
    ADD_TEST(testParserStruct);
    ADD_TEST(testParserIsStructName);
    ADD_TEST(testParserMakeInstance);
    ADD_TEST(testParserIsInstanceOf);
    ADD_TEST(testParserStringLen);
    ADD_TEST(testParserGetChar);
    ADD_TEST(testParserSetChar);
}
#undef ADD_TEST

// -------------------------------------------------------------
bool UnitTest::run(const std::string &test) {
    success_ = true;
    if (test.empty()) {
        for (Tests::iterator iter = tests_.begin(); iter != tests_.end(); ++iter) {
            runTest(iter->first, iter->second);
        }
    }
    else {
        Tests::iterator iter = tests_.find(test);
        if (iter == tests_.end()) {
            std::cerr << "Unknown test - " << test << '\n';
            return false;
        }
        runTest(iter->first, iter->second);
    }
    return success_;
}

// -------------------------------------------------------------
void UnitTest::list() const {
    for (Tests::const_iterator iter = tests_.begin(); iter != tests_.end(); ++iter) {
        std::cout << iter->first << '\n';
    }
}

// -------------------------------------------------------------
void UnitTest::runTest(const std::string &name, Function ftn) {
    if (verbose_) { std::cout << "Testing: " << name << '\n'; }
    ftn();
}

// -------------------------------------------------------------
void UnitTest::testToken() {

    {
        std::string str;
        size_t pos = 0;

        {
            TEST_CASE(Util::nextToken(str, pos) == "");
            TEST_CASE(pos == std::string::npos);
        }

        {
            str = "   \t  \n ";
            pos = 0;
            TEST_CASE(Util::nextToken(str, pos) == "");
            TEST_CASE(pos == std::string::npos);
        }

        {
            str = "1";
            pos = 0;
            TEST_CASE(Util::nextToken(str, pos) == "1");
            TEST_CASE(pos == std::string::npos);
        }

        {
            str = "\"1\"";
            pos = 0;
            TEST_CASE(Util::nextToken(str, pos) == "\"1\"");
            TEST_CASE(pos == std::string::npos);
        }

        {
            str = "abc 2 \t 3\n";
            pos = 0;
            TEST_CASE(Util::nextToken(str, pos) == "abc");
            TEST_CASE(pos == 4);
            TEST_CASE(Util::nextToken(str, pos) == "2");
            TEST_CASE(pos == 6);
            TEST_CASE(Util::nextToken(str, pos) == "3");
            TEST_CASE(pos == std::string::npos);
        }

        {
            str = "(+ 1 2)";
            pos = 0;
            TEST_CASE(Util::nextToken(str, pos) == "(");
            TEST_CASE(pos == 1);
            TEST_CASE(Util::nextToken(str, pos) == "+");
            TEST_CASE(pos == 3);
            TEST_CASE(Util::nextToken(str, pos) == "1");
            TEST_CASE(pos == 5);
            TEST_CASE(Util::nextToken(str, pos) == "2");
            TEST_CASE(pos == 6);
            TEST_CASE(Util::nextToken(str, pos) == ")");
            TEST_CASE(pos == std::string::npos);
        }

        {
            str = "( 4 5 abc + \tfoobar \t\n)";
            pos = 0;
            TEST_CASE(Util::nextToken(str, pos) == "(");
            TEST_CASE(pos == 1);
            TEST_CASE(Util::nextToken(str, pos) == "4");
            TEST_CASE(pos == 4);
            TEST_CASE(Util::nextToken(str, pos) == "5");
            TEST_CASE(pos == 6);
            TEST_CASE(Util::nextToken(str, pos) == "abc");
            TEST_CASE(pos == 10);
            TEST_CASE(Util::nextToken(str, pos) == "+");
            TEST_CASE(pos == 12);
            TEST_CASE(Util::nextToken(str, pos) == "foobar");
            TEST_CASE(pos == 20);
            TEST_CASE(Util::nextToken(str, pos) == ")");
            TEST_CASE(pos == std::string::npos);
        }

        {
            str = "'a'";
            pos = 0;
            TEST_CASE(Util::nextToken(str, pos) == "'a'");
            TEST_CASE(pos == std::string::npos);
        }

        {
            str = "(= c 'b')";
            pos = 0;
            TEST_CASE(Util::nextToken(str, pos) == "(");
            TEST_CASE(pos == 1);
            TEST_CASE(Util::nextToken(str, pos) == "=");
            TEST_CASE(pos == 3);
            TEST_CASE(Util::nextToken(str, pos) == "c");
            TEST_CASE(pos == 5);
            TEST_CASE(Util::nextToken(str, pos) == "'b'");
            TEST_CASE(pos == 8);
            TEST_CASE(Util::nextToken(str, pos) == ")");
            TEST_CASE(pos == std::string::npos);
        }

        {
            try {
                str = "'";
                pos = 0;
                Util::nextToken(str, pos);
                TEST_CASE(false);
            }
            catch (const InvalidExpression &ex) {}
        }

        {
            try {
                str = "'a";
                pos = 0;
                Util::nextToken(str, pos);
                TEST_CASE(false);
            }
            catch (const InvalidExpression &ex) {}
        }

        {
            try {
                str = "'ab";
                pos = 0;
                Util::nextToken(str, pos);
                TEST_CASE(false);
            }
            catch (const InvalidExpression &ex) {}
        }
    }


    {
        Util::TokenList tokens;

        {
            TEST_CASE(Util::tokenize("", tokens) == 0);
            TEST_CASE(tokens.empty());
        }

        {
            TEST_CASE(Util::tokenize("   \t  \n ", tokens) == 0);
            TEST_CASE(tokens.empty());
        }

        {
            TEST_CASE(Util::tokenize("1", tokens) == 1);
            TEST_CASE(tokens.front() == "1");
        }

        {
            TEST_CASE(Util::tokenize("abc 2 \t 3\n", tokens) == 3);

            Util::TokenList::const_iterator iter = tokens.begin();
            TEST_CASE(*iter++ == "abc");
            TEST_CASE(*iter++ == "2");
            TEST_CASE(*iter++ == "3");
            TEST_CASE(iter == tokens.end());
        }

        {
            TEST_CASE(Util::tokenize("(+ 1 2)", tokens) == 5);

            Util::TokenList::const_iterator iter = tokens.begin();
            TEST_CASE(*iter++ == "(");
            TEST_CASE(*iter++ == "+");
            TEST_CASE(*iter++ == "1");
            TEST_CASE(*iter++ == "2");
            TEST_CASE(*iter++ == ")");
            TEST_CASE(iter == tokens.end());
        }

        {
            TEST_CASE(Util::tokenize("( 4 5 abc + \tfoobar \t\n)", tokens) == 7);

            Util::TokenList::const_iterator iter = tokens.begin();
            TEST_CASE(*iter++ == "(");
            TEST_CASE(*iter++ == "4");
            TEST_CASE(*iter++ == "5");
            TEST_CASE(*iter++ == "abc");
            TEST_CASE(*iter++ == "+");
            TEST_CASE(*iter++ == "foobar");
            TEST_CASE(*iter++ == ")");
            TEST_CASE(iter == tokens.end());
        }

        {
            TEST_CASE(Util::tokenize("(= c 'b')", tokens) == 5);

            Util::TokenList::const_iterator iter = tokens.begin();
            TEST_CASE(*iter++ == "(");
            TEST_CASE(*iter++ == "=");
            TEST_CASE(*iter++ == "c");
            TEST_CASE(*iter++ == "'b'");
            TEST_CASE(*iter++ == ")");
            TEST_CASE(iter == tokens.end());
        }

        {
            TEST_CASE(Util::tokenize("(= c \"b\")", tokens) == 5);

            Util::TokenList::const_iterator iter = tokens.begin();
            TEST_CASE(*iter++ == "(");
            TEST_CASE(*iter++ == "=");
            TEST_CASE(*iter++ == "c");
            TEST_CASE(*iter++ == "\"b\"");
            TEST_CASE(*iter++ == ")");
            TEST_CASE(iter == tokens.end());
        }
    }
}

// -------------------------------------------------------------
void UnitTest::testValue() {
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
        for (int i = 0; i < numCases; ++i) {
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
        TEST_CASE(Value::stringToType("none")       == Value::eNone);
        TEST_CASE(Value::stringToType("int")        == Value::eInteger);
        TEST_CASE(Value::stringToType("real")       == Value::eReal);
        TEST_CASE(Value::stringToType("char")       == Value::eCharacter);
        TEST_CASE(Value::stringToType("bool")       == Value::eBoolean);
        TEST_CASE(Value::stringToType("string")     == Value::eString);
        TEST_CASE(Value::stringToType("closure")    == Value::eClosure);
        TEST_CASE(Value::stringToType("usertype")   == Value::eUserType);
        TEST_CASE(Value::stringToType("userobject") == Value::eUserObject);

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
void UnitTest::testEnvironment() {
    Environment::SharedPtr env(new Environment());
    try {
        env->def("x", Value::Null);
        env->def("y", Value::Null);
    }
    catch (const DuplicateDef &ex) { TEST_CASE(false); }
    catch (...) { TEST_CASE(false); }

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
        Environment::SharedPtr childEnv(new Environment(env));
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
    
    Environment::SharedPtr child(new Environment(env));
    
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
void UnitTest::testLambda() {
    Environment::SharedPtr env(new Environment());
    env->def("x", Value::Zero);

    const Lambda::ParamList params({"x", "y"});    
    ByteCode::SharedPtr body(
        new ArithOp(ArithOp::Add,
                    ByteCode::SharedPtr(new Variable("x")),
                    ByteCode::SharedPtr(new Variable("y"))));

    Lambda lambda(params, body, env);

    Lambda::ArgList args;
    args.push_back(Value(3ll));
    args.push_back(Value(4ll));

    Value result = lambda.exec(args);
    TEST_CASE_MSG(result.isInt(), "actual=" << result.typeToString());
    TEST_CASE_MSG(result.integer() == 7ll, "actual=" << result);
}

// -------------------------------------------------------------
void UnitTest::testStruct() {
    Struct s("Foo", {"mem1", "mem2", "mem3"});

    TEST_CASE_MSG(s.name() == "Foo", "actual=" << s.name());
    TEST_CASE_MSG(s.members().size() == 3, "actual=" << s.members().size());
    TEST_CASE_MSG(s.members()[0] == "mem1", "actual=" << s.members()[0]);
    TEST_CASE_MSG(s.members()[1] == "mem2", "actual=" << s.members()[1]);
    TEST_CASE_MSG(s.members()[2] == "mem3", "actual=" << s.members()[2]);
}

// -------------------------------------------------------------
void UnitTest::testStructValue() {
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
void UnitTest::testInstance() {
    Struct s("Person", {"name", "age"});
    Instance i(s);

    TEST_CASE_MSG(i.type() == s, "actual=" << i.type().name());
    TEST_CASE_MSG(i.get("name") == Value::Null, "actual=" << i.get("name"));
    TEST_CASE_MSG(i.get("age") == Value::Null, "actual=" << i.get("age"));

    i.set("name", Value("Jack"));
    i.set("age", Value(32ll));

    TEST_CASE_MSG(i.get("name") == Value("Jack"), "actual=" << i.get("name"));
    TEST_CASE_MSG(i.get("age") == Value(32ll), "actual=" << i.get("age"));
}

// -------------------------------------------------------------
void UnitTest::testInstanceValue() {
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
    catch (const Int::Exception &ex) {
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
    catch (const Int::Exception &ex) {
        TEST_CASE_MSG(false, "Caught exception: " << ex.what());
    }
}

// -------------------------------------------------------------
void UnitTest::testByteCodeBasic() {
    Environment::SharedPtr env(new Environment());

    ByteCode::SharedPtr literal(new Literal(Value('c')));
    Value result = literal->eval(env);
    TEST_CASE_MSG(result == Value('c'), "actual=" << result << " (" << result.typeToString() << ')');
    
    ByteCode::SharedPtr definition(new Define("var", literal));
    result = definition->eval(env);
    TEST_CASE_MSG(result == Value('c'), "actual=" << result << " (" << result.typeToString() << ')');

    ByteCode::SharedPtr definition2(new Define("var2", ByteCode::SharedPtr(new Literal(Value(2ll)))));
    result = definition2->eval(env);
    TEST_CASE_MSG(result == Value(2ll), "actual=" << result << " (" << result.typeToString() << ')');
    
    ByteCode::SharedPtr variable(new Variable("var"));
    result = variable->eval(env);
    TEST_CASE_MSG(result == Value('c'), "actual=" << result << " (" << result.typeToString() << ')');
    
    ByteCode::SharedPtr assignment(new Assign("var", ByteCode::SharedPtr(new Variable("var2"))));
    result = assignment->eval(env);
    TEST_CASE_MSG(result == Value(2ll), "actual=" << result << " (" << result.typeToString() << ')');

    ByteCode::SharedPtr exists(new Exists("var"));
    result = exists->eval(env);
    TEST_CASE_MSG(result == Value::True, "actual=" << result << " (" << result.typeToString() << ')');

    ByteCode::SharedPtr doesNotExist(new Exists("novar"));
    result = doesNotExist->eval(env);
    TEST_CASE_MSG(result == Value::False, "actual=" << result << " (" << result.typeToString() << ')');
    
    variable.reset(new Variable("var"));
    result = variable->eval(env);
    TEST_CASE_MSG(result == Value(2ll), "actual=" << result << " (" << result.typeToString() << ')');
}

// -------------------------------------------------------------
void UnitTest::testByteCodeClone() {
    Environment::SharedPtr env(new Environment());

    ByteCode::SharedPtr definition(new Define("var", ByteCode::SharedPtr(new Literal(Value("foobar")))));
    Value result1 = definition->eval(env);

    ByteCode::SharedPtr cloneExpr(new Clone(ByteCode::SharedPtr(new Variable("var"))));
    Value result2 = cloneExpr->eval(env);

    TEST_CASE(result1 == result2);

    auto & text1 = result1.text();
    auto & text2 = result2.text();

    TEST_CASE(text1 == text2);
    TEST_CASE(&text1 != &text2);
}

// -------------------------------------------------------------
void UnitTest::testByteCodeIsType() {
    Environment::SharedPtr env(new Environment());

    Value lambdaValue(
        Lambda(ByteCode::ParamList(), 
               ByteCode::SharedPtr(new Literal(Value::Zero)), 
               env));

    Value structValue(Struct("Person", {"name", "age"}));

    Value instanceValue(Instance(structValue.userType()));

    const std::array<Value::Type, 9> types = 
        { Value::eNone, Value::eInteger, Value::eReal, Value::eCharacter, Value::eBoolean, Value::eString, Value::eClosure, Value::eUserType, Value::eUserObject };

    std::map<ByteCode::SharedPtr, Value::Type> valueTypes; {
        valueTypes.insert(std::make_pair(ByteCode::SharedPtr(new Literal(Value::Null)), Value::eNone));
        valueTypes.insert(std::make_pair(ByteCode::SharedPtr(new Literal(Value(1ll))), Value::eInteger));
        valueTypes.insert(std::make_pair(ByteCode::SharedPtr(new Literal(Value(1.0))), Value::eReal));
        valueTypes.insert(std::make_pair(ByteCode::SharedPtr(new Literal(Value('c'))),  Value::eCharacter));
        valueTypes.insert(std::make_pair(ByteCode::SharedPtr(new Literal(Value::True)), Value::eBoolean));
        valueTypes.insert(std::make_pair(ByteCode::SharedPtr(new Literal(Value("txt"))), Value::eString));
        valueTypes.insert(std::make_pair(ByteCode::SharedPtr(new Literal(lambdaValue)), Value::eClosure));
        valueTypes.insert(std::make_pair(ByteCode::SharedPtr(new Literal(structValue)), Value::eUserType));
        valueTypes.insert(std::make_pair(ByteCode::SharedPtr(new Literal(instanceValue)), Value::eUserObject));
    }

    for (auto valueType : valueTypes) {
        ByteCode::SharedPtr expr = valueType.first;
        Value::Type exprType = valueType.second;

        for (auto type : types) {
            ByteCode::SharedPtr isType(new IsType(expr, type));
            Value result = isType->exec(env);
            bool expected = exprType == type ? true : false;
            TEST_CASE_MSG(result.isBool() && result.boolean() == expected,
                          "actual=" << Value::typeToString(exprType) << " check=" << Value::typeToString(type));
        }
    }
}

// -------------------------------------------------------------
void UnitTest::testByteCodeArithOp() {
    Environment::SharedPtr env(new Environment());
    
    ByteCode::SharedPtr iLit1(new Literal(Value(1ll)));
    ByteCode::SharedPtr iLit2(new Literal(Value(2ll)));
    ByteCode::SharedPtr iLit3(new Literal(Value(0ll)));
    ByteCode::SharedPtr iLit4(new Literal(Value(4ll)));
    ByteCode::SharedPtr iLit5(new Literal(Value(5ll)));
    ByteCode::SharedPtr rLit1(new Literal(Value(1.2)));
    ByteCode::SharedPtr rLit2(new Literal(Value(1.3)));
    ByteCode::SharedPtr rLit3(new Literal(Value(0.0)));
    ByteCode::SharedPtr cLit(new Literal(Value('c')));
    ByteCode::SharedPtr bLit(new Literal(Value(true)));
    
    ByteCode::SharedPtr arith(new ArithOp(ArithOp::Add, iLit1, iLit2));
    Value result = arith->eval(env);
    TEST_CASE_MSG(result.isInt(), "actual=" << result.typeToString());
    TEST_CASE_MSG(result.integer() == 3ll, "actual=" << result);

    arith.reset(new ArithOp(ArithOp::Add, iLit1, rLit1));
    result = arith->eval(env);
    TEST_CASE_MSG(result.isReal(), "actual=" << result.typeToString());
    TEST_CASE_MSG(Util::isEqual(result.real(), 2.2), "actual=" << result);
    
    arith.reset(new ArithOp(ArithOp::Sub, rLit2, rLit1));
    result = arith->eval(env);
    TEST_CASE_MSG(result.isReal(), "actual=" << result.typeToString());
    TEST_CASE_MSG(Util::isEqual(result.real(), 0.1), "actual=" << result);
    
    arith.reset(new ArithOp(ArithOp::Mul, rLit1, iLit2));
    result = arith->eval(env);
    TEST_CASE_MSG(result.isReal(), "actual=" << result.typeToString());
    TEST_CASE_MSG(Util::isEqual(result.real(), 2.4), "actual=" << result);
    
    arith.reset(new ArithOp(ArithOp::Div, iLit1, iLit2));
    result = arith->eval(env);
    TEST_CASE_MSG(result.isInt(), "actual=" << result.typeToString());
    TEST_CASE_MSG(result.integer() == 0, "actual=" << result);
    
    arith.reset(new ArithOp(ArithOp::Div, rLit1, iLit2));
    result = arith->eval(env);
    TEST_CASE_MSG(result.isReal(), "actual=" << result.typeToString());
    TEST_CASE_MSG(Util::isEqual(result.real(), 0.6), "actual=" << result);

    arith.reset(new ArithOp(ArithOp::Mod, iLit4, iLit2));
    result = arith->eval(env);
    TEST_CASE_MSG(result.isInt(), "actual=" << result.typeToString());
    TEST_CASE_MSG(Util::isEqual(result.integer(), 0), "actual" << result);

    arith.reset(new ArithOp(ArithOp::Mod, iLit5, iLit2));
    result = arith->eval(env);
    TEST_CASE_MSG(result.isInt(), "actual=" << result.typeToString());
    TEST_CASE_MSG(Util::isEqual(result.integer(), 1), "actual" << result);
    
    try {
        arith.reset(new ArithOp(ArithOp::Div, iLit1, iLit3));
        arith->eval(env);
        TEST_CASE(false);
    }
    catch (const DivByZero &ex) {}
    catch (...) { TEST_CASE(false); }
    
    try {
        arith.reset(new ArithOp(ArithOp::Div, rLit1, rLit3));
        arith->eval(env);
        TEST_CASE(false);
    }
    catch (const DivByZero &ex) {}
    catch (...) { TEST_CASE(false); }
    
    try {
        arith.reset(new ArithOp(ArithOp::Add, iLit1, cLit));
        arith->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &ex) {}
    catch (...) { TEST_CASE(false); }

    try {
        arith.reset(new ArithOp(ArithOp::Add, bLit, rLit2));
        arith->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &ex) {}
    catch (...) { TEST_CASE(false); }

    try {
        arith.reset(new ArithOp(ArithOp::Add, bLit, cLit));
        arith->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &ex) {}
    catch (...) { TEST_CASE(false); }

    try {
        arith.reset(new ArithOp(ArithOp::Mod, rLit2, iLit1));
        arith->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &ex) {}
    catch (...) { TEST_CASE(false); }

    try {
        arith.reset(new ArithOp(ArithOp::Mod, iLit1, rLit2));
        arith->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &ex) {}
    catch (...) { TEST_CASE(false); }

    try {
        arith.reset(new ArithOp(ArithOp::Mod, rLit1, rLit2));
        arith->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &ex) {}
    catch (...) { TEST_CASE(false); }
}

// -------------------------------------------------------------
#define TEST_COMP(TYPE, LHS, RHS, RVALUE) \
    comp.reset(new CompOp(CompOp::TYPE, LHS, RHS)); \
    result = comp->eval(env); \
    TEST_CASE_MSG(result.isBool(), "actual=" << result.typeToString()); \
    TEST_CASE_MSG(result.boolean() == RVALUE, "actual=" << result);
void UnitTest::testByteCodeCompOp() {
    Environment::SharedPtr env(new Environment());
    
    ByteCode::SharedPtr iLit0(new Literal(Value(0ll)));
    ByteCode::SharedPtr iLit1(new Literal(Value(1ll)));
    ByteCode::SharedPtr rLit0(new Literal(Value(0.0)));
    ByteCode::SharedPtr rLit1(new Literal(Value(1.0)));
    ByteCode::SharedPtr cLitA(new Literal(Value('a')));
    ByteCode::SharedPtr cLitB(new Literal(Value('b')));
    ByteCode::SharedPtr bLitT(new Literal(Value(true)));
    ByteCode::SharedPtr bLitF(new Literal(Value(false)));

    ByteCode::SharedPtr comp;
    Value result;
    
    TEST_COMP(EQ, iLit0, iLit0, true);
    TEST_COMP(EQ, iLit0, iLit1, false);
    TEST_COMP(EQ, rLit1, rLit1, true);
    TEST_COMP(EQ, rLit0, rLit1, false);
    TEST_COMP(EQ, cLitA, cLitA, true);
    TEST_COMP(EQ, cLitA, cLitB, false);
    TEST_COMP(EQ, bLitT, bLitT, true);
    TEST_COMP(EQ, bLitT, bLitF, false);
    
    TEST_COMP(NE, iLit0, iLit0, false);
    TEST_COMP(NE, iLit0, iLit1, true);
    TEST_COMP(NE, rLit1, rLit1, false);
    TEST_COMP(NE, rLit0, rLit1, true);
    TEST_COMP(NE, cLitA, cLitA, false);
    TEST_COMP(NE, cLitA, cLitB, true);
    TEST_COMP(NE, bLitT, bLitT, false);
    TEST_COMP(NE, bLitT, bLitF, true);

    TEST_COMP(LT, iLit0, iLit0, false);
    TEST_COMP(LT, iLit0, iLit1, true);
    TEST_COMP(LT, rLit1, rLit1, false);
    TEST_COMP(LT, rLit0, rLit1, true);
    TEST_COMP(LT, cLitA, cLitA, false);
    TEST_COMP(LT, cLitA, cLitB, true);
    TEST_COMP(LT, bLitT, bLitT, false);
    TEST_COMP(LT, bLitF, bLitT, true);

    TEST_COMP(GT, iLit0, iLit0, false);
    TEST_COMP(GT, iLit1, iLit0, true);
    TEST_COMP(GT, rLit1, rLit1, false);
    TEST_COMP(GT, rLit1, rLit0, true);
    TEST_COMP(GT, cLitA, cLitA, false);
    TEST_COMP(GT, cLitB, cLitA, true);
    TEST_COMP(GT, bLitT, bLitT, false);
    TEST_COMP(GT, bLitT, bLitF, true);

    TEST_COMP(LE, iLit0, iLit0, true);
    TEST_COMP(LE, iLit0, iLit1, true);
    TEST_COMP(LE, rLit1, rLit1, true);
    TEST_COMP(LE, rLit0, rLit1, true);
    TEST_COMP(LE, cLitA, cLitA, true);
    TEST_COMP(LE, cLitA, cLitB, true);
    TEST_COMP(LE, bLitT, bLitT, true);
    TEST_COMP(LE, bLitF, bLitT, true);

    TEST_COMP(GE, iLit0, iLit0, true);
    TEST_COMP(GE, iLit1, iLit0, true);
    TEST_COMP(GE, rLit1, rLit1, true);
    TEST_COMP(GE, rLit1, rLit0, true);
    TEST_COMP(GE, cLitA, cLitA, true);
    TEST_COMP(GE, cLitB, cLitA, true);
    TEST_COMP(GE, bLitT, bLitT, true);
    TEST_COMP(GE, bLitT, bLitF, true);
    
    TEST_COMP(EQ, iLit1, rLit1, true);
    TEST_COMP(NE, iLit1, rLit0, true);
    TEST_COMP(LT, iLit0, rLit1, true);
    TEST_COMP(GT, iLit1, rLit0, true);
    TEST_COMP(LE, iLit1, rLit1, true);
    TEST_COMP(GE, iLit1, rLit1, true);
    
    try {
        comp.reset(new CompOp(CompOp::EQ, iLit0, cLitA));
        result = comp->eval(env);
        TEST_CASE(false);
    }
    catch (const IncompatibleTypes &ex) {}
    catch (...) { TEST_CASE(false); }
    
    try {
        comp.reset(new CompOp(CompOp::NE, iLit0, cLitA));
        result = comp->eval(env);
        TEST_CASE(false);
    }
    catch (const IncompatibleTypes &ex) {}
    catch (...) { TEST_CASE(false); }

    try {
        comp.reset(new CompOp(CompOp::LT, iLit0, cLitA));
        result = comp->eval(env);
        TEST_CASE(false);
    }
    catch (const IncompatibleTypes &ex) {}
    catch (...) { TEST_CASE(false); }

    try {
        comp.reset(new CompOp(CompOp::GT, iLit0, cLitA));
        result = comp->eval(env);
        TEST_CASE(false);
    }
    catch (const IncompatibleTypes &ex) {}
    catch (...) { TEST_CASE(false); }

    try {
        comp.reset(new CompOp(CompOp::LE, iLit0, cLitA));
        result = comp->eval(env);
        TEST_CASE(false);
    }
    catch (const IncompatibleTypes &ex) {}
    catch (...) { TEST_CASE(false); }

    try {
        comp.reset(new CompOp(CompOp::GE, iLit0, cLitA));
        result = comp->eval(env);
        TEST_CASE(false);
    }
    catch (const IncompatibleTypes &ex) {}
    catch (...) { TEST_CASE(false); }
}
#undef TEST_COMP

// -------------------------------------------------------------
void UnitTest::testByteCodeLogicOp() {
    Environment::SharedPtr env(new Environment());
    
    struct Case {
        LogicOp::Type type;
        bool lhs;
        bool rhs;
        bool result;
        bool other;
        bool error;
        
        std::string toString() const {
            std::string str;
            str += static_cast<char>(type);
            str += (lhs ? " true" : " false");
            str += (rhs ? " true" : " false");
            str += (result ? " true" : " false");
            str += (other ? " true" : " false");
            str += (error ? " true" : " false");
            return str;
        }
    } cases[] = {
        { LogicOp::Conjunction, true,  true,  true,  false, false },
        { LogicOp::Conjunction, true,  false, false, false, false },
        { LogicOp::Conjunction, false, true,  false, false, false },
        { LogicOp::Conjunction, false, false, false, false, false },
        { LogicOp::Conjunction, true,  true,  false, true,  true  },
        { LogicOp::Disjunction, true,  true,  true,  false, false },
        { LogicOp::Disjunction, true,  false, true,  false, false },
        { LogicOp::Disjunction, false, true,  true,  false, false },
        { LogicOp::Disjunction, false, false, false, false, false },
        { LogicOp::Disjunction, true,  true,  false, true,  true }
    };
    const size_t size = sizeof(cases)/sizeof(cases[0]);
    for (int i = 0; i < size; ++i) {
        const Case &c = cases[i];
        
        try {
            ByteCode::SharedPtr lhsLit(new Literal(Value(c.lhs)));
            ByteCode::SharedPtr rhsLit(new Literal(!c.other ? Value(c.rhs) : Value('a')));
            ByteCode::SharedPtr logic(new LogicOp(c.type, lhsLit, rhsLit));
            Value result = logic->eval(env);
            if (!c.error) {
                TEST_CASE_MSG(result.isBool(), "actual" << result.typeToString() << " (" << c.toString() << ')');
                TEST_CASE_MSG(result.boolean() == c.result, "actual=" << result << " (" << c.toString() << ')');
            }
            else {
                TEST_CASE_MSG(false, c.toString());
            }
        }
        catch (const InvalidOperandType &ex) { if (!c.error) { TEST_CASE_MSG(false, c.toString()); } }
        catch (...) { TEST_CASE_MSG(false, c.toString()); }
    }
}

// -------------------------------------------------------------
void UnitTest::testByteCodeSequence() {
    Environment::SharedPtr env(new Environment());
    
    env->def("x", Value(1ll));
    env->def("y", Value(2ll));

    { // Block
        ByteCode::SharedPtr def1(new Define("x", ByteCode::SharedPtr(new Literal(Value(1ll)))));
        ByteCode::SharedPtr def2(new Define("y", ByteCode::SharedPtr(new Literal(Value(1ll)))));
        ByteCode::SharedPtr assign1(new Assign("x", ByteCode::SharedPtr(new Variable("y"))));
        ByteCode::SharedPtr assign2(new Assign("y", ByteCode::SharedPtr(new Literal(Value(3ll)))));
        ByteCode::SharedPtr litT(new Literal(Value('T')));

        ByteCode::SharedPtrList exprs;
        exprs.push_back(def1);
        exprs.push_back(def2);
        exprs.push_back(assign1);
        exprs.push_back(assign2);
        exprs.push_back(litT);

        ByteCode::SharedPtr block(new Block(exprs));
        Value result = block->eval(env);
        TEST_CASE_MSG(result.isChar(), "actual=" << result.typeToString());
        TEST_CASE_MSG(result.character() == 'T', "actual=" << result);
        TEST_CASE_MSG(env->get("x") == Value(1ll), "actual=" << env->get("x"));
        TEST_CASE_MSG(env->get("y") == Value(2ll), "actual=" << env->get("y"));
    }

    { // ProgN
        ByteCode::SharedPtr assign1(new Assign("x", ByteCode::SharedPtr(new Variable("y"))));
        ByteCode::SharedPtr assign2(new Assign("y", ByteCode::SharedPtr(new Literal(Value(3ll)))));
        ByteCode::SharedPtr litT(new Literal(Value('T')));

        ByteCode::SharedPtrList exprs;
        exprs.push_back(assign1);
        exprs.push_back(assign2);
        exprs.push_back(litT);

        ByteCode::SharedPtr progN(new ProgN(exprs));
        Value result = progN->eval(env);
        TEST_CASE_MSG(result.isChar(), "actual=" << result.typeToString());
        TEST_CASE_MSG(result.character() == 'T', "actual=" << result);
        TEST_CASE_MSG(env->get("x") == Value(2ll), "actual=" << env->get("x"));
        TEST_CASE_MSG(env->get("y") == Value(3ll), "actual=" << env->get("y"));
    }
}

// -------------------------------------------------------------
void UnitTest::testByteCodeCond() {
    { // If
        Environment::SharedPtr env(new Environment());

        {
            ByteCode::SharedPtr bLitT(new Literal(Value::True));
            ByteCode::SharedPtr bLitF(new Literal(Value::False));
            ByteCode::SharedPtr iLit1(new Literal(Value(1ll)));
            ByteCode::SharedPtr iLit2(new Literal(Value(2ll)));

            ByteCode::SharedPtr cond(new If(bLitT, iLit1, iLit2));
            Value result = cond->eval(env);
            TEST_CASE_MSG(result.isInt(), "actual=" << result.typeToString());
            TEST_CASE_MSG(result.integer() == 1ll, "actual=" << result);

            cond.reset(new If(bLitF, iLit1, iLit2));
            result = cond->eval(env);
            TEST_CASE_MSG(result.isInt(), "actual=" << result.typeToString());
            TEST_CASE_MSG(result.integer() == 2ll, "actual=" << result);
        }
    
        {
            env->def("x", Value(1ll));
            env->def("y", Value(2ll));

            ByteCode::SharedPtr tPred(new Literal(Value::True));
            ByteCode::SharedPtr fPred(new Literal(Value::False));
            ByteCode::SharedPtr tCode(new Define("x", ByteCode::SharedPtr(new Literal(Value(3ll)))));
            ByteCode::SharedPtr fCode(new Assign("y", ByteCode::SharedPtr(new Literal(Value(4ll)))));

            ByteCode::SharedPtr cond(new If(tPred, tCode, fCode));
            Value result = cond->eval(env);
            TEST_CASE_MSG(result.isInt(), "actual=" << result.typeToString());
            TEST_CASE_MSG(result.integer() == 3ll, "actual=" << result);
            TEST_CASE_MSG(env->get("x") == Value(1ll), "actual=" << env->get("x"));
            TEST_CASE_MSG(env->get("y") == Value(2ll), "actual=" << env->get("y"));

            cond.reset(new If(fPred, tCode, fCode));
            result = cond->eval(env);
            TEST_CASE_MSG(result.isInt(), "actual=" << result.typeToString());
            TEST_CASE_MSG(result.integer() == 4ll, "actual=" << result);
            TEST_CASE_MSG(env->get("x") == Value(1ll), "actual=" << env->get("x"));
            TEST_CASE_MSG(env->get("y") == Value(4ll), "actual=" << env->get("y"));
        }
    }
    
    { // Cond
        Environment::SharedPtr env(new Environment());
        
        {
            ByteCode::SharedPtrPairs cases;
            cases.push_back(
                std::make_pair(
                    ByteCode::SharedPtr(new Literal(Value::False)),
                    ByteCode::SharedPtr(new Literal(Value(1ll)))));
            cases.push_back(
                std::make_pair(
                    ByteCode::SharedPtr(new Literal(Value::True)),
                    ByteCode::SharedPtr(new Literal(Value(2ll)))));
            ByteCode::SharedPtr cond(new Cond(cases));
            Value result = cond->eval(env);
            TEST_CASE_MSG(result.isInt(), "actual=" << result.typeToString());
            TEST_CASE_MSG(result.integer() == 2ll, "actual=" << result);
        }

        {
            ByteCode::SharedPtrPairs cases;
            cases.push_back(
                std::make_pair(
                    ByteCode::SharedPtr(new Literal(Value::False)),
                    ByteCode::SharedPtr(new Literal(Value(1ll)))));
            cases.push_back(
                std::make_pair(
                    ByteCode::SharedPtr(new Literal(Value::True)),
                    ByteCode::SharedPtr()));
            ByteCode::SharedPtr cond(new Cond(cases));
            Value result = cond->eval(env);
            TEST_CASE_MSG(result.isNull(), "actual=" << result.typeToString());
        }
        
        {
            env->def("x", Value(3ll));
            ByteCode::SharedPtr var(new Variable("x"));
            
            ByteCode::SharedPtrPairs cases;
            cases.push_back(
                std::make_pair(
                    ByteCode::SharedPtr(new CompOp(CompOp::EQ, var, ByteCode::SharedPtr(new Literal(Value(1ll))))),
                    ByteCode::SharedPtr(new Literal(Value('a')))));
            cases.push_back(
                std::make_pair(
                    ByteCode::SharedPtr(new CompOp(CompOp::EQ, var, ByteCode::SharedPtr(new Literal(Value(2ll))))),
                    ByteCode::SharedPtr(new Literal(Value('b')))));
            cases.push_back(
                std::make_pair(
                    ByteCode::SharedPtr(new CompOp(CompOp::EQ, var, ByteCode::SharedPtr(new Literal(Value(3ll))))),
                    ByteCode::SharedPtr(new Literal(Value('c')))));
            cases.push_back(
                std::make_pair(
                    ByteCode::SharedPtr(new CompOp(CompOp::EQ, var, ByteCode::SharedPtr(new Literal(Value(4ll))))),
                    ByteCode::SharedPtr(new Literal(Value('d')))));
            cases.push_back(
                std::make_pair(
                    ByteCode::SharedPtr(new Literal(Value::True)),
                    ByteCode::SharedPtr(new Literal(Value('e')))));
            ByteCode::SharedPtr cond(new Cond(cases));
            Value result = cond->eval(env);
            TEST_CASE_MSG(result.isChar(), "actual=" << result.typeToString());
            TEST_CASE_MSG(result.character() == 'c', "actual=" << result);
        }
        
        {
            try {
                ByteCode::SharedPtrPairs cases;
                cases.push_back(std::make_pair(ByteCode::SharedPtr(), ByteCode::SharedPtr()));
                ByteCode::SharedPtr cond(new Cond(cases));
                cond->eval(env);
                TEST_CASE(false);
            }
            catch (const InvalidExpressionType &ex) {}
            catch (...) { TEST_CASE(false); }
        }
        
        {
            try {
                ByteCode::SharedPtrPairs cases;
                cases.push_back(std::make_pair(ByteCode::SharedPtr(new Literal(Value::Zero)), ByteCode::SharedPtr()));
                ByteCode::SharedPtr cond(new Cond(cases));
                cond->eval(env);
                TEST_CASE(false);
            }
            catch (const InvalidExpressionType &ex) {}
            catch (...) { TEST_CASE(false); }
        }
    }
}

// -------------------------------------------------------------
void UnitTest::testByteCodeLoop() {
    Environment::SharedPtr env(new Environment());

    { // For
        env->def("sum", Value(0ll));

        ByteCode::SharedPtr loop(
            new Loop(
                ByteCode::SharedPtr(
                    new Define("i", ByteCode::SharedPtr(new Literal(Value(1ll))))),
                ByteCode::SharedPtr(
                    new CompOp(CompOp::LE, 
                               ByteCode::SharedPtr(new Variable("i")),
                               ByteCode::SharedPtr(new Literal(Value(10ll))))),
                ByteCode::SharedPtr(
                    new Assign("i", ByteCode::SharedPtr(
                                        new ArithOp(ArithOp::Add,
                                                    ByteCode::SharedPtr(new Variable("i")),
                                                    ByteCode::SharedPtr(new Literal(Value(1ll))))))),
                ByteCode::SharedPtr(
                    new Assign("sum", ByteCode::SharedPtr(
                                        new ArithOp(ArithOp::Add,
                                                    ByteCode::SharedPtr(new Variable("sum")),
                                                    ByteCode::SharedPtr(new Variable("i"))))))));
        Value result = loop->eval(env);
        TEST_CASE_MSG(result.isInt(), "actual=" << result.typeToString());
        TEST_CASE_MSG(result.integer() == 55ll, "actual=" << result);
        
        try {
            env->get("i");
            TEST_CASE(false);
        }
        catch (const UnknownSymbol &ex) {}
        catch (...) { TEST_CASE(false); }
    }
    
    { // While
        env->def("count", Value(1ll));
        
        ByteCode::SharedPtr loop(
                new Loop(
                    ByteCode::SharedPtr(
                        new CompOp(CompOp::LT,
                                   ByteCode::SharedPtr(new Variable("count")),
                                   ByteCode::SharedPtr(new Literal(Value(32ll))))),
                    ByteCode::SharedPtr(
                        new Assign("count", ByteCode::SharedPtr(
                                                new ArithOp(ArithOp::Mul,
                                                            ByteCode::SharedPtr(new Variable("count")),
                                                            ByteCode::SharedPtr(new Literal(Value(2ll)))))))));
        Value result = loop->eval(env);
        TEST_CASE_MSG(result.isInt(), "actual=" << result.typeToString());
        TEST_CASE_MSG(result.integer() == 32ll, "actual=" << result);
    }
    
    { // Break
        env->def("x", Value(0ll));

        ByteCode::SharedPtrList exprs;
        exprs.push_back(
            ByteCode::SharedPtr(
                new Assign("x", ByteCode::SharedPtr(
                                    new ArithOp(ArithOp::Add,
                                                ByteCode::SharedPtr(new Variable("x")),
                                                ByteCode::SharedPtr(new Literal(Value(1ll))))))));
        exprs.push_back(
            ByteCode::SharedPtr(
                new If(ByteCode::SharedPtr(
                           new CompOp(CompOp::EQ,
                                      ByteCode::SharedPtr(new Variable("x")),
                                      ByteCode::SharedPtr(new Literal(Value(2ll))))),
                       ByteCode::SharedPtr(new Break()),
                       ByteCode::SharedPtr())));

        ByteCode::SharedPtr loop(
                new Loop(
                    ByteCode::SharedPtr(new Literal(Value::True)),
                    ByteCode::SharedPtr(new ProgN(exprs))));

        Value result = loop->eval(env);
        TEST_CASE_MSG(result.isNull(), "actual=" << result.typeToString());
        TEST_CASE_MSG(env->get("x") == Value(2ll), "actual=" << env->get("x"));
    }
}

// -------------------------------------------------------------
void UnitTest::testByteCodeLambdaExpr() {
    Environment::SharedPtr env(new Environment());
    env->def("foo", Value(1ll));

    const Lambda::ParamList params({"x", "y"});

    ByteCode::SharedPtr body(
        new ArithOp(ArithOp::Add,
                    ByteCode::SharedPtr(
                        new ArithOp(ArithOp::Add,
                                    ByteCode::SharedPtr(new Variable("x")),
                                    ByteCode::SharedPtr(new Variable("y")))),
                    ByteCode::SharedPtr(new Variable("foo"))));

    ByteCode::SharedPtr lambdaExpr(new LambdaExpr(params, body));
    Value lambdaVal = lambdaExpr->exec(env);
    TEST_CASE_MSG(lambdaVal.isClosure(), "actual=" << lambdaVal.typeToString());

    Lambda::ArgList args;
    args.push_back(Value(3ll));
    args.push_back(Value(4ll));

    const Lambda &func = lambdaVal.closure();
    Value result = func.exec(args);
    TEST_CASE_MSG(result.isInt(), "actual=" << result.typeToString());
    TEST_CASE_MSG(result.integer() == 8ll, "actual=" << result);
}

// -------------------------------------------------------------
void UnitTest::testByteCodeLambdaApp() {
    Environment::SharedPtr env(new Environment());

    const Lambda::ParamList params({"x", "y"});

    ByteCode::SharedPtr body(
        new ArithOp(ArithOp::Add,
                    ByteCode::SharedPtr(new Variable("x")),
                    ByteCode::SharedPtr(new Variable("y"))));

    ByteCode::SharedPtr lambdaExpr(new LambdaExpr(params, body));

    ByteCode::SharedPtrList args;
    args.push_back(ByteCode::SharedPtr(new Literal(Value(3ll))));
    args.push_back(ByteCode::SharedPtr(new Literal(Value(4ll))));
    ByteCode::SharedPtr appExpr(new LambdaApp(lambdaExpr, args));

    Value result = appExpr->exec(env);
    TEST_CASE_MSG(result.isInt(), "actual=" << result.typeToString());
    TEST_CASE_MSG(result.integer() == 7ll, "actual=" << result);
}

// -------------------------------------------------------------
void UnitTest::testByteCodeFunctionExpr() {
    Environment::SharedPtr env(new Environment());

    const Lambda::ParamList params({"x", "y"});

    ByteCode::SharedPtr body(
        new ArithOp(ArithOp::Add,
                    ByteCode::SharedPtr(new Variable("x")),
                    ByteCode::SharedPtr(new Variable("y"))));

    ByteCode::SharedPtr ftnExpr(new FunctionExpr("myfunc", params, body));
    Value ftnVal = ftnExpr->exec(env);
    TEST_CASE_MSG(ftnVal.isClosure(), "actual=" << ftnVal.typeToString());

    Lambda::ArgList args;
    args.push_back(Value(3ll));
    args.push_back(Value(4ll));

    const Value &ftn = env->get("myfunc");
    TEST_CASE_MSG(ftn.isClosure(), "actual=" << ftn.typeToString());

    Value result = ftn.closure().exec(args);
    TEST_CASE_MSG(result.isInt(), "actual=" << result.typeToString());
    TEST_CASE_MSG(result.integer() == 7ll, "actual=" << result);
}

// -------------------------------------------------------------
void UnitTest::testByteCodeFunctionApp() {
    Environment::SharedPtr env(new Environment());

    ByteCode::SharedPtr ftnExpr(
        std::make_shared<FunctionExpr>(
            "myfunc",
            Lambda::ParamList({"x", "y"}),
            std::make_shared<ArithOp>(
                ArithOp::Add,
                std::make_shared<Variable>("x"),
                std::make_shared<Variable>("y"))));
    Value ftnVal = ftnExpr->exec(env);
    TEST_CASE_MSG(ftnVal.isClosure(), "actual=" << ftnVal.typeToString());

    ByteCode::SharedPtr ftnApp(
        std::make_shared<FunctionApp>(
            "myfunc",
            ByteCode::SharedPtrList({
                    std::make_shared<Literal>(Value(3ll)),
                        std::make_shared<Literal>(Value(4ll))})));

    Value result = ftnApp->exec(env);
    TEST_CASE_MSG(result.isInt(), "actual=" << result.typeToString());
    TEST_CASE_MSG(result.integer() == 7ll, "actual=" << result);    
}

// -------------------------------------------------------------
void UnitTest::testByteCodeStruct() {
    Environment::SharedPtr env(new Environment());

    ByteCode::SharedPtr structExpr(
        std::make_shared<StructExpr>("Person", Struct::MemberList({"name", "age"})));
    Value structVal = structExpr->exec(env);
    TEST_CASE_MSG(structVal.isUserType(), "actual=" << structVal.typeToString());

    const Struct &s = structVal.userType();
    TEST_CASE_MSG(s.name() == "Person", "actual=" << s.name());
    TEST_CASE_MSG(s.members().size() == 2, "actual=" << s.members().size());
    TEST_CASE_MSG(s.members()[0] == "name", "actual=" << s.members()[0]);
    TEST_CASE_MSG(s.members()[1] == "age", "actual=" << s.members()[1]);
}

// -------------------------------------------------------------
void UnitTest::testByteCodeIsStructName() {
    Environment::SharedPtr env(new Environment());

    ByteCode::SharedPtr isStructName(
        std::make_shared<IsStructName>(
            std::make_shared<StructExpr>("Person", Struct::MemberList({"name", "age"})),
            "Person"));

    Value result = isStructName->exec(env);
    TEST_CASE_MSG(result.isBool(), "actual=" << result.typeToString());
    TEST_CASE_MSG(result == Value::True, "actual=" << result << " (" << result.typeToString() << ')');
}

// -------------------------------------------------------------
void UnitTest::testByteCodeMakeInstance() {
    Environment::SharedPtr env(new Environment());

    ByteCode::SharedPtr structExpr(
        std::make_shared<StructExpr>("Person", Struct::MemberList({"name", "age"})));

    Value structValue = structExpr->exec(env);
    TEST_CASE_MSG(structValue.isUserType(), "actual=" << structValue.typeToString());
    TEST_CASE_MSG(structValue.userType().name() == "Person", "actual=" << structValue.userType().name());

    ByteCode::SharedPtr makeInstance(
        std::make_shared<MakeInstance>("Person"));

    Value instValue = makeInstance->exec(env);
    TEST_CASE_MSG(instValue.isUserObject(), "actual=" << instValue.typeToString());

    const Struct &type = instValue.userObject().type();
    TEST_CASE_MSG(type.name() == "Person", "actual=" << type.name());

    const Instance &inst = instValue.userObject();
    TEST_CASE_MSG(inst.get("name") == Value::Null, "actual=" << inst.get("name"));
    TEST_CASE_MSG(inst.get("age") == Value::Null, "actual=" << inst.get("age"));
}

// -------------------------------------------------------------
void UnitTest::testByteCodeIsInstanceOf() {
    Environment::SharedPtr env(new Environment());

    std::make_shared<StructExpr>("Person", Struct::MemberList({"name", "age"}))
        ->exec(env);

    Value value = 
        std::make_shared<IsInstanceOf>(
            std::make_shared<MakeInstance>("Person"),
            "Person")
        ->exec(env);
    TEST_CASE_MSG(value == Value::True, "actual=" << value);
}

// -------------------------------------------------------------
void UnitTest::testByteCodeGetSetMember() {
    Environment::SharedPtr env(new Environment());

    std::make_shared<StructExpr>("Person", Struct::MemberList({"name", "age"}))
        ->exec(env);

    std::make_shared<Define>(
        "inst",
        std::make_shared<MakeInstance>("Person"))
        ->exec(env);

    Value value = 
        std::make_shared<IsInstanceOf>(
            std::make_shared<Variable>("inst"),
            "Person")
        ->exec(env);
    TEST_CASE_MSG(value == Value::True, "actual=" << value);

    value = 
        std::make_shared<GetMember>(
            std::make_shared<Variable>("inst"),
            "name")
        ->exec(env);
    TEST_CASE_MSG(value == Value::Null, "actual=" << value);

    value = 
        std::make_shared<SetMember>(
            std::make_shared<Variable>("inst"),
            "name",
            std::make_shared<Literal>(Value("John")))
        ->exec(env);
    TEST_CASE_MSG(value == Value("John"), "actual=" << value);

    value = 
        std::make_shared<GetMember>(
            std::make_shared<Variable>("inst"),
            "name")
        ->exec(env);
    TEST_CASE_MSG(value == Value("John"), "actual=" << value);
}

// -------------------------------------------------------------
void UnitTest::testByteCodeStringLen() {
    Environment::SharedPtr env(new Environment());

    Value value =
        std::make_shared<StringLen>(
            std::make_shared<Literal>(Value("")))
        ->exec(env);
    TEST_CASE_MSG(value == Value::Zero, "actual=" << value);

    value =
        std::make_shared<StringLen>(
            std::make_shared<Literal>("a"))
        ->exec(env);
    TEST_CASE_MSG(value == Value(1ll), "actual=" << value);

    value =
        std::make_shared<StringLen>(
            std::make_shared<Literal>("abcde"))
        ->exec(env);
    TEST_CASE_MSG(value == Value(5ll), "actual=" << value);

    try {
        value =
            std::make_shared<StringLen>(
                std::make_shared<Literal>(Value::Zero))
            ->exec(env);
        TEST_CASE(false);
    }
    catch (InvalidOperandType const &) {}
    catch (...) { TEST_CASE(false); }
}

// -------------------------------------------------------------
void UnitTest::testByteCodeCharAt() {
    Environment::SharedPtr env(new Environment());

    Value value =
        std::make_shared<GetCharAt>(
            std::make_shared<Literal>(Value("abc")),
            std::make_shared<Literal>(Value(0ll)))
        ->exec(env);
    TEST_CASE_MSG(value == Value('a'), "actual=" << value);

    value =
        std::make_shared<GetCharAt>(
            std::make_shared<Literal>(Value("abc")),
            std::make_shared<Literal>(Value(1ll)))
        ->exec(env);
    TEST_CASE_MSG(value == Value('b'), "actual=" << value);

    value =
        std::make_shared<GetCharAt>(
            std::make_shared<Literal>(Value("abc")),
            std::make_shared<Literal>(Value(2ll)))
        ->exec(env);
    TEST_CASE_MSG(value == Value('c'), "actual=" << value);

    try {
        value =
            std::make_shared<GetCharAt>(
                std::make_shared<Literal>(Value("abc")),
                std::make_shared<Literal>(Value(3ll)))
            ->exec(env);
        TEST_CASE(false);
    }
    catch (OutOfRange const &) {}
    catch (...) { TEST_CASE(false); }

    try {
        value =
            std::make_shared<GetCharAt>(
                std::make_shared<Literal>(Value("abc")),
                std::make_shared<Literal>(Value(-1ll)))
            ->exec(env);
        TEST_CASE(false);
    }
    catch (OutOfRange const &) {}
    catch (...) { TEST_CASE(false); }
}

// -------------------------------------------------------------
void UnitTest::testByteCodeSetCharAt() {
    Environment::SharedPtr env(new Environment());
    env->def("str", Value("abc"));

    ByteCode::SharedPtr var = std::make_shared<Variable>("str");

    Value value =
        std::make_shared<SetCharAt>(
            var,
            std::make_shared<Literal>(Value(0ll)),
            std::make_shared<Literal>(Value('A')))
        ->exec(env);
    TEST_CASE_MSG(value == Value('A'), "actual=" << value);

    value = var->exec(env);
    TEST_CASE_MSG(value == Value("Abc"), "actual=" << value);

    value =
        std::make_shared<SetCharAt>(
            var,
            std::make_shared<Literal>(Value(1ll)),
            std::make_shared<Literal>(Value('B')))
        ->exec(env);
    TEST_CASE_MSG(value == Value('B'), "actual=" << value);

    value = var->exec(env);
    TEST_CASE_MSG(value == Value("ABc"), "actual=" << value);

    value =
        std::make_shared<SetCharAt>(
            var,
            std::make_shared<Literal>(Value(2ll)),
            std::make_shared<Literal>(Value('C')))
        ->exec(env);
    TEST_CASE_MSG(value == Value('C'), "actual=" << value);

    value = var->exec(env);
    TEST_CASE_MSG(value == Value("ABC"), "actual=" << value);

    try {
        value =
            std::make_shared<SetCharAt>(
                var,
                std::make_shared<Literal>(Value(-1ll)),
                std::make_shared<Literal>(Value('X')))
            ->exec(env);
        TEST_CASE(false);
    }
    catch (OutOfRange const &) {}
    catch (...) { TEST_CASE(false); }

    try {
        value =
            std::make_shared<SetCharAt>(
                var,
                std::make_shared<Literal>(Value(3ll)),
                std::make_shared<Literal>(Value('Y')))
            ->exec(env);
        TEST_CASE(false);
    }
    catch (OutOfRange const &) {}
    catch (...) { TEST_CASE(false); }

    value = var->exec(env);
    TEST_CASE_MSG(value == Value("ABC"), "actual=" << value);
}

// -------------------------------------------------------------
void UnitTest::testTokenType() {
    TEST_CASE(Lexer::tokenType("") == Lexer::Unknown);
    TEST_CASE(Lexer::tokenType("'") == Lexer::Unknown);
    TEST_CASE(Lexer::tokenType("''") == Lexer::Unknown);
    TEST_CASE(Lexer::tokenType("\"") == Lexer::Unknown);
    TEST_CASE(Lexer::tokenType("_") == Lexer::Unknown);
    TEST_CASE(Lexer::tokenType(".") == Lexer::Unknown);
    TEST_CASE(Lexer::tokenType("9ABC") == Lexer::Unknown);
    TEST_CASE(Lexer::tokenType("ABC-2") == Lexer::Unknown);
    TEST_CASE(Lexer::tokenType("#ABC") == Lexer::Unknown);
    TEST_CASE(Lexer::tokenType("ABC/3") == Lexer::Unknown);
    TEST_CASE(Lexer::tokenType("A@BC3") == Lexer::Unknown);

    TEST_CASE(Lexer::tokenType("-") == Lexer::Symbol);
    TEST_CASE(Lexer::tokenType("+") == Lexer::Symbol);
    TEST_CASE(Lexer::tokenType("*") == Lexer::Symbol);
    TEST_CASE(Lexer::tokenType("<") == Lexer::Symbol);
    TEST_CASE(Lexer::tokenType("=") == Lexer::Symbol);
    TEST_CASE(Lexer::tokenType("==") == Lexer::Symbol);
    TEST_CASE(Lexer::tokenType("!=") == Lexer::Symbol);

    TEST_CASE(Lexer::tokenType("(") == Lexer::LeftP);
    TEST_CASE(Lexer::tokenType(")") == Lexer::RightP);
    TEST_CASE(Lexer::tokenType("'a'") == Lexer::Char);
    TEST_CASE(Lexer::tokenType("\"\"") == Lexer::String);
    TEST_CASE(Lexer::tokenType("\"a\"") == Lexer::String);
    TEST_CASE(Lexer::tokenType("\"text\"") == Lexer::String);
    TEST_CASE(Lexer::tokenType("123") == Lexer::Int);
    TEST_CASE(Lexer::tokenType("+123") == Lexer::Int);
    TEST_CASE(Lexer::tokenType("-123") == Lexer::Int);
    TEST_CASE(Lexer::tokenType("123.45") == Lexer::Real);
    TEST_CASE(Lexer::tokenType("+123.45") == Lexer::Real);
    TEST_CASE(Lexer::tokenType("-123.45") == Lexer::Real);
    TEST_CASE(Lexer::tokenType("123.") == Lexer::Real);
    TEST_CASE(Lexer::tokenType(".45") == Lexer::Real);
    TEST_CASE(Lexer::tokenType("true") == Lexer::Bool);
    TEST_CASE(Lexer::tokenType("false") == Lexer::Bool);
    TEST_CASE(Lexer::tokenType("null") == Lexer::Null);
    TEST_CASE(Lexer::tokenType("ABC") == Lexer::Symbol);
    TEST_CASE(Lexer::tokenType("ABC9") == Lexer::Symbol);
    TEST_CASE(Lexer::tokenType("ABC09sds") == Lexer::Symbol);
    TEST_CASE(Lexer::tokenType("ABC_3") == Lexer::Symbol);
}

// -------------------------------------------------------------
bool parserTest(Parser &parser, Environment::SharedPtr env, const std::string &expr, const Value &value, bool success) {
    try {
        ByteCode::SharedPtr code(parser.read(expr));
        if (code.get()) {
            Value result = code->eval(env);
            if (result.type() != value.type()) {
                std::cerr << "Parser - " << expr << ": type expected=" << value.typeToString() << " actual=" << result.typeToString() << '\n';
                return false;
            }
            if (result != value) {
                std::cerr << "Parser - " << expr << ": value expected=" << value << " actual=" << result << '\n';
                return false;
            }
            return true;
        }
    }
    catch (const std::exception &ex) {
        if (success) {
            std::cerr << "Parser - " << expr << ": error " << ex.what() << '\n';
            return false;
        }
        return true;
    }
    return false;
}

// -------------------------------------------------------------
void UnitTest::testParserBasic() {
    Environment::SharedPtr env(new Environment());
    Parser parser;

    TEST_CASE(parserTest(parser, env, "'a'",      Value('a'),     true));
    TEST_CASE(parserTest(parser, env, "\"text\"", Value("text"),  true));
    TEST_CASE(parserTest(parser, env, "12",       Value(12ll), true));
    TEST_CASE(parserTest(parser, env, "-20",      Value(-20ll),   true));
    TEST_CASE(parserTest(parser, env, "24.56",    Value(24.56),   true));
    TEST_CASE(parserTest(parser, env, "-34.125",  Value(-34.125), true));
    TEST_CASE(parserTest(parser, env, "true",     Value(true),    true));
    TEST_CASE(parserTest(parser, env, "false",    Value(false),   true));
    TEST_CASE(parserTest(parser, env, "null",     Value::Null,    true));

    TEST_CASE(parserTest(parser, env, "'q",     Value::Null, false));
    TEST_CASE(parserTest(parser, env, "\"",     Value::Null, false));
    TEST_CASE(parserTest(parser, env, "\"text", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "2-0",    Value::Null, false));
    TEST_CASE(parserTest(parser, env, "2+0",    Value::Null, false));
    TEST_CASE(parserTest(parser, env, "23.3.4", Value::Null, false));
}

// -------------------------------------------------------------
void UnitTest::testParserIsType() {
    Environment::SharedPtr env(new Environment());
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(istypeof null none)",                   Value::True, true));
    TEST_CASE(parserTest(parser, env, "(istypeof 3 int)",                       Value::True, true));
    TEST_CASE(parserTest(parser, env, "(istypeof 1.0 real)",                    Value::True, true));
    TEST_CASE(parserTest(parser, env, "(istypeof 'c' char)",                    Value::True, true));
    TEST_CASE(parserTest(parser, env, "(istypeof true bool)",                   Value::True, true));
    TEST_CASE(parserTest(parser, env, "(istypeof \"txt\" string)",              Value::True, true));
    TEST_CASE(parserTest(parser, env, "(istypeof (lambda () 1) closure)",       Value::True, true));
    TEST_CASE(parserTest(parser, env, "(istypeof (struct Foo (x y)) usertype)", Value::True, true));

    TEST_CASE(parserTest(parser, env, "(istypeof null int)",                   Value::False, true));
    TEST_CASE(parserTest(parser, env, "(istypeof 3 char)",                     Value::False, true));
    TEST_CASE(parserTest(parser, env, "(istypeof 1.0 int)",                    Value::False, true));
    TEST_CASE(parserTest(parser, env, "(istypeof 'c' string)",                 Value::False, true));
    TEST_CASE(parserTest(parser, env, "(istypeof \"txt\" char)",               Value::False, true));
    TEST_CASE(parserTest(parser, env, "(istypeof (lambda () 1) none)",         Value::False, true));
    TEST_CASE(parserTest(parser, env, "(istypeof (struct Bar (y z)) closure)", Value::False, true));

    TEST_CASE(parserTest(parser, env, "(istypeof 3 three)", Value::Null, false));
}

// -------------------------------------------------------------
void UnitTest::testParserVar() {
    Environment::SharedPtr env(new Environment());
    Parser parser;

    TEST_CASE(parserTest(parser, env, "foo",         Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(var foo 5)", Value(5ll),  true));
    TEST_CASE(parserTest(parser, env, "foo",         Value(5ll),  true));
    TEST_CASE(parserTest(parser, env, "(= foo 12)",  Value(12ll), true));
    TEST_CASE(parserTest(parser, env, "foo",         Value(12ll), true));
}

// -------------------------------------------------------------
void UnitTest::testParserArith() {
    Environment::SharedPtr env(new Environment());
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(+ 1 2)",     Value(3ll),  true));
    TEST_CASE(parserTest(parser, env, "(+ 1.2 2.3)", Value(3.5),  true));
    TEST_CASE(parserTest(parser, env, "(+ 2 2.5)",   Value(4.5),  true));
    TEST_CASE(parserTest(parser, env, "(- 3 1)",     Value(2ll),  true));
    TEST_CASE(parserTest(parser, env, "(- 2 5)",     Value(-3ll), true));
    TEST_CASE(parserTest(parser, env, "(* 2 3)",     Value(6ll),  true));
    TEST_CASE(parserTest(parser, env, "(/ 10 2)",    Value(5ll),  true));
    TEST_CASE(parserTest(parser, env, "(/ 2 0)",     Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(% 4 2)",     Value(0ll),  true));
    TEST_CASE(parserTest(parser, env, "(% 2.4 2)",   Value::Null, false));
}

// -------------------------------------------------------------
void UnitTest::testParserComp() {
    Environment::SharedPtr env(new Environment());
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(== 2 2)",     Value::True,  true));
    TEST_CASE(parserTest(parser, env, "(== 'a' 'b')", Value::False, true));
    TEST_CASE(parserTest(parser, env, "(!= 2 3)",     Value::True,  true));
    TEST_CASE(parserTest(parser, env, "(!= 'a' 'a')", Value::False, true));
    TEST_CASE(parserTest(parser, env, "(> 3 2)",      Value::True,  true));
    TEST_CASE(parserTest(parser, env, "(> 'a' 'b')",  Value::False, true));
    TEST_CASE(parserTest(parser, env, "(< 3 2)",      Value::False, true));
    TEST_CASE(parserTest(parser, env, "(< 'a' 'b')",  Value::True,  true));
    TEST_CASE(parserTest(parser, env, "(>= 3 2)",     Value::True,  true));
    TEST_CASE(parserTest(parser, env, "(>= 3 3)",     Value::True,  true));
    TEST_CASE(parserTest(parser, env, "(>= 'a' 'b')", Value::False, true));
    TEST_CASE(parserTest(parser, env, "(<= 3 2)",     Value::False, true));
    TEST_CASE(parserTest(parser, env, "(<= 3 3)",     Value::True,  true));
    TEST_CASE(parserTest(parser, env, "(<= 'a' 'b')", Value::True,  true));
}

// -------------------------------------------------------------
void UnitTest::testParserLogic() {
    Environment::SharedPtr env(new Environment());
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(and true true)",    Value::True,  true));
    TEST_CASE(parserTest(parser, env, "(and true false)",   Value::False, true));
    TEST_CASE(parserTest(parser, env, "(and false true)",   Value::False, true));
    TEST_CASE(parserTest(parser, env, "(and false false)",  Value::False, true));
    TEST_CASE(parserTest(parser, env, "(or true true)",     Value::True,  true));
    TEST_CASE(parserTest(parser, env, "(or true false)",    Value::True,  true));
    TEST_CASE(parserTest(parser, env, "(or false true)",    Value::True,  true));
    TEST_CASE(parserTest(parser, env, "(or false false)",   Value::False, true));
}

// -------------------------------------------------------------
void UnitTest::testParserBlock() {
    Environment::SharedPtr env(new Environment());
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(progn (var c 'a') (= c 'b') c)",   Value('b'),  true));
    TEST_CASE(parserTest(parser, env, "(progn (var x 2) (= x (+ x 1)))",   Value(3ll),  true));
    TEST_CASE(parserTest(parser, env, "(block (var f false) (= f true))",  Value(true), true));
    TEST_CASE(parserTest(parser, env, "(block (var y 5) (= y (* 2 y)) y)", Value(10ll), true));
}

// -------------------------------------------------------------
void UnitTest::testParserCond() {
    Environment::SharedPtr env(new Environment());
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(if true 1 2)",                 Value(1ll),  true));
    TEST_CASE(parserTest(parser, env, "(if false 1 2)",                Value(2ll),  true));
    TEST_CASE(parserTest(parser, env, "(if (== 1 1) (+ 1 1) (+ 2 2))", Value(2ll),  true));
    TEST_CASE(parserTest(parser, env, "(if (== 1 2) (+ 1 1) (+ 2 2))", Value(4ll),  true));
    TEST_CASE(parserTest(parser, env, "(if (>= -2 0) 1 -1)",           Value(-1ll), true));

    TEST_CASE(parserTest(parser, env, "(cond (true 1))",                           Value(1ll),  true));
    TEST_CASE(parserTest(parser, env, "(cond (false 1))",                          Value::Null, true));
    TEST_CASE(parserTest(parser, env, "(cond (true 1) (false 2) (true 3))",        Value(1ll),  true));
    TEST_CASE(parserTest(parser, env, "(cond ((== 1 2) 1) ((== 1 1) 2) (true 3))", Value(2ll),  true));
    TEST_CASE(parserTest(parser, env, "(cond ((== 1 2) 1) ((== 2 3) 2) (true 3))", Value(3ll),  true));
    TEST_CASE(parserTest(parser, env, "(cond)",                                    Value::Null, false));
}

// -------------------------------------------------------------
void UnitTest::testParserLoop() {
    Environment::SharedPtr env(new Environment());
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(loop false 10)",                                                              Value::Null, true));
    TEST_CASE(parserTest(parser, env, "(loop (var i 1) (< i 4) (= i (+ i 1)) i)",                                     Value(3ll),  true));
    TEST_CASE(parserTest(parser, env, "(loop (progn (var i 1) (var sum 0)) (< i 4) (= i (+ i 1)) (= sum (+ sum i)))", Value(6ll),  true));
    TEST_CASE(parserTest(parser, env, "(block (var sum 0) (loop (< sum 5) (= sum (+ sum 2))))",                       Value(6ll),  true));
    TEST_CASE(parserTest(parser, env, "(loop true (break))",                                                          Value::Null, true));
    TEST_CASE(parserTest(parser, env, "(block (var sum 0) (loop true (progn (= sum 3) (break))))",                    Value::Null, true));
    TEST_CASE(parserTest(parser, env, "(block (var sum 0) (loop true (progn (= sum 3) (break))) sum)",                Value(3ll),  true));
    TEST_CASE(parserTest(parser, env, "(loop (var i 1) (< i 4) (= i (+ i 1)))",                                       Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(loop (var i 1))",                                                             Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(loop)",                                                                       Value::Null, false));
}

// -------------------------------------------------------------
void UnitTest::testParserLambda() {
    Environment::SharedPtr env(new Environment());
    Parser parser;

    TEST_CASE(parserTest(parser, env, "((lambda (x) (+ x 1)) 3)",           Value(4ll),  true));
    TEST_CASE(parserTest(parser, env, "((lambda (x y) (+ (* x 2) y)) 2 1)", Value(5ll),  true));
    TEST_CASE(parserTest(parser, env, "((var x 1) 3)",                      Value::Null, false));
}

// -------------------------------------------------------------
void UnitTest::testParserFtn() {
    Environment::SharedPtr env(new Environment());
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(block (def add (x y) (+ x y)) (add 1 2))",                                          Value(3ll),  true));
    TEST_CASE(parserTest(parser, env, "(block (var x 3) (var y 2) (def sum () (+ x y)) (block (var x 4) (var y 5) (sum)))", Value(5ll),  true));
    TEST_CASE(parserTest(parser, env, "(block (def inc (x) (+ x 1)) (def add3 (x) (inc (inc (inc x)))) (add3 5))",          Value(8ll),  true));

    TEST_CASE(parserTest(parser, env, "(progn (def add3 (x) (progn (def inc (y) (+ y 1)) (inc (inc (inc x))))) (add3 4))",  Value(7ll),  true));
    TEST_CASE(parserTest(parser, env, "(add3 1)",                                                                           Value(4ll),  true));
    TEST_CASE(parserTest(parser, env, "(inc 2)",                                                                            Value::Null, false));
}

// -------------------------------------------------------------
void UnitTest::testParserStruct() {
    Environment::SharedPtr env(new Environment());
    Parser parser;

    const Struct stest("Person", {"name", "age"});
    TEST_CASE(parserTest(parser, env, "(struct Person (name age))", Value(stest), true));

    const Value &sValue = env->get("Person");
    TEST_CASE_MSG(sValue.isUserType(), "actual=" << sValue.typeToString());

    const Struct &s = sValue.userType();
    TEST_CASE_MSG(s.name() == "Person", "actual=" << s.name());
    TEST_CASE_MSG(s.members().size() == 2, "actual=" << s.members().size());
    TEST_CASE_MSG(s.members()[0] == "name", "actual=" << s.members()[0]);
    TEST_CASE_MSG(s.members()[1] == "age", "actual=" << s.members()[1]);
}

// -------------------------------------------------------------
void UnitTest::testParserIsStructName() {
    Environment::SharedPtr env(new Environment());
    Parser parser;

    const Struct stest("Person", {"name", "age"});
    TEST_CASE(parserTest(parser, env, "(var s (struct Person (name age)))", Value(stest), true));
    TEST_CASE(parserTest(parser, env, "(isstructname Person Person)",           Value::True,  true));
    TEST_CASE(parserTest(parser, env, "(isstructname s Person)",                Value::True,  true));

    TEST_CASE(parserTest(parser, env, "(isstructname Person Foobar)", Value::False,  true));
    TEST_CASE(parserTest(parser, env, "(isstructname s Foobar)",      Value::False,  true));
    TEST_CASE(parserTest(parser, env, "(isstructname 3 Person)",      Value::False,  true));
}

// -------------------------------------------------------------
void UnitTest::testParserMakeInstance() {
    Environment::SharedPtr env(new Environment());
    Parser parser;

    const Struct stest("Person", {"name", "age"});
    const Instance itest(stest);
    TEST_CASE(parserTest(parser, env, "(var s (struct Person (name age)))", Value(stest), true));
    TEST_CASE(parserTest(parser, env, "(var p (makeinstance Person))",      Value(itest), true));
    TEST_CASE(parserTest(parser, env, "(istypeof p userobject)",            Value::True,  true));
}

// -------------------------------------------------------------
void UnitTest::testParserIsInstanceOf() {
    Environment::SharedPtr env(new Environment());
    Parser parser;

    const Struct stest("Person", {"name", "age"});
    const Instance itest(stest);
    TEST_CASE(parserTest(parser, env, "(struct Person (name age))",    Value(stest), true));
    TEST_CASE(parserTest(parser, env, "(var p (makeinstance Person))", Value(itest), true));
    TEST_CASE(parserTest(parser, env, "(isinstanceof p Person)",       Value::True,  true));
}

// -------------------------------------------------------------
void UnitTest::testParserStringLen() {
    Environment::SharedPtr env(new Environment());
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(strlen \"\")",       Value::Zero, true));
    TEST_CASE(parserTest(parser, env, "(strlen \"a\")",      Value(1ll),  true));
    TEST_CASE(parserTest(parser, env, "(strlen \"string\")", Value(6ll),  true));
    TEST_CASE(parserTest(parser, env, "(strlen 10)",         Value::Null, false));
}

// -------------------------------------------------------------
void UnitTest::testParserGetChar() {
    Environment::SharedPtr env(new Environment());
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(getchar \"By\" 0)", Value('B'),  true));
    TEST_CASE(parserTest(parser, env, "(getchar \"By\" 1)", Value('y'),  true));
    TEST_CASE(parserTest(parser, env, "(getchar \"By\" 2)", Value::Null, false));
}

// -------------------------------------------------------------
void UnitTest::testParserSetChar() {
    Environment::SharedPtr env(new Environment());
    Parser parser;

    env->def("str", Value("12345"));

    TEST_CASE(parserTest(parser, env, "(setchar str 0 'a')", Value('a'),  true));
    TEST_CASE(parserTest(parser, env, "(setchar str 2 'C')", Value('C'),  true));
    TEST_CASE(parserTest(parser, env, "(setchar str 6 'b')", Value::Null, false));
}
