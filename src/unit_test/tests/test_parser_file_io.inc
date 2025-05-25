#include "unit_test_function.h"

#include "environment.h"
#include "parser.h"
#include "util.h"
#include "value.h"

#include <fstream>
#include <string>

using namespace Ishlang;

// -------------------------------------------------------------
DEFINE_TEST(testParserFileIO_OpenIntrospectAndClose) {
    Util::TemporaryFile tempFile("testParserFileIO_OpenIntrospectAndClose.txt");

    auto env = Environment::make();
    Parser parser;

    const auto fopen = std::string("(progn ")
        + "(var f (fopen \"" + tempFile.path().c_str() + "\" 'r'))"
        + "(fisopen f))";
    const auto fclose = std::string("(fclose f)");
    const auto fisopen = std::string("(fisopen f)");
    const auto fname = std::string("(fname f)");
    const auto fmode = std::string("(fmode f)");

    TEST_CASE(parserTest(parser, env, fopen,   Value::True,            true));
    TEST_CASE(parserTest(parser, env, fisopen, Value::True,            true));
    TEST_CASE(parserTest(parser, env, fname,   Value(tempFile.path()), true));
    TEST_CASE(parserTest(parser, env, fmode,   Value('r'),             true));
    TEST_CASE(parserTest(parser, env, fclose,  Value::True,            true));
    TEST_CASE(parserTest(parser, env, fisopen, Value::False,           true));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserFileIO_OpenReadAndClose) {
    Util::TemporaryFile tempFile("testParserFileIO_OpenReadAndClose.txt",
                                 "12\n"
                                 "345\n"
                                 "6789\n");

    auto env = Environment::make();
    Parser parser;

    const auto fopen = std::string("(progn ")
        + "(var f (fopen \"" + tempFile.path().c_str() + "\" 'r'))"
        + "(fisopen f))";
    const auto fclose = std::string("(fclose f)");
    const auto fread = std::string("(fread f)");
    const auto freadln = std::string("(freadln f)");

    TEST_CASE(parserTest(parser, env, fopen,   Value::True,   true));
    TEST_CASE(parserTest(parser, env, fread,   Value('1'),    true));
    TEST_CASE(parserTest(parser, env, fread,   Value('2'),    true));
    TEST_CASE(parserTest(parser, env, fread,   Value('\n'),   true));
    TEST_CASE(parserTest(parser, env, freadln, Value("345"),  true));
    TEST_CASE(parserTest(parser, env, freadln, Value("6789"), true));
    TEST_CASE(parserTest(parser, env, freadln, Value::Null,   true));
    TEST_CASE(parserTest(parser, env, fclose,  Value::True,   true));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserFileIO_OpenWriteAndClose) {
    Util::TemporaryFile tempFile("testParserFileIO_OpenWriteAndClose.txt");

    auto env = Environment::make();
    Parser parser;

    const auto fopen = std::string("(progn ")
        + "(var f (fopen \"" + tempFile.path().c_str() + "\" 'w'))"
        + "(fisopen f))";
    const auto fclose = std::string("(fclose f)");
    const auto fflush = std::string("(fflush f)");

    const auto fwrite = [&](const char *cOrS) { return std::string("(fwrite f ") + cOrS + ')'; };
    const auto fwriteln = [&](const char *cOrS) { return std::string("(fwriteln f ") + cOrS + ')'; };

    TEST_CASE(parserTest(parser, env, fopen,               Value::True, true));
    TEST_CASE(parserTest(parser, env, fwrite("'a'"),       Value::True, true));
    TEST_CASE(parserTest(parser, env, fwrite("'b'"),       Value::True, true));
    TEST_CASE(parserTest(parser, env, fwrite("'\n'"),      Value::True, true));
    TEST_CASE(parserTest(parser, env, fwriteln("\"cde\""), Value::True, true));
    TEST_CASE(parserTest(parser, env, fwriteln("\"fg\""),  Value::True, true));
    TEST_CASE(parserTest(parser, env, fwriteln("\"hij\""), Value::True, true));
    TEST_CASE(parserTest(parser, env, fwriteln("'k'"),     Value::True, true));
    TEST_CASE(parserTest(parser, env, fflush,              Value::True, true));
    TEST_CASE(parserTest(parser, env, fclose,              Value::True, true));

    {
        std::ifstream ifs(tempFile.path().c_str());
        std::string line;
        std::getline(ifs, line); TEST_CASE_MSG(ifs && line == "ab",  "actual=" << line);
        std::getline(ifs, line); TEST_CASE_MSG(ifs && line == "cde", "actual=" << line);
        std::getline(ifs, line); TEST_CASE_MSG(ifs && line == "fg",  "actual=" << line);
        std::getline(ifs, line); TEST_CASE_MSG(ifs && line == "hij", "actual=" << line);
        std::getline(ifs, line); TEST_CASE_MSG(ifs && line == "k",   "actual=" << line);
        std::getline(ifs, line); TEST_CASE(!ifs);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testParserFileIO_Errors) {
    Util::TemporaryFile tempFile("testParserFileIO_Errors.txt");

    auto env = Environment::make();
    Parser parser;

    const auto fopen = [&](const std::string &filename, char mode) {
        return std::string("(progn ")
            + "(var f (fopen \"" + filename + "\" '" + mode + "')) "
            + "(fisopen f))";
    };
    const auto fclose = std::string("(fclose f)");

    const auto fwrite = [&](const char *cOrS) { return std::string("(fwrite f ") + cOrS + ')'; };
    const auto fwriteln = [&](const char *s) { return std::string("(fwriteln f ") + s + ')'; };

    TEST_CASE(parserTest(parser, env, fopen("noSuchFileExists.stuff", 'r'), Value::Null, false));
    TEST_CASE(parserTest(parser, env, fopen(tempFile.path(), 'W'),          Value::Null, false));

    TEST_CASE(parserTest(parser, env, fopen(tempFile.path(), 'r'), Value::True, true));
    TEST_CASE(parserTest(parser, env, fclose,                      Value::True, true));

    TEST_CASE(parserTest(parser, env, fwrite("'a'"),       Value::Null, false));
    TEST_CASE(parserTest(parser, env, fwrite("\"abc\""),   Value::Null, false));
    TEST_CASE(parserTest(parser, env, fwriteln("\"abc\""), Value::Null, false));
}

// -------------------------------------------------------------
DEFINE_TEST(testParserFileIO_ExistsAndRemove) {
    Util::TemporaryFile tempFile("testParserFileIO_ExistsAndRemove.txt");

    auto env = Environment::make();
    Parser parser;

    const auto fexists = [](const std::string &filename) { return std::string("(fexists \"") + filename + "\")"; };
    const auto fremove = [](const std::string &filename) { return std::string("(fremove \"") + filename + "\")"; };

    TEST_CASE(parserTest(parser, env, fexists("doesNotExist.stuff"), Value::False, true));
    TEST_CASE(parserTest(parser, env, fremove("doesNotExist.stuff"), Value::False, true));
    TEST_CASE(parserTest(parser, env, fexists(tempFile.path()),      Value::True,  true));
    TEST_CASE(parserTest(parser, env, fremove(tempFile.path()),      Value::True,  true));
    TEST_CASE(parserTest(parser, env, fexists(tempFile.path()),      Value::False, true));
    TEST_CASE(parserTest(parser, env, fremove(tempFile.path()),      Value::False, true));
}
