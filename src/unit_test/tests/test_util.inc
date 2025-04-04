#include "unit_test_function.h"

#include "exception.h"
#include "util.h"

#include <string>
#include <sstream>

using namespace Ishlang;

// -------------------------------------------------------------
DEFINE_TEST(testToken) {
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
DEFINE_TEST(testUtilSplit) {
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
DEFINE_TEST(testUtilPrintContainer) {
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
DEFINE_TEST(testComment) {
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
