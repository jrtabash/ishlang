#include "environment.h"
#include "parser.h"
#include "sequence.h"
#include "value.h"

#include <vector>

using namespace Ishlang;

// -------------------------------------------------------------
DEFINE_TEST(testParserStringLen) {
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
DEFINE_TEST(testParserStrGet) {
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
DEFINE_TEST(testParserStrSrt) {
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
DEFINE_TEST(testParserStringCat) {
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
DEFINE_TEST(testParserSubString) {
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
DEFINE_TEST(testParserStringFind) {
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
DEFINE_TEST(testParserStringCount) {
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
DEFINE_TEST(testParserStringCompare) {
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
DEFINE_TEST(testParserStringSort) {
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
DEFINE_TEST(testParserStringReverse) {
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
DEFINE_TEST(testParserStringSplit) {
    auto env = Environment::make();
    Parser parser;

    const auto vals = [](const std::vector<const char *> &vs) {
        auto seq = Value(Sequence());
        auto & arr = seq.array();
        for (const char * t : vs) { arr.push(Value(t)); }
        return seq;
    };

    TEST_CASE(parserTest(parser, env, "(strsplit \"\" ' ')",          vals({}),             true));
    TEST_CASE(parserTest(parser, env, "(strsplit \",\" ',')",         vals({"", ""}),       true));
    TEST_CASE(parserTest(parser, env, "(strsplit \"a\" ' ')",         vals({"a"}),          true));
    TEST_CASE(parserTest(parser, env, "(strsplit \"a b\" ' ')",       vals({"a", "b"}),     true));
    TEST_CASE(parserTest(parser, env, "(strsplit \"a,b,\" ',')",      vals({"a", "b", ""}), true));
    TEST_CASE(parserTest(parser, env, "(strsplit \",a,,b,,c,\" ',')", vals({"", "a", "", "b", "", "c", ""}), true));

    TEST_CASE(parserTest(parser, env, "(strsplit)",        Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(strsplit \"\")",   Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(strsplit 5 ' ')",  Value::Null, false));
    TEST_CASE(parserTest(parser, env, "(strsplit \"\" 5)", Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserStrCharCheck) {
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
DEFINE_TEST(testParserStrCharTransform) {
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
