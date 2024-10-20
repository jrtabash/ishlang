#include "unit_test.h"

#include <array>
#include <functional>
#include <iostream>
#include <map>
#include <sstream>
#include <unordered_map>

using namespace Ishlang;

#define TEST_CASE(EXPR) if (!(EXPR)) { std::cerr << '[' << __LINE__ << "] " << #EXPR << '\n'; success_ = false; }
#define TEST_CASE_MSG(EXPR, MSG) if (!(EXPR)) { std::cerr << '[' << __LINE__ << "] " << #EXPR << ": " << MSG << '\n'; success_ = false; }

// -------------------------------------------------------------
#define ADD_TEST(NAME) tests_.insert(Tests::value_type(#NAME, std::bind(&UnitTest::NAME, this)))
UnitTest::UnitTest()
    : success_(true)
    , verbose_(false)
    , tests_()
{
    ModuleStorage::addPath(Util::temporaryPath());

    ADD_TEST(testToken);
    ADD_TEST(testUtilSplit);
    ADD_TEST(testUtilPrintContainer);
    ADD_TEST(testComment);
    ADD_TEST(testValue);
    ADD_TEST(testValueAsType);
    ADD_TEST(testValuePair);
    ADD_TEST(testEnvironment);
    ADD_TEST(testEnvironmentForeach);
    ADD_TEST(testLambda);
    ADD_TEST(testStruct);
    ADD_TEST(testStructValue);
    ADD_TEST(testInstance);
    ADD_TEST(testInstanceValue);
    ADD_TEST(testSequence);
    ADD_TEST(testSequenceFind);
    ADD_TEST(testSequenceCount);
    ADD_TEST(testSequenceSort);
    ADD_TEST(testSequenceReverse);
    ADD_TEST(testSequenceInsert);
    ADD_TEST(testSequenceErase);
    ADD_TEST(testSequenceClear);
    ADD_TEST(testSequenceValue);
    ADD_TEST(testSequencePrint);
    ADD_TEST(testSequenceGenerate);
    ADD_TEST(testHashtable);
    ADD_TEST(testHashtableFind);
    ADD_TEST(testHashtableCount);
    ADD_TEST(testHashtableKeysValues);
    ADD_TEST(testIntegerRange);
    ADD_TEST(testIntegerRangeComparison);
    ADD_TEST(testIntegerRangeStartNext);
    ADD_TEST(testIntegerRangeGenerator);
    ADD_TEST(testModule);
    ADD_TEST(testModuleStorage);
    ADD_TEST(testCodeNodeBasic);
    ADD_TEST(testCodeNodeClone);
    ADD_TEST(testCodeNodeIsType);
    ADD_TEST(testCodeNodeTypeName);
    ADD_TEST(testCodeNodeAsType);
    ADD_TEST(testCodeNodeArithOp);
    ADD_TEST(testCodeNodeCompOp);
    ADD_TEST(testCodeNodeLogicOp);
    ADD_TEST(testCodeNodeNot);
    ADD_TEST(testCodeNodeNegativeOf);
    ADD_TEST(testCodeNodeSequence);
    ADD_TEST(testCodeNodeCond);
    ADD_TEST(testCodeNodeLoop);
    ADD_TEST(testCodeNodeForeach);
    ADD_TEST(testCodeNodeLambdaExpr);
    ADD_TEST(testCodeNodeLambdaApp);
    ADD_TEST(testCodeNodeFunctionExpr);
    ADD_TEST(testCodeNodeFunctionApp);
    ADD_TEST(testCodeNodeStruct);
    ADD_TEST(testCodeNodeIsStructName);
    ADD_TEST(testCodeNodeMakeInstance);
    ADD_TEST(testCodeNodeMakeInstanceWithInitArgs);
    ADD_TEST(testCodeNodeIsInstanceOf);
    ADD_TEST(testCodeNodeStructName);
    ADD_TEST(testCodeNodeGetSetMember);
    ADD_TEST(testCodeNodeStringCat);
    ADD_TEST(testCodeNodeSubString);
    ADD_TEST(testCodeNodeStringFind);
    ADD_TEST(testCodeNodeStringCount);
    ADD_TEST(testCodeNodeStringCompare);
    ADD_TEST(testCodeNodeStringSort);
    ADD_TEST(testCodeNodeStringReverse);
    ADD_TEST(testCodeNodeMakeArray);
    ADD_TEST(testCodeNodeMakeArraySV);
    ADD_TEST(testCodeNodeArrayLen);
    ADD_TEST(testCodeNodeArrayGet);
    ADD_TEST(testCodeNodeArraySet);
    ADD_TEST(testCodeNodeArrayPush);
    ADD_TEST(testCodeNodeArrayPop);
    ADD_TEST(testCodeNodeArrayFind);
    ADD_TEST(testCodeNodeArrayCount);
    ADD_TEST(testCodeNodeArraySort);
    ADD_TEST(testCodeNodeArrayReverse);
    ADD_TEST(testCodeNodeArrayClear);
    ADD_TEST(testCodeNodeArrayInsert);
    ADD_TEST(testCodeNodeArrayRemove);
    ADD_TEST(testCodeNodeStrCharCheck);
    ADD_TEST(testCodeNodeStrCharTransform);
    ADD_TEST(testCodeNodeImportModule);
    ADD_TEST(testCodeNodeFromModuleImport);
    ADD_TEST(testCodeNodeRandom);
    ADD_TEST(testCodeNodeHash);
    ADD_TEST(testCodeNodeMakeHashMap);
    ADD_TEST(testCodeNodeHashMapLen);
    ADD_TEST(testCodeNodeHashMapContains);
    ADD_TEST(testCodeNodeHashMapGet);
    ADD_TEST(testCodeNodeHashMapSet);
    ADD_TEST(testCodeNodeHashMapRemove);
    ADD_TEST(testCodeNodeHashMapClear);
    ADD_TEST(testCodeNodeHashMapFind);
    ADD_TEST(testCodeNodeHashMapCount);
    ADD_TEST(testCodeNodeHashMapKeys);
    ADD_TEST(testCodeNodeHashMapValues);
    ADD_TEST(testCodeNodeHashMapItems);
    ADD_TEST(testCodeNodePairOperations);
    ADD_TEST(testCodeNodeMakeRange);
    ADD_TEST(testCodeNodeRangeGetters);
    ADD_TEST(testCodeNodeExpand);
    ADD_TEST(testCodeNodeGenericLen);
    ADD_TEST(testCodeNodeGenericGet);
    ADD_TEST(testTokenType);
    ADD_TEST(testCodeNodeStringLen);
    ADD_TEST(testCodeNodeStringGet);
    ADD_TEST(testCodeNodeStringSet);
    ADD_TEST(testParserBasic);
    ADD_TEST(testParserClone);
    ADD_TEST(testParserIsType);
    ADD_TEST(testParserTypeName);
    ADD_TEST(testParserAsType);
    ADD_TEST(testParserVar);
    ADD_TEST(testParserArith);
    ADD_TEST(testParserComp);
    ADD_TEST(testParserLogicOp);
    ADD_TEST(testParserNot);
    ADD_TEST(testParserNegativeOf);
    ADD_TEST(testParserBlock);
    ADD_TEST(testParserCond);
    ADD_TEST(testParserLoop);
    ADD_TEST(testParserForeach);
    ADD_TEST(testParserLambda);
    ADD_TEST(testParserFtn);
    ADD_TEST(testParserStruct);
    ADD_TEST(testParserIsStructName);
    ADD_TEST(testParserMakeInstance);
    ADD_TEST(testParserMakeInstanceWithInitArgs);
    ADD_TEST(testParserIsInstanceOf);
    ADD_TEST(testParserStructName);
    ADD_TEST(testParserGetSetMember);
    ADD_TEST(testParserStringLen);
    ADD_TEST(testParserStrGet);
    ADD_TEST(testParserStrSrt);
    ADD_TEST(testParserStringCat);
    ADD_TEST(testParserSubString);
    ADD_TEST(testParserStringFind);
    ADD_TEST(testParserStringCompare);
    ADD_TEST(testParserStringSort);
    ADD_TEST(testParserStringReverse);
    ADD_TEST(testParserStringCount);
    ADD_TEST(testParserMakeArray);
    ADD_TEST(testParserMakeArraySV);
    ADD_TEST(testParserArrayLen);
    ADD_TEST(testParserArrayGet);
    ADD_TEST(testParserArraySet);
    ADD_TEST(testParserArrayPush);
    ADD_TEST(testParserArrayPop);
    ADD_TEST(testParserArrayFind);
    ADD_TEST(testParserArrayCount);
    ADD_TEST(testParserArraySort);
    ADD_TEST(testParserArrayReverse);
    ADD_TEST(testParserArrayClear);
    ADD_TEST(testParserArrayInsert);
    ADD_TEST(testParserArrayRemove);
    ADD_TEST(testParserStrCharCheck);
    ADD_TEST(testParserStrCharTransform);
    ADD_TEST(testParserImportModule);
    ADD_TEST(testParserFromModuleImport);
    ADD_TEST(testParserRandom);
    ADD_TEST(testParserHash);
    ADD_TEST(testParserMakeHashMap);
    ADD_TEST(testParserHashMapLen);
    ADD_TEST(testParserHashMapContains);
    ADD_TEST(testParserHashMapGet);
    ADD_TEST(testParserHashMapSet);
    ADD_TEST(testParserHashMapRemove);
    ADD_TEST(testParserHashMapClear);
    ADD_TEST(testParserHashMapFind);
    ADD_TEST(testParserHashMapCount);
    ADD_TEST(testParserHashMapKeys);
    ADD_TEST(testParserHashMapVals);
    ADD_TEST(testParserHashMapItems);
    ADD_TEST(testParserPairOperations);
    ADD_TEST(testParserMakeRange);
    ADD_TEST(testParserRangeGetters);
    ADD_TEST(testParserExpand);
    ADD_TEST(testParserGenericLen);
    ADD_TEST(testParserGenericGet);
}
#undef ADD_TEST

// -------------------------------------------------------------
bool UnitTest::run(const std::string &test) {
    success_ = true;
    if (test.empty()) {
        std::cout << "\n***** Running unit tests" << '\n';
        for (Tests::iterator iter = tests_.begin(); iter != tests_.end(); ++iter) {
            runTest(iter->first, iter->second);
        }
    }
    else {
        std::cout << "\n***** Running unit test " << test << '\n';
        Tests::iterator iter = tests_.find(test);
        if (iter == tests_.end()) {
            std::cerr << "Unknown test - " << test << '\n';
            return false;
        }
        runTest(iter->first, iter->second);
    }

    std::cout << "\n***** Ran " << (test.empty() ? tests_.size() : 1ul) << " unit test" << (test.empty() ? "s" : "")
              << "\n***** " << (success_ ? "Success" : "Failure")
              << std::endl;
    return success_;
}

// -------------------------------------------------------------
void UnitTest::list() const {
    for (Tests::const_iterator iter = tests_.begin(); iter != tests_.end(); ++iter) {
        std::cout << iter->first << '\n';
    }
}

// -------------------------------------------------------------
void UnitTest::setVerbose(bool flag) {
    verbose_ = flag;
}

// -------------------------------------------------------------
void UnitTest::runTest(const std::string &name, Function ftn) {
    if (verbose_) { std::cout << "Testing: " << name << '\n'; }
    ftn();
}

// -------------------------------------------------------------
const std::string & UnitTest::defaultModuleCode() {
    static const std::string defaultCode =
        "(var PI 3.14)\n"
        "(defun add (x y) (+ x y))\n"
        "(defun sub (x y) (- x y))\n";
    return defaultCode;
}

// -------------------------------------------------------------
Util::TemporaryFile UnitTest::createTempModuleFile(const std::string &name, const std::string &code) {
    return Util::TemporaryFile(name + ".ish", code.empty() ? defaultModuleCode() : code);
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
void UnitTest::testUtilSplit() {
    auto test =
        [this](const std::string &str, const Util::StringVector &expected, char delim = ',') {
            TEST_CASE_MSG(Util::split(str, delim) == expected, "str='" << str << '\'');
        };

    test("",         {});
    test("abc",      { "abc" });
    test("ab,cd",    { "ab", "cd" });
    test("ab,cd,ef", { "ab", "cd", "ef" });

    test("/some/path", { "/some/path" }, ':');
    test("/some/path:/another/path/", { "/some/path", "/another/path/" }, ':');
    test("/some/path:/another/path/:/other/", { "/some/path", "/another/path/", "/other/" }, ':');
}

// -------------------------------------------------------------
void UnitTest::testUtilPrintContainer() {
    auto itemPrinter = [](std::ostream &os, const auto &item) { os << item; };

    {
        std::ostringstream oss;
        Util::printContainer(oss, std::vector<int>(), itemPrinter, '[', ']');
        TEST_CASE_MSG(oss.str() == "[]", "actual=" << oss.str());
    }

    {
        std::ostringstream oss;
        Util::printContainer(oss, std::vector<int>({1, 2, 3}), itemPrinter, '[', ']');
        TEST_CASE_MSG(oss.str() == "[1 2 3]", "actual=" << oss.str());
    }

    {
        std::ostringstream oss;
        Util::printContainer(oss, std::vector<int>({1, 2, 3, 4, 5, 6, 7, 8, 9, 10}), itemPrinter, '[', ']');
        TEST_CASE_MSG(oss.str() == "[1 2 3 4 5 6 7 8 9 10]", "actual=" << oss.str());
    }

    {
        std::ostringstream oss;
        Util::printContainer(oss, std::vector<int>({1, 2, 3, 4, 5, 6, 7, 8, 9, 10}), itemPrinter, '[', ']', 3);
        TEST_CASE_MSG(oss.str() == "[1 2 3 ...]", "actual=" << oss.str());
    }

    {
        std::ostringstream oss;
        Util::printContainer(oss, std::vector<int>({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}), itemPrinter, '[', ']');
        TEST_CASE_MSG(oss.str() == "[1 2 3 4 5 6 7 8 9 10 ...]", "actual=" << oss.str());
    }

    {
        std::ostringstream oss;
        Util::printContainer(oss, std::vector<int>({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}), itemPrinter, '[', ']');
        TEST_CASE_MSG(oss.str() == "[1 2 3 4 5 6 7 8 9 10 ...]", "actual=" << oss.str());
    }
}


// -------------------------------------------------------------
void UnitTest::testComment() {
    std::string str;
    size_t pos = 0;

    {
        str = ";; This is s comment";
        pos = 0;
        TEST_CASE(Util::nextToken(str, pos) == "");
        TEST_CASE(pos == std::string::npos);
    }

    {
        str = ";; This is line one\n;; This is another line\n;; And here line 3";
        pos = 0;
        TEST_CASE(Util::nextToken(str, pos) == "");
        TEST_CASE(pos == std::string::npos);
    }

    {
        str = "(+ 1 2) ;; Adding 2 numbers";
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
        TEST_CASE(pos == 7);
        TEST_CASE(Util::nextToken(str, pos) == "");
        TEST_CASE(pos == std::string::npos);
    }

    {
        str = "(+ 1 2) ;; Adding then print\n(println x)";
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
        TEST_CASE(pos == 7);
        TEST_CASE(Util::nextToken(str, pos) == "(");
        TEST_CASE(pos == 30);
        TEST_CASE(Util::nextToken(str, pos) == "println");
        TEST_CASE(pos == 38);
        TEST_CASE(Util::nextToken(str, pos) == "x");
        TEST_CASE(pos == 39);
        TEST_CASE(Util::nextToken(str, pos) == ")");
        TEST_CASE(pos == std::string::npos);
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

void UnitTest::testValueAsType() {
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
void UnitTest::testValuePair() {
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
void UnitTest::testEnvironment() {
    auto env = Environment::make();
    TEST_CASE(env->empty());
    TEST_CASE(env->size() == 0);

    try {
        env->def("x", Value::Null);
        env->def("y", Value::Null);
    }
    catch (const DuplicateDef &ex) { TEST_CASE(false); }
    catch (...) { TEST_CASE(false); }

    TEST_CASE(!env->empty());
    TEST_CASE(env->size() == 2);
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
        auto childEnv = Environment::make(env);
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
    
    auto child = Environment::make(env);
    
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
void UnitTest::testEnvironmentForeach() {
    auto env = Environment::make();
    env->def("one", Value(1ll));
    env->def("two", Value(2ll));
    env->def("three", Value(3ll));

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

// -------------------------------------------------------------
void UnitTest::testLambda() {
    auto env = Environment::make();
    env->def("x", Value::Zero);

    const Lambda::ParamList params({"x", "y"});    
    CodeNode::SharedPtr body(
        new ArithOp(ArithOp::Add,
                    CodeNode::SharedPtr(new Variable("x")),
                    CodeNode::SharedPtr(new Variable("y"))));

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

    Instance i2(s, {{"name", Value("Stan")}, {"age", Value(25ll)}});
    TEST_CASE_MSG(i2.get("name") == Value("Stan"), "actual=" << i2.get("name"));
    TEST_CASE_MSG(i2.get("age") == Value(25ll), "actual=" << i2.get("age"));

    Instance i3(s, {{"name", Value("Sara")}});
    TEST_CASE_MSG(i3.get("name") == Value("Sara"), "actual=" << i3.get("name"));
    TEST_CASE_MSG(i3.get("age") == Value::Null, "actual=" << i3.get("age"));
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
    catch (const Ishlang::Exception &ex) {
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
    catch (const Ishlang::Exception &ex) {
        TEST_CASE_MSG(false, "Caught exception: " << ex.what());
    }
}

// -------------------------------------------------------------
void UnitTest::testSequence() {
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
void UnitTest::testSequenceFind() {
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
void UnitTest::testSequenceCount() {
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
void UnitTest::testSequenceSort() {
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
void UnitTest::testSequenceReverse() {
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
void UnitTest::testSequenceInsert() {
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
void UnitTest::testSequenceErase() {
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
void UnitTest::testSequenceClear() {
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
void UnitTest::testSequenceValue() {
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
void UnitTest::testSequencePrint() {
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
void UnitTest::testSequenceGenerate() {
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

// -------------------------------------------------------------
void UnitTest::testHashtable() {
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
void UnitTest::testHashtableFind() {
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
void UnitTest::testHashtableCount() {
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
void UnitTest::testHashtableKeysValues() {
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

// -------------------------------------------------------------
#define TEST_INT_RANGE(RNG, BEGIN, END, STEP, SIZE)                     \
    TEST_CASE_MSG(RNG.begin() == BEGIN, "actual=" << RNG.begin());      \
    TEST_CASE_MSG(RNG.end() == END, "actual=" << RNG.end());            \
    TEST_CASE_MSG(RNG.step() == STEP, "actual=" << RNG.step());         \
    TEST_CASE_MSG(RNG.size() == SIZE, "actual=" << RNG.size());
void UnitTest::testIntegerRange() {
    TEST_INT_RANGE(IntegerRange(10), 0, 10, 1, 10);
    TEST_INT_RANGE(IntegerRange(0, 10), 0, 10, 1, 10);
    TEST_INT_RANGE(IntegerRange(0, 10, 1),  0, 10, 1, 10);
    TEST_INT_RANGE(IntegerRange(10,  0, -1), 10, 0, -1, 10);
    TEST_INT_RANGE(IntegerRange(-10, 0, 1), -10, 0, 1, 10);
    TEST_INT_RANGE(IntegerRange(0, -10, -1), 0, -10, -1, 10);

    TEST_INT_RANGE(IntegerRange(2, 23, 3), 2, 23, 3, 7);
    TEST_INT_RANGE(IntegerRange(23, 2, -3), 23, 2, -3, 7);
    TEST_INT_RANGE(IntegerRange(23, -2, -3), 23, -2, -3, 9);
    TEST_INT_RANGE(IntegerRange(-2, 23, 3), -2, 23, 3, 9);
    TEST_INT_RANGE(IntegerRange(-2, -23, -3), -2, -23, -3, 7);
    TEST_INT_RANGE(IntegerRange(-23, -2, 3), -23, -2, 3, 7);

    try {
        IntegerRange(1, 10, 0);
        TEST_CASE(false);
    }
    catch (const Exception &ex) {
        TEST_CASE_MSG(std::string(ex.what()) == "Range step 0", "actual=" << ex.what());
    }

    try {
        IntegerRange(1, 10, -1);
        TEST_CASE(false);
    }
    catch (const Exception &ex) {
        TEST_CASE_MSG(std::string(ex.what()) == "Invalid range 1:-1:10", "actual=" << ex.what());
    }
}
#undef TEST_INT_RANGE

// -------------------------------------------------------------
void UnitTest::testIntegerRangeComparison() {
    const auto r1 = IntegerRange(5);
    const auto r2 = IntegerRange(0, 5, 1);
    const auto r3 = IntegerRange(5, 0, -1);
    const auto r4 = IntegerRange(1, 5, 1);
    const auto r5 = IntegerRange(1, 6, 1);
    const auto r6 = IntegerRange(1, 7, 1);
    const auto r7 = IntegerRange(5, 1, -1);
    const auto r8 = IntegerRange(6, 1, -1);

    TEST_CASE(r1 == r1);
    TEST_CASE(r1 == r2);
    TEST_CASE(r1 != r3);
    TEST_CASE(r1 != r4);

    TEST_CASE(r1 < r6);
    TEST_CASE(r1 <= r6);
    TEST_CASE(r1 <= r5);

    TEST_CASE(r6 > r1);
    TEST_CASE(r6 >= r1);
    TEST_CASE(r5 >= r1);

    TEST_CASE(!(r1 < r3));
    TEST_CASE(!(r3 > r1));

    TEST_CASE(r8 >= r3);
    TEST_CASE(r3 > r7);

    TEST_CASE(r3 <= r8);
    TEST_CASE(r7 < r3);
}

// -------------------------------------------------------------
void UnitTest::testIntegerRangeStartNext() {
    {
        const auto rng = IntegerRange(2, 12, 2);
        auto cur = rng.start(); TEST_CASE(cur == 2);
        cur = rng.next(*cur); TEST_CASE(cur == 4);
        cur = rng.next(*cur); TEST_CASE(cur == 6);
        cur = rng.next(*cur); TEST_CASE(cur == 8);
        cur = rng.next(*cur); TEST_CASE(cur == 10);
        cur = rng.next(*cur); TEST_CASE(cur == std::nullopt);
    }

    {
        const auto rng = IntegerRange(-2, -12, -2);
        auto cur = rng.start(); TEST_CASE(cur == -2);
        cur = rng.next(*cur); TEST_CASE(cur == -4);
        cur = rng.next(*cur); TEST_CASE(cur == -6);
        cur = rng.next(*cur); TEST_CASE(cur == -8);
        cur = rng.next(*cur); TEST_CASE(cur == -10);
        cur = rng.next(*cur); TEST_CASE(cur == std::nullopt);
    }
}

// -------------------------------------------------------------
void UnitTest::testIntegerRangeGenerator() {
    {
        const auto rng = IntegerRange(2, 12, 2);
        auto gen = rng.generator();
        TEST_CASE(gen.next() == 2);
        TEST_CASE(gen.next() == 4);
        TEST_CASE(gen.next() == 6);
        TEST_CASE(gen.next() == 8);
        TEST_CASE(gen.next() == 10);
        TEST_CASE(gen.next() == std::nullopt);
    }

    {
        const auto rng = IntegerRange(-2, -12, -2);
        auto gen = rng.generator();
        TEST_CASE(gen.next() == -2);
        TEST_CASE(gen.next() == -4);
        TEST_CASE(gen.next() == -6);
        TEST_CASE(gen.next() == -8);
        TEST_CASE(gen.next() == -10);
        TEST_CASE(gen.next() == std::nullopt);
    }

    {
        const auto rng = IntegerRange(0, 0, 1);
        auto gen = rng.generator();
        TEST_CASE(gen.next() == std::nullopt);
    }
}

// -------------------------------------------------------------
void UnitTest::testModule() {
    const std::string moduleCode = defaultModuleCode();

    { // Test import and alias
        auto testEnv = Environment::make();
        Module::SharedPtr module(new Module("test", ""));

        TEST_CASE_MSG(module->name() == "test", "name actual=" << module->name());
        TEST_CASE_MSG(module->sourceFile() == "", "sourceFile actual=" << module->sourceFile());

        Value value = module->loadFromString(moduleCode);
        TEST_CASE_MSG(value == Value::True, "loadFromString moduleCode actual=" << value);

        // Module is already loaded.
        value = module->loadFromString(moduleCode);
        TEST_CASE_MSG(value == Value::False, "loadFromString moduleCode actual=" << value);

        value = module->import(testEnv);
        TEST_CASE_MSG(value == Value::True, "import actual=" << value);
        TEST_CASE_MSG(testEnv->size() == 3, "size actual=" << testEnv->size());
        TEST_CASE(testEnv->exists("test.PI"));
        TEST_CASE(testEnv->exists("test.add"));
        TEST_CASE(testEnv->exists("test.sub"));

        value = module->alias(testEnv, "add");
        TEST_CASE_MSG(value == Value::True, "alias add actual=" << value);
        TEST_CASE_MSG(testEnv->size() == 4, "size actual=" << testEnv->size());
        TEST_CASE(testEnv->exists("test.PI"));
        TEST_CASE(testEnv->exists("test.add"));
        TEST_CASE(testEnv->exists("test.sub"));
        TEST_CASE(testEnv->exists("add"));

        value = module->alias(testEnv, "mul");
        TEST_CASE_MSG(value == Value::False, "alias mul actual=" << value);
        TEST_CASE_MSG(testEnv->size() == 4, "size actual=" << testEnv->size());
        TEST_CASE(testEnv->exists("test.PI"));
        TEST_CASE(testEnv->exists("test.add"));
        TEST_CASE(testEnv->exists("test.sub"));
        TEST_CASE(testEnv->exists("add"));
        TEST_CASE(!testEnv->exists("mul"));
    }

    { // Test alias list
        auto testEnv = Environment::make();
        Module::SharedPtr module(new Module("test", ""));

        Value value = module->loadFromString(moduleCode);
        TEST_CASE_MSG(value == Value::True, "loadFromString moduleCode actual=" << value);

        value = module->aliases(testEnv, { { "add", Module::OptionalName() }, { "sub", "minus" } });
        TEST_CASE_MSG(value == Value::True, "alias add actual=" << value);
        TEST_CASE_MSG(testEnv->size() == 2, "size actual=" << testEnv->size());
        TEST_CASE(testEnv->exists("add"));
        TEST_CASE(testEnv->exists("minus"));
    }

    { // Test import and alias with as name
        auto testEnv = Environment::make();
        Module::SharedPtr module(new Module("test", ""));

        Value value = module->loadFromString(moduleCode);
        TEST_CASE_MSG(value == Value::True, "loadFromString moduleCode actual=" << value);

        value = module->import(testEnv, "astest");
        TEST_CASE_MSG(value == Value::True, "import actual=" << value);
        TEST_CASE_MSG(testEnv->size() == 3, "size actual=" << testEnv->size());
        TEST_CASE(testEnv->exists("astest.PI"));
        TEST_CASE(testEnv->exists("astest.add"));
        TEST_CASE(testEnv->exists("astest.sub"));

        value = module->alias(testEnv, "add", "asadd");
        TEST_CASE_MSG(value == Value::True, "alias add actual=" << value);
        TEST_CASE_MSG(testEnv->size() == 4, "size actual=" << testEnv->size());
        TEST_CASE(testEnv->exists("astest.PI"));
        TEST_CASE(testEnv->exists("astest.add"));
        TEST_CASE(testEnv->exists("astest.sub"));
        TEST_CASE(testEnv->exists("asadd"));
    }

    {
        Module::SharedPtr module(new Module("test", "module.ish"));
        TEST_CASE_MSG(module->name() == "test", "name actual=" << module->name());
        TEST_CASE_MSG(module->sourceFile() == "module.ish", "sourceFile actual=" << module->sourceFile());

        // Module has a source file, cannot load from string.
        Value value = module->loadFromString(moduleCode);
        TEST_CASE_MSG(value == Value::False, "actual=" << value);
    }

    try {
        Module::SharedPtr module(new Module("test", ""));
        const std::string incompleteCode = "(defun mul (x y)";
        module->loadFromString(incompleteCode);
        TEST_CASE(false);
    }
    catch (const IncompleteExpression &ex) {
        TEST_CASE_MSG(ex.what() == std::string("Incomplete expression: Module 'test' loadFromString"), "actual=" << ex.what());
    }
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
void UnitTest::testModuleStorage() {
    const std::string name = "test";
    const std::string name2 = "test2";

    // Test basic exists, add and get
    TEST_CASE(!ModuleStorage::exists(name));
    TEST_CASE(ModuleStorage::add(name, ""));
    TEST_CASE(ModuleStorage::exists(name));
    TEST_CASE(ModuleStorage::get(name));

    // Test adding a duplicate module name
    try {
        ModuleStorage::add(name, "");
        TEST_CASE(false);
    }
    catch (const ModuleError &ex) {
        TEST_CASE(std::string("Module 'test' - Failed to add duplicate module name to module storage") == ex.what());
    }
    catch (...) {
        TEST_CASE(false);
    }

    // Test getting a non-existing module name
    try {
        ModuleStorage::get(name2);
        TEST_CASE(false);
    }
    catch (const ModuleError &ex) {
        TEST_CASE(std::string("Module 'test2' - Failed to find module in module storage") == ex.what());
    }
    catch (...) {
        TEST_CASE(false);
    }

    // Test adding another module
    TEST_CASE(!ModuleStorage::exists(name2));
    TEST_CASE(ModuleStorage::add(name2, ""));
    TEST_CASE(ModuleStorage::get(name2));
    TEST_CASE(ModuleStorage::exists(name2));

    // Test get returns the correct module
    TEST_CASE(ModuleStorage::get(name)->name() == name);
    TEST_CASE(ModuleStorage::get(name2)->name() == name2);
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeBasic() {
    auto env = Environment::make();

    CodeNode::SharedPtr literal(new Literal(Value('c')));
    Value result = literal->eval(env);
    TEST_CASE_MSG(result == Value('c'), "actual=" << result << " (" << result.typeToString() << ')');
    
    CodeNode::SharedPtr definition(new Define("var", literal));
    result = definition->eval(env);
    TEST_CASE_MSG(result == Value('c'), "actual=" << result << " (" << result.typeToString() << ')');

    CodeNode::SharedPtr definition2(new Define("var2", CodeNode::SharedPtr(new Literal(Value(2ll)))));
    result = definition2->eval(env);
    TEST_CASE_MSG(result == Value(2ll), "actual=" << result << " (" << result.typeToString() << ')');
    
    CodeNode::SharedPtr variable(new Variable("var"));
    result = variable->eval(env);
    TEST_CASE_MSG(result == Value('c'), "actual=" << result << " (" << result.typeToString() << ')');
    
    CodeNode::SharedPtr assignment(new Assign("var", CodeNode::SharedPtr(new Variable("var2"))));
    result = assignment->eval(env);
    TEST_CASE_MSG(result == Value(2ll), "actual=" << result << " (" << result.typeToString() << ')');

    CodeNode::SharedPtr exists(new Exists("var"));
    result = exists->eval(env);
    TEST_CASE_MSG(result == Value::True, "actual=" << result << " (" << result.typeToString() << ')');

    CodeNode::SharedPtr doesNotExist(new Exists("novar"));
    result = doesNotExist->eval(env);
    TEST_CASE_MSG(result == Value::False, "actual=" << result << " (" << result.typeToString() << ')');
    
    variable.reset(new Variable("var"));
    result = variable->eval(env);
    TEST_CASE_MSG(result == Value(2ll), "actual=" << result << " (" << result.typeToString() << ')');
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeClone() {
    auto env = Environment::make();

    CodeNode::SharedPtr definition(new Define("var", CodeNode::SharedPtr(new Literal(Value("foobar")))));
    Value result1 = definition->eval(env);

    CodeNode::SharedPtr cloneExpr(new Clone(CodeNode::SharedPtr(new Variable("var"))));
    Value result2 = cloneExpr->eval(env);

    TEST_CASE(result1 == result2);

    auto & text1 = result1.text();
    auto & text2 = result2.text();

    TEST_CASE(text1 == text2);
    TEST_CASE(&text1 != &text2);
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeIsType() {
    auto env = Environment::make();

    Value lambdaValue(
        Lambda(CodeNode::ParamList(), 
               CodeNode::SharedPtr(new Literal(Value::Zero)), 
               env));

    Value structValue(Struct("Person", {"name", "age"}));

    Value instanceValue(Instance(structValue.userType()));

    Value arrayValue(Sequence(1, Value::Zero));

    const std::array<Value::Type, 10> types = {
        Value::eNone,
        Value::eInteger,
        Value::eReal,
        Value::eCharacter,
        Value::eBoolean,
        Value::eString,
        Value::eClosure,
        Value::eUserType,
        Value::eUserObject,
        Value::eArray
    };

    std::map<CodeNode::SharedPtr, Value::Type> valueTypes; {
        valueTypes.insert(std::make_pair(CodeNode::SharedPtr(new Literal(Value::Null)), Value::eNone));
        valueTypes.insert(std::make_pair(CodeNode::SharedPtr(new Literal(Value(1ll))), Value::eInteger));
        valueTypes.insert(std::make_pair(CodeNode::SharedPtr(new Literal(Value(1.0))), Value::eReal));
        valueTypes.insert(std::make_pair(CodeNode::SharedPtr(new Literal(Value('c'))),  Value::eCharacter));
        valueTypes.insert(std::make_pair(CodeNode::SharedPtr(new Literal(Value::True)), Value::eBoolean));
        valueTypes.insert(std::make_pair(CodeNode::SharedPtr(new Literal(Value("txt"))), Value::eString));
        valueTypes.insert(std::make_pair(CodeNode::SharedPtr(new Literal(lambdaValue)), Value::eClosure));
        valueTypes.insert(std::make_pair(CodeNode::SharedPtr(new Literal(structValue)), Value::eUserType));
        valueTypes.insert(std::make_pair(CodeNode::SharedPtr(new Literal(instanceValue)), Value::eUserObject));
        valueTypes.insert(std::make_pair(CodeNode::SharedPtr(new Literal(arrayValue)), Value::eArray));
    }

    for (auto valueType : valueTypes) {
        CodeNode::SharedPtr expr = valueType.first;
        Value::Type exprType = valueType.second;

        for (auto type : types) {
            CodeNode::SharedPtr isType(new IsType(expr, type));
            Value result = isType->eval(env);
            bool expected = exprType == type ? true : false;
            TEST_CASE_MSG(result.isBool() && result.boolean() == expected,
                          "actual=" << Value::typeToString(exprType) << " check=" << Value::typeToString(type));
        }
    }
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeTypeName() {
    auto env = Environment::make();

    Value lambdaValue(
        Lambda(CodeNode::ParamList(),
               CodeNode::SharedPtr(new Literal(Value::Zero)),
               env));

    Value structValue(Struct("Person", {"name", "age"}));

    Value instanceValue(Instance(structValue.userType()));

    Value arrayValue(Sequence(1, Value::Zero));

    std::vector<std::pair<CodeNode::SharedPtr, std::string>> valueTypePairs; {
        valueTypePairs.push_back(std::make_pair(CodeNode::SharedPtr(new Literal(Value::Null)),   "none"));
        valueTypePairs.push_back(std::make_pair(CodeNode::SharedPtr(new Literal(Value(1ll))),    "int"));
        valueTypePairs.push_back(std::make_pair(CodeNode::SharedPtr(new Literal(Value(1.0))),    "real"));
        valueTypePairs.push_back(std::make_pair(CodeNode::SharedPtr(new Literal(Value('c'))),    "char"));
        valueTypePairs.push_back(std::make_pair(CodeNode::SharedPtr(new Literal(Value::True)),   "bool"));
        valueTypePairs.push_back(std::make_pair(CodeNode::SharedPtr(new Literal(Value("txt"))),  "string"));
        valueTypePairs.push_back(std::make_pair(CodeNode::SharedPtr(new Literal(lambdaValue)),   "closure"));
        valueTypePairs.push_back(std::make_pair(CodeNode::SharedPtr(new Literal(structValue)),   "usertype"));
        valueTypePairs.push_back(std::make_pair(CodeNode::SharedPtr(new Literal(instanceValue)), "userobject"));
        valueTypePairs.push_back(std::make_pair(CodeNode::SharedPtr(new Literal(arrayValue)),    "array"));
    }

    for (auto const & valueType : valueTypePairs) {
        auto expr = CodeNode::SharedPtr(new TypeName(valueType.first));
        auto const & expectedName = valueType.second;

        Value typeName = expr->eval(env);
        TEST_CASE_MSG(typeName.isString() && typeName.text() == expectedName,
                      "actual=" << Value::typeToString(typeName.type()) << " expected=" << expectedName);
    }
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeAsType() {
    auto env = Environment::make();

    const Value int1(1ll);
    const Value int98(98ll);
    const Value real1(1.0);
    const Value real1p5(1.5);
    const Value real98(98.0);
    const Value charB('b');
    const Value charT('t');
    const Value boolTrue(Value::True);
    const Value string98("98");
    const Value string1p5("1.500000");
    const Value stringTrue("true");
    const Value stringB("b");

    std::vector<std::pair<Value, Value>> testCases = {
        { int1,  real1 },
        { int98, charB },
        { int1,  boolTrue },
        { int98,  string98 },
        { real1p5, int1 },
        { real98,  charB },
        { real1,   boolTrue },
        { real1p5, string1p5 },
        { charB, int98 },
        { charB, real98 },
        { charT, boolTrue },
        { charB, stringB },
        { boolTrue, int1 },
        { boolTrue, real1 },
        { boolTrue, charT },
        { boolTrue, stringTrue },
        { string98, int98 },
        { string98, real98 },
        { stringB, charB },
        { stringTrue, boolTrue }
    };

    for (auto const & fromAndTo : testCases) {
        CodeNode::SharedPtr asType =
            CodeNode::make<AsType>(
                CodeNode::make<Literal>(fromAndTo.first), fromAndTo.second.type());
        Value value = asType->eval(env);
        TEST_CASE_MSG(value == fromAndTo.second, "actual=" << value << " expected=" << fromAndTo.second);
    }
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeArithOp() {
    auto env = Environment::make();
    
    CodeNode::SharedPtr iLit1(new Literal(Value(1ll)));
    CodeNode::SharedPtr iLit2(new Literal(Value(2ll)));
    CodeNode::SharedPtr iLit3(new Literal(Value(0ll)));
    CodeNode::SharedPtr iLit4(new Literal(Value(4ll)));
    CodeNode::SharedPtr iLit5(new Literal(Value(5ll)));
    CodeNode::SharedPtr rLit1(new Literal(Value(1.2)));
    CodeNode::SharedPtr rLit2(new Literal(Value(1.3)));
    CodeNode::SharedPtr rLit3(new Literal(Value(0.0)));
    CodeNode::SharedPtr rLit4(new Literal(Value(9.0)));
    CodeNode::SharedPtr rLit5(new Literal(Value(0.5)));
    CodeNode::SharedPtr cLit(new Literal(Value('c')));
    CodeNode::SharedPtr bLit(new Literal(Value(true)));
    
    CodeNode::SharedPtr arith(new ArithOp(ArithOp::Add, iLit1, iLit2));
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
    TEST_CASE_MSG(Util::isEqual(result.integer(), 0), "actual=" << result);

    arith.reset(new ArithOp(ArithOp::Mod, iLit5, iLit2));
    result = arith->eval(env);
    TEST_CASE_MSG(result.isInt(), "actual=" << result.typeToString());
    TEST_CASE_MSG(Util::isEqual(result.integer(), 1), "actual=" << result);

    arith.reset(new ArithOp(ArithOp::Pow, iLit4, iLit2));
    result = arith->eval(env);
    TEST_CASE_MSG(result.isReal(), "actual=" << result.typeToString());
    TEST_CASE_MSG(Util::isEqual(result.real(), 16.0), "actual=" << result);
    
    arith.reset(new ArithOp(ArithOp::Pow, iLit4, iLit3));
    result = arith->eval(env);
    TEST_CASE_MSG(result.isReal(), "actual=" << result.typeToString());
    TEST_CASE_MSG(Util::isEqual(result.real(), 1.0), "actual=" << result);

    arith.reset(new ArithOp(ArithOp::Pow, iLit4, iLit1));
    result = arith->eval(env);
    TEST_CASE_MSG(result.isReal(), "actual=" << result.typeToString());
    TEST_CASE_MSG(Util::isEqual(result.real(), 4.0), "actual=" << result);

    arith.reset(new ArithOp(ArithOp::Pow, rLit4, rLit5));
    result = arith->eval(env);
    TEST_CASE_MSG(result.isReal(), "actual=" << result.typeToString());
    TEST_CASE_MSG(Util::isEqual(result.real(), 3.0), "actual=" << result);

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
void UnitTest::testCodeNodeCompOp() {
    auto env = Environment::make();
    
    CodeNode::SharedPtr iLit0(new Literal(Value(0ll)));
    CodeNode::SharedPtr iLit1(new Literal(Value(1ll)));
    CodeNode::SharedPtr rLit0(new Literal(Value(0.0)));
    CodeNode::SharedPtr rLit1(new Literal(Value(1.0)));
    CodeNode::SharedPtr cLitA(new Literal(Value('a')));
    CodeNode::SharedPtr cLitB(new Literal(Value('b')));
    CodeNode::SharedPtr bLitT(new Literal(Value(true)));
    CodeNode::SharedPtr bLitF(new Literal(Value(false)));

    CodeNode::SharedPtr comp;
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
void UnitTest::testCodeNodeLogicOp() {
    auto env = Environment::make();
    
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
        { LogicOp::Disjunction, true,  true,  true,  true,  false }
    };
    const size_t size = sizeof(cases)/sizeof(cases[0]);
    for (size_t i = 0; i < size; ++i) {
        const Case &c = cases[i];
        
        try {
            CodeNode::SharedPtr lhsLit(new Literal(Value(c.lhs)));
            CodeNode::SharedPtr rhsLit(new Literal(!c.other ? Value(c.rhs) : Value('a')));
            CodeNode::SharedPtr logic(new LogicOp(c.type, CodeNode::SharedPtrList{lhsLit, rhsLit}));
            Value result = logic->eval(env);
            if (!c.error) {
                TEST_CASE_MSG(result.isBool(), "actual=" << result.typeToString() << " (" << c.toString() << ')');
                TEST_CASE_MSG(result.boolean() == c.result, "actual=" << result << " (" << c.toString() << ')');
            }
            else {
                TEST_CASE_MSG(false, c.toString());
            }
        }
        catch (const InvalidOperandType &ex) { if (!c.error) { TEST_CASE_MSG(false, c.toString()); } }
        catch (...) { TEST_CASE_MSG(false, c.toString()); }
    }

    auto value = CodeNode::make<LogicOp>(
        LogicOp::Conjunction,
        CodeNode::SharedPtrList{
            CodeNode::make<Literal>(Value::True),
            CodeNode::make<Literal>(Value::True),
            CodeNode::make<Literal>(Value::False)})->eval(env);
    TEST_CASE_MSG(value.isBool(), "actual=" << value.typeToString());
    TEST_CASE_MSG(value == Value::False, "actual=" << value);

    value = CodeNode::make<LogicOp>(
        LogicOp::Disjunction,
        CodeNode::SharedPtrList{
            CodeNode::make<Literal>(Value::False),
            CodeNode::make<Literal>(Value::False),
            CodeNode::make<Literal>(Value::True)})->eval(env);
    TEST_CASE_MSG(value.isBool(), "actual=" << value.typeToString());
    TEST_CASE_MSG(value == Value::True, "actual=" << value);
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeNot() {
    auto env = Environment::make();

    Value value =
        CodeNode::make<Not>(CodeNode::make<Literal>(Value::True))
        ->eval(env);
    TEST_CASE_MSG(value.isBool(), "actual=" << value.typeToString());
    TEST_CASE_MSG(value == Value::False, "actual=" << value);

    value =
        CodeNode::make<Not>(CodeNode::make<Literal>(Value::False))
        ->eval(env);
    TEST_CASE_MSG(value.isBool(), "actual=" << value.typeToString());
    TEST_CASE_MSG(value == Value::True, "actual=" << value);

    try {
        value =
            CodeNode::make<Not>(CodeNode::make<Literal>(Value::Zero))
            ->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeNegativeOf() {
    auto env = Environment::make();

    auto iVal = CodeNode::make<Literal>(Value(5ll));
    auto fVal = CodeNode::make<Literal>(Value(3.5));

    auto inv = [env](CodeNode::SharedPtr operand) { return CodeNode::make<NegativeOf>(operand)->eval(env); };

    Value value = inv(iVal);
    TEST_CASE_MSG(value == Value(-5ll), "actual=" << value);

    value = inv(fVal);
    TEST_CASE_MSG(value == Value(-3.5), "actual=" << value);

    try {
        value = inv(CodeNode::make<Literal>(Value('a')));
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeSequence() {
    auto env = Environment::make();
    
    env->def("x", Value(1ll));
    env->def("y", Value(2ll));

    { // Block
        CodeNode::SharedPtr def1(new Define("x", CodeNode::SharedPtr(new Literal(Value(1ll)))));
        CodeNode::SharedPtr def2(new Define("y", CodeNode::SharedPtr(new Literal(Value(1ll)))));
        CodeNode::SharedPtr assign1(new Assign("x", CodeNode::SharedPtr(new Variable("y"))));
        CodeNode::SharedPtr assign2(new Assign("y", CodeNode::SharedPtr(new Literal(Value(3ll)))));
        CodeNode::SharedPtr litT(new Literal(Value('T')));

        CodeNode::SharedPtrList exprs;
        exprs.push_back(def1);
        exprs.push_back(def2);
        exprs.push_back(assign1);
        exprs.push_back(assign2);
        exprs.push_back(litT);

        CodeNode::SharedPtr block(new Block(exprs));
        Value result = block->eval(env);
        TEST_CASE_MSG(result.isChar(), "actual=" << result.typeToString());
        TEST_CASE_MSG(result.character() == 'T', "actual=" << result);
        TEST_CASE_MSG(env->get("x") == Value(1ll), "actual=" << env->get("x"));
        TEST_CASE_MSG(env->get("y") == Value(2ll), "actual=" << env->get("y"));
    }

    { // ProgN
        CodeNode::SharedPtr assign1(new Assign("x", CodeNode::SharedPtr(new Variable("y"))));
        CodeNode::SharedPtr assign2(new Assign("y", CodeNode::SharedPtr(new Literal(Value(3ll)))));
        CodeNode::SharedPtr litT(new Literal(Value('T')));

        CodeNode::SharedPtrList exprs;
        exprs.push_back(assign1);
        exprs.push_back(assign2);
        exprs.push_back(litT);

        CodeNode::SharedPtr progN(new ProgN(exprs));
        Value result = progN->eval(env);
        TEST_CASE_MSG(result.isChar(), "actual=" << result.typeToString());
        TEST_CASE_MSG(result.character() == 'T', "actual=" << result);
        TEST_CASE_MSG(env->get("x") == Value(2ll), "actual=" << env->get("x"));
        TEST_CASE_MSG(env->get("y") == Value(3ll), "actual=" << env->get("y"));
    }
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeCond() {
    { // If
        auto env = Environment::make();

        {
            CodeNode::SharedPtr bLitT(new Literal(Value::True));
            CodeNode::SharedPtr bLitF(new Literal(Value::False));
            CodeNode::SharedPtr iLit1(new Literal(Value(1ll)));
            CodeNode::SharedPtr iLit2(new Literal(Value(2ll)));

            CodeNode::SharedPtr cond(new If(bLitT, iLit1));
            Value result = cond->eval(env);
            TEST_CASE_MSG(result.isInt(), "actual=" << result.typeToString());
            TEST_CASE_MSG(result.integer() == 1ll, "actual=" << result);

            cond.reset(new If(bLitF, iLit1));
            result = cond->eval(env);
            TEST_CASE_MSG(result.isNull(), "actual=" << result.typeToString());

            cond.reset(new If(bLitT, iLit1, iLit2));
            result = cond->eval(env);
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

            CodeNode::SharedPtr tPred(new Literal(Value::True));
            CodeNode::SharedPtr fPred(new Literal(Value::False));
            CodeNode::SharedPtr tCode(new Define("x", CodeNode::SharedPtr(new Literal(Value(3ll)))));
            CodeNode::SharedPtr fCode(new Assign("y", CodeNode::SharedPtr(new Literal(Value(4ll)))));

            CodeNode::SharedPtr cond(new If(tPred, tCode, fCode));
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
        auto env = Environment::make();
        
        {
            CodeNode::SharedPtrPairs cases;
            cases.push_back(
                std::make_pair(
                    CodeNode::SharedPtr(new Literal(Value::False)),
                    CodeNode::SharedPtr(new Literal(Value(1ll)))));
            cases.push_back(
                std::make_pair(
                    CodeNode::SharedPtr(new Literal(Value::True)),
                    CodeNode::SharedPtr(new Literal(Value(2ll)))));
            CodeNode::SharedPtr cond(new Cond(cases));
            Value result = cond->eval(env);
            TEST_CASE_MSG(result.isInt(), "actual=" << result.typeToString());
            TEST_CASE_MSG(result.integer() == 2ll, "actual=" << result);
        }

        {
            CodeNode::SharedPtrPairs cases;
            cases.push_back(
                std::make_pair(
                    CodeNode::SharedPtr(new Literal(Value::False)),
                    CodeNode::SharedPtr(new Literal(Value(1ll)))));
            cases.push_back(
                std::make_pair(
                    CodeNode::SharedPtr(new Literal(Value::True)),
                    CodeNode::SharedPtr()));
            CodeNode::SharedPtr cond(new Cond(cases));
            Value result = cond->eval(env);
            TEST_CASE_MSG(result.isNull(), "actual=" << result.typeToString());
        }
        
        {
            env->def("x", Value(3ll));
            CodeNode::SharedPtr var(new Variable("x"));
            
            CodeNode::SharedPtrPairs cases;
            cases.push_back(
                std::make_pair(
                    CodeNode::SharedPtr(new CompOp(CompOp::EQ, var, CodeNode::SharedPtr(new Literal(Value(1ll))))),
                    CodeNode::SharedPtr(new Literal(Value('a')))));
            cases.push_back(
                std::make_pair(
                    CodeNode::SharedPtr(new CompOp(CompOp::EQ, var, CodeNode::SharedPtr(new Literal(Value(2ll))))),
                    CodeNode::SharedPtr(new Literal(Value('b')))));
            cases.push_back(
                std::make_pair(
                    CodeNode::SharedPtr(new CompOp(CompOp::EQ, var, CodeNode::SharedPtr(new Literal(Value(3ll))))),
                    CodeNode::SharedPtr(new Literal(Value('c')))));
            cases.push_back(
                std::make_pair(
                    CodeNode::SharedPtr(new CompOp(CompOp::EQ, var, CodeNode::SharedPtr(new Literal(Value(4ll))))),
                    CodeNode::SharedPtr(new Literal(Value('d')))));
            cases.push_back(
                std::make_pair(
                    CodeNode::SharedPtr(new Literal(Value::True)),
                    CodeNode::SharedPtr(new Literal(Value('e')))));
            CodeNode::SharedPtr cond(new Cond(cases));
            Value result = cond->eval(env);
            TEST_CASE_MSG(result.isChar(), "actual=" << result.typeToString());
            TEST_CASE_MSG(result.character() == 'c', "actual=" << result);
        }
        
        {
            try {
                CodeNode::SharedPtrPairs cases;
                cases.push_back(std::make_pair(CodeNode::SharedPtr(), CodeNode::SharedPtr()));
                CodeNode::SharedPtr cond(new Cond(cases));
                cond->eval(env);
                TEST_CASE(false);
            }
            catch (const InvalidExpressionType &ex) {}
            catch (...) { TEST_CASE(false); }
        }
        
        {
            try {
                CodeNode::SharedPtrPairs cases;
                cases.push_back(std::make_pair(CodeNode::SharedPtr(new Literal(Value::Zero)), CodeNode::SharedPtr()));
                CodeNode::SharedPtr cond(new Cond(cases));
                cond->eval(env);
                TEST_CASE(false);
            }
            catch (const InvalidExpressionType &ex) {}
            catch (...) { TEST_CASE(false); }
        }
    }
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeLoop() {
    auto env = Environment::make();

    { // For
        env->def("sum", Value(0ll));

        CodeNode::SharedPtr loop(
            new Loop(
                CodeNode::SharedPtr(
                    new Define("i", CodeNode::SharedPtr(new Literal(Value(1ll))))),
                CodeNode::SharedPtr(
                    new CompOp(CompOp::LE, 
                               CodeNode::SharedPtr(new Variable("i")),
                               CodeNode::SharedPtr(new Literal(Value(10ll))))),
                CodeNode::SharedPtr(
                    new Assign("i", CodeNode::SharedPtr(
                                        new ArithOp(ArithOp::Add,
                                                    CodeNode::SharedPtr(new Variable("i")),
                                                    CodeNode::SharedPtr(new Literal(Value(1ll))))))),
                CodeNode::SharedPtr(
                    new Assign("sum", CodeNode::SharedPtr(
                                        new ArithOp(ArithOp::Add,
                                                    CodeNode::SharedPtr(new Variable("sum")),
                                                    CodeNode::SharedPtr(new Variable("i"))))))));
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
        
        CodeNode::SharedPtr loop(
                new Loop(
                    CodeNode::SharedPtr(
                        new CompOp(CompOp::LT,
                                   CodeNode::SharedPtr(new Variable("count")),
                                   CodeNode::SharedPtr(new Literal(Value(32ll))))),
                    CodeNode::SharedPtr(
                        new Assign("count", CodeNode::SharedPtr(
                                                new ArithOp(ArithOp::Mul,
                                                            CodeNode::SharedPtr(new Variable("count")),
                                                            CodeNode::SharedPtr(new Literal(Value(2ll)))))))));
        Value result = loop->eval(env);
        TEST_CASE_MSG(result.isInt(), "actual=" << result.typeToString());
        TEST_CASE_MSG(result.integer() == 32ll, "actual=" << result);
    }
    
    { // Break
        env->def("x", Value(0ll));

        CodeNode::SharedPtrList exprs;
        exprs.push_back(
            CodeNode::SharedPtr(
                new Assign("x", CodeNode::SharedPtr(
                                    new ArithOp(ArithOp::Add,
                                                CodeNode::SharedPtr(new Variable("x")),
                                                CodeNode::SharedPtr(new Literal(Value(1ll))))))));
        exprs.push_back(
            CodeNode::SharedPtr(
                new If(CodeNode::SharedPtr(
                           new CompOp(CompOp::EQ,
                                      CodeNode::SharedPtr(new Variable("x")),
                                      CodeNode::SharedPtr(new Literal(Value(2ll))))),
                       CodeNode::SharedPtr(new Break()),
                       CodeNode::SharedPtr())));

        CodeNode::SharedPtr loop(
                new Loop(
                    CodeNode::SharedPtr(new Literal(Value::True)),
                    CodeNode::SharedPtr(new ProgN(exprs))));

        Value result = loop->eval(env);
        TEST_CASE_MSG(result.isNull(), "actual=" << result.typeToString());
        TEST_CASE_MSG(env->get("x") == Value(2ll), "actual=" << env->get("x"));
    }
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeForeach() {
    auto var = [](const char *name) {
        return CodeNode::make<Variable>(name);
    };
    auto lit = [](auto val) {
        return CodeNode::make<Literal>(Value(val));
    };
    auto add = [](auto lhs, auto rhs) {
        return CodeNode::make<ArithOp>(ArithOp::Add, lhs, rhs);
    };
    auto assign = [](const char *name, auto expr) {
        return CodeNode::make<Assign>(name, expr);
    };
    auto first = [](auto expr) {
        return CodeNode::make<PairFirst>(expr);
    };
    auto second = [](auto expr) {
        return CodeNode::make<PairSecond>(expr);
    };
    auto progn = [](auto e1, auto e2, auto e3) {
        CodeNode::SharedPtrList exprs;
        exprs.push_back(e1);
        exprs.push_back(e2);
        exprs.push_back(e3);
        return CodeNode::make<ProgN>(exprs);
    };

    { // String
        auto env = Environment::make();
        env->def("str", Value("1234567890"));
        env->def("cnt", Value(0ll));

        auto foreach = CodeNode::make<Foreach>(
            "c",
            var("str"),
            assign("cnt", add(var("cnt"), lit(1ll))));
        Value result = foreach->eval(env);
        TEST_CASE_MSG(result.isInt(), "actual=" << result.typeToString());
        TEST_CASE_MSG(result.integer() == 10ll, "actual=" << result);

        auto const & cnt = env->get("cnt");
        TEST_CASE_MSG(cnt.isInt(), "actual=" << cnt.typeToString());
        TEST_CASE_MSG(cnt.integer() == 10ll, "actual=" << cnt);
    }

    { // Array
        auto env = Environment::make();
        env->def("arr", Value(Sequence::generate(10, [i = 1ll]() mutable { return Value(i++); })));
        env->def("sum", Value(0ll));

        auto foreach = CodeNode::make<Foreach>(
            "i",
            var("arr"),
            assign("sum", add(var("sum"), var("i"))));
        Value result = foreach->eval(env);
        TEST_CASE_MSG(result.isInt(), "actual=" << result.typeToString());
        TEST_CASE_MSG(result.integer() == 55ll, "actual=" << result);

        auto const & sum = env->get("sum");
        TEST_CASE_MSG(sum.isInt(), "actual=" << sum.typeToString());
        TEST_CASE_MSG(sum.integer() == 55ll, "actual=" << sum);
    }

    { // Hashtable
        auto env = Environment::make();
        Hashtable ht;
        for (auto [k, v] : {
                std::pair{ 1ll, 10ll },
                std::pair{ 2ll, 20ll },
                std::pair{ 3ll, 30ll },
                std::pair{ 4ll, 40ll },
                std::pair{ 5ll, 50ll } }) {
            ht.set(Value(k), Value(v));
        }
        env->def("ht", Value(ht));
        env->def("cnt", Value(0ll));
        env->def("keySum", Value(0ll));
        env->def("valSum", Value(0ll));

        auto foreach = CodeNode::make<Foreach>(
            "kv",
            var("ht"),
            progn(
                assign("cnt", add(var("cnt"), lit(1ll))),
                assign("keySum", add(var("keySum"), first(var("kv")))),
                assign("valSum", add(var("valSum"), second(var("kv"))))));
        Value result = foreach->eval(env);
        TEST_CASE_MSG(result.isInt(), "actual=" << result.typeToString());
        TEST_CASE_MSG(result.integer() == 150ll, "actual=" << result);

        auto const & cnt = env->get("cnt");
        TEST_CASE_MSG(cnt.isInt(), "actual=" << cnt.typeToString());
        TEST_CASE_MSG(cnt.integer() == 5ll, "actual=" << cnt);

        auto const & keySum = env->get("keySum");
        TEST_CASE_MSG(keySum.isInt(), "actual=" << keySum.typeToString());
        TEST_CASE_MSG(keySum.integer() == 15ll, "actual=" << keySum);

        auto const & valSum = env->get("valSum");
        TEST_CASE_MSG(valSum.isInt(), "actual=" << valSum.typeToString());
        TEST_CASE_MSG(valSum.integer() == 150ll, "actual=" << valSum);
    }

    { // Range
        auto env = Environment::make();
        env->def("rng", Value(IntegerRange(0, 20, 2)));
        env->def("sum", Value(0ll));

        auto foreach = CodeNode::make<Foreach>(
            "i",
            var("rng"),
            assign("sum", add(var("sum"), var("i"))));
        Value result = foreach->eval(env);
        TEST_CASE_MSG(result.isInt(), "actual=" << result.typeToString());
        TEST_CASE_MSG(result.integer() == 90ll, "actual=" << result);

        auto const & sum = env->get("sum");
        TEST_CASE_MSG(sum.isInt(), "actual=" << sum.typeToString());
        TEST_CASE_MSG(sum.integer() == 90ll, "actual=" << sum);
    }
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeLambdaExpr() {
    auto env = Environment::make();
    env->def("foo", Value(1ll));

    const Lambda::ParamList params({"x", "y"});

    CodeNode::SharedPtr body(
        new ArithOp(ArithOp::Add,
                    CodeNode::SharedPtr(
                        new ArithOp(ArithOp::Add,
                                    CodeNode::SharedPtr(new Variable("x")),
                                    CodeNode::SharedPtr(new Variable("y")))),
                    CodeNode::SharedPtr(new Variable("foo"))));

    CodeNode::SharedPtr lambdaExpr(new LambdaExpr(params, body));
    Value lambdaVal = lambdaExpr->eval(env);
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
void UnitTest::testCodeNodeLambdaApp() {
    auto env = Environment::make();

    const Lambda::ParamList params({"x", "y"});

    CodeNode::SharedPtr body(
        new ArithOp(ArithOp::Add,
                    CodeNode::SharedPtr(new Variable("x")),
                    CodeNode::SharedPtr(new Variable("y"))));

    CodeNode::SharedPtr lambdaExpr(new LambdaExpr(params, body));

    CodeNode::SharedPtrList args;
    args.push_back(CodeNode::SharedPtr(new Literal(Value(3ll))));
    args.push_back(CodeNode::SharedPtr(new Literal(Value(4ll))));
    CodeNode::SharedPtr appExpr(new LambdaApp(lambdaExpr, args));

    Value result = appExpr->eval(env);
    TEST_CASE_MSG(result.isInt(), "actual=" << result.typeToString());
    TEST_CASE_MSG(result.integer() == 7ll, "actual=" << result);
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeFunctionExpr() {
    auto env = Environment::make();

    const Lambda::ParamList params({"x", "y"});

    CodeNode::SharedPtr body(
        new ArithOp(ArithOp::Add,
                    CodeNode::SharedPtr(new Variable("x")),
                    CodeNode::SharedPtr(new Variable("y"))));

    CodeNode::SharedPtr ftnExpr(new FunctionExpr("myfunc", params, body));
    Value ftnVal = ftnExpr->eval(env);
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
void UnitTest::testCodeNodeFunctionApp() {
    auto env = Environment::make();

    CodeNode::SharedPtr ftnExpr(
        CodeNode::make<FunctionExpr>(
            "myfunc",
            Lambda::ParamList({"x", "y"}),
            CodeNode::make<ArithOp>(
                ArithOp::Add,
                CodeNode::make<Variable>("x"),
                CodeNode::make<Variable>("y"))));
    Value ftnVal = ftnExpr->eval(env);
    TEST_CASE_MSG(ftnVal.isClosure(), "actual=" << ftnVal.typeToString());

    CodeNode::SharedPtr ftnApp(
        CodeNode::make<FunctionApp>(
            "myfunc",
            CodeNode::SharedPtrList({
                    CodeNode::make<Literal>(Value(3ll)),
                        CodeNode::make<Literal>(Value(4ll))})));

    Value result = ftnApp->eval(env);
    TEST_CASE_MSG(result.isInt(), "actual=" << result.typeToString());
    TEST_CASE_MSG(result.integer() == 7ll, "actual=" << result);    
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeStruct() {
    auto env = Environment::make();

    CodeNode::SharedPtr structExpr(
        CodeNode::make<StructExpr>("Person", Struct::MemberList({"name", "age"})));
    Value structVal = structExpr->eval(env);
    TEST_CASE_MSG(structVal.isUserType(), "actual=" << structVal.typeToString());

    const Struct &s = structVal.userType();
    TEST_CASE_MSG(s.name() == "Person", "actual=" << s.name());
    TEST_CASE_MSG(s.members().size() == 2, "actual=" << s.members().size());
    TEST_CASE_MSG(s.members()[0] == "name", "actual=" << s.members()[0]);
    TEST_CASE_MSG(s.members()[1] == "age", "actual=" << s.members()[1]);
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeIsStructName() {
    auto env = Environment::make();

    CodeNode::SharedPtr isStructName(
        CodeNode::make<IsStructName>(
            CodeNode::make<StructExpr>("Person", Struct::MemberList({"name", "age"})),
            "Person"));

    Value result = isStructName->eval(env);
    TEST_CASE_MSG(result.isBool(), "actual=" << result.typeToString());
    TEST_CASE_MSG(result == Value::True, "actual=" << result << " (" << result.typeToString() << ')');
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeMakeInstance() {
    auto env = Environment::make();

    CodeNode::SharedPtr structExpr(
        CodeNode::make<StructExpr>("Person", Struct::MemberList({"name", "age"})));

    Value structValue = structExpr->eval(env);
    TEST_CASE_MSG(structValue.isUserType(), "actual=" << structValue.typeToString());
    TEST_CASE_MSG(structValue.userType().name() == "Person", "actual=" << structValue.userType().name());

    CodeNode::SharedPtr makeInstance(
        CodeNode::make<MakeInstance>("Person"));

    Value instValue = makeInstance->eval(env);
    TEST_CASE_MSG(instValue.isUserObject(), "actual=" << instValue.typeToString());

    const Struct &type = instValue.userObject().type();
    TEST_CASE_MSG(type.name() == "Person", "actual=" << type.name());

    const Instance &inst = instValue.userObject();
    TEST_CASE_MSG(inst.get("name") == Value::Null, "actual=" << inst.get("name"));
    TEST_CASE_MSG(inst.get("age") == Value::Null, "actual=" << inst.get("age"));
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeMakeInstanceWithInitArgs() {
    auto env = Environment::make();

    CodeNode::SharedPtr structExpr(
        CodeNode::make<StructExpr>("Person", Struct::MemberList({"name", "age"})));

    Value structValue = structExpr->eval(env);
    TEST_CASE_MSG(structValue.isUserType(), "actual=" << structValue.typeToString());
    TEST_CASE_MSG(structValue.userType().name() == "Person", "actual=" << structValue.userType().name());

    CodeNode::SharedPtr nameInit = CodeNode::make<Literal>(Value("John"));
    CodeNode::SharedPtr ageInit = CodeNode::make<Literal>(Value(30ll));

    CodeNode::SharedPtr makeInstance(
        CodeNode::make<MakeInstance>("Person", CodeNode::NameSharedPtrs{{"name", nameInit}, {"age", ageInit}}));

    Value instValue = makeInstance->eval(env);
    TEST_CASE_MSG(instValue.isUserObject(), "actual=" << instValue.typeToString());

    const Struct &type = instValue.userObject().type();
    TEST_CASE_MSG(type.name() == "Person", "actual=" << type.name());

    const Instance &inst = instValue.userObject();
    TEST_CASE_MSG(inst.get("name") == Value("John"), "actual=" << inst.get("name"));
    TEST_CASE_MSG(inst.get("age") == Value(30ll), "actual=" << inst.get("age"));

    // Partial init args
    makeInstance = CodeNode::make<MakeInstance>("Person", CodeNode::NameSharedPtrs{{"name", nameInit}});

    instValue = makeInstance->eval(env);
    TEST_CASE_MSG(instValue.isUserObject(), "actual=" << instValue.typeToString());

    const Instance &inst2 = instValue.userObject();
    TEST_CASE_MSG(inst2.get("name") == Value("John"), "actual=" << inst2.get("name"));
    TEST_CASE_MSG(inst2.get("age") == Value::Null, "actual=" << inst2.get("age"));
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeIsInstanceOf() {
    auto env = Environment::make();

    CodeNode::make<StructExpr>("Person", Struct::MemberList({"name", "age"}))
        ->eval(env);

    Value value = 
        CodeNode::make<IsInstanceOf>(
            CodeNode::make<MakeInstance>("Person"),
            "Person")
        ->eval(env);
    TEST_CASE_MSG(value == Value::True, "actual=" << value);
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeStructName() {
    auto env = Environment::make();

    CodeNode::make<StructExpr>("Person", Struct::MemberList({"name", "age"}))
        ->eval(env);

    Value value =
        CodeNode::make<StructName>(
            CodeNode::make<MakeInstance>("Person"))
        ->eval(env);
    TEST_CASE_MSG(value == Value("Person"), "actual=" << value);

    value =
        CodeNode::make<StructName>(
            CodeNode::make<Variable>("Person"))
        ->eval(env);
    TEST_CASE_MSG(value == Value("Person"), "actual" << value);

    try {
        value =
            CodeNode::make<StructName>(
                CodeNode::make<Literal>(Value::Zero))
            ->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidExpressionType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeGetSetMember() {
    auto env = Environment::make();

    CodeNode::make<StructExpr>("Person", Struct::MemberList({"name", "age"}))
        ->eval(env);

    CodeNode::make<Define>(
        "inst",
        CodeNode::make<MakeInstance>("Person"))
        ->eval(env);

    Value value = 
        CodeNode::make<IsInstanceOf>(
            CodeNode::make<Variable>("inst"),
            "Person")
        ->eval(env);
    TEST_CASE_MSG(value == Value::True, "actual=" << value);

    value = 
        CodeNode::make<GetMember>(
            CodeNode::make<Variable>("inst"),
            "name")
        ->eval(env);
    TEST_CASE_MSG(value == Value::Null, "actual=" << value);

    value = 
        CodeNode::make<SetMember>(
            CodeNode::make<Variable>("inst"),
            "name",
            CodeNode::make<Literal>(Value("John")))
        ->eval(env);
    TEST_CASE_MSG(value == Value("John"), "actual=" << value);

    value = 
        CodeNode::make<GetMember>(
            CodeNode::make<Variable>("inst"),
            "name")
        ->eval(env);
    TEST_CASE_MSG(value == Value("John"), "actual=" << value);
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeStringLen() {
    auto env = Environment::make();

    Value value =
        CodeNode::make<StringLen>(
            CodeNode::make<Literal>(Value("")))
        ->eval(env);
    TEST_CASE_MSG(value == Value::Zero, "actual=" << value);

    value =
        CodeNode::make<StringLen>(
            CodeNode::make<Literal>("a"))
        ->eval(env);
    TEST_CASE_MSG(value == Value(1ll), "actual=" << value);

    value =
        CodeNode::make<StringLen>(
            CodeNode::make<Literal>("abcde"))
        ->eval(env);
    TEST_CASE_MSG(value == Value(5ll), "actual=" << value);

    try {
        value =
            CodeNode::make<StringLen>(
                CodeNode::make<Literal>(Value::Zero))
            ->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) { TEST_CASE(false); }
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeStringGet() {
    auto env = Environment::make();

    Value value =
        CodeNode::make<StringGet>(
            CodeNode::make<Literal>(Value("abc")),
            CodeNode::make<Literal>(Value(0ll)))
        ->eval(env);
    TEST_CASE_MSG(value == Value('a'), "actual=" << value);

    value =
        CodeNode::make<StringGet>(
            CodeNode::make<Literal>(Value("abc")),
            CodeNode::make<Literal>(Value(1ll)))
        ->eval(env);
    TEST_CASE_MSG(value == Value('b'), "actual=" << value);

    value =
        CodeNode::make<StringGet>(
            CodeNode::make<Literal>(Value("abc")),
            CodeNode::make<Literal>(Value(2ll)))
        ->eval(env);
    TEST_CASE_MSG(value == Value('c'), "actual=" << value);

    try {
        value =
            CodeNode::make<StringGet>(
                CodeNode::make<Literal>(Value("abc")),
                CodeNode::make<Literal>(Value(3ll)))
            ->eval(env);
        TEST_CASE(false);
    }
    catch (OutOfRange const &) {}
    catch (...) { TEST_CASE(false); }

    try {
        value =
            CodeNode::make<StringGet>(
                CodeNode::make<Literal>(Value("abc")),
                CodeNode::make<Literal>(Value(-1ll)))
            ->eval(env);
        TEST_CASE(false);
    }
    catch (OutOfRange const &) {}
    catch (...) { TEST_CASE(false); }
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeStringSet() {
    auto env = Environment::make();
    env->def("str", Value("abc"));

    CodeNode::SharedPtr var = CodeNode::make<Variable>("str");

    Value value =
        CodeNode::make<StringSet>(
            var,
            CodeNode::make<Literal>(Value(0ll)),
            CodeNode::make<Literal>(Value('A')))
        ->eval(env);
    TEST_CASE_MSG(value == Value('A'), "actual=" << value);

    value = var->eval(env);
    TEST_CASE_MSG(value == Value("Abc"), "actual=" << value);

    value =
        CodeNode::make<StringSet>(
            var,
            CodeNode::make<Literal>(Value(1ll)),
            CodeNode::make<Literal>(Value('B')))
        ->eval(env);
    TEST_CASE_MSG(value == Value('B'), "actual=" << value);

    value = var->eval(env);
    TEST_CASE_MSG(value == Value("ABc"), "actual=" << value);

    value =
        CodeNode::make<StringSet>(
            var,
            CodeNode::make<Literal>(Value(2ll)),
            CodeNode::make<Literal>(Value('C')))
        ->eval(env);
    TEST_CASE_MSG(value == Value('C'), "actual=" << value);

    value = var->eval(env);
    TEST_CASE_MSG(value == Value("ABC"), "actual=" << value);

    try {
        value =
            CodeNode::make<StringSet>(
                var,
                CodeNode::make<Literal>(Value(-1ll)),
                CodeNode::make<Literal>(Value('X')))
            ->eval(env);
        TEST_CASE(false);
    }
    catch (OutOfRange const &) {}
    catch (...) { TEST_CASE(false); }

    try {
        value =
            CodeNode::make<StringSet>(
                var,
                CodeNode::make<Literal>(Value(3ll)),
                CodeNode::make<Literal>(Value('Y')))
            ->eval(env);
        TEST_CASE(false);
    }
    catch (OutOfRange const &) {}
    catch (...) { TEST_CASE(false); }

    value = var->eval(env);
    TEST_CASE_MSG(value == Value("ABC"), "actual=" << value);
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeStringCat() {
    auto env = Environment::make();
    env->def("str", Value("abc"));

    CodeNode::SharedPtr var = CodeNode::make<Variable>("str");

    Value value =
        CodeNode::make<StringCat>(
            var,
            CodeNode::make<Literal>(Value("de")))
        ->eval(env);
    TEST_CASE_MSG(value == Value("abcde"), "actual=" << value);

    value = var->eval(env);
    TEST_CASE_MSG(value == Value("abcde"), "actual=" << value);

    value =
        CodeNode::make<StringCat>(
            var,
            CodeNode::make<Literal>(Value('f')))
        ->eval(env);
    TEST_CASE_MSG(value == Value("abcdef"), "actual=" << value);

    value = var->eval(env);
    TEST_CASE_MSG(value == Value("abcdef"), "actual=" << value);

    try {
        value =
            CodeNode::make<StringCat>(
                var,
                CodeNode::make<Literal>(Value::Zero))
            ->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) { TEST_CASE(false); }

    value = var->eval(env);
    TEST_CASE_MSG(value == Value("abcdef"), "actual=" << value);
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeSubString() {
    auto env = Environment::make();
    env->def("str", Value("0123456789"));

    CodeNode::SharedPtr var = CodeNode::make<Variable>("str");
    CodeNode::SharedPtr minus1 = CodeNode::make<Literal>(Value(-1ll));
    CodeNode::SharedPtr zero = CodeNode::make<Literal>(Value::Zero);
    CodeNode::SharedPtr three = CodeNode::make<Literal>(Value(3ll));
    CodeNode::SharedPtr four = CodeNode::make<Literal>(Value(4ll));
    CodeNode::SharedPtr five = CodeNode::make<Literal>(Value(5ll));
    CodeNode::SharedPtr ten = CodeNode::make<Literal>(Value(10ll));
    CodeNode::SharedPtr eleven = CodeNode::make<Literal>(Value(11ll));

    Value value = CodeNode::make<SubString>(var, zero)->eval(env);
    TEST_CASE_MSG(value == Value("0123456789"), "actual=" << value);

    value = CodeNode::make<SubString>(var, five)->eval(env);
    TEST_CASE_MSG(value == Value("56789"), "actual=" << value);

    value = CodeNode::make<SubString>(var, zero, ten)->eval(env);
    TEST_CASE_MSG(value == Value("0123456789"), "actual=" << value);

    value = CodeNode::make<SubString>(var, zero, five)->eval(env);
    TEST_CASE_MSG(value == Value("01234"), "actual=" << value);

    value = CodeNode::make<SubString>(var, four, three)->eval(env);
    TEST_CASE_MSG(value == Value("456"), "actual=" << value);

    try {
        CodeNode::make<SubString>(var, minus1)->eval(env);
        TEST_CASE(false);
    }
    catch (OutOfRange const &) {}
    catch (...) {
        TEST_CASE(false);
    }

    try {
        CodeNode::make<SubString>(var, ten)->eval(env);
        TEST_CASE(false);
    }
    catch (OutOfRange const &) {}
    catch (...) {
        TEST_CASE(false);
    }

    try {
        CodeNode::make<SubString>(var, zero, minus1)->eval(env);
        TEST_CASE(false);
    }
    catch (OutOfRange const &) {}
    catch (...) {
        TEST_CASE(false);
    }

    try {
        CodeNode::make<SubString>(var, zero, eleven)->eval(env);
        TEST_CASE(false);
    }
    catch (OutOfRange const &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeStringFind() {
    auto env = Environment::make();
    env->def("str", Value("0123456789"));

    CodeNode::SharedPtr var = CodeNode::make<Variable>("str");

    auto clit = [](Value::Char c) { return CodeNode::make<Literal>(Value(c)); };
    auto ilit = [](Value::Long i) { return CodeNode::make<Literal>(Value(i)); };
    auto find = [env, var, clit, ilit](Value::Char c, std::optional<Value::Long> i = std::nullopt) {
        return (i
                ? CodeNode::make<StringFind>(var, clit(c), ilit(*i))
                : CodeNode::make<StringFind>(var, clit(c)));
    };

    Value value;

    value = find('0')->eval(env); TEST_CASE_MSG(value == Value::Zero, "actual=" << value);
    value = find('5')->eval(env); TEST_CASE_MSG(value == Value(5ll),  "actual=" << value);
    value = find('9')->eval(env); TEST_CASE_MSG(value == Value(9ll),  "actual=" << value);

    value = find('0', 0)->eval(env); TEST_CASE_MSG(value == Value::Zero, "actual=" << value);
    value = find('5', 0)->eval(env); TEST_CASE_MSG(value == Value(5ll),  "actual=" << value);
    value = find('9', 0)->eval(env); TEST_CASE_MSG(value == Value(9ll),  "actual=" << value);

    value = find('5', 3)->eval(env); TEST_CASE_MSG(value == Value(5ll), "actual=" << value);
    value = find('8', 6)->eval(env); TEST_CASE_MSG(value == Value(8ll), "actual=" << value);
    value = find('9', 9)->eval(env); TEST_CASE_MSG(value == Value(9ll), "actual=" << value);

    value = find('0', 1)->eval(env); TEST_CASE_MSG(value == Value(-1ll), "actual=" << value);
    value = find('5', 6)->eval(env); TEST_CASE_MSG(value == Value(-1ll), "actual=" << value);

    value = find('A', 0)->eval(env); TEST_CASE_MSG(value == Value(-1ll), "actual=" << value);
    value = find('B', 5)->eval(env); TEST_CASE_MSG(value == Value(-1ll), "actual=" << value);

    try {
        CodeNode::make<StringFind>(var, ilit(0))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }

    try {
        CodeNode::make<StringFind>(var, clit('0'), clit('0'))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }

    try {
        CodeNode::make<StringFind>(var, clit('0'), ilit(10))->eval(env);
        TEST_CASE(false);
    }
    catch (const OutOfRange &) {}
    catch (...) {
        TEST_CASE(false);
    }

    try {
        CodeNode::make<StringFind>(var, clit('0'), ilit(-1))->eval(env);
        TEST_CASE(false);
    }
    catch (const OutOfRange &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeStringCount() {
    auto env = Environment::make();
    env->def("str", Value("0120344501223678910"));

    CodeNode::SharedPtr var = CodeNode::make<Variable>("str");

    auto clit = [](Value::Char c) { return CodeNode::make<Literal>(Value(c)); };
    auto count = [var, clit](Value::Char c) { return CodeNode::make<StringCount>(var, clit(c)); };

    Value value;

    value = count('9')->eval(env); TEST_CASE_MSG(value == Value(1ll),  "actual=" << value);
    value = count('8')->eval(env); TEST_CASE_MSG(value == Value(1ll),  "actual=" << value);
    value = count('7')->eval(env); TEST_CASE_MSG(value == Value(1ll),  "actual=" << value);
    value = count('6')->eval(env); TEST_CASE_MSG(value == Value(1ll),  "actual=" << value);
    value = count('5')->eval(env); TEST_CASE_MSG(value == Value(1ll),  "actual=" << value);
    value = count('4')->eval(env); TEST_CASE_MSG(value == Value(2ll),  "actual=" << value);
    value = count('3')->eval(env); TEST_CASE_MSG(value == Value(2ll),  "actual=" << value);
    value = count('2')->eval(env); TEST_CASE_MSG(value == Value(3ll),  "actual=" << value);
    value = count('1')->eval(env); TEST_CASE_MSG(value == Value(3ll),  "actual=" << value);
    value = count('0')->eval(env); TEST_CASE_MSG(value == Value(4ll),  "actual=" << value);
    value = count('a')->eval(env); TEST_CASE_MSG(value == Value::Zero, "actual=" << value);

    try {
        CodeNode::make<StringCount>(clit('b'), clit('0'))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }

    try {
        CodeNode::make<StringCount>(var, CodeNode::make<Literal>(Value::Zero))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeStringCompare() {
    auto env = Environment::make();
    env->def("str1", Value("abcde"));
    env->def("str2", Value("bcdef"));
    env->def("str3", Value("cdefg"));
    env->def("str4", Value("cdefg"));
    env->def("num", Value::Zero);

    CodeNode::SharedPtr str1 = CodeNode::make<Variable>("str1");
    CodeNode::SharedPtr str2 = CodeNode::make<Variable>("str2");
    CodeNode::SharedPtr str3 = CodeNode::make<Variable>("str3");
    CodeNode::SharedPtr str4 = CodeNode::make<Variable>("str4");
    CodeNode::SharedPtr num = CodeNode::make<Variable>("num");

    auto compare = [](auto lhs, auto rhs) { return CodeNode::make<StringCompare>(lhs, rhs); };

    Value value;

    value = compare(str1, str2)->eval(env); TEST_CASE_MSG(value == Value(-1ll), "actual=" << value);
    value = compare(str2, str1)->eval(env); TEST_CASE_MSG(value == Value(1ll),  "actual=" << value);
    value = compare(str1, str3)->eval(env); TEST_CASE_MSG(value == Value(-2ll), "actual=" << value);
    value = compare(str3, str1)->eval(env); TEST_CASE_MSG(value == Value(2ll),  "actual=" << value);
    value = compare(str2, str3)->eval(env); TEST_CASE_MSG(value == Value(-1ll), "actual=" << value);
    value = compare(str3, str2)->eval(env); TEST_CASE_MSG(value == Value(1ll),  "actual=" << value);
    value = compare(str3, str4)->eval(env); TEST_CASE_MSG(value == Value::Zero, "actual=" << value);

    try {
        value = compare(str1, num)->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &ex) {
        TEST_CASE(std::string("Invalid operand type, expected=String actual=int") == ex.what());
    }
    catch (...) {
        TEST_CASE(false);
    }

    try {
        value = compare(num, str1)->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &ex) {
        TEST_CASE(std::string("Invalid operand type, expected=String actual=int") == ex.what());
    }
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeStringSort() {
    auto env = Environment::make();
    env->def("str0", Value(""));
    env->def("str1", Value("a"));
    env->def("str2", Value("ba"));
    env->def("str3", Value("bca"));
    env->def("str4", Value("dbca"));
    env->def("str5", Value("bdcabdcdcd"));
    env->def("num", Value::Zero);

    CodeNode::SharedPtr str0 = CodeNode::make<Variable>("str0");
    CodeNode::SharedPtr str1 = CodeNode::make<Variable>("str1");
    CodeNode::SharedPtr str2 = CodeNode::make<Variable>("str2");
    CodeNode::SharedPtr str3 = CodeNode::make<Variable>("str3");
    CodeNode::SharedPtr str4 = CodeNode::make<Variable>("str4");
    CodeNode::SharedPtr str5 = CodeNode::make<Variable>("str5");
    CodeNode::SharedPtr num = CodeNode::make<Variable>("num");

    CodeNode::SharedPtr asc = CodeNode::make<Literal>(false);
    CodeNode::SharedPtr desc = CodeNode::make<Literal>(true);
    CodeNode::SharedPtr none = CodeNode::SharedPtr();

    auto testcase =
        [this, &env](auto str, auto desc, auto expect) {
            auto value = CodeNode::make<StringSort>(str, desc)->eval(env);
            TEST_CASE_MSG(value == expect, "value actual=" << value);
            TEST_CASE_MSG(str->eval(env) == expect, "str actual=" << str);
        };

    testcase(str0, asc, Value(""));
    testcase(str1, asc, Value("a"));
    testcase(str2, asc, Value("ab"));
    testcase(str3, asc, Value("abc"));
    testcase(str4, asc, Value("abcd"));
    testcase(str5, asc, Value("abbcccdddd"));

    testcase(str0, desc, Value(""));
    testcase(str1, desc, Value("a"));
    testcase(str2, desc, Value("ba"));
    testcase(str3, desc, Value("cba"));
    testcase(str4, desc, Value("dcba"));
    testcase(str5, desc, Value("ddddcccbba"));

    testcase(str0, none, Value(""));
    testcase(str1, none, Value("a"));
    testcase(str2, none, Value("ab"));
    testcase(str3, none, Value("abc"));
    testcase(str4, none, Value("abcd"));
    testcase(str5, none, Value("abbcccdddd"));

    try {
        testcase(num, asc, Value::Null);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &ex) {
        TEST_CASE(std::string("Invalid operand type, expected=String actual=int") == ex.what());
    }
    catch (...) {
        TEST_CASE(false);
    }

    try {
        testcase(str3, num, Value::Null);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &ex) {
        TEST_CASE(std::string("Invalid operand type, expected=Boolean actual=int") == ex.what());
    }
    catch (...) {
        TEST_CASE(false);
    }

    Value check;
    check = str0->eval(env); TEST_CASE_MSG(check == Value(""),           "actual=" << check);
    check = str1->eval(env); TEST_CASE_MSG(check == Value("a"),          "actual=" << check);
    check = str2->eval(env); TEST_CASE_MSG(check == Value("ab"),         "actual=" << check);
    check = str3->eval(env); TEST_CASE_MSG(check == Value("abc"),        "actual=" << check);
    check = str4->eval(env); TEST_CASE_MSG(check == Value("abcd"),       "actual=" << check);
    check = str5->eval(env); TEST_CASE_MSG(check == Value("abbcccdddd"), "actual=" << check);
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeStringReverse() {
    auto env = Environment::make();
    env->def("str0", Value(""));
    env->def("str1", Value("a"));
    env->def("str2", Value("ab"));
    env->def("str3", Value("abc"));
    env->def("str4", Value("abcd"));
    env->def("num", Value::Zero);

    CodeNode::SharedPtr str0 = CodeNode::make<Variable>("str0");
    CodeNode::SharedPtr str1 = CodeNode::make<Variable>("str1");
    CodeNode::SharedPtr str2 = CodeNode::make<Variable>("str2");
    CodeNode::SharedPtr str3 = CodeNode::make<Variable>("str3");
    CodeNode::SharedPtr str4 = CodeNode::make<Variable>("str4");
    CodeNode::SharedPtr num = CodeNode::make<Variable>("num");

    auto testcase =
        [this, &env](auto str, auto expect) {
            auto value = CodeNode::make<StringReverse>(str)->eval(env);
            TEST_CASE_MSG(value == expect, "value actual=" << value);
            TEST_CASE_MSG(str->eval(env) == expect, "str actual=" << str);
        };

    testcase(str0, Value(""));
    testcase(str1, Value("a"));
    testcase(str2, Value("ba"));
    testcase(str3, Value("cba"));
    testcase(str4, Value("dcba"));

    testcase(str0, Value(""));
    testcase(str1, Value("a"));
    testcase(str2, Value("ab"));
    testcase(str3, Value("abc"));
    testcase(str4, Value("abcd"));

    try {
        testcase(num, Value::Null);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &ex) {
        TEST_CASE(std::string("Invalid operand type, expected=String actual=int") == ex.what());
    }
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeMakeArray() {
    auto env = Environment::make();

    Value value = CodeNode::make<MakeArray>()->eval(env);
    TEST_CASE_MSG(value.isArray(), "actual=" << value.typeToString());
    TEST_CASE_MSG(value.array().size() == 0lu, "actual=" << value.array().size());

    value =
        CodeNode::make<MakeArray>(
            CodeNode::SharedPtrList(
                { CodeNode::make<Literal>(Value('a')),
                  CodeNode::make<Literal>(Value('b')),
                  CodeNode::make<Literal>(Value('c')) }))
        ->eval(env);
    TEST_CASE_MSG(value.isArray(), "actual=" << value.typeToString());
    TEST_CASE_MSG(value.array().size() == 3lu, "actual=" << value.array().size());
    TEST_CASE_MSG(value.array().get(0lu) == Value('a'), "actual=" << value.array().get(0lu));
    TEST_CASE_MSG(value.array().get(1lu) == Value('b'), "actual=" << value.array().get(1lu));
    TEST_CASE_MSG(value.array().get(2lu) == Value('c'), "actual=" << value.array().get(2lu));
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeMakeArraySV() {
    auto env = Environment::make();

    Value value =
        CodeNode::make<MakeArraySV>(
            CodeNode::make<Literal>(Value(3ll)),
            CodeNode::make<Literal>(Value('0')))
        ->eval(env);

    TEST_CASE_MSG(value.isArray(), "actual=" << value.typeToString());
    TEST_CASE_MSG(value.array().size() == 3lu, "actual=" << value.array().size());
    TEST_CASE_MSG(value.array().get(0lu) == Value('0'), "actual=" << value.array().get(0lu));
    TEST_CASE_MSG(value.array().get(1lu) == Value('0'), "actual=" << value.array().get(1lu));
    TEST_CASE_MSG(value.array().get(2lu) == Value('0'), "actual=" << value.array().get(2lu));

    value =
        CodeNode::make<MakeArraySV>(
            CodeNode::make<Literal>(Value(2ll)))
        ->eval(env);

    TEST_CASE_MSG(value.isArray(), "actual=" << value.typeToString());
    TEST_CASE_MSG(value.array().size() == 2lu, "actual=" << value.array().size());
    TEST_CASE_MSG(value.array().get(0lu) == Value::Null, "actual=" << value.array().get(0lu));
    TEST_CASE_MSG(value.array().get(1lu) == Value::Null, "actual=" << value.array().get(1lu));
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeArrayLen() {
    auto env = Environment::make();

    Value value =
        CodeNode::make<ArrayLen>(
            CodeNode::make<Literal>(Value(Sequence())))
        ->eval(env);
    TEST_CASE_MSG(value == Value::Zero, "actual=" << value);

    value =
        CodeNode::make<ArrayLen>(
            CodeNode::make<Literal>(Value(Sequence({Value::Zero}))))
        ->eval(env);
    TEST_CASE_MSG(value == Value(1ll), "actual=" << value);

    value =
        CodeNode::make<ArrayLen>(
            CodeNode::make<Literal>(Value(Sequence({Value::Zero, Value::True, Value::False}))))
        ->eval(env);
    TEST_CASE_MSG(value == Value(3ll), "actual=" << value);

    try {
        value =
            CodeNode::make<ArrayLen>(
                CodeNode::make<Literal>(Value::Zero))
            ->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) { TEST_CASE(false); }
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeArrayGet() {
    auto env = Environment::make();
    env->def("arr", Value(Sequence({Value('a'), Value('b'), Value('c')})));

    const CodeNode::SharedPtr var = CodeNode::make<Variable>("arr");

    auto ilit = [](Value::Long i) {
        return CodeNode::make<Literal>(Value(i));
    };

    Value value = CodeNode::make<ArrayGet>(var, ilit(0))->eval(env);
    TEST_CASE_MSG(value == Value('a'), "actual=" << value);

    value = CodeNode::make<ArrayGet>(var, ilit(1))->eval(env);
    TEST_CASE_MSG(value == Value('b'), "actual=" << value);

    value = CodeNode::make<ArrayGet>(var, ilit(2))->eval(env);
    TEST_CASE_MSG(value == Value('c'), "actual=" << value);

    try {
        CodeNode::make<ArrayGet>(var, ilit(3))->eval(env);
        TEST_CASE(false);
    }
    catch (OutOfRange const &) {}
    catch (...) { TEST_CASE(false); }

    try {
        CodeNode::make<ArrayGet>(var, ilit(-1))->eval(env);
        TEST_CASE(false);
    }
    catch (OutOfRange const &) {}
    catch (...) { TEST_CASE(false); }
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeArraySet() {
    auto env = Environment::make();
    env->def("arr", Value(Sequence({Value('a'), Value('b'), Value('c')})));

    CodeNode::SharedPtr var = CodeNode::make<Variable>("arr");

    Value expectedValue(Sequence({Value('a'), Value('b'), Value('c')}));
    auto &expected = expectedValue.array();

    auto ilit = [](Value::Long i) { return CodeNode::make<Literal>(Value(i)); };
    auto clit = [](Value::Char c) { return CodeNode::make<Literal>(Value(c)); };

    Value value = CodeNode::make<ArraySet>(var, ilit(0), clit('A'))->eval(env);
    TEST_CASE_MSG(value == Value('A'), "actual=" << value);

    value = var->eval(env);
    expected.set(0, 'A');
    TEST_CASE_MSG(value == expectedValue, "actual=" << value);

    value = CodeNode::make<ArraySet>(var, ilit(1), clit('B'))->eval(env);
    TEST_CASE_MSG(value == Value('B'), "actual=" << value);

    value = var->eval(env);
    expected.set(1, 'B');
    TEST_CASE_MSG(value == expectedValue, "actual=" << value);

    value = CodeNode::make<ArraySet>(var, ilit(2), clit('C'))->eval(env);
    TEST_CASE_MSG(value == Value('C'), "actual=" << value);

    value = var->eval(env);
    expected.set(2, 'C');
    TEST_CASE_MSG(value == expectedValue, "actual=" << value);

    try {
        CodeNode::make<ArraySet>(var, ilit(-1), clit('X'))->eval(env);
        TEST_CASE(false);
    }
    catch (OutOfRange const &) {}
    catch (...) { TEST_CASE(false); }

    try {
        CodeNode::make<ArraySet>(var, ilit(3), clit('Y'))->eval(env);
        TEST_CASE(false);
    }
    catch (OutOfRange const &) {}
    catch (...) { TEST_CASE(false); }

    value = var->eval(env);
    TEST_CASE_MSG(value == expectedValue, "actual=" << value);
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeArrayPush() {
    auto env = Environment::make();
    env->def("arr", Value(Sequence()));

    CodeNode::SharedPtr var = CodeNode::make<Variable>("arr");

    auto ilit = [](Value::Long i) { return CodeNode::make<Literal>(Value(i)); };
    auto clit = [](Value::Char c) { return CodeNode::make<Literal>(Value(c)); };

    Value value = CodeNode::make<ArrayPush>(var, ilit(10))->eval(env);
    Value expected = Value(Sequence({Value(10ll)}));
    TEST_CASE_MSG(value == expected, "actual=" << value);

    value = CodeNode::make<ArrayPush>(var, clit('b'))->eval(env);
    expected = Value(Sequence({Value(10ll), Value('b')}));
    TEST_CASE_MSG(value == expected, "actual=" << value);

    value = CodeNode::make<ArrayPush>(var, ilit(25ll))->eval(env);
    expected = Value(Sequence({Value(10ll), Value('b'), Value(25ll)}));
    TEST_CASE_MSG(value == expected, "actual=" << value);
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeArrayPop() {
    auto env = Environment::make();
    env->def("arr", arrval(Value(0ll), Value(1ll)));

    CodeNode::SharedPtr var = CodeNode::make<Variable>("arr");

    Value value = CodeNode::make<ArrayPop>(var)->eval(env);
    Value arrValue = var->eval(env);
    TEST_CASE_MSG(value == Value(1ll), "actual" << value);
    TEST_CASE_MSG(arrValue == arrval(Value(0ll)), "actual" << arrValue);

    value = CodeNode::make<ArrayPop>(var)->eval(env);
    arrValue = var->eval(env);
    TEST_CASE_MSG(value == Value(0ll), "actual" << value);
    TEST_CASE_MSG(arrValue == arrval(), "actual" << arrValue);

    try {
        value = CodeNode::make<ArrayPop>(var)->eval(env);
        TEST_CASE(false);
    }
    catch (OutOfRange const & ex) {
        TEST_CASE_MSG(std::string(ex.what()) == "Out of range: Pop empty array", "actual=" << ex.what());
    }
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeArrayFind() {
    auto env = Environment::make();

    Value const a('a');
    Value const b('b');
    Value const c('c');
    Value const d('d');

    env->def("arr", Value(Sequence({a, b, c, a, b, c})));

    CodeNode::SharedPtr var = CodeNode::make<Variable>("arr");

    auto ilit = [](Value::Long pos) { return CodeNode::make<Literal>(Value(pos)); };
    auto clit = [](Value const & chr) { return CodeNode::make<Literal>(chr); };

    Value value = CodeNode::make<ArrayFind>(var, clit(a))->eval(env);
    TEST_CASE_MSG(value == Value(0ll), "actual=" << value);

    value = CodeNode::make<ArrayFind>(var, clit(b))->eval(env);
    TEST_CASE_MSG(value == Value(1ll), "actual=" << value);

    value = CodeNode::make<ArrayFind>(var, clit(c))->eval(env);
    TEST_CASE_MSG(value == Value(2ll), "actual=" << value);

    value = CodeNode::make<ArrayFind>(var, clit(d))->eval(env);
    TEST_CASE_MSG(value == Value(-1ll), "actual=" << value);

    value = CodeNode::make<ArrayFind>(var, clit(a), ilit(1))->eval(env);
    TEST_CASE_MSG(value == Value(3ll), "actual=" << value);

    value = CodeNode::make<ArrayFind>(var, clit(b), ilit(2))->eval(env);
    TEST_CASE_MSG(value == Value(4ll), "actual=" << value);

    value = CodeNode::make<ArrayFind>(var, clit(c), ilit(3))->eval(env);
    TEST_CASE_MSG(value == Value(5ll), "actual=" << value);

    value = CodeNode::make<ArrayFind>(var, clit(d), ilit(3))->eval(env);
    TEST_CASE_MSG(value == Value(-1ll), "actual=" << value);

    value = CodeNode::make<ArrayFind>(var, clit(a), ilit(4))->eval(env);
    TEST_CASE_MSG(value == Value(-1ll), "actual=" << value);

    value = CodeNode::make<ArrayFind>(var, clit(b), ilit(5))->eval(env);
    TEST_CASE_MSG(value == Value(-1ll), "actual=" << value);

    try {
        value = CodeNode::make<ArrayFind>(var, clit(c), ilit(6))->eval(env);
        TEST_CASE(false);
    }
    catch (OutOfRange const & ex) {
        TEST_CASE_MSG(std::string("Out of range: arrfind position access") == ex.what(),
                      "actual='" << ex.what() << "'");
    }
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeArrayCount() {
    auto env = Environment::make();

    const Value v1(1ll);
    const Value v2(2ll);
    const Value v3(3ll);
    const Value v4(4ll);

    env->def("arr", Value(Sequence({v1, v2, v3, v2, v3, v4, v3, v4, v4, v4})));

    CodeNode::SharedPtr var = CodeNode::make<Variable>("arr");

    auto ilit = [](Value::Long i) { return CodeNode::make<Literal>(Value(i)); };
    auto count = [var, ilit](Value::Long i) { return CodeNode::make<ArrayCount>(var, ilit(i)); };

    Value value;

    value = count(1)->eval(env); TEST_CASE_MSG(value == Value(1ll), "actual=" << value);
    value = count(2)->eval(env); TEST_CASE_MSG(value == Value(2ll), "actual=" << value);
    value = count(3)->eval(env); TEST_CASE_MSG(value == Value(3ll), "actual=" << value);
    value = count(4)->eval(env); TEST_CASE_MSG(value == Value(4ll), "actual=" << value);
    value = count(5)->eval(env); TEST_CASE_MSG(value == Value(0ll), "actual=" << value);

    try {
        CodeNode::make<ArrayCount>(ilit(1), ilit(0))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeArraySort() {
    auto env = Environment::make();

    const Value v1(1ll);
    const Value v2(2ll);
    const Value v3(3ll);
    const Value v4(4ll);

    env->def("arr", Value::Null);
    env->def("num", Value::Zero);

    CodeNode::SharedPtr arr = CodeNode::make<Variable>("arr");
    CodeNode::SharedPtr num = CodeNode::make<Variable>("num");

    CodeNode::SharedPtr asc = CodeNode::make<Literal>(false);
    CodeNode::SharedPtr desc = CodeNode::make<Literal>(true);
    CodeNode::SharedPtr none = CodeNode::SharedPtr();

    auto mkarr =
        [&](std::initializer_list<Value> seq) {
            env->set("arr", Value(Sequence(seq)));
            return arr;
        };

    auto testcase =
        [this, &env](auto arr, auto desc, auto expect) {
            auto value = CodeNode::make<ArraySort>(arr, desc)->eval(env);
            TEST_CASE_MSG(value == expect, "value actual=" << value);
            TEST_CASE_MSG(arr->eval(env) == expect, "arr actual=" << arr);
        };

    testcase(mkarr({}),               none, arrval());
    testcase(mkarr({v1}),             none, arrval(v1));
    testcase(mkarr({v2, v1}),         none, arrval(v1, v2));
    testcase(mkarr({v3, v2, v1}),     none, arrval(v1, v2, v3));
    testcase(mkarr({v4, v3, v2, v1}), none, arrval(v1, v2, v3, v4));

    testcase(mkarr({}),               asc, arrval());
    testcase(mkarr({v1}),             asc, arrval(v1));
    testcase(mkarr({v2, v1}),         asc, arrval(v1, v2));
    testcase(mkarr({v3, v2, v1}),     asc, arrval(v1, v2, v3));
    testcase(mkarr({v4, v3, v2, v1}), asc, arrval(v1, v2, v3, v4));

    testcase(mkarr({}),               desc, arrval());
    testcase(mkarr({v1}),             desc, arrval(v1));
    testcase(mkarr({v1, v2}),         desc, arrval(v2, v1));
    testcase(mkarr({v1, v2, v3}),     desc, arrval(v3, v2, v1));
    testcase(mkarr({v1, v2, v3, v4}), desc, arrval(v4, v3, v2, v1));

    try {
        testcase(num, asc, Value::Null);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &ex) {
        TEST_CASE(std::string("Invalid operand type, expected=Array actual=int") == ex.what());
    }
    catch (...) {
        TEST_CASE(false);
    }

    try {
        testcase(arr, num, Value::Null);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &ex) {
        TEST_CASE(std::string("Invalid operand type, expected=Boolean actual=int") == ex.what());
    }
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeArrayReverse() {
    auto env = Environment::make();

    const Value v1(1ll);
    const Value v2(2ll);
    const Value v3(3ll);
    const Value v4(4ll);

    env->def("arr", Value::Null);
    env->def("num", Value::Zero);

    CodeNode::SharedPtr arr = CodeNode::make<Variable>("arr");
    CodeNode::SharedPtr num = CodeNode::make<Variable>("num");

    auto mkarr =
        [&](std::initializer_list<Value> seq) {
            env->set("arr", Value(Sequence(seq)));
            return arr;
        };

    auto testcase =
        [this, &env](auto arr, auto expect) {
            auto value = CodeNode::make<ArrayReverse>(arr)->eval(env);
            TEST_CASE_MSG(value == expect, "value actual=" << value);
            TEST_CASE_MSG(arr->eval(env) == expect, "arr actual=" << arr);
        };

    testcase(mkarr({}),               arrval());
    testcase(mkarr({v1}),             arrval(v1));
    testcase(mkarr({v2, v1}),         arrval(v1, v2));
    testcase(mkarr({v3, v2, v1}),     arrval(v1, v2, v3));
    testcase(mkarr({v4, v3, v2, v1}), arrval(v1, v2, v3, v4));

    testcase(mkarr({}),               arrval());
    testcase(mkarr({v1}),             arrval(v1));
    testcase(mkarr({v1, v2}),         arrval(v2, v1));
    testcase(mkarr({v1, v2, v3}),     arrval(v3, v2, v1));
    testcase(mkarr({v1, v2, v3, v4}), arrval(v4, v3, v2, v1));

    try {
        testcase(num, Value::Null);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &ex) {
        TEST_CASE(std::string("Invalid operand type, expected=Array actual=int") == ex.what());
    }
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeArrayClear() {
    auto env = Environment::make();

    const Value v1(1ll);
    const Value v2(2ll);
    const Value v3(3ll);

    env->def("arr", Value(Sequence({v1, v2, v3})));

    CodeNode::SharedPtr arr = CodeNode::make<Variable>("arr");

    auto arrVal = arr->eval(env);
    TEST_CASE_MSG(arrVal == Value(Sequence({v1, v2, v3})), "array actual=" << arrVal);

    auto result = CodeNode::make<ArrayClear>(arr)->eval(env);
    TEST_CASE_MSG(result.isNull(), "result actual=" << result);

    arrVal = arr->eval(env);
    TEST_CASE_MSG(arrVal == Value(Sequence()), "array actual=" << arrVal);

    try {
        CodeNode::make<ArrayClear>(
            CodeNode::make<Literal>(Value(5ll)))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &ex) {
        TEST_CASE(std::string("Invalid operand type, expected=Array actual=int") == ex.what());
    }
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeArrayInsert() {
    auto env = Environment::make();

    CodeNode::SharedPtr arr = CodeNode::make<Variable>("arr");
    CodeNode::SharedPtr num = CodeNode::make<Variable>("num");

    const Value v0 = Value::Zero;
    const Value v1 = Value(1ll);
    const Value v2 = Value(2ll);
    const Value v3 = Value(3ll);
    const Value v4 = Value(4ll);
    const Value v5 = Value(5ll);

    env->def("arr", Value(Sequence()));
    env->def("num", Value::Zero);

    auto testcase =
        [this, &env](CodeNode::SharedPtr arrCode, const Value &pos, const Value &item, const Value &expect) {
            auto posCode = CodeNode::make<Literal>(pos);
            auto itemCode = CodeNode::make<Literal>(item);

            auto result = CodeNode::make<ArrayInsert>(arrCode, posCode, itemCode)->eval(env);
            TEST_CASE_MSG(result == item, "result actual=" << result);

            auto arr = arrCode->eval(env);
            TEST_CASE_MSG(arr == expect, "arr actual=" << arr);
        };

    testcase(arr, v0, v3, arrval(v3));
    testcase(arr, v0, v1, arrval(v1, v3));
    testcase(arr, v2, v5, arrval(v1, v3, v5));
    testcase(arr, v0, v0, arrval(v0, v1, v3, v5));
    testcase(arr, v2, v2, arrval(v0, v1, v2, v3, v5));
    testcase(arr, v4, v4, arrval(v0, v1, v2, v3, v4, v5));

    try {
        testcase(num, v0, v5, Value::Null);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &ex) {
        TEST_CASE(std::string("Invalid operand type, expected=Array actual=int") == ex.what());
    }
    catch (...) {
        TEST_CASE(false);
    }

    try {
        testcase(arr, Value('a'), v5, Value::Null);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &ex) {
        TEST_CASE(std::string("Invalid operand type, expected=Integer actual=char") == ex.what());
    }
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeArrayRemove() {
    auto env = Environment::make();

    CodeNode::SharedPtr arr = CodeNode::make<Variable>("arr");
    CodeNode::SharedPtr num = CodeNode::make<Variable>("num");

    const Value v0 = Value::Zero;
    const Value v1 = Value(1ll);
    const Value v2 = Value(2ll);
    const Value v3 = Value(3ll);
    const Value v4 = Value(4ll);
    const Value v5 = Value(5ll);

    env->def("arr", Value(Sequence({v0, v1, v2, v3, v4, v5})));
    env->def("num", Value::Zero);

    auto testcase =
        [this, &env](CodeNode::SharedPtr arrCode, const Value &pos, const Value &expect) {
            auto posCode = CodeNode::make<Literal>(pos);
            auto result = CodeNode::make<ArrayRemove>(arrCode, posCode)->eval(env);
            TEST_CASE_MSG(result.isNull(), "value actual=" << result);

            auto arr = arrCode->eval(env);
            TEST_CASE_MSG(arr == expect, "arr actual=" << arr);
        };

    testcase(arr, v4, arrval(v0, v1, v2, v3, v5));
    testcase(arr, v2, arrval(v0, v1, v3, v5));
    testcase(arr, v0, arrval(v1, v3, v5));
    testcase(arr, v2, arrval(v1, v3));
    testcase(arr, v0, arrval(v3));
    testcase(arr, v0, arrval());

    try {
        testcase(arr, v3, Value::Null);
        TEST_CASE(false);
    }
    catch (const OutOfRange &ex) {
        TEST_CASE(std::string("Out of range: Array remove access") == ex.what());
    }
    catch (...) {
        TEST_CASE(false);
    }

    try {
        testcase(num, v0, Value::Null);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &ex) {
        TEST_CASE(std::string("Invalid operand type, expected=Array actual=int") == ex.what());
    }
    catch (...) {
        TEST_CASE(false);
    }

    try {
        testcase(arr, Value('a'), Value::Null);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &ex) {
        TEST_CASE(std::string("Invalid operand type, expected=Integer actual=char") == ex.what());
    }
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeStrCharCheck() {
    auto env = Environment::make();

    const Value cUp = Value('U');
    const Value cLo = Value('l');
    const Value cNm = Value('5');
    const Value cPt = Value('?');
    const Value cSp = Value(' ');

    const Value sUp = Value("UPP");
    const Value sLo = Value("low");
    const Value sNm = Value("123");
    const Value sPt = Value("@?!");
    const Value sSp = Value(" \t\n");
    const Value mix = Value("Ul5? ");

    auto lit = [](const Value &val) { return CodeNode::make<Literal>(val); };
    auto check = [lit](StrCharCheck::Type chkType, const Value &val) {
        return CodeNode::make<StrCharCheck>(chkType, lit(val));
    };

    Value value;

    value = check(StrCharCheck::Upper, cUp)->eval(env); TEST_CASE_MSG(value == Value::True, "actual=" << value);
    value = check(StrCharCheck::Lower, cLo)->eval(env); TEST_CASE_MSG(value == Value::True, "actual=" << value);
    value = check(StrCharCheck::Alpha, cLo)->eval(env); TEST_CASE_MSG(value == Value::True, "actual=" << value);
    value = check(StrCharCheck::Numer, cNm)->eval(env); TEST_CASE_MSG(value == Value::True, "actual=" << value);
    value = check(StrCharCheck::Alnum, cLo)->eval(env); TEST_CASE_MSG(value == Value::True, "actual=" << value);
    value = check(StrCharCheck::Alnum, cNm)->eval(env); TEST_CASE_MSG(value == Value::True, "actual=" << value);
    value = check(StrCharCheck::Punct, cPt)->eval(env); TEST_CASE_MSG(value == Value::True, "actual=" << value);
    value = check(StrCharCheck::Space, cSp)->eval(env); TEST_CASE_MSG(value == Value::True, "actual=" << value);

    value = check(StrCharCheck::Upper, cLo)->eval(env); TEST_CASE_MSG(value == Value::False, "actual=" << value);
    value = check(StrCharCheck::Lower, cUp)->eval(env); TEST_CASE_MSG(value == Value::False, "actual=" << value);
    value = check(StrCharCheck::Alpha, cNm)->eval(env); TEST_CASE_MSG(value == Value::False, "actual=" << value);
    value = check(StrCharCheck::Numer, cUp)->eval(env); TEST_CASE_MSG(value == Value::False, "actual=" << value);
    value = check(StrCharCheck::Alnum, cPt)->eval(env); TEST_CASE_MSG(value == Value::False, "actual=" << value);
    value = check(StrCharCheck::Punct, cSp)->eval(env); TEST_CASE_MSG(value == Value::False, "actual=" << value);
    value = check(StrCharCheck::Space, cPt)->eval(env); TEST_CASE_MSG(value == Value::False, "actual=" << value);

    value = check(StrCharCheck::Upper, sUp)->eval(env); TEST_CASE_MSG(value == Value::True, "actual=" << value);
    value = check(StrCharCheck::Lower, sLo)->eval(env); TEST_CASE_MSG(value == Value::True, "actual=" << value);
    value = check(StrCharCheck::Alpha, sLo)->eval(env); TEST_CASE_MSG(value == Value::True, "actual=" << value);
    value = check(StrCharCheck::Numer, sNm)->eval(env); TEST_CASE_MSG(value == Value::True, "actual=" << value);
    value = check(StrCharCheck::Alnum, sLo)->eval(env); TEST_CASE_MSG(value == Value::True, "actual=" << value);
    value = check(StrCharCheck::Alnum, sNm)->eval(env); TEST_CASE_MSG(value == Value::True, "actual=" << value);
    value = check(StrCharCheck::Punct, sPt)->eval(env); TEST_CASE_MSG(value == Value::True, "actual=" << value);
    value = check(StrCharCheck::Space, sSp)->eval(env); TEST_CASE_MSG(value == Value::True, "actual=" << value);

    value = check(StrCharCheck::Upper, sLo)->eval(env); TEST_CASE_MSG(value == Value::False, "actual=" << value);
    value = check(StrCharCheck::Lower, sUp)->eval(env); TEST_CASE_MSG(value == Value::False, "actual=" << value);
    value = check(StrCharCheck::Alpha, sNm)->eval(env); TEST_CASE_MSG(value == Value::False, "actual=" << value);
    value = check(StrCharCheck::Numer, sUp)->eval(env); TEST_CASE_MSG(value == Value::False, "actual=" << value);
    value = check(StrCharCheck::Alnum, sPt)->eval(env); TEST_CASE_MSG(value == Value::False, "actual=" << value);
    value = check(StrCharCheck::Punct, sSp)->eval(env); TEST_CASE_MSG(value == Value::False, "actual=" << value);
    value = check(StrCharCheck::Space, sPt)->eval(env); TEST_CASE_MSG(value == Value::False, "actual=" << value);

    value = check(StrCharCheck::Upper, mix)->eval(env); TEST_CASE_MSG(value == Value::False, "actual=" << value);
    value = check(StrCharCheck::Lower, mix)->eval(env); TEST_CASE_MSG(value == Value::False, "actual=" << value);
    value = check(StrCharCheck::Alpha, mix)->eval(env); TEST_CASE_MSG(value == Value::False, "actual=" << value);
    value = check(StrCharCheck::Numer, mix)->eval(env); TEST_CASE_MSG(value == Value::False, "actual=" << value);
    value = check(StrCharCheck::Alnum, mix)->eval(env); TEST_CASE_MSG(value == Value::False, "actual=" << value);
    value = check(StrCharCheck::Punct, mix)->eval(env); TEST_CASE_MSG(value == Value::False, "actual=" << value);
    value = check(StrCharCheck::Space, mix)->eval(env); TEST_CASE_MSG(value == Value::False, "actual=" << value);

    try {
        check(StrCharCheck::Upper, Value(5ll))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeStrCharTransform() {
    auto env = Environment::make();

    const Value a = Value('a');
    const Value A = Value('A');
    const Value n = Value('5');

    const Value str = Value("str");
    const Value STR = Value("STR");
    const Value num = Value("525");

    auto lit = [](const Value &val) { return CodeNode::make<Literal>(val); };
    auto trans = [lit](StrCharTransform::Type type, const Value &val) {
        return CodeNode::make<StrCharTransform>(type, lit(val));
    };

    Value value;

    value = trans(StrCharTransform::ToUpper, a)->eval(env); TEST_CASE_MSG(value == Value('A'), "actual=" << value);
    value = trans(StrCharTransform::ToUpper, A)->eval(env); TEST_CASE_MSG(value == Value('A'), "actual=" << value);
    value = trans(StrCharTransform::ToUpper, n)->eval(env); TEST_CASE_MSG(value == Value('5'), "actual=" << value);
    value = trans(StrCharTransform::ToLower, a)->eval(env); TEST_CASE_MSG(value == Value('a'), "actual=" << value);
    value = trans(StrCharTransform::ToLower, A)->eval(env); TEST_CASE_MSG(value == Value('a'), "actual=" << value);
    value = trans(StrCharTransform::ToLower, n)->eval(env); TEST_CASE_MSG(value == Value('5'), "actual=" << value);

    value = trans(StrCharTransform::ToUpper, str)->eval(env); TEST_CASE_MSG(value == Value("STR"), "actual=" << value);
    value = trans(StrCharTransform::ToUpper, STR)->eval(env); TEST_CASE_MSG(value == Value("STR"), "actual=" << value);
    value = trans(StrCharTransform::ToUpper, num)->eval(env); TEST_CASE_MSG(value == Value("525"), "actual=" << value);
    value = trans(StrCharTransform::ToLower, str)->eval(env); TEST_CASE_MSG(value == Value("str"), "actual=" << value);
    value = trans(StrCharTransform::ToLower, STR)->eval(env); TEST_CASE_MSG(value == Value("str"), "actual=" << value);
    value = trans(StrCharTransform::ToLower, num)->eval(env); TEST_CASE_MSG(value == Value("525"), "actual=" << value);

    try {
        trans(StrCharTransform::ToUpper, Value(5ll))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeImportModule() {
    const std::string moduleName = "importtest";
    auto tempFile(createTempModuleFile(moduleName));

    {
        auto env = Environment::make();

        auto varName = [&moduleName](const char *name) { return moduleName + '.' + name; };

        auto import = CodeNode::make<ImportModule>(moduleName);
        TEST_CASE(import->eval(env) == Value::True);

        TEST_CASE_MSG(env->size() == 3, "actual=" << env->size());
        TEST_CASE(env->exists(varName("PI")));
        TEST_CASE(env->exists(varName("add")));
        TEST_CASE(env->exists(varName("sub")));

        TEST_CASE(env->get(varName("PI")).isReal());
        TEST_CASE(env->get(varName("add")).isClosure());
        TEST_CASE(env->get(varName("sub")).isClosure());
    }

    {
        auto env = Environment::make();

        const std::string asName = "testmod";
        auto varAsName = [&asName](const char *name) { return asName + '.' + name; };

        auto import = CodeNode::make<ImportModule>(moduleName, asName);
        TEST_CASE(import->eval(env) == Value::True);

        TEST_CASE_MSG(env->size() == 3, "actual=" << env->size());
        TEST_CASE(env->exists(varAsName("PI")));
        TEST_CASE(env->exists(varAsName("add")));
        TEST_CASE(env->exists(varAsName("sub")));

        TEST_CASE(env->get(varAsName("PI")).isReal());
        TEST_CASE(env->get(varAsName("add")).isClosure());
        TEST_CASE(env->get(varAsName("sub")).isClosure());
    }
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeFromModuleImport() {
    const std::string moduleName = "fromtest";
    auto tempFile(createTempModuleFile(moduleName));

    {
        auto env = Environment::make();

        auto fromImport = CodeNode::make<FromModuleImport>(moduleName, "add");
        TEST_CASE(fromImport->eval(env) == Value::True);

        TEST_CASE_MSG(env->size() == 1, "actual=" << env->size());
        TEST_CASE(env->exists("add"));

        fromImport = CodeNode::make<FromModuleImport>(moduleName, "add", "plus");
        TEST_CASE(fromImport->eval(env) == Value::True);

        TEST_CASE_MSG(env->size() == 2, "actual=" << env->size());
        TEST_CASE(env->exists("add"));
        TEST_CASE(env->exists("plus"));

        TEST_CASE(env->get("add").isClosure());
        TEST_CASE(env->get("plus").isClosure());
        TEST_CASE(env->get("add") == env->get("plus"));
    }

    {
        auto env = Environment::make();

        auto fromImport = CodeNode::make<FromModuleImport>(
            moduleName,
            Module::AliasList({ { "add", Module::OptionalName() }, { "sub", "minus" } }));

        TEST_CASE(fromImport->eval(env) == Value::True);

        TEST_CASE_MSG(env->size() == 2, "actual=" << env->size());
        TEST_CASE(env->exists("add"));
        TEST_CASE(env->exists("minus"));

        TEST_CASE(env->get("add").isClosure());
        TEST_CASE(env->get("minus").isClosure());
    }
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeRandom() {
    auto env = Environment::make();

    auto lit = [](const Value &val) { return CodeNode::make<Literal>(val); };

    auto test = [&env, lit](Value::Long mod, std::unordered_set<Value::Long> expectedSet) {
                    auto randCode = CodeNode::make<Random>(lit(Value(mod)));
                    for (int i = 0; i < 1000; ++i) {
                        if (expectedSet.find(randCode->eval(env).integer()) == expectedSet.end()) {
                            return false;
                        }
                    }
                    return true;
                };

    TEST_CASE_MSG(test(0, { 0 }), "rand(0)");
    TEST_CASE_MSG(test(1, { 0, 1 }), "rand(1)");
    TEST_CASE_MSG(test(2, { 0, 1, 2 }), "rand(2)");
    TEST_CASE_MSG(test(10, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 }), "rand(10)");

    try {
        CodeNode::make<Random>(lit(Value(-1ll)))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidExpression &ex) {
        TEST_CASE(std::string("Invalid expression - max negative") == ex.what());
    }
    catch (...) {
        TEST_CASE(false);
    }

    try {
        CodeNode::make<Random>(lit(Value('a')))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &ex) {
        TEST_CASE(std::string("Invalid operand type, expected=Integer actual=char") == ex.what());
    }
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeHash() {
    auto env = Environment::make();

    auto hash = [&env](const Value &value) {
                    auto lit = CodeNode::make<Literal>(value);
                    return CodeNode::make<Hash>(lit)->eval(env);
                };

    auto test = [hash](const Value &value) {
                    auto hashValue = hash(value);
                    return hashValue.isInt() && hashValue.integer() >= 0;
                };

    TEST_CASE(hash(Value::Null) == Value::Zero);

    TEST_CASE_MSG(test(Value(5ll)), "hash(5)");
    TEST_CASE_MSG(test(Value(2.5)), "hash(2.5)");
    TEST_CASE_MSG(test(Value::True), "hash(true)");
    TEST_CASE_MSG(test(Value::False), "hash(false");
    TEST_CASE_MSG(test(Value('a')), "hash('a')");
    TEST_CASE_MSG(test(Value("str")), "hash(\"str\")");
    TEST_CASE_MSG(test(Value(Value::Pair(Value(1ll), Value(2ll)))), "hash(pair)");
    TEST_CASE_MSG(test(Value(Sequence())), "hash(array");
    TEST_CASE_MSG(test(Value(Hashtable())), "hash(hashmap)");
    TEST_CASE_MSG(test(Value(IntegerRange())), "hash(range)");
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeMakeHashMap() {
    auto env = Environment::make();

    auto ilit = [](Value::Long value) { return CodeNode::make<Literal>(Value(value)); };
    auto slit = [](const Value::Text &value) { return CodeNode::make<Literal>(Value(value)); };

    Value value = CodeNode::make<MakeHashMap>()->eval(env);
    TEST_CASE_MSG(value.isHashMap(), "actual=" << value.typeToString());
    TEST_CASE_MSG(value.hashMap().size() == 0lu, "actual=" << value.hashMap().size());

    value =
        CodeNode::make<MakeHashMap>(
            CodeNode::SharedPtrList(
                { CodeNode::make<MakeArray>(CodeNode::SharedPtrList({slit("one"), ilit(1)})),
                  CodeNode::make<MakeArray>(CodeNode::SharedPtrList({slit("two"), ilit(2)})),
                  CodeNode::make<MakeArray>(CodeNode::SharedPtrList({slit("three"), ilit(3)}))
                }))
        ->eval(env);
    TEST_CASE_MSG(value.isHashMap(), "actual=" << value.typeToString());
    TEST_CASE_MSG(value.hashMap().size() == 3lu, "actual=" << value.hashMap().size());
    TEST_CASE_MSG(value.hashMap().get(Value("one")) == Value(1ll), "actual=" << value.hashMap().get(Value("one")));
    TEST_CASE_MSG(value.hashMap().get(Value("two")) == Value(2ll), "actual=" << value.hashMap().get(Value("two")));
    TEST_CASE_MSG(value.hashMap().get(Value("three")) == Value(3ll), "actual=" << value.hashMap().get(Value("three")));

    value =
        CodeNode::make<MakeHashMap>(
            CodeNode::SharedPtrList(
                { CodeNode::make<MakePair>(slit("a"), ilit(10)),
                  CodeNode::make<MakePair>(slit("b"), ilit(20)),
                  CodeNode::make<MakePair>(slit("c"), ilit(30))
                }))
        ->eval(env);
    TEST_CASE_MSG(value.isHashMap(), "actual=" << value.typeToString());
    TEST_CASE_MSG(value.hashMap().size() == 3lu, "actual=" << value.hashMap().size());
    TEST_CASE_MSG(value.hashMap().get(Value("a")) == Value(10ll), "actual=" << value.hashMap().get(Value("a")));
    TEST_CASE_MSG(value.hashMap().get(Value("b")) == Value(20ll), "actual=" << value.hashMap().get(Value("b")));
    TEST_CASE_MSG(value.hashMap().get(Value("c")) == Value(30ll), "actual=" << value.hashMap().get(Value("c")));

    try {
        CodeNode::make<MakeHashMap>(CodeNode::SharedPtrList({ilit(5)}))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &ex) {
        TEST_CASE_MSG(std::string("Invalid operand type, expected=Pair or Array actual=int") == ex.what(), ex.what());
    }
    catch (...) {
        TEST_CASE(false);
    }

    try {
        CodeNode::make<MakeHashMap>(
            CodeNode::SharedPtrList(
                { CodeNode::make<MakeArray>(CodeNode::SharedPtrList({slit("one")})) }))
            ->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidExpression &ex) {
        TEST_CASE_MSG(std::string("Invalid expression - Wrong array size, expecting 2") == ex.what(), ex.what());
    }
    catch (...) {
        TEST_CASE(false);
    }

    try {
        CodeNode::make<MakeHashMap>(
            CodeNode::SharedPtrList(
                { CodeNode::make<MakeArray>(CodeNode::SharedPtrList({slit("one"), ilit(1), ilit(2)})) }))
            ->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidExpression &ex) {
        TEST_CASE_MSG(std::string("Invalid expression - Wrong array size, expecting 2") == ex.what(), ex.what());
    }
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeHashMapLen() {
    auto env = Environment::make();

    auto ht = [](int count) {
                  Hashtable table;
                  if (count >= 1) { table.set(Value(1ll), Value(100ll)); }
                  if (count >= 2) { table.set(Value(2ll), Value(200ll)); }
                  if (count >= 3) { table.set(Value(3ll), Value(300ll)); }
                  return table;
              };

    Value value =
        CodeNode::make<HashMapLen>(
            CodeNode::make<Literal>(Value(ht(0))))
        ->eval(env);
    TEST_CASE_MSG(value == Value::Zero, "actual=" << value);

    value =
        CodeNode::make<HashMapLen>(
            CodeNode::make<Literal>(Value(ht(1))))
        ->eval(env);
    TEST_CASE_MSG(value == Value(1ll), "actual=" << value);

    value =
        CodeNode::make<HashMapLen>(
            CodeNode::make<Literal>(Value(ht(3))))
        ->eval(env);
    TEST_CASE_MSG(value == Value(3ll), "actual=" << value);

    try {
        value =
            CodeNode::make<HashMapLen>(
                CodeNode::make<Literal>(Value::Zero))
            ->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) { TEST_CASE(false); }
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeHashMapContains() {
    auto env = Environment::make();

    const Value one("one");
    const Value two("two");
    const Value three("three");
    const Value four("four");

    auto htlit = [one, two, three](int count) {
                     Hashtable table;
                     if (count >= 1) { table.set(one, Value(1ll)); }
                     if (count >= 2) { table.set(two, Value(2ll)); }
                     if (count >= 3) { table.set(three, Value(3ll)); }
                     return CodeNode::make<Literal>(Value(table));
                 };

    auto exists = [htlit, &env](int count, const Value &key) {
                      return CodeNode::make<HashMapContains>(htlit(count), CodeNode::make<Literal>(key))->eval(env);
                  };

    Value result;

    result = exists(0, one);   TEST_CASE_MSG(result == Value::False, "actual=" << result);
    result = exists(1, one);   TEST_CASE_MSG(result == Value::True,  "actual=" << result);
    result = exists(1, two);   TEST_CASE_MSG(result == Value::False, "actual=" << result);
    result = exists(2, one);   TEST_CASE_MSG(result == Value::True,  "actual=" << result);
    result = exists(2, two);   TEST_CASE_MSG(result == Value::True,  "actual=" << result);
    result = exists(2, three); TEST_CASE_MSG(result == Value::False, "actual=" << result);
    result = exists(3, one);   TEST_CASE_MSG(result == Value::True,  "actual=" << result);
    result = exists(3, two);   TEST_CASE_MSG(result == Value::True,  "actual=" << result);
    result = exists(3, three); TEST_CASE_MSG(result == Value::True,  "actual=" << result);
    result = exists(3, four);  TEST_CASE_MSG(result == Value::False, "actual=" << result);

    try {
        CodeNode::make<HashMapContains>(
            CodeNode::make<Literal>(one),
            CodeNode::make<Literal>(two))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeHashMapGet() {
    auto env = Environment::make();

    const Value kOne("one");
    const Value kTwo("two");
    const Value kThree("three");
    const Value kFour("four");

    const Value vOne(1ll);
    const Value vTwo(2ll);
    const Value vThree(3ll);

    Hashtable ht;
    ht.set(kOne, vOne);
    ht.set(kTwo, vTwo);
    ht.set(kThree, vThree);
    env->def("ht", Value(ht));

    auto get = [&env](const Value &key, const std::optional<Value> &defaultValue = std::nullopt) {
        return CodeNode::make<HashMapGet>(CodeNode::make<Variable>("ht"),
                                                     CodeNode::make<Literal>(key),
                                                     (defaultValue
                                                      ? CodeNode::make<Literal>(*defaultValue)
                                                      : CodeNode::SharedPtr()))->eval(env);
    };

    Value result;

    result = get(kOne);   TEST_CASE_MSG(result == vOne,        "actual=" << result);
    result = get(kTwo);   TEST_CASE_MSG(result == vTwo,        "actual=" << result);
    result = get(kThree); TEST_CASE_MSG(result == vThree,      "actual=" << result);
    result = get(kFour);  TEST_CASE_MSG(result == Value::Null, "actual=" << result);

    result = get(kOne,   Value::Zero); TEST_CASE_MSG(result == vOne,        "actual=" << result);
    result = get(kTwo,   Value::Zero); TEST_CASE_MSG(result == vTwo,        "actual=" << result);
    result = get(kThree, Value::Zero); TEST_CASE_MSG(result == vThree,      "actual=" << result);
    result = get(kFour,  Value::Zero); TEST_CASE_MSG(result == Value::Zero, "actual=" << result);

    try {
        CodeNode::make<HashMapGet>(CodeNode::make<Literal>(Value::Zero),
                                   CodeNode::make<Literal>(kOne))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeHashMapSet() {
    auto env = Environment::make();
    env->def("ht", Value(Hashtable()));

    CodeNode::SharedPtr ht = CodeNode::make<Variable>("ht");

    auto clit = [](Value::Char c) { return CodeNode::make<Literal>(Value(c)); };
    auto ilit = [](Value::Long i) { return CodeNode::make<Literal>(Value(i)); };

    auto set = [&env, &ht, clit, ilit](Value::Char key, Value::Long val) {
                   return CodeNode::make<HashMapSet>(ht, clit(key), ilit(val))->eval(env);
               };

    auto htget = [&env](Value::Char key) { return env->get("ht").hashMap().get(Value(key)); };
    auto htlen = [&env]() { return env->get("ht").hashMap().size(); };

    Value result = set('a', 1);
    TEST_CASE_MSG(result == Value(1ll), "actual=" << result);
    TEST_CASE_MSG(htget('a') == result, "actual=" << htget('a'));
    TEST_CASE_MSG(htlen() == 1, "actual=" << htlen());

    result = set('b', 2);
    TEST_CASE_MSG(result == Value(2ll), "actual=" << result);
    TEST_CASE_MSG(htget('b') == result, "actual=" << htget('b'));
    TEST_CASE_MSG(htlen() == 2, "actual=" << htlen());

    result = set('c', 3);
    TEST_CASE_MSG(result == Value(3ll), "actual=" << result);
    TEST_CASE_MSG(htget('c') == result, "actual=" << htget('c'));
    TEST_CASE_MSG(htlen() == 3, "actual=" << htlen());

    result = set('a', 100);
    TEST_CASE_MSG(result == Value(100ll), "actual=" << result);
    TEST_CASE_MSG(htget('a') == result, "actual=" << htget('a'));
    TEST_CASE_MSG(htlen() == 3, "actual=" << htlen());

    try {
        CodeNode::make<HashMapSet>(ilit(0), ilit(1), ilit(2))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeHashMapRemove() {
    auto env = Environment::make();

    {
        Hashtable ht;
        ht.set(Value('a'), Value(1ll));
        ht.set(Value('b'), Value(2ll));
        ht.set(Value('c'), Value(3ll));
        env->def("ht", Value(ht));
    }

    CodeNode::SharedPtr ht = CodeNode::make<Variable>("ht");

    auto clit = [](Value::Char c) { return CodeNode::make<Literal>(Value(c)); };

    auto remove = [&env, &ht, clit](Value::Char key) {
                      return CodeNode::make<HashMapRemove>(ht, clit(key))->eval(env);
                  };

    auto htlen = [&env]() { return env->get("ht").hashMap().size(); };
    auto hthas = [&env](Value::Char key) { return env->get("ht").hashMap().exists(Value(key)); };

    TEST_CASE_MSG(htlen() == 3, "actual=" << htlen());
    TEST_CASE(hthas('a'));
    remove('a');
    TEST_CASE(!hthas('a'));

    TEST_CASE_MSG(htlen() == 2, "actual=" << htlen());
    TEST_CASE(hthas('b'));
    remove('b');
    TEST_CASE(!hthas('b'));

    TEST_CASE_MSG(htlen() == 1, "actual=" << htlen());
    TEST_CASE(hthas('c'));
    remove('c');
    TEST_CASE(!hthas('c'));

    TEST_CASE_MSG(htlen() == 0, "actual=" << htlen());
    TEST_CASE(!hthas('d'));
    remove('d');
    TEST_CASE(!hthas('d'));

    TEST_CASE_MSG(htlen() == 0, "actual=" << htlen());

    try {
        CodeNode::make<HashMapRemove>(clit('a'), clit('b'))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeHashMapClear() {
    auto env = Environment::make();

    {
        Hashtable ht;
        ht.set(Value('a'), Value(1ll));
        ht.set(Value('b'), Value(2ll));
        ht.set(Value('c'), Value(3ll));
        env->def("ht", Value(ht));
    }

    CodeNode::SharedPtr ht = CodeNode::make<Variable>("ht");

    auto clear = [&env, &ht]() {
                     return CodeNode::make<HashMapClear>(ht)->eval(env);
                 };

    auto htlen = [&env]() { return env->get("ht").hashMap().size(); };
    auto hthas = [&env](Value::Char key) { return env->get("ht").hashMap().exists(Value(key)); };

    TEST_CASE(htlen() == 3);
    TEST_CASE(hthas('a'));
    TEST_CASE(hthas('b'));
    TEST_CASE(hthas('c'));

    clear();

    TEST_CASE(htlen() == 0);
    TEST_CASE(!hthas('a'));
    TEST_CASE(!hthas('b'));
    TEST_CASE(!hthas('c'));

    try {
        CodeNode::make<HashMapClear>(CodeNode::make<Literal>(Value('a')))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeHashMapFind() {
    auto env = Environment::make();

    {
        Hashtable ht;
        ht.set(Value('a'), Value(1ll));
        ht.set(Value('b'), Value(2ll));
        ht.set(Value('c'), Value(3ll));
        env->def("ht", Value(ht));
    }

    CodeNode::SharedPtr ht = CodeNode::make<Variable>("ht");

    auto ilit = [](Value::Long i) { return CodeNode::make<Literal>(i); };

    auto find = [&env, &ht, ilit](Value::Long value) {
                    return CodeNode::make<HashMapFind>(ht, ilit(value))->eval(env);
                };

    Value result;
    result = find(1); TEST_CASE_MSG(result == Value('a'),  "actual=" << result);
    result = find(2); TEST_CASE_MSG(result == Value('b'),  "actual=" << result);
    result = find(3); TEST_CASE_MSG(result == Value('c'),  "actual=" << result);
    result = find(4); TEST_CASE_MSG(result == Value::Null, "actual=" << result);

    try {
        CodeNode::make<HashMapFind>(ilit(1), ilit(2))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeHashMapCount() {
    auto env = Environment::make();

    {
        Hashtable ht;
        ht.set(Value('a'), Value(1ll));
        ht.set(Value('b'), Value(2ll));
        ht.set(Value('c'), Value(3ll));
        ht.set(Value('2'), Value(2ll));
        env->def("ht", Value(ht));
    }

    CodeNode::SharedPtr ht = CodeNode::make<Variable>("ht");

    auto ilit = [](Value::Long i) { return CodeNode::make<Literal>(i); };

    auto count = [&env, &ht, ilit](Value::Long value) {
                     return CodeNode::make<HashMapCount>(ht, ilit(value))->eval(env);
                 };

    Value result;
    result = count(1); TEST_CASE_MSG(result == Value(1ll),  "actual=" << result);
    result = count(2); TEST_CASE_MSG(result == Value(2ll),  "actual=" << result);
    result = count(3); TEST_CASE_MSG(result == Value(1ll),  "actual=" << result);
    result = count(4); TEST_CASE_MSG(result == Value::Zero, "actual=" << result);

    try {
        CodeNode::make<HashMapCount>(ilit(1), ilit(2))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeHashMapKeys() {
    auto env = Environment::make();

    {
        Hashtable ht;
        ht.set(Value('a'), Value(1ll));
        ht.set(Value('b'), Value(2ll));
        ht.set(Value('c'), Value(3ll));
        env->def("ht", Value(ht));
    }

    CodeNode::SharedPtr ht = CodeNode::make<Variable>("ht");

    auto keys = [&env, &ht]() {
                    return CodeNode::make<HashMapKeys>(ht)->eval(env);
                };

    const Value ks = keys();
    TEST_CASE(ks.isArray());

    const Sequence &seq = ks.array();
    TEST_CASE_MSG(seq.size() == 3, "actual=" << seq.size());

    std::size_t cnt = 0;
    cnt = seq.count(Value('a')); TEST_CASE_MSG(cnt == 1, "actual=" << cnt);
    cnt = seq.count(Value('b')); TEST_CASE_MSG(cnt == 1, "actual=" << cnt);
    cnt = seq.count(Value('c')); TEST_CASE_MSG(cnt == 1, "actual=" << cnt);

    try {
        CodeNode::make<HashMapKeys>(CodeNode::make<Literal>(Value('a')))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeHashMapValues() {
    auto env = Environment::make();

    {
        Hashtable ht;
        ht.set(Value('a'), Value(1ll));
        ht.set(Value('b'), Value(2ll));
        ht.set(Value('c'), Value(3ll));
        ht.set(Value('2'), Value(2ll));
        env->def("ht", Value(ht));
    }

    CodeNode::SharedPtr ht = CodeNode::make<Variable>("ht");

    auto values = [&env, &ht]() {
                      return CodeNode::make<HashMapValues>(ht)->eval(env);
                  };

    const Value vs = values();
    TEST_CASE(vs.isArray());

    const Sequence &vals = vs.array();
    TEST_CASE_MSG(vals.size() == 4, "actual=" << vals.size());

    std::size_t cnt = 0;
    cnt = vals.count(Value(1ll)); TEST_CASE_MSG(cnt == 1, "actual=" << cnt);
    cnt = vals.count(Value(2ll)); TEST_CASE_MSG(cnt == 2, "actual=" << cnt);
    cnt = vals.count(Value(3ll)); TEST_CASE_MSG(cnt == 1, "actual=" << cnt);

    try {
        CodeNode::make<HashMapValues>(CodeNode::make<Literal>(Value(1ll)))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeHashMapItems() {
    auto env = Environment::make();

    {
        Hashtable ht;
        ht.set(Value('a'), Value(1ll));
        ht.set(Value('b'), Value(2ll));
        ht.set(Value('c'), Value(3ll));
        ht.set(Value('2'), Value(2ll));
        env->def("ht", Value(ht));
    }

    CodeNode::SharedPtr ht = CodeNode::make<Variable>("ht");

    auto items = [&env, &ht]() {
                     return CodeNode::make<HashMapItems>(ht)->eval(env);
                 };

    const Value itms = items();
    TEST_CASE(itms.isArray());

    const Sequence &pairs = itms.array();
    TEST_CASE_MSG(pairs.size() == 4, "actual=" << pairs.size());

    std::size_t cnt = 0;
    cnt = pairs.count(Value(Value::Pair(Value('a'), Value(1ll)))); TEST_CASE_MSG(cnt == 1, "actual=" << cnt);
    cnt = pairs.count(Value(Value::Pair(Value('b'), Value(2ll)))); TEST_CASE_MSG(cnt == 1, "actual=" << cnt);
    cnt = pairs.count(Value(Value::Pair(Value('c'), Value(3ll)))); TEST_CASE_MSG(cnt == 1, "actual=" << cnt);
    cnt = pairs.count(Value(Value::Pair(Value('2'), Value(2ll)))); TEST_CASE_MSG(cnt == 1, "actual=" << cnt);

    try {
        CodeNode::make<HashMapItems>(CodeNode::make<Literal>(Value(1ll)))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
void UnitTest::testCodeNodePairOperations() {
    auto env = Environment::make();

    auto ilit = [](Value::Long value) { return CodeNode::make<Literal>(Value(value)); };

    Value value = CodeNode::make<MakePair>(ilit(1), ilit(2))->eval(env);
    TEST_CASE_MSG(value.isPair(), "actual=" << value.isPair());
    TEST_CASE_MSG(value.pair().first() == Value(1ll), "actual=" << value.pair().first());
    TEST_CASE_MSG(value.pair().second() == Value(2ll), "actual=" << value.pair().second());

    env->def("p", value);

    Value fValue = CodeNode::make<PairFirst>(CodeNode::make<Variable>("p"))->eval(env);
    TEST_CASE_MSG(fValue == Value(1ll), "actual=" << fValue);

    Value sValue = CodeNode::make<PairSecond>(CodeNode::make<Variable>("p"))->eval(env);
    TEST_CASE_MSG(sValue == Value(2ll), "actual=" << sValue);

    try {
        CodeNode::make<PairFirst>(CodeNode::make<Literal>(Value(1ll)))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }

    try {
        CodeNode::make<PairSecond>(CodeNode::make<Literal>(Value(1ll)))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeMakeRange() {
    auto env = Environment::make();

    auto ilit = [](Value::Long value) { return CodeNode::make<Literal>(Value(value)); };
    auto clit = [](Value::Char value) { return CodeNode::make<Literal>(Value(value)); };

    Value value = CodeNode::make<MakeRange>(ilit(10))->eval(env);
    TEST_CASE_MSG(value.isRange(), "actual=" << value.isRange());
    TEST_CASE_MSG(value.range().begin() == 0ll, "actual=" << value.range().begin());
    TEST_CASE_MSG(value.range().end() == 10ll, "actual=" << value.range().end());
    TEST_CASE_MSG(value.range().step() == 1ll, "actual=" << value.range().step());

    value = CodeNode::make<MakeRange>(ilit(5), ilit(10), CodeNode::SharedPtr())->eval(env);
    TEST_CASE_MSG(value.isRange(), "actual=" << value.isRange());
    TEST_CASE_MSG(value.range().begin() == 5ll, "actual=" << value.range().begin());
    TEST_CASE_MSG(value.range().end() == 10ll, "actual=" << value.range().end());
    TEST_CASE_MSG(value.range().step() == 1ll, "actual=" << value.range().step());

    value = CodeNode::make<MakeRange>(ilit(0), ilit(10), ilit(2))->eval(env);
    TEST_CASE_MSG(value.isRange(), "actual=" << value.isRange());
    TEST_CASE_MSG(value.range().begin() == 0ll, "actual=" << value.range().begin());
    TEST_CASE_MSG(value.range().end() == 10ll, "actual=" << value.range().end());
    TEST_CASE_MSG(value.range().step() == 2ll, "actual=" << value.range().step());

    try {
        CodeNode::make<MakeRange>(CodeNode::SharedPtr())->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidExpression &) {}
    catch (...) {
        TEST_CASE(false);
    }

    try {
        CodeNode::make<MakeRange>(clit('a'))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }

    try {
        CodeNode::make<MakeRange>(ilit(0), clit('a'), CodeNode::SharedPtr())->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }

    try {
        CodeNode::make<MakeRange>(ilit(0), ilit(10), clit('a'))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeRangeGetters() {
    auto env = Environment::make();
    env->def("rng", Value(IntegerRange(10)));

    auto ilit = [](Value::Long value) { return CodeNode::make<Literal>(Value(value)); };
    auto rVar = []() { return CodeNode::make<Variable>("rng"); };

    auto val = CodeNode::make<RangeBegin>(rVar())->eval(env);
    TEST_CASE_MSG(val.isInt(), "actual=" << Value::typeToString(val.type()));
    TEST_CASE_MSG(val == Value(0ll), "actual=" << val);

    val = CodeNode::make<RangeEnd>(rVar())->eval(env);
    TEST_CASE_MSG(val.isInt(), "actual=" << Value::typeToString(val.type()));
    TEST_CASE_MSG(val == Value(10ll), "actual=" << val);

    val = CodeNode::make<RangeStep>(rVar())->eval(env);
    TEST_CASE_MSG(val.isInt(), "actual=" << Value::typeToString(val.type()));
    TEST_CASE_MSG(val == Value(1ll), "actual=" << val);

    val = CodeNode::make<RangeLen>(rVar())->eval(env);
    TEST_CASE_MSG(val.isInt(), "actual=" << Value::typeToString(val.type()));
    TEST_CASE_MSG(val == Value(10ll), "actual=" << val);

    try {
        CodeNode::make<RangeBegin>(ilit(5))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }

    try {
        CodeNode::make<RangeEnd>(ilit(5))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }

    try {
        CodeNode::make<RangeStep>(ilit(5))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }

    try {
        CodeNode::make<RangeLen>(ilit(5))->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeExpand() {
    auto env = Environment::make();
    env->def("rng1", Value(IntegerRange(4)));
    env->def("rng2", Value(IntegerRange(4, 8, 2)));

    auto ilit = [](Value::Long value) { return CodeNode::make<Literal>(Value(value)); };
    auto rVar = [](const char * name) { return CodeNode::make<Variable>(name); };

    auto val = CodeNode::make<Expand>(CodeNode::SharedPtrList{ rVar("rng1") })->eval(env);
    TEST_CASE_MSG(val.isArray(), "actual=" << Value::typeToString(val.type()));
    TEST_CASE_MSG(val.array().size() == 4lu, "actual=" << val.array().size());
    TEST_CASE_MSG(val.array().get(0lu) == Value(0ll), "actual=" << val.array().get(0lu));
    TEST_CASE_MSG(val.array().get(1lu) == Value(1ll), "actual=" << val.array().get(1lu));
    TEST_CASE_MSG(val.array().get(2lu) == Value(2ll), "actual=" << val.array().get(2lu));
    TEST_CASE_MSG(val.array().get(3lu) == Value(3ll), "actual=" << val.array().get(3lu));

    val = CodeNode::make<Expand>(CodeNode::SharedPtrList{ rVar("rng1"), rVar("rng2") })->eval(env);
    TEST_CASE_MSG(val.isArray(), "actual=" << Value::typeToString(val.type()));
    TEST_CASE_MSG(val.array().size() == 6lu, "actual=" << val.array().size());
    TEST_CASE_MSG(val.array().get(0lu) == Value(0ll), "actual=" << val.array().get(0lu));
    TEST_CASE_MSG(val.array().get(1lu) == Value(1ll), "actual=" << val.array().get(1lu));
    TEST_CASE_MSG(val.array().get(2lu) == Value(2ll), "actual=" << val.array().get(2lu));
    TEST_CASE_MSG(val.array().get(3lu) == Value(3ll), "actual=" << val.array().get(3lu));
    TEST_CASE_MSG(val.array().get(4lu) == Value(4ll), "actual=" << val.array().get(4lu));
    TEST_CASE_MSG(val.array().get(5lu) == Value(6ll), "actual=" << val.array().get(5lu));

    try {
        CodeNode::make<Expand>(CodeNode::SharedPtrList{ ilit(5) })->eval(env);
        TEST_CASE(false);
    }
    catch (const InvalidOperandType &) {}
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
void UnitTest::testCodeNodeGenericLen() {
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
void UnitTest::testCodeNodeGenericGet() {
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
            TEST_CASE_MSG(val.integer() == rawSeq.get(i), "actual=" << val << " index=" << i);
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
            TEST_CASE_MSG(val.integer() == rawTab.get(Value(i)), "actual=" << val << " key=" << i);
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
        CodeNode::SharedPtr code(parser.read(expr));
        if (code) {
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
        if (parser.hasIncompleteExpr()) {
            parser.clearIncompleteExpr();
        }

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
    auto env = Environment::make();
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
void UnitTest::testParserClone() {
    auto env = Environment::make();
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(var str1 \"hello\")",    Value("hello"), true));
    TEST_CASE(parserTest(parser, env, "(var str2 (clone str1))", Value("hello"), true));
    TEST_CASE(parserTest(parser, env, "(== str1 str2)",          Value::True,    true));
    TEST_CASE(parserTest(parser, env, "(== str1 \"hello\")",     Value::True,    true));
    TEST_CASE(parserTest(parser, env, "(== str2 \"hello\")",     Value::True,    true));

    TEST_CASE(parserTest(parser, env, "(= str1 \"world\")",  Value("world"), true));
    TEST_CASE(parserTest(parser, env, "(!= str1 str2)",      Value::True,    true));
    TEST_CASE(parserTest(parser, env, "(== str1 \"world\")", Value::True,    true));
    TEST_CASE(parserTest(parser, env, "(== str2 \"hello\")", Value::True,    true));

    TEST_CASE(parserTest(parser, env, "(clone)",           Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(clone str1 str2)", Value::Null, false));
}

// -------------------------------------------------------------
void UnitTest::testParserIsType() {
    auto env = Environment::make();
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(istypeof null none)",                     Value::True, true));
    TEST_CASE(parserTest(parser, env, "(istypeof 3 int)",                         Value::True, true));
    TEST_CASE(parserTest(parser, env, "(istypeof 1.0 real)",                      Value::True, true));
    TEST_CASE(parserTest(parser, env, "(istypeof 'c' char)",                      Value::True, true));
    TEST_CASE(parserTest(parser, env, "(istypeof true bool)",                     Value::True, true));
    TEST_CASE(parserTest(parser, env, "(istypeof \"txt\" string)",                Value::True, true));
    TEST_CASE(parserTest(parser, env, "(istypeof (lambda () 1) closure)",         Value::True, true));
    TEST_CASE(parserTest(parser, env, "(istypeof (struct Foo (x y)) usertype)",   Value::True, true));
    TEST_CASE(parserTest(parser, env, "(istypeof (makeinstance Foo) userobject)", Value::True, true));
    TEST_CASE(parserTest(parser, env, "(istypeof (array 1 1) array)",             Value::True, true));

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
void UnitTest::testParserTypeName() {
    auto env = Environment::make();
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(typename null)",                   Value("none"),       true));
    TEST_CASE(parserTest(parser, env, "(typename 3)",                      Value("int"),        true));
    TEST_CASE(parserTest(parser, env, "(typename 1.0)",                    Value("real"),       true));
    TEST_CASE(parserTest(parser, env, "(typename 'c')",                    Value("char"),       true));
    TEST_CASE(parserTest(parser, env, "(typename true)",                   Value("bool"),       true));
    TEST_CASE(parserTest(parser, env, "(typename \"txt\")",                Value("string"),     true));
    TEST_CASE(parserTest(parser, env, "(typename (lambda () 1))",          Value("closure"),    true));
    TEST_CASE(parserTest(parser, env, "(typename (struct Foo (x y)))",     Value("usertype"),   true));
    TEST_CASE(parserTest(parser, env, "(typename (makeinstance Foo))",     Value("userobject"), true));
    TEST_CASE(parserTest(parser, env, "(typename (array 1 1))",            Value("array"),      true));

    TEST_CASE(parserTest(parser, env, "(typename)",       Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(typename 1 int)", Value::Null, false));
}

// -------------------------------------------------------------
void UnitTest::testParserAsType() {
    auto env = Environment::make();
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(astype 1 real)",        Value(1.0),        true));
    TEST_CASE(parserTest(parser, env, "(astype 98 char)",       Value('b'),        true));
    TEST_CASE(parserTest(parser, env, "(astype 1 bool)",        Value::True,       true));
    TEST_CASE(parserTest(parser, env, "(astype 98 string)",     Value("98"),       true));
    TEST_CASE(parserTest(parser, env, "(astype 1.5 int)",       Value(1ll),        true));
    TEST_CASE(parserTest(parser, env, "(astype 98.0 char)",     Value('b'),        true));
    TEST_CASE(parserTest(parser, env, "(astype 1.0 bool)",      Value::True,       true));
    TEST_CASE(parserTest(parser, env, "(astype 1.5 string)",    Value("1.500000"), true));
    TEST_CASE(parserTest(parser, env, "(astype 'b' int)",       Value(98ll),       true));
    TEST_CASE(parserTest(parser, env, "(astype 'b' real)",      Value(98.0),       true));
    TEST_CASE(parserTest(parser, env, "(astype 't' bool)",      Value::True,       true));
    TEST_CASE(parserTest(parser, env, "(astype 'b' string)",    Value("b"),        true));
    TEST_CASE(parserTest(parser, env, "(astype true int)",      Value(1ll),        true));
    TEST_CASE(parserTest(parser, env, "(astype true real)",     Value(1.0),        true));
    TEST_CASE(parserTest(parser, env, "(astype true char)",     Value('t'),        true));
    TEST_CASE(parserTest(parser, env, "(astype true string)",   Value("true"),     true));
    TEST_CASE(parserTest(parser, env, "(astype \"98\" int)",    Value(98ll),       true));
    TEST_CASE(parserTest(parser, env, "(astype \"98\" real)",   Value(98.0),       true));
    TEST_CASE(parserTest(parser, env, "(astype \"b\" char)",    Value('b'),        true));
    TEST_CASE(parserTest(parser, env, "(astype \"true\" bool)", Value::True,       true));

    TEST_CASE(parserTest(parser, env, "(astype null int)", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(astype 'b' bool)", Value::Null, false));
}

// -------------------------------------------------------------
void UnitTest::testParserVar() {
    auto env = Environment::make();
    Parser parser;

    TEST_CASE(parserTest(parser, env, "foo",         Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(var foo 5)", Value(5ll),  true));
    TEST_CASE(parserTest(parser, env, "foo",         Value(5ll),  true));
    TEST_CASE(parserTest(parser, env, "(= foo 12)",  Value(12ll), true));
    TEST_CASE(parserTest(parser, env, "foo",         Value(12ll), true));

    TEST_CASE(parserTest(parser, env, "(var x)",     Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(var x 1 2)", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(= x)",       Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(= x 1 2)",   Value::Null, false));
}

// -------------------------------------------------------------
void UnitTest::testParserArith() {
    auto env = Environment::make();
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
    TEST_CASE(parserTest(parser, env, "(^ 3.0 2)",   Value(9.0),  true));
    TEST_CASE(parserTest(parser, env, "(^ 9.0 0.5)", Value(3.0),  true));
}

// -------------------------------------------------------------
void UnitTest::testParserComp() {
    auto env = Environment::make();
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
void UnitTest::testParserLogicOp() {
    auto env = Environment::make();
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(and true true)",    Value::True,  true));
    TEST_CASE(parserTest(parser, env, "(and true false)",   Value::False, true));
    TEST_CASE(parserTest(parser, env, "(and false true)",   Value::False, true));
    TEST_CASE(parserTest(parser, env, "(and false false)",  Value::False, true));
    TEST_CASE(parserTest(parser, env, "(or true true)",     Value::True,  true));
    TEST_CASE(parserTest(parser, env, "(or true false)",    Value::True,  true));
    TEST_CASE(parserTest(parser, env, "(or false true)",    Value::True,  true));
    TEST_CASE(parserTest(parser, env, "(or false false)",   Value::False, true));

    TEST_CASE(parserTest(parser, env, "(and true true true)",    Value::True,  true));
    TEST_CASE(parserTest(parser, env, "(and false true true)",   Value::False, true));
    TEST_CASE(parserTest(parser, env, "(and true false true)",   Value::False, true));
    TEST_CASE(parserTest(parser, env, "(and true true false)",   Value::False, true));
    TEST_CASE(parserTest(parser, env, "(and false false false)", Value::False, true));

    TEST_CASE(parserTest(parser, env, "(or true true true)",    Value::True,  true));
    TEST_CASE(parserTest(parser, env, "(or false true true)",   Value::True,  true));
    TEST_CASE(parserTest(parser, env, "(or true false true)",   Value::True,  true));
    TEST_CASE(parserTest(parser, env, "(or true true false)",   Value::True,  true));
    TEST_CASE(parserTest(parser, env, "(or false false false)", Value::False, true));
}

// -------------------------------------------------------------
void UnitTest::testParserNot() {
    auto env = Environment::make();
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(not true)",     Value::False, true));
    TEST_CASE(parserTest(parser, env, "(not false)",    Value::True,  true));
    TEST_CASE(parserTest(parser, env, "(not (== 1 1))", Value::False, true));
    TEST_CASE(parserTest(parser, env, "(not (== 1 2))", Value::True,  true));
    TEST_CASE(parserTest(parser, env, "(not 3)",        Value::False, false));

    TEST_CASE(parserTest(parser, env, "(not)",            Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(not True False)", Value::Null, false));
}

// -------------------------------------------------------------
void UnitTest::testParserNegativeOf() {
    auto env = Environment::make();
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(neg 0)",    Value::Zero, true));
    TEST_CASE(parserTest(parser, env, "(neg 1)",    Value(-1ll), true));
    TEST_CASE(parserTest(parser, env, "(neg -1)",   Value(1ll),  true));
    TEST_CASE(parserTest(parser, env, "(neg 2.5)",  Value(-2.5), true));
    TEST_CASE(parserTest(parser, env, "(neg -2.5)", Value(2.5),  true));

    TEST_CASE(parserTest(parser, env, "(neg)",     Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(neg 'a')", Value::Null, false));
}

// -------------------------------------------------------------
void UnitTest::testParserBlock() {
    auto env = Environment::make();
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(progn (var c 'a') (= c 'b') c)",   Value('b'),  true));
    TEST_CASE(parserTest(parser, env, "(progn (var x 2) (= x (+ x 1)))",   Value(3ll),  true));
    TEST_CASE(parserTest(parser, env, "(block (var f false) (= f true))",  Value(true), true));
    TEST_CASE(parserTest(parser, env, "(block (var y 5) (= y (* 2 y)) y)", Value(10ll), true));
}

// -------------------------------------------------------------
void UnitTest::testParserCond() {
    auto env = Environment::make();
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(if true 1)",                   Value(1ll),  true));
    TEST_CASE(parserTest(parser, env, "(if false 1)",                  Value::Null, true));
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

    TEST_CASE(parserTest(parser, env, "(when true 1)",   Value(1ll),  true));
    TEST_CASE(parserTest(parser, env, "(when false 1)",  Value::Null, true));
    TEST_CASE(parserTest(parser, env, "(when true)",     Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(when true 1 2)", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(when)",          Value::Null, false));

    TEST_CASE(parserTest(parser, env, "(unless true 1)",    Value::Null, true));
    TEST_CASE(parserTest(parser, env, "(unless false 1)",   Value(1ll),  true));
    TEST_CASE(parserTest(parser, env, "(unless false)",     Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(unless false 1 2)", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(unless)",           Value::Null, false));
}

// -------------------------------------------------------------
void UnitTest::testParserLoop() {
    auto env = Environment::make();
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
void UnitTest::testParserForeach() {
    auto env = Environment::make();
    Parser parser;

    auto add_var = [&](const char *name, const char *val) {
        const auto expr = std::string("(var ") + name + " " + val + ")";
        parser.read(expr)->eval(env);
    };
    add_var("str", "\"abcdef\"");
    add_var("arr", "(array 1 2 3 4 5)");
    add_var("ht", "(hashmap (pair 1 10) (pair 2 20))");
    add_var("rng", "(range 10)");
    add_var("strCnt", "0");
    add_var("arrSum", "0");
    add_var("htSum", "0");
    add_var("keySum", "0");
    add_var("valSum", "0");
    add_var("rngSum", "0");
    add_var("dummy", "0");

    TEST_CASE(parserTest(parser, env, "(foreach c str (= strCnt (+ strCnt 1)))",                        Value(6ll),  true));
    TEST_CASE(parserTest(parser, env, "(foreach i arr (= arrSum (+ arrSum i)))",                        Value(15ll), true));
    TEST_CASE(parserTest(parser, env, "(foreach kv ht (= htSum (+ htSum (* (first kv) (second kv)))))", Value(50ll), true));
    TEST_CASE(parserTest(parser, env, "(foreach kv ht (progn (= keySum (+ keySum (first kv))) (= valSum (+ valSum (second kv)))))", Value(30ll), true));
    TEST_CASE(parserTest(parser, env, "(foreach i rng (= rngSum (+ rngSum i)))",                        Value(45ll), true));

    add_var("arrCnt", "0");
    TEST_CASE(parserTest(parser, env, "(foreach i arr (progn (when (> i 3) (break)) (= arrCnt (+ arrCnt 1))))", Value::Null, true));
    TEST_CASE(env->get("arrCnt") == Value(3ll));

    TEST_CASE(parserTest(parser, env, "(foreach)",                 Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(foreach x)",               Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(foreach x str)",           Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(foreach x 5 (+ dummy 1))", Value::Null, false));
}

// -------------------------------------------------------------
void UnitTest::testParserLambda() {
    auto env = Environment::make();
    Parser parser;

    TEST_CASE(parserTest(parser, env, "((lambda (x) (+ x 1)) 3)",           Value(4ll),  true));
    TEST_CASE(parserTest(parser, env, "((lambda (x y) (+ (* x 2) y)) 2 1)", Value(5ll),  true));
    TEST_CASE(parserTest(parser, env, "((var x 1) 3)",                      Value::Null, false));
}

// -------------------------------------------------------------
void UnitTest::testParserFtn() {
    auto env = Environment::make();
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(block (defun add (x y) (+ x y)) (add 1 2))",                                          Value(3ll),  true));
    TEST_CASE(parserTest(parser, env, "(block (var x 3) (var y 2) (defun sum () (+ x y)) (block (var x 4) (var y 5) (sum)))", Value(5ll),  true));
    TEST_CASE(parserTest(parser, env, "(block (defun inc (x) (+ x 1)) (defun add3 (x) (inc (inc (inc x)))) (add3 5))",        Value(8ll),  true));

    TEST_CASE(parserTest(parser, env, "(progn (defun add4 (x) (defun inc2 (y) (+ y 2)) (inc2 (inc2 x))) (add4 4))", Value(8ll),  true));
    TEST_CASE(parserTest(parser, env, "(add4 1)",                                                                   Value(5ll),  true));
    TEST_CASE(parserTest(parser, env, "(inc 2)",                                                                    Value::Null, false));
}

// -------------------------------------------------------------
void UnitTest::testParserStruct() {
    auto env = Environment::make();
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
    auto env = Environment::make();
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
    auto env = Environment::make();
    Parser parser;

    const Struct stest("Person", {"name", "age"});
    const Instance itest(stest);
    TEST_CASE(parserTest(parser, env, "(var s (struct Person (name age)))", Value(stest), true));
    TEST_CASE(parserTest(parser, env, "(var p (makeinstance Person))",      Value(itest), true));
    TEST_CASE(parserTest(parser, env, "(istypeof p userobject)",            Value::True,  true));
}

// -------------------------------------------------------------
void UnitTest::testParserMakeInstanceWithInitArgs() {
    auto env = Environment::make();
    Parser parser;

    const Struct stest("Person", {"name", "age"});
    const Instance itest(stest, {{"name", Value("John")}, {"age", Value(25ll)}});
    TEST_CASE(parserTest(parser, env, "(var s (struct Person (name age)))",                     Value(stest), true));
    TEST_CASE(parserTest(parser, env, "(var p (makeinstance Person (name \"John\") (age 25)))", Value(itest), true));
    TEST_CASE(parserTest(parser, env, "(istypeof p userobject)",                                Value::True,  true));

    const Instance itest2(stest, {{"name", Value("Sara")}});
    TEST_CASE(parserTest(parser, env, "(var p2 (makeinstance Person (name \"Sara\")))", Value(itest2), true));
    TEST_CASE(parserTest(parser, env, "(istypeof p2 userobject)",                       Value::True,   true));
}

// -------------------------------------------------------------
void UnitTest::testParserIsInstanceOf() {
    auto env = Environment::make();
    Parser parser;

    const Struct stest("Person", {"name", "age"});
    const Instance itest(stest);
    TEST_CASE(parserTest(parser, env, "(struct Person (name age))",    Value(stest), true));
    TEST_CASE(parserTest(parser, env, "(var p (makeinstance Person))", Value(itest), true));
    TEST_CASE(parserTest(parser, env, "(isinstanceof p Person)",       Value::True,  true));
}

// -------------------------------------------------------------
void UnitTest::testParserStructName() {
    auto env = Environment::make();
    Parser parser;

    const Struct stest("Person", {"name", "age"});
    const Instance itest(stest);

    TEST_CASE(parserTest(parser, env, "(struct Person (name age))",    Value(stest),    true));
    TEST_CASE(parserTest(parser, env, "(var p (makeinstance Person))", Value(itest),    true));
    TEST_CASE(parserTest(parser, env, "(structname Person)",           Value("Person"), true));
    TEST_CASE(parserTest(parser, env, "(structname p)",                Value("Person"), true));
    TEST_CASE(parserTest(parser, env, "(structname 5)",                Value::Null,     false));
    TEST_CASE(parserTest(parser, env, "(structname)",                  Value::Null,     false));
    TEST_CASE(parserTest(parser, env, "(structname p Person)",         Value::Null,     false));
}

// -------------------------------------------------------------
void UnitTest::testParserGetSetMember() {
    auto env = Environment::make();
    Parser parser;

    const Struct stest("Person", {"name", "age"});
    const Instance itest(stest);

    TEST_CASE(parserTest(parser, env, "(struct Person (name age))",    Value(stest),  true));
    TEST_CASE(parserTest(parser, env, "(var p (makeinstance Person))", Value(itest),  true));
    TEST_CASE(parserTest(parser, env, "(memget p name)",               Value::Null,   true));
    TEST_CASE(parserTest(parser, env, "(memget p age)",                Value::Null,   true));
    TEST_CASE(parserTest(parser, env, "(memset p name \"John\")",      Value("John"), true));
    TEST_CASE(parserTest(parser, env, "(memset p age 25)",             Value(25ll),   true));
    TEST_CASE(parserTest(parser, env, "(memget p name)",               Value("John"), true));
    TEST_CASE(parserTest(parser, env, "(memget p age)",                Value(25ll),   true));
}

// -------------------------------------------------------------
void UnitTest::testParserStringLen() {
    auto env = Environment::make();
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(strlen \"\")",        Value::Zero, true));
    TEST_CASE(parserTest(parser, env, "(strlen \"a\")",       Value(1ll),  true));
    TEST_CASE(parserTest(parser, env, "(strlen \"string\")",  Value(6ll),  true));
    TEST_CASE(parserTest(parser, env, "(strlen 10)",          Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(strlen)",             Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(strlen \"1\" \"2\")", Value::Null, false));
}

// -------------------------------------------------------------
void UnitTest::testParserStrGet() {
    auto env = Environment::make();
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(strget \"By\" 0)",   Value('B'),  true));
    TEST_CASE(parserTest(parser, env, "(strget \"By\" 1)",   Value('y'),  true));
    TEST_CASE(parserTest(parser, env, "(strget \"By\" 2)",   Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(strget)",            Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(strget \"By\")",     Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(strget \"By\" 0 1)", Value::Null, false));
}

// -------------------------------------------------------------
void UnitTest::testParserStrSrt() {
    auto env = Environment::make();
    Parser parser;

    env->def("str", Value("12345"));

    TEST_CASE(parserTest(parser, env, "(strset str 0 'a')", Value('a'),  true));
    TEST_CASE(parserTest(parser, env, "(strset str 2 'C')", Value('C'),  true));
    TEST_CASE(parserTest(parser, env, "(strset str 6 'b')", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(strset)",           Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(strset str)",       Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(strset str 0)",     Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(strset str 0 1 2)", Value::Null, false));

    TEST_CASE(parserTest(parser, env, "str", Value("a2C45"), true));
}

// -------------------------------------------------------------
void UnitTest::testParserStringCat() {
    auto env = Environment::make();
    Parser parser;

    env->def("str", Value("abc"));

    TEST_CASE(parserTest(parser, env, "(strcat str \"def\")", Value("abcdef"),    true));
    TEST_CASE(parserTest(parser, env, "(strcat str \"gh\")",  Value("abcdefgh"),  true));
    TEST_CASE(parserTest(parser, env, "(strcat str 'i')",     Value("abcdefghi"), true));
    TEST_CASE(parserTest(parser, env, "(strcat str 5)",       Value::Null,        false));
    TEST_CASE(parserTest(parser, env, "(strcat)",             Value::Null,        false));
    TEST_CASE(parserTest(parser, env, "(strcat str)",         Value::Null,        false));
    TEST_CASE(parserTest(parser, env, "(strcat str str str)", Value::Null,        false));

    TEST_CASE(parserTest(parser, env, "str", Value("abcdefghi"), true));
}

// -------------------------------------------------------------
void UnitTest::testParserSubString() {
    auto env = Environment::make();
    Parser parser;

    env->def("str", Value("0123456789"));

    TEST_CASE(parserTest(parser, env, "(substr str 0)",    Value("0123456789"), true));
    TEST_CASE(parserTest(parser, env, "(substr str 5)",    Value("56789"),      true));
    TEST_CASE(parserTest(parser, env, "(substr str 0 10)", Value("0123456789"), true));
    TEST_CASE(parserTest(parser, env, "(substr str 0 5)",  Value("01234"),      true));
    TEST_CASE(parserTest(parser, env, "(substr str 5 5)",  Value("56789"),      true));
    TEST_CASE(parserTest(parser, env, "(substr str 3 5)",  Value("34567"),      true));
    TEST_CASE(parserTest(parser, env, "(sbustr str -1)",   Value::Null,         false));
    TEST_CASE(parserTest(parser, env, "(sbustr str 11)",   Value::Null,         false));
    TEST_CASE(parserTest(parser, env, "(sbustr str 0 -1)", Value::Null,         false));
    TEST_CASE(parserTest(parser, env, "(sbustr str 0 10)", Value::Null,         false));
    TEST_CASE(parserTest(parser, env, "(substr)",          Value::Null,         false));
}

// -------------------------------------------------------------
void UnitTest::testParserStringFind() {
    auto env = Environment::make();
    Parser parser;

    env->def("str", Value("0123456789"));

    TEST_CASE(parserTest(parser, env, "(strfind str '0')",     Value::Zero, true));
    TEST_CASE(parserTest(parser, env, "(strfind str '4')",     Value(4ll),  true));
    TEST_CASE(parserTest(parser, env, "(strfind str '9')",     Value(9ll),  true));
    TEST_CASE(parserTest(parser, env, "(strfind str '0' 0)",   Value(0ll),  true));
    TEST_CASE(parserTest(parser, env, "(strfind str '4' 0)",   Value(4ll),  true));
    TEST_CASE(parserTest(parser, env, "(strfind str '9' 0)",   Value(9ll),  true));
    TEST_CASE(parserTest(parser, env, "(strfind str '4' 2)",   Value(4ll),  true));
    TEST_CASE(parserTest(parser, env, "(strfind str '9' 6)",   Value(9ll),  true));
    TEST_CASE(parserTest(parser, env, "(strfind str 'A')",     Value(-1ll), true));
    TEST_CASE(parserTest(parser, env, "(strfind str 1)",       Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(strfind str '0' '0')", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(strfind)",             Value::Null, false));
}

// -------------------------------------------------------------
void UnitTest::testParserStringCount() {
    auto env = Environment::make();
    Parser parser;

    env->def("str", Value("0120344501223678910"));

    TEST_CASE(parserTest(parser, env, "(strcount str '9')", Value(1ll),  true));
    TEST_CASE(parserTest(parser, env, "(strcount str '8')", Value(1ll),  true));
    TEST_CASE(parserTest(parser, env, "(strcount str '7')", Value(1ll),  true));
    TEST_CASE(parserTest(parser, env, "(strcount str '6')", Value(1ll),  true));
    TEST_CASE(parserTest(parser, env, "(strcount str '5')", Value(1ll),  true));
    TEST_CASE(parserTest(parser, env, "(strcount str '4')", Value(2ll),  true));
    TEST_CASE(parserTest(parser, env, "(strcount str '3')", Value(2ll),  true));
    TEST_CASE(parserTest(parser, env, "(strcount str '2')", Value(3ll),  true));
    TEST_CASE(parserTest(parser, env, "(strcount str '1')", Value(3ll),  true));
    TEST_CASE(parserTest(parser, env, "(strcount str '0')", Value(4ll),  true));
    TEST_CASE(parserTest(parser, env, "(strcount str 'a')", Value::Zero, true));
    TEST_CASE(parserTest(parser, env, "(strcount 500 '0')", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(strcount str 5)",   Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(strcount)",         Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(strcount str)",     Value::Null, false));
}

// -------------------------------------------------------------
void UnitTest::testParserStringCompare() {
    auto env = Environment::make();
    Parser parser;

    env->def("str1", Value("abcde"));
    env->def("str2", Value("abcde"));
    env->def("str3", Value("bcdef"));
    env->def("str4", Value("abcdef"));

    TEST_CASE(parserTest(parser, env, "(strcmp str1 str2)", Value::Zero, true));
    TEST_CASE(parserTest(parser, env, "(strcmp str2 str1)", Value::Zero, true));
    TEST_CASE(parserTest(parser, env, "(strcmp str1 str3)", Value(-1ll), true));
    TEST_CASE(parserTest(parser, env, "(strcmp str3 str1)", Value(1ll),  true));
    TEST_CASE(parserTest(parser, env, "(strcmp str1 str4)", Value(-102ll), true));
    TEST_CASE(parserTest(parser, env, "(strcmp str4 str1)", Value(102ll), true));

    TEST_CASE(parserTest(parser, env, "(strcmp str1 5)",   Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(strcmp 5 str2)",   Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(strcmp str1 'a')", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(strcmp 'a' str2)", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(strcmp 5 10)",     Value::Null, false));
}

// -------------------------------------------------------------
void UnitTest::testParserStringSort() {
    auto env = Environment::make();
    Parser parser;

    env->def("str1", Value("badc"));
    env->def("str2", Value("3421"));

    TEST_CASE(parserTest(parser, env, "(progn (strsort str1) str1)",       Value("abcd"), true));
    TEST_CASE(parserTest(parser, env, "(progn (strsort str1 true) str1)",  Value("dcba"), true));
    TEST_CASE(parserTest(parser, env, "(progn (strsort str1 false) str1)", Value("abcd"), true));

    TEST_CASE(parserTest(parser, env, "(progn (strsort str2) str2)",       Value("1234"), true));
    TEST_CASE(parserTest(parser, env, "(progn (strsort str2 true) str2)",  Value("4321"), true));
    TEST_CASE(parserTest(parser, env, "(progn (strsort str2 false) str2)", Value("1234"), true));

    TEST_CASE(parserTest(parser, env, "(strsort \"34251\")",       Value("12345"), true));
    TEST_CASE(parserTest(parser, env, "(strsort \"34251\" true)",  Value("54321"), true));
    TEST_CASE(parserTest(parser, env, "(strsort \"34251\" false)", Value("12345"), true));

    TEST_CASE(parserTest(parser, env, "(strsort \"333444422555551\")",       Value("122333444455555"), true));
    TEST_CASE(parserTest(parser, env, "(strsort \"333444422555551\" true)",  Value("555554444333221"), true));
    TEST_CASE(parserTest(parser, env, "(strsort \"333444422555551\" false)", Value("122333444455555"), true));

    TEST_CASE(parserTest(parser, env, "(strsort 34251)",       Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(strsort 34251 true)",  Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(strsort 34251 false)", Value::Null, false));

    TEST_CASE(parserTest(parser, env, "(strsort str1 3)", Value::Null, false));
}

// -------------------------------------------------------------
void UnitTest::testParserStringReverse() {
    auto env = Environment::make();
    Parser parser;

    env->def("str1", Value("abcd"));
    env->def("str2", Value("1234"));

    TEST_CASE(parserTest(parser, env, "(progn (strrev str1) str1)", Value("dcba"), true));
    TEST_CASE(parserTest(parser, env, "(progn (strrev str1) str1)", Value("abcd"), true));
    TEST_CASE(parserTest(parser, env, "(progn (strrev str2) str2)", Value("4321"), true));
    TEST_CASE(parserTest(parser, env, "(progn (strrev str2) str2)", Value("1234"), true));

    TEST_CASE(parserTest(parser, env, "(strrev \"12345\")", Value("54321"), true));
    TEST_CASE(parserTest(parser, env, "(strrev \"54321\")", Value("12345"), true));

    TEST_CASE(parserTest(parser, env, "(strsort 34251)", Value::Null, false));
}

// -------------------------------------------------------------
void UnitTest::testParserMakeArray() {
    auto env = Environment::make();
    Parser parser;

    const Value v1(1ll);
    const Value v2(2ll);
    const Value v3(3ll);

    TEST_CASE(parserTest(parser, env, "(array)",       Value(Sequence()),             true));
    TEST_CASE(parserTest(parser, env, "(array 1)",     Value(Sequence({v1})),         true));
    TEST_CASE(parserTest(parser, env, "(array 1 2)",   Value(Sequence({v1, v2})),     true));
    TEST_CASE(parserTest(parser, env, "(array 1 2 3)", Value(Sequence({v1, v2, v3})), true));
}

// -------------------------------------------------------------
void UnitTest::testParserMakeArraySV() {
    auto env = Environment::make();
    Parser parser;

    const Value v0(0ll);
    const Value v1(1ll);
    const Value v2(2ll);

    TEST_CASE(parserTest(parser, env, "(arraysv 1 0)",   Value(Sequence({v0})),          true));
    TEST_CASE(parserTest(parser, env, "(arraysv 2 1)",   Value(Sequence({v1, v1})),      true));
    TEST_CASE(parserTest(parser, env, "(arraysv 3 2)",   Value(Sequence({v2, v2, v2})),  true));
    TEST_CASE(parserTest(parser, env, "(arraysv 1)",     Value(Sequence({Value::Null})), true));
    TEST_CASE(parserTest(parser, env, "(arraysv)",       Value::Null,                    false));
    TEST_CASE(parserTest(parser, env, "(arraysv 'a' 0)", Value::Null,                    false));
    TEST_CASE(parserTest(parser, env, "(arraysv)",       Value::Null,                    false));
}

// -------------------------------------------------------------
void UnitTest::testParserArrayLen() {
    auto env = Environment::make();
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(arrlen (array))",         Value::Zero, true));
    TEST_CASE(parserTest(parser, env, "(arrlen (array 1))",       Value(1ll),  true));
    TEST_CASE(parserTest(parser, env, "(arrlen (array 1 2 3 4))", Value(4ll),  true));
    TEST_CASE(parserTest(parser, env, "(arrlen 10)",              Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(arrlen)",                 Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(arrlen (array) (array))", Value::Null, false));
}

// -------------------------------------------------------------
void UnitTest::testParserArrayGet() {
    auto env = Environment::make();
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(arrget (array 'B' 'y') 0)", Value('B'),  true));
    TEST_CASE(parserTest(parser, env, "(arrget (array 'B' 'y') 1)", Value('y'),  true));
    TEST_CASE(parserTest(parser, env, "(arrget (array 'B' 'y') 2)", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(arrget)",                   Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(arrget (array 1))",         Value::Null, false));
}

// -------------------------------------------------------------
void UnitTest::testParserArraySet() {
    auto env = Environment::make();
    Parser parser;

    env->def("arr", Value(Sequence({Value(1ll), Value(2ll), Value(3ll), Value(4ll), Value(5ll)})));

    TEST_CASE(parserTest(parser, env, "(arrset arr 0 'a')", Value('a'),  true));
    TEST_CASE(parserTest(parser, env, "(arrset arr 2 'C')", Value('C'),  true));
    TEST_CASE(parserTest(parser, env, "(arrset arr 6 'b')", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(arrset)",           Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(arrset arr)",       Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(arrset arr 0)",     Value::Null, false));

    Value expected(Sequence({Value('a'), Value(2ll), Value('C'), Value(4ll), Value(5ll)}));
    TEST_CASE(parserTest(parser, env, "arr", expected, true));
}

// -------------------------------------------------------------
void UnitTest::testParserArrayPush() {
    auto env = Environment::make();
    Parser parser;

    env->def("arr", Value(Sequence()));

    Value expected = Value(Sequence({Value(1ll)}));
    TEST_CASE(parserTest(parser, env, "(arrpush arr 1)", expected, true));

    expected = Value(Sequence({Value(1ll), Value(2ll)}));
    TEST_CASE(parserTest(parser, env, "(arrpush arr 2)", expected, true));

    TEST_CASE(parserTest(parser, env, "(arrpush)",      Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(arrpush arr)",  Value::Null, false));
}

// -------------------------------------------------------------
void UnitTest::testParserArrayPop() {
    auto env = Environment::make();
    Parser parser;

    env->def("arr", arrval(Value(0ll), Value(1ll)));

    TEST_CASE(parserTest(parser, env, "(arrpop arr)", Value(1ll),         true));
    TEST_CASE(parserTest(parser, env, "arr",          arrval(Value(0ll)), true));
    TEST_CASE(parserTest(parser, env, "(arrpop arr)", Value(0ll),         true));
    TEST_CASE(parserTest(parser, env, "arr",          arrval(),           true));

    TEST_CASE(parserTest(parser, env, "(arrpop arr)", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(arrpop)",     Value::Null, false));
}

// -------------------------------------------------------------
void UnitTest::testParserArrayFind() {
    auto env = Environment::make();
    Parser parser;

    env->def("arr", Value(Sequence({Value('a'), Value('b'), Value('c'), Value('a')})));

    TEST_CASE(parserTest(parser, env, "(arrfind arr 'a')", Value(0ll),  true));
    TEST_CASE(parserTest(parser, env, "(arrfind arr 'b')", Value(1ll),  true));
    TEST_CASE(parserTest(parser, env, "(arrfind arr 'c')", Value(2ll),  true));
    TEST_CASE(parserTest(parser, env, "(arrfind arr 'd')", Value(-1ll), true));

    TEST_CASE(parserTest(parser, env, "(arrfind arr 'a' 1)", Value(3ll),  true));
    TEST_CASE(parserTest(parser, env, "(arrfind arr 'b' 1)", Value(1ll),  true));
    TEST_CASE(parserTest(parser, env, "(arrfind arr 'c' 1)", Value(2ll),  true));
    TEST_CASE(parserTest(parser, env, "(arrfind arr 'd' 1)", Value(-1ll), true));

    TEST_CASE(parserTest(parser, env, "(arrfind arr 'c' -1)", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(arrfind arr 'c' 4)",  Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(arrfind)",            Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(arrfind arr)",        Value::Null, false));
}

// -------------------------------------------------------------
void UnitTest::testParserArrayCount() {
    auto env = Environment::make();
    Parser parser;

    const Value v1(1ll);
    const Value v2(2ll);
    const Value v3(3ll);
    const Value v4(4ll);

    env->def("arr", Value(Sequence({v1, v2, v3, v2, v3, v4, v3, v4, v4, v4})));

    TEST_CASE(parserTest(parser, env, "(arrcount arr 1)", Value(1ll), true));
    TEST_CASE(parserTest(parser, env, "(arrcount arr 2)", Value(2ll), true));
    TEST_CASE(parserTest(parser, env, "(arrcount arr 3)", Value(3ll), true));
    TEST_CASE(parserTest(parser, env, "(arrcount arr 4)", Value(4ll), true));
    TEST_CASE(parserTest(parser, env, "(arrcount arr 5)", Value(0ll), true));

    TEST_CASE(parserTest(parser, env, "(arrcount '5' 0)", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(arrcount)",       Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(arrcount arr)",   Value::Null, false));
}

// -------------------------------------------------------------
void UnitTest::testParserArraySort() {
    auto env = Environment::make();
    Parser parser;

    env->def("arr1", Value(Sequence({Value('b'), Value('d'), Value('c'), Value('a')})));
    env->def("arr2", Value(Sequence({Value(2ll), Value(3ll), Value(4ll), Value(1ll)})));

    const Value arr1Asc(Sequence({Value('a'), Value('b'), Value('c'), Value('d')}));
    const Value arr2Asc(Sequence({Value(1ll), Value(2ll), Value(3ll), Value(4ll)}));
    const Value arr1Desc(Sequence({Value('d'), Value('c'), Value('b'), Value('a')}));
    const Value arr2Desc(Sequence({Value(4ll), Value(3ll), Value(2ll), Value(1ll)}));

    TEST_CASE(parserTest(parser, env, "(progn (arrsort arr1) arr1)",       arr1Asc,  true));
    TEST_CASE(parserTest(parser, env, "(progn (arrsort arr1 true) arr1)",  arr1Desc, true));
    TEST_CASE(parserTest(parser, env, "(progn (arrsort arr1 false) arr1)", arr1Asc,  true));

    TEST_CASE(parserTest(parser, env, "(progn (arrsort arr2) arr2)",       arr2Asc,  true));
    TEST_CASE(parserTest(parser, env, "(progn (arrsort arr2 true) arr2)",  arr2Desc, true));
    TEST_CASE(parserTest(parser, env, "(progn (arrsort arr2 false) arr2)", arr2Asc,  true));

    TEST_CASE(parserTest(parser, env, "(arrsort (array 3 1))",       Value(Sequence({Value(1ll), Value(3ll)})), true));
    TEST_CASE(parserTest(parser, env, "(arrsort (array 1 3) true)",  Value(Sequence({Value(3ll), Value(1ll)})), true));
    TEST_CASE(parserTest(parser, env, "(arrsort (array 1 3) false)", Value(Sequence({Value(1ll), Value(3ll)})), true));

    TEST_CASE(parserTest(parser, env, "(arrsort (array 3 1 1))",       Value(Sequence({Value(1ll), Value(1ll), Value(3ll)})), true));
    TEST_CASE(parserTest(parser, env, "(arrsort (array 1 1 3) true)",  Value(Sequence({Value(3ll), Value(1ll), Value(1ll)})), true));
    TEST_CASE(parserTest(parser, env, "(arrsort (array 1 1 3) false)", Value(Sequence({Value(1ll), Value(1ll), Value(3ll)})), true));

    TEST_CASE(parserTest(parser, env, "(arrsort 34251)",       Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(arrsort 34251 true)",  Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(arrsort 34251 false)", Value::Null, false));

    TEST_CASE(parserTest(parser, env, "(strsort arr1 3)", Value::Null, false));
}

// -------------------------------------------------------------
void UnitTest::testParserArrayReverse() {
    auto env = Environment::make();
    Parser parser;

    env->def("arr1", Value(Sequence({Value('b'), Value('d'), Value('c'), Value('a')})));
    env->def("arr2", Value(Sequence({Value(2ll), Value(3ll), Value(4ll), Value(1ll)})));

    const Value arr1Orig(Sequence({Value('b'), Value('d'), Value('c'), Value('a')}));
    const Value arr2Orig(Sequence({Value(2ll), Value(3ll), Value(4ll), Value(1ll)}));
    const Value arr1Rev(Sequence({Value('a'), Value('c'), Value('d'), Value('b')}));
    const Value arr2Rev(Sequence({Value(1ll), Value(4ll), Value(3ll), Value(2ll)}));

    TEST_CASE(parserTest(parser, env, "(progn (arrrev arr1) arr1)",  arr1Rev,  true));
    TEST_CASE(parserTest(parser, env, "(progn (arrrev arr1) arr1)",  arr1Orig,  true));
    TEST_CASE(parserTest(parser, env, "(progn (arrrev arr2) arr2)",  arr2Rev,  true));
    TEST_CASE(parserTest(parser, env, "(progn (arrrev arr2) arr2)",  arr2Orig,  true));

    TEST_CASE(parserTest(parser, env, "(arrrev (array 3 1))",   Value(Sequence({Value(1ll), Value(3ll)})), true));
    TEST_CASE(parserTest(parser, env, "(arrrev (array 3 1 1))", Value(Sequence({Value(1ll), Value(1ll), Value(3ll)})), true));

    TEST_CASE(parserTest(parser, env, "(arrrev 34251)", Value::Null, false));
}

// -------------------------------------------------------------
void UnitTest::testParserArrayClear() {
    auto env = Environment::make();
    Parser parser;

    env->def("a", Value(Sequence({Value('a'), Value('b'), Value('c')})));

    TEST_CASE(parserTest(parser, env, "(arrlen a)", Value(3ll),  true));
    TEST_CASE(parserTest(parser, env, "(arrclr a)", Value::Null, true));
    TEST_CASE(parserTest(parser, env, "(arrlen a)", Value(0ll),  true));

    TEST_CASE(parserTest(parser, env, "(arrlen)",     Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(arrlen a 2)", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(arrlen 5)",   Value::Null, false));
}

// -------------------------------------------------------------
void UnitTest::testParserArrayInsert() {
    auto env = Environment::make();
    Parser parser;

    env->def("arr", Value(Sequence()));

    const Value v0 = Value::Zero;
    const Value v1 = Value(1ll);
    const Value v2 = Value(2ll);
    const Value v3 = Value(3ll);
    const Value v4 = Value(4ll);
    const Value v5 = Value(5ll);

    TEST_CASE(parserTest(parser, env, "(arrins arr 0 3)", v3,         true));
    TEST_CASE(parserTest(parser, env, "arr",              arrval(v3), true));

    TEST_CASE(parserTest(parser, env, "(arrins arr 0 1)", v1,             true));
    TEST_CASE(parserTest(parser, env, "arr",              arrval(v1, v3), true));

    TEST_CASE(parserTest(parser, env, "(arrins arr 2 5)", v5,                 true));
    TEST_CASE(parserTest(parser, env, "arr",              arrval(v1, v3, v5), true));

    TEST_CASE(parserTest(parser, env, "(arrins arr 0 0)", v0,                     true));
    TEST_CASE(parserTest(parser, env, "arr",              arrval(v0, v1, v3, v5), true));

    TEST_CASE(parserTest(parser, env, "(arrins arr 2 2)", v2,                         true));
    TEST_CASE(parserTest(parser, env, "arr",              arrval(v0, v1, v2, v3, v5), true));

    TEST_CASE(parserTest(parser, env, "(arrins arr 4 4)", v4,                             true));
    TEST_CASE(parserTest(parser, env, "arr",              arrval(v0, v1, v2, v3, v4, v5), true));

    TEST_CASE(parserTest(parser, env, "(arrins 3 5 5)",     Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(arrins arr 'a' 5)", Value::Null, false));
}

// -------------------------------------------------------------
void UnitTest::testParserArrayRemove() {
    auto env = Environment::make();
    Parser parser;

    const Value v0 = Value::Zero;
    const Value v1 = Value(1ll);
    const Value v2 = Value(2ll);
    const Value v3 = Value(3ll);
    const Value v4 = Value(4ll);
    const Value v5 = Value(5ll);

    env->def("arr", arrval(v0, v1, v2, v3, v4, v5));

    TEST_CASE(parserTest(parser, env, "(arrrem arr 4)", Value::Null,                true));
    TEST_CASE(parserTest(parser, env, "arr",            arrval(v0, v1, v2, v3, v5), true));

    TEST_CASE(parserTest(parser, env, "(arrrem arr 2)", Value::Null,            true));
    TEST_CASE(parserTest(parser, env, "arr",            arrval(v0, v1, v3, v5), true));

    TEST_CASE(parserTest(parser, env, "(arrrem arr 0)", Value::Null,            true));
    TEST_CASE(parserTest(parser, env, "arr",            arrval(v1, v3, v5),     true));

    TEST_CASE(parserTest(parser, env, "(arrrem arr 2)", Value::Null,    true));
    TEST_CASE(parserTest(parser, env, "arr",            arrval(v1, v3), true));

    TEST_CASE(parserTest(parser, env, "(arrrem arr 0)", Value::Null, true));
    TEST_CASE(parserTest(parser, env, "arr",            arrval(v3),  true));

    TEST_CASE(parserTest(parser, env, "(arrrem arr 0)", Value::Null, true));
    TEST_CASE(parserTest(parser, env, "arr",            arrval(),    true));

    TEST_CASE(parserTest(parser, env, "(arrrem 3 5)",     Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(arrrem arr 'a')", Value::Null, false));
}

// -------------------------------------------------------------
void UnitTest::testParserStrCharCheck() {
    auto env = Environment::make();
    Parser parser;

    env->def("cUp", Value('U'));
    env->def("cLo", Value('l'));
    env->def("cNm", Value('5'));
    env->def("cPt", Value('?'));
    env->def("cSp", Value(' '));
    env->def("sUp", Value("UPP"));
    env->def("sLo", Value("low"));
    env->def("sNm", Value("123"));
    env->def("sPt", Value("@?!"));
    env->def("sSp", Value(" \t\n"));
    env->def("mix", Value("Ul5? "));

    TEST_CASE(parserTest(parser, env, "(isupper cUp)", Value::True, true));
    TEST_CASE(parserTest(parser, env, "(islower cLo)", Value::True, true));
    TEST_CASE(parserTest(parser, env, "(isalpha cLo)", Value::True, true));
    TEST_CASE(parserTest(parser, env, "(isnumer cNm)", Value::True, true));
    TEST_CASE(parserTest(parser, env, "(isalnum cLo)", Value::True, true));
    TEST_CASE(parserTest(parser, env, "(isalnum cNm)", Value::True, true));
    TEST_CASE(parserTest(parser, env, "(ispunct cPt)", Value::True, true));
    TEST_CASE(parserTest(parser, env, "(isspace cSp)", Value::True, true));

    TEST_CASE(parserTest(parser, env, "(isupper cLo)", Value::False, true));
    TEST_CASE(parserTest(parser, env, "(islower cUp)", Value::False, true));
    TEST_CASE(parserTest(parser, env, "(isalpha cNm)", Value::False, true));
    TEST_CASE(parserTest(parser, env, "(isnumer cLo)", Value::False, true));
    TEST_CASE(parserTest(parser, env, "(isalnum cPt)", Value::False, true));
    TEST_CASE(parserTest(parser, env, "(ispunct cSp)", Value::False, true));
    TEST_CASE(parserTest(parser, env, "(isspace cPt)", Value::False, true));

    TEST_CASE(parserTest(parser, env, "(isupper sUp)", Value::True, true));
    TEST_CASE(parserTest(parser, env, "(islower sLo)", Value::True, true));
    TEST_CASE(parserTest(parser, env, "(isalpha sLo)", Value::True, true));
    TEST_CASE(parserTest(parser, env, "(isnumer sNm)", Value::True, true));
    TEST_CASE(parserTest(parser, env, "(isalnum sLo)", Value::True, true));
    TEST_CASE(parserTest(parser, env, "(isalnum sNm)", Value::True, true));
    TEST_CASE(parserTest(parser, env, "(ispunct sPt)", Value::True, true));
    TEST_CASE(parserTest(parser, env, "(isspace sSp)", Value::True, true));

    TEST_CASE(parserTest(parser, env, "(isupper sLo)", Value::False, true));
    TEST_CASE(parserTest(parser, env, "(islower sUp)", Value::False, true));
    TEST_CASE(parserTest(parser, env, "(isalpha sNm)", Value::False, true));
    TEST_CASE(parserTest(parser, env, "(isnumer sLo)", Value::False, true));
    TEST_CASE(parserTest(parser, env, "(isalnum sPt)", Value::False, true));
    TEST_CASE(parserTest(parser, env, "(ispunct sSp)", Value::False, true));
    TEST_CASE(parserTest(parser, env, "(isspace sPt)", Value::False, true));

    TEST_CASE(parserTest(parser, env, "(isupper mix)", Value::False, true));
    TEST_CASE(parserTest(parser, env, "(islower mix)", Value::False, true));
    TEST_CASE(parserTest(parser, env, "(isalpha mix)", Value::False, true));
    TEST_CASE(parserTest(parser, env, "(isnumer mix)", Value::False, true));
    TEST_CASE(parserTest(parser, env, "(isalnum mix)", Value::False, true));
    TEST_CASE(parserTest(parser, env, "(ispunct mix)", Value::False, true));
    TEST_CASE(parserTest(parser, env, "(isspace mix)", Value::False, true));

    TEST_CASE(parserTest(parser, env, "(isupper 5)", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(islower 5)", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(isalpha 5)", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(isnumer 5)", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(isalnum 5)", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(ispunct 5)", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(isspace 5)", Value::Null, false));
}

// -------------------------------------------------------------
void UnitTest::testParserStrCharTransform() {
    auto env = Environment::make();
    Parser parser;

    env->def("cUp", Value('A'));
    env->def("cLo", Value('a'));
    env->def("cNm", Value('5'));
    env->def("sUp", Value("ABC"));
    env->def("sLo", Value("abc"));
    env->def("sNm", Value("123"));

    TEST_CASE(parserTest(parser, env, "(toupper cUp)", Value('A'), true));
    TEST_CASE(parserTest(parser, env, "(toupper cLo)", Value('A'), true));
    TEST_CASE(parserTest(parser, env, "(toupper cNm)", Value('5'), true));
    TEST_CASE(parserTest(parser, env, "(tolower cUp)", Value('a'), true));
    TEST_CASE(parserTest(parser, env, "(tolower cLo)", Value('a'), true));
    TEST_CASE(parserTest(parser, env, "(tolower cNm)", Value('5'), true));

    TEST_CASE(parserTest(parser, env, "(toupper sUp)", Value("ABC"), true));
    TEST_CASE(parserTest(parser, env, "(toupper sLo)", Value("ABC"), true));
    TEST_CASE(parserTest(parser, env, "(toupper sNm)", Value("123"), true));
    TEST_CASE(parserTest(parser, env, "(tolower sUp)", Value("abc"), true));
    TEST_CASE(parserTest(parser, env, "(tolower sLo)", Value("abc"), true));
    TEST_CASE(parserTest(parser, env, "(tolower sNm)", Value("123"), true));

    TEST_CASE(parserTest(parser, env, "(toupper 10)", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(tolower 10)", Value::Null, false));
}

// -------------------------------------------------------------
void UnitTest::testParserImportModule() {
    const std::string moduleName = "pimporttest";
    auto tempFile(createTempModuleFile(moduleName));

    Parser parser;

    {
        auto env = Environment::make();
        const std::string asName = "test";

        auto varName = [&moduleName](const char * var) { return moduleName + '.' + var; };
        auto varAsName = [&asName](const char *var) { return asName + '.' + var; };

        TEST_CASE(parserTest(parser, env, "(import pimporttest)", Value::True, true));

        TEST_CASE_MSG(env->size() == 3, "actual=" << env->size());

        TEST_CASE(parserTest(parser, env, "(import pimporttest as test)", Value::True, true));

        TEST_CASE_MSG(env->size() == 6, "actual=" << env->size());

        TEST_CASE(env->get(varName("PI")) == env->get(varAsName("PI")));
        TEST_CASE(env->get(varName("add")) == env->get(varAsName("add")));
        TEST_CASE(env->get(varName("sub")) == env->get(varAsName("sub")));

        TEST_CASE(parserTest(parser, env, "(import pimporttest)",         Value::Null, false));
        TEST_CASE(parserTest(parser, env, "(import pimporttest as test)", Value::Null, false));
    }

    {
        auto env = Environment::make();
        TEST_CASE(parserTest(parser, env, "(import)",                Value::Null, false));
        TEST_CASE(parserTest(parser, env, "(import as)",             Value::Null, false));
        TEST_CASE(parserTest(parser, env, "(import pimporttest as)", Value::Null, false));

        TEST_CASE_MSG(env->size() == 0, "actual=" << env->size());
    }
}

// -------------------------------------------------------------
void UnitTest::testParserFromModuleImport() {
    const std::string moduleName = "pfromtest";
    auto tempFile(createTempModuleFile(moduleName));

    Parser parser;

    {
        auto env = Environment::make();

        TEST_CASE(parserTest(parser, env, "(from pfromtest import add)", Value::True, true));
        TEST_CASE(parserTest(parser, env, "(from pfromtest import sub)", Value::True, true));

        TEST_CASE_MSG(env->size() == 2, "actual=" << env->size());

        TEST_CASE(parserTest(parser, env, "(from pfromtest import add as plus)",  Value::True, true));
        TEST_CASE(parserTest(parser, env, "(from pfromtest import sub as minus)", Value::True, true));

        TEST_CASE_MSG(env->size() == 4, "actual=" << env->size());

        TEST_CASE(env->get("add") == env->get("plus"));
        TEST_CASE(env->get("sub") == env->get("minus"));

        TEST_CASE(parserTest(parser, env, "(from pfromtest import add)", Value::Null, false));
        TEST_CASE(parserTest(parser, env, "(from pfromtest import sub)", Value::Null, false));
        TEST_CASE(parserTest(parser, env, "(from pfromtest import add as plus)",  Value::Null, false));
        TEST_CASE(parserTest(parser, env, "(from pfromtest import sub as minus)", Value::Null, false));
    }

    {
        auto env = Environment::make();

        TEST_CASE(parserTest(parser, env, "(from pfromtest import add sub)", Value::True, true));

        TEST_CASE_MSG(env->size() == 2, "actual=" << env->size());

        TEST_CASE(parserTest(parser, env, "(from pfromtest import add as plus sub as minus)",  Value::True, true));

        TEST_CASE_MSG(env->size() == 4, "actual=" << env->size());

        TEST_CASE(env->get("add") == env->get("plus"));
        TEST_CASE(env->get("sub") == env->get("minus"));
    }

    {
        auto env = Environment::make();

        TEST_CASE(parserTest(parser, env, "(from)",                                     Value::Null, false));
        TEST_CASE(parserTest(parser, env, "(from pfromtest)",                           Value::Null, false));
        TEST_CASE(parserTest(parser, env, "(from pfromtest import)",                    Value::Null, false));
        TEST_CASE(parserTest(parser, env, "(from pfromtest import as)",                 Value::Null, false));
        TEST_CASE(parserTest(parser, env, "(from pfromtest import add as)",             Value::Null, false));
        TEST_CASE(parserTest(parser, env, "(from pfromtest import add as plus as)",     Value::Null, false));
        TEST_CASE(parserTest(parser, env, "(from pfromtest import add as plus sub as)", Value::Null, false));

        TEST_CASE_MSG(env->size() == 0, "actual=" << env->size());
    }
}

// -------------------------------------------------------------
void UnitTest::testParserRandom() {
    auto env = Environment::make();
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(rand 0)",   Value::Zero, true));
    TEST_CASE(parserTest(parser, env, "(rand -1)",  Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(rand 'a')", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(rand 1 2)", Value::Null, false));
}

// -------------------------------------------------------------
void UnitTest::testParserHash() {
    auto env = Environment::make();
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(hash null)", Value::Zero, true));
    TEST_CASE(parserTest(parser, env, "(hash 0)",    Value::Zero, true));
    TEST_CASE(parserTest(parser, env, "(hash)",      Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hash 1 2)",  Value::Null, false));
}

// -------------------------------------------------------------
void UnitTest::testParserMakeHashMap() {
    auto env = Environment::make();
    Parser parser;

    auto ht = [](int count) {
                  Hashtable ht;
                  if (count >= 1) { ht.set(Value('a'), Value(0ll)); }
                  if (count >= 2) { ht.set(Value('b'), Value(1ll)); }
                  if (count >= 3) { ht.set(Value('c'), Value(2ll)); }
                  return ht;
              };

    TEST_CASE(parserTest(parser, env, "(hashmap)",                                           Value(ht(0)), true));
    TEST_CASE(parserTest(parser, env, "(hashmap (pair 'a' 0))",                              Value(ht(1)), true));
    TEST_CASE(parserTest(parser, env, "(hashmap (pair 'a' 0) (pair 'b' 1))",                 Value(ht(2)), true));
    TEST_CASE(parserTest(parser, env, "(hashmap (pair 'a' 0) (pair 'b' 1) (pair 'c' 2))",    Value(ht(3)), true));
    TEST_CASE(parserTest(parser, env, "(hashmap (array 'a' 0))",                             Value(ht(1)), true));
    TEST_CASE(parserTest(parser, env, "(hashmap (array 'a' 0) (array 'b' 1))",               Value(ht(2)), true));
    TEST_CASE(parserTest(parser, env, "(hashmap (array 'a' 0) (array 'b' 1) (array 'c' 2))", Value(ht(3)), true));

    TEST_CASE(parserTest(parser, env, "(hashmap (pair))",        Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hashmap (pair 1))",      Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hashmap (pair 1 2 3))",  Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hashmap (array))",       Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hashmap (array 1))",     Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hashmap (array 1 2 3))", Value::Null, false));
}

// -------------------------------------------------------------
void UnitTest::testParserHashMapLen() {
    auto env = Environment::make();
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(hmlen (hashmap))",                                           Value::Zero, true));
    TEST_CASE(parserTest(parser, env, "(hmlen (hashmap (array 'a' 1)))",                             Value(1ll),  true));
    TEST_CASE(parserTest(parser, env, "(hmlen (hashmap (array 'a' 1) (array 'b' 2) (array 'c' 3)))", Value(3ll),  true));

    TEST_CASE(parserTest(parser, env, "(hmlen 10)",                  Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmlen)",                     Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmlen (hashamp) (hashmap))", Value::Null, false));
}

// -------------------------------------------------------------
void UnitTest::testParserHashMapContains() {
    auto env = Environment::make();
    Parser parser;

    Hashtable ht;
    ht.set(Value('a'), Value(1ll));
    ht.set(Value('b'), Value(2ll));
    ht.set(Value('c'), Value(3ll));
    env->def("ht", ht);

    TEST_CASE(parserTest(parser, env, "(hmhas (hashmap) 'a')",  Value::False, true));
    TEST_CASE(parserTest(parser, env, "(hmhas ht 'a')",         Value::True,  true));
    TEST_CASE(parserTest(parser, env, "(hmhas ht 'b')",         Value::True,  true));
    TEST_CASE(parserTest(parser, env, "(hmhas ht 'c')",         Value::True,  true));
    TEST_CASE(parserTest(parser, env, "(hmhas ht 'd')",         Value::False, true));

    TEST_CASE(parserTest(parser, env, "(hmhas)",            Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmhas ht)",         Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmhas ht 'a' 'b')", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmhas 'a')",        Value::Null, false));
}

// -------------------------------------------------------------
void UnitTest::testParserHashMapGet() {
    auto env = Environment::make();
    Parser parser;

    Hashtable ht;
    ht.set(Value('a'), Value(1ll));
    ht.set(Value('b'), Value(2ll));
    ht.set(Value('c'), Value(3ll));
    env->def("ht", ht);

    TEST_CASE(parserTest(parser, env, "(hmget (hashmap) 'a')", Value::Null, true));
    TEST_CASE(parserTest(parser, env, "(hmget ht 'a')",        Value(1ll),  true));
    TEST_CASE(parserTest(parser, env, "(hmget ht 'b')",        Value(2ll),  true));
    TEST_CASE(parserTest(parser, env, "(hmget ht 'c')",        Value(3ll),  true));
    TEST_CASE(parserTest(parser, env, "(hmget ht 'd')",        Value::Null, true));

    TEST_CASE(parserTest(parser, env, "(hmget (hashmap) 'a' 0)", Value::Zero, true));
    TEST_CASE(parserTest(parser, env, "(hmget ht 'a' 0)",        Value(1ll),  true));
    TEST_CASE(parserTest(parser, env, "(hmget ht 'b' 0)",        Value(2ll),  true));
    TEST_CASE(parserTest(parser, env, "(hmget ht 'c' 0)",        Value(3ll),  true));
    TEST_CASE(parserTest(parser, env, "(hmget ht 'd' 0)",        Value::Zero, true));

    TEST_CASE(parserTest(parser, env, "(hmget)",            Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmget ht)",         Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmget ht 'a' 0 1)", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmget 'a' 0)",      Value::Null, false));
}

// -------------------------------------------------------------
void UnitTest::testParserHashMapSet() {
    auto env = Environment::make();
    Parser parser;

    Hashtable ht;
    env->def("ht", Value(ht));

    auto htget = [&env](Value::Char key) { return env->get("ht").hashMap().get(Value(key)); };
    auto htlen = [&env]() { return env->get("ht").hashMap().size(); };

    TEST_CASE(parserTest(parser, env, "(hmset ht 'a' 1)", Value(1ll), true));
    TEST_CASE(parserTest(parser, env, "(hmset ht 'b' 2)", Value(2ll), true));
    TEST_CASE(parserTest(parser, env, "(hmset ht 'c' 3)", Value(3ll), true));

    TEST_CASE(htlen() == 3);
    TEST_CASE(htget('a') == Value(1ll));
    TEST_CASE(htget('b') == Value(2ll));
    TEST_CASE(htget('c') == Value(3ll));

    TEST_CASE(parserTest(parser, env, "(hmset ht 'a' 100)", Value(100ll), true));
    TEST_CASE(parserTest(parser, env, "(hmset ht 'b' 200)", Value(200ll), true));
    TEST_CASE(parserTest(parser, env, "(hmset ht 'c' 300)", Value(300ll), true));

    TEST_CASE(htlen() == 3);
    TEST_CASE(htget('a') == Value(100ll));
    TEST_CASE(htget('b') == Value(200ll));
    TEST_CASE(htget('c') == Value(300ll));

    TEST_CASE(parserTest(parser, env, "(hmset)",            Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmset ht)",         Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmset ht 'a')",     Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmset ht 'a' 1 2)", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmset 'a' 1 2)",    Value::Null, false));
}

// -------------------------------------------------------------
void UnitTest::testParserHashMapRemove() {
    auto env = Environment::make();
    Parser parser;

    Hashtable ht;
    ht.set(Value('a'), Value(1ll));
    ht.set(Value('b'), Value(2ll));
    ht.set(Value('c'), Value(3ll));
    env->def("ht", Value(ht));

    auto hthas = [&env](Value::Char key) { return env->get("ht").hashMap().exists(Value(key)); };
    auto htlen = [&env]() { return env->get("ht").hashMap().size(); };

    TEST_CASE(parserTest(parser, env, "(hmrem ht 'd')", Value::Null, true));
    TEST_CASE(htlen() == 3);
    TEST_CASE(hthas('a'));
    TEST_CASE(hthas('b'));
    TEST_CASE(hthas('c'));

    TEST_CASE(parserTest(parser, env, "(hmrem ht 'c')", Value::Null, true));
    TEST_CASE(htlen() == 2);
    TEST_CASE(hthas('a'));
    TEST_CASE(hthas('b'));
    TEST_CASE(!hthas('c'));

    TEST_CASE(parserTest(parser, env, "(hmrem ht 'b')", Value::Null, true));
    TEST_CASE(htlen() == 1);
    TEST_CASE(hthas('a'));
    TEST_CASE(!hthas('b'));
    TEST_CASE(!hthas('c'));

    TEST_CASE(parserTest(parser, env, "(hmrem ht 'a')", Value::Null, true));
    TEST_CASE(htlen() == 0);
    TEST_CASE(!hthas('a'));
    TEST_CASE(!hthas('b'));
    TEST_CASE(!hthas('c'));

    TEST_CASE(parserTest(parser, env, "(hmrem)",          Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmrem ht)",       Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmrem ht 'a' 1)", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmrem 'a' 1)",    Value::Null, false));
}

// -------------------------------------------------------------
void UnitTest::testParserHashMapClear() {
    auto env = Environment::make();
    Parser parser;

    Hashtable ht;
    ht.set(Value('a'), Value(1ll));
    ht.set(Value('b'), Value(2ll));
    ht.set(Value('c'), Value(3ll));
    env->def("ht", Value(ht));

    auto hthas = [&env](Value::Char key) { return env->get("ht").hashMap().exists(Value(key)); };
    auto htlen = [&env]() { return env->get("ht").hashMap().size(); };

    TEST_CASE(htlen() == 3);
    TEST_CASE(hthas('a'));
    TEST_CASE(hthas('b'));
    TEST_CASE(hthas('c'));

    TEST_CASE(parserTest(parser, env, "(hmclr ht)", Value::Null, true));

    TEST_CASE(htlen() == 0);
    TEST_CASE(!hthas('a'));
    TEST_CASE(!hthas('b'));
    TEST_CASE(!hthas('c'));

    TEST_CASE(parserTest(parser, env, "(hmclr ht)", Value::Null, true));

    TEST_CASE(parserTest(parser, env, "(hmclr)",        Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmclr ht 'a')", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmclr 'a')",    Value::Null, false));
}

// -------------------------------------------------------------
void UnitTest::testParserHashMapFind() {
    auto env = Environment::make();
    Parser parser;

    Hashtable ht;
    ht.set(Value('a'), Value(1ll));
    ht.set(Value('b'), Value(2ll));
    ht.set(Value('c'), Value(3ll));
    env->def("ht", Value(ht));

    TEST_CASE(parserTest(parser, env, "(hmfind ht 1)", Value('a'),  true));
    TEST_CASE(parserTest(parser, env, "(hmfind ht 2)", Value('b'),  true));
    TEST_CASE(parserTest(parser, env, "(hmfind ht 3)", Value('c'),  true));
    TEST_CASE(parserTest(parser, env, "(hmfind ht 4)", Value::Null, true));

    TEST_CASE(parserTest(parser, env, "(hmfind)",        Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmfind ht)",     Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmfind ht 1 2)", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmfind 1 2)",    Value::Null, false));
}

// -------------------------------------------------------------
void UnitTest::testParserHashMapCount() {
    auto env = Environment::make();
    Parser parser;

    Hashtable ht;
    ht.set(Value('a'), Value(1ll));
    ht.set(Value('b'), Value(2ll));
    ht.set(Value('c'), Value(3ll));
    ht.set(Value('2'), Value(2ll));
    env->def("ht", Value(ht));

    TEST_CASE(parserTest(parser, env, "(hmcount ht 1)", Value(1ll),  true));
    TEST_CASE(parserTest(parser, env, "(hmcount ht 2)", Value(2ll),  true));
    TEST_CASE(parserTest(parser, env, "(hmcount ht 3)", Value(1ll),  true));
    TEST_CASE(parserTest(parser, env, "(hmcount ht 4)", Value::Zero, true));

    TEST_CASE(parserTest(parser, env, "(hmcount)",        Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmcount ht)",     Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmcount ht 1 2)", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmcount 1 2)",    Value::Null, false));
}

// -------------------------------------------------------------
void UnitTest::testParserHashMapKeys() {
    auto env = Environment::make();
    Parser parser;

    Hashtable ht;
    ht.set(Value('a'), Value(1ll));
    ht.set(Value('b'), Value(2ll));
    ht.set(Value('c'), Value(3ll));
    env->def("ht", Value(ht));

    TEST_CASE(parserTest(parser, env, "(hmkeys (hashmap))",   Value(Sequence()), true));
    TEST_CASE(parserTest(parser, env, "(arrlen (hmkeys ht))", Value(3ll),        true));

    TEST_CASE(parserTest(parser, env, "(hmkeys)",         Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmkeys ht 'a')",  Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmkeys 'a' 'b')", Value::Null, false));
}

// -------------------------------------------------------------
void UnitTest::testParserHashMapVals() {
    auto env = Environment::make();
    Parser parser;

    Hashtable ht;
    ht.set(Value('a'), Value(1ll));
    ht.set(Value('b'), Value(2ll));
    ht.set(Value('c'), Value(3ll));
    ht.set(Value('2'), Value(2ll));
    env->def("ht", Value(ht));

    TEST_CASE(parserTest(parser, env, "(hmvals (hashmap))",   Value(Sequence()), true));
    TEST_CASE(parserTest(parser, env, "(arrlen (hmvals ht))", Value(4ll),        true));

    TEST_CASE(parserTest(parser, env, "(hmvals)",         Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmvals ht 'a')",  Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmvals 'a' 'b')", Value::Null, false));
}

// -------------------------------------------------------------
void UnitTest::testParserHashMapItems() {
    auto env = Environment::make();
    Parser parser;

    Hashtable ht;
    ht.set(Value('a'), Value(1ll));
    ht.set(Value('b'), Value(2ll));
    ht.set(Value('c'), Value(3ll));
    ht.set(Value('2'), Value(2ll));
    env->def("ht", Value(ht));

    TEST_CASE(parserTest(parser, env, "(hmitems (hashmap))",   Value(Sequence()), true));
    TEST_CASE(parserTest(parser, env, "(arrlen (hmitems ht))", Value(4ll),        true));

    TEST_CASE(parserTest(parser, env, "(hmitems)",         Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmitems ht 'a')",  Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(hmitems 'a' 'b')", Value::Null, false));
}

// -------------------------------------------------------------
void UnitTest::testParserPairOperations() {
    auto env = Environment::make();
    Parser parser;

    env->def("p", Value(Value::Pair(Value('a'), Value('b'))));

    TEST_CASE(parserTest(parser, env, "(pair 'a' 'b')", Value(Value::Pair(Value('a'), Value('b'))), true));

    TEST_CASE(parserTest(parser, env, "(first p)",  Value('a'), true));
    TEST_CASE(parserTest(parser, env, "(second p)", Value('b'), true));

    TEST_CASE(parserTest(parser, env, "(pair)",             Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(pair 'a')",         Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(pair 'a' 'b' 'c')", Value::Null, false));

    TEST_CASE(parserTest(parser, env, "(first)",         Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(first p 'a')",   Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(second)",        Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(second p 'a')",  Value::Null, false));
}

// -------------------------------------------------------------
void UnitTest::testParserMakeRange() {
    auto env = Environment::make();
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(range 10)",        Value(IntegerRange(10)),          true));
    TEST_CASE(parserTest(parser, env, "(range 1 10)",      Value(IntegerRange(1, 10)),       true));
    TEST_CASE(parserTest(parser, env, "(range 0 12 3)",    Value(IntegerRange(0, 12, 3)),    true));
    TEST_CASE(parserTest(parser, env, "(range -10 -1)",    Value(IntegerRange(-10, -1, 1)),  true));
    TEST_CASE(parserTest(parser, env, "(range -10 0 2)",   Value(IntegerRange(-10, 0, 2)),   true));
    TEST_CASE(parserTest(parser, env, "(range -1 -10 -1)", Value(IntegerRange(-1, -10, -1)), true));

    TEST_CASE(parserTest(parser, env, "(range)",          Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(range -10)",      Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(range -1 -10)",   Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(range 10 1)",     Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(range -1 -10 2)", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(range 10 1 2)",   Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(range 'a')",      Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(range 1 'a')",    Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(range 1 10 'a')", Value::Null, false));
}

// -------------------------------------------------------------
void UnitTest::testParserRangeGetters() {
    auto env = Environment::make();
    Parser parser;

    env->def("rng", Value(IntegerRange(1, 25, 2)));

    TEST_CASE(parserTest(parser, env, "(rngbegin rng)", Value(1ll),  true));
    TEST_CASE(parserTest(parser, env, "(rngend rng)",   Value(25ll), true));
    TEST_CASE(parserTest(parser, env, "(rngstep rng)",  Value(2ll),  true));
    TEST_CASE(parserTest(parser, env, "(rnglen rng)",   Value(12ll), true));

    TEST_CASE(parserTest(parser, env, "(rngbegin)",   Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(rngbegin 5)", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(rngend)",     Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(rngend 5)",   Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(rngstep)",    Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(rngstep 5)",  Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(rnglen)",     Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(rnglen 5)",   Value::Null, false));
}

// -------------------------------------------------------------
void UnitTest::testParserExpand() {
    auto env = Environment::make();
    Parser parser;

    env->def("rng1", Value(IntegerRange(1, 6, 2)));
    env->def("rng2", Value(IntegerRange(7, 9, 2)));

    const auto val1 = Value(Sequence(std::vector{Value(1ll), Value(3ll), Value(5ll)}));
    const auto val2 = Value(Sequence(std::vector{Value(1ll), Value(3ll), Value(5ll), Value(7ll)}));

    TEST_CASE(parserTest(parser, env, "(expand rng1)",      val1, true));
    TEST_CASE(parserTest(parser, env, "(expand rng1 rng2)", val2, true));

    TEST_CASE(parserTest(parser, env, "(expand)",        Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(expand 1)",      Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(expand rng1 1)", Value::Null, false));
}

// -------------------------------------------------------------
void UnitTest::testParserGenericLen() {
    auto env = Environment::make();
    Parser parser;

    TEST_CASE(parserTest(parser, env, "(len \"\")",                 Value(0ll), true));
    TEST_CASE(parserTest(parser, env, "(len \"text\")",             Value(4ll), true));
    TEST_CASE(parserTest(parser, env, "(len (array))",              Value(0ll), true));
    TEST_CASE(parserTest(parser, env, "(len (array 2 4 6))",        Value(3ll), true));
    TEST_CASE(parserTest(parser, env, "(len (hashmap))",            Value(0ll), true));
    TEST_CASE(parserTest(parser, env, "(len (hashmap (pair 2 4)))", Value(1ll), true));
    TEST_CASE(parserTest(parser, env, "(len (range 1 1))",          Value(0ll), true));
    TEST_CASE(parserTest(parser, env, "(len (range 5))",            Value(5ll), true));

    TEST_CASE(parserTest(parser, env, "(len)",   Value(3ll), false));
    TEST_CASE(parserTest(parser, env, "(len 5)", Value(3ll), false));
}

// -------------------------------------------------------------
void UnitTest::testParserGenericGet() {
    auto env = Environment::make();
    Parser parser;

    env->def("txt", Value("abc"));
    env->def("seq", Value(Sequence(std::vector{Value(1ll), Value(2ll), Value(3ll)})));

    Hashtable ht;
    ht.set(Value(1ll), Value(10ll));
    ht.set(Value(2ll), Value(20ll));
    env->def("ht", Value(ht));

    auto rawStruct = Struct("Person", Struct::MemberList{"name", "age"});
    env->def("inst", Value(Instance(rawStruct, Instance::InitArgs{{"name", Value("Jon")}, {"age", Value(25ll)}})));

    TEST_CASE(parserTest(parser, env, "(get txt 0)",   Value('a'),  true));
    TEST_CASE(parserTest(parser, env, "(get txt 1)",   Value('b'),  true));
    TEST_CASE(parserTest(parser, env, "(get txt 2)",   Value('c'),  true));

    TEST_CASE(parserTest(parser, env, "(get seq 0)",   Value(1ll),  true));
    TEST_CASE(parserTest(parser, env, "(get seq 1)",   Value(2ll),  true));
    TEST_CASE(parserTest(parser, env, "(get seq 2)",   Value(3ll),  true));

    TEST_CASE(parserTest(parser, env, "(get ht 1)",    Value(10ll), true));
    TEST_CASE(parserTest(parser, env, "(get ht 2)",    Value(20ll), true));
    TEST_CASE(parserTest(parser, env, "(get ht 3)",    Value::Null, true));
    TEST_CASE(parserTest(parser, env, "(get ht 3 30)", Value(30ll), true));

    TEST_CASE(parserTest(parser, env, "(get inst \"name\")", Value("Jon"), true));
    TEST_CASE(parserTest(parser, env, "(get inst \"age\")",  Value(25ll), true));
    TEST_CASE(parserTest(parser, env, "(get inst name)",     Value("Jon"), true));
    TEST_CASE(parserTest(parser, env, "(get inst age)",      Value(25ll), true));

    TEST_CASE(parserTest(parser, env, "(get txt 'a')",  Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(get txt 5)",    Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(get seq 'a')",  Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(get seq 5)",    Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(get inst 5)",   Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(get inst mem)", Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(get 5 0)",      Value::Null, false));
}
