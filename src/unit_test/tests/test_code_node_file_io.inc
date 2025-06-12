#include "unit_test_function.h"

#include "code_node.h"
#include "environment.h"
#include "exception.h"
#include "file_io.h"
#include "util.h"
#include "value.h"

#include <iostream>
#include <string>

using namespace Ishlang;

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeFileIO_OpenIntrospectAndClose) {
    Util::TemporaryFile tempFile("testCodeNodeFileIO_OpenIntrospectAndClose.txt");

    auto env = Environment::make();

    const auto fileVal = CodeNode::make<Define>(
        "f",
        CodeNode::make<FileOpen>(
            CodeNode::make<Literal>(Value(tempFile.path())),
            CodeNode::make<Literal>(Value('r'))))->eval(env);
    TEST_CASE(fileVal.isFile());
    TEST_CASE(fileVal.file().isOpen());

    const auto isOpenVal = CodeNode::make<FileIsOpen>(
        CodeNode::make<Variable>("f"))->eval(env);
    TEST_CASE(isOpenVal.isBool());
    TEST_CASE(isOpenVal.boolean());

    const auto fnameVal = CodeNode::make<FileFName>(
        CodeNode::make<Variable>("f"))->eval(env);
    TEST_CASE(fnameVal.isString());
    TEST_CASE(fnameVal.text() == tempFile.path());

    const auto fmodeVal = CodeNode::make<FileFMode>(
        CodeNode::make<Variable>("f"))->eval(env);
    TEST_CASE(fmodeVal.isChar());
    TEST_CASE(fmodeVal.character() == 'r');

    const auto closeVal = CodeNode::make<FileClose>(
        CodeNode::make<Variable>("f"))->eval(env);
    TEST_CASE(closeVal.isBool());
    TEST_CASE(closeVal.boolean());
    TEST_CASE(!fileVal.file().isOpen());
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeFileIO_OpenReadClose) {
    Util::TemporaryFile tempFile("testCodeNodeFileIO_OpenReadClose.txt",
                                 "12\n"
                                 "345\n"
                                 "6789\n");

    auto env = Environment::make();

    const auto fileVal = CodeNode::make<Define>(
        "f",
        CodeNode::make<FileOpen>(
            CodeNode::make<Literal>(Value(tempFile.path())),
            CodeNode::make<Literal>(Value('r'))))->eval(env);
    TEST_CASE(fileVal.isFile());
    TEST_CASE(fileVal.file().isOpen());

    const auto fread = CodeNode::make<FileRead>(CodeNode::make<Variable>("f"));
    for (const char c : {'1', '2', '\n'}) {
        auto readVal = fread->eval(env);
        TEST_CASE(readVal.isChar());
        TEST_CASE_MSG(readVal.character() == c, "actual=" << readVal.character() << " expected=" << c);
    }

    const auto freadln = CodeNode::make<FileReadLn>(CodeNode::make<Variable>("f"));
    for (const char *s : {"345", "6789"}) {
        auto readVal = freadln->eval(env);
        TEST_CASE(readVal.isString());
        TEST_CASE_MSG(readVal.text() == s, "actual=" << readVal.text() << " expected=" << s);
    }

    const auto closeVal = CodeNode::make<FileClose>(
        CodeNode::make<Variable>("f"))->eval(env);
    TEST_CASE(closeVal.isBool());
    TEST_CASE(closeVal.boolean());
    TEST_CASE(!fileVal.file().isOpen());
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeFileIO_OpenWriteClose) {
    Util::TemporaryFile tempFile("testCodeNodeFileIO_OpenWriteClose.txt");

    auto env = Environment::make();

    const auto fileVal = CodeNode::make<Define>(
        "f",
        CodeNode::make<FileOpen>(
            CodeNode::make<Literal>(Value(tempFile.path())),
            CodeNode::make<Literal>(Value('w'))))->eval(env);
    TEST_CASE(fileVal.isFile());
    TEST_CASE(fileVal.file().isOpen());

    const auto fwrite = [&](const auto &cOrS) {
        return CodeNode::make<FileWrite>(
            CodeNode::make<Variable>("f"),
            CodeNode::make<Literal>(Value(cOrS)))->eval(env);
    };
    const auto fwriteln = [&](const auto &cOrS) {
        return CodeNode::make<FileWriteLn>(
            CodeNode::make<Variable>("f"),
            CodeNode::make<Literal>(Value(cOrS)))->eval(env);
    };

    Value writeVal;
    writeVal = fwrite('a');      TEST_CASE(writeVal.isBool() && writeVal.boolean());
    writeVal = fwrite('b');      TEST_CASE(writeVal.isBool() && writeVal.boolean());
    writeVal = fwrite('\n');     TEST_CASE(writeVal.isBool() && writeVal.boolean());
    writeVal = fwrite("cde\n");  TEST_CASE(writeVal.isBool() && writeVal.boolean());
    writeVal = fwriteln("fg");   TEST_CASE(writeVal.isBool() && writeVal.boolean());
    writeVal = fwriteln("hij");  TEST_CASE(writeVal.isBool() && writeVal.boolean());
    writeVal = fwriteln('k');    TEST_CASE(writeVal.isBool() && writeVal.boolean());

    const auto flushVal = CodeNode::make<FileFlush>(
        CodeNode::make<Variable>("f"))->eval(env);
    TEST_CASE(flushVal.isBool());
    TEST_CASE(flushVal.boolean());

    const auto closeVal = CodeNode::make<FileClose>(
        CodeNode::make<Variable>("f"))->eval(env);
    TEST_CASE(closeVal.isBool());
    TEST_CASE(closeVal.boolean());
    TEST_CASE(!fileVal.file().isOpen());

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
DEFINE_TEST(testCodeNodeFileIO_Errors) {
    Util::TemporaryFile tempFile("testCodeNodeFileIO_Errors.txt");

    auto env = Environment::make();

    const auto fopen = [&](const std::string &filename, char mode) {
        return CodeNode::make<Define>(
            "f",
            CodeNode::make<FileOpen>(
                CodeNode::make<Literal>(Value(filename)),
                CodeNode::make<Literal>(Value(mode))))->eval(env);
    };
    const auto fwrite = [&](const auto & cOrS) {
        return CodeNode::make<FileWrite>(
            CodeNode::make<Variable>("f"),
            CodeNode::make<Literal>(Value(cOrS)))->eval(env);
    };
    const auto fwriteln = [&](const std::string &s) {
        return CodeNode::make<FileWriteLn>(
            CodeNode::make<Variable>("f"),
            CodeNode::make<Literal>(Value(s)))->eval(env);
    };
    const auto testError = [&](int line, const auto & ftn, const std::string &err) {
        try {
            ftn();
            TEST_CASE(false);
        }
        catch (const FileIOError &ex) {
            TEST_CASE_MSG(err == ex.what(), "line=" << line << " actual=" << ex.what());
        }
        catch (const std::exception &ex) {
            TEST_CASE_MSG(false, "line=" << line << ' ' << ex.what());
        }
        catch (...) {
            TEST_CASE(false);
        }
    };

    testError(__LINE__,
              [&]() { fopen("noSuchFileExists.stuff", 'r'); },
              "FileIO Error noSuchFileExists.stuff - Failed to open file");
    testError(__LINE__,
              [&]() { fopen(tempFile.path(), 'W'); },
              "FileIO Error - Invalid mode 'W'");

    const auto fileVal = fopen(tempFile.path(), 'w');
    TEST_CASE(fileVal.isFile());
    TEST_CASE(fileVal.file().isOpen());

    const auto closeVal = CodeNode::make<FileClose>(CodeNode::make<Variable>("f"))->eval(env);
    TEST_CASE(closeVal.isBool());
    TEST_CASE(closeVal.boolean());
    TEST_CASE(!fileVal.file().isOpen());

    testError(__LINE__,
              [&]() { fwrite('a'); },
              "FileIO Error - Failed to write char");
    testError(__LINE__,
              [&]() { fwrite("abc"); },
              "FileIO Error - Failed to write string");
    testError(__LINE__,
              [&]() { fwriteln("abc"); },
              "FileIO Error - Failed to write line");
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeFileIO_ExistsAndRemove) {
    Util::TemporaryFile tempFile("testCodeNodeFileIO_ExistsAndRemove.txt");

    auto env = Environment::make();

    const auto fexists = [&](const std::string &path) {
        return CodeNode::make<FileExists>(CodeNode::make<Literal>(Value(path)))->eval(env);
    };
    const auto fremove = [&](const std::string &path) {
        return CodeNode::make<FileRemove>(CodeNode::make<Literal>(Value(path)))->eval(env);
    };

    Value value;

    value = fexists("doesNotExist.stuff");
    TEST_CASE(value.isBool());
    TEST_CASE(!value.boolean());

    value = fremove("doesNotExist.stuff");
    TEST_CASE(value.isBool());
    TEST_CASE(!value.boolean());

    value = fexists(tempFile.path());
    TEST_CASE(value.isBool());
    TEST_CASE(value.boolean());

    value = fremove(tempFile.path());
    TEST_CASE(value.isBool());
    TEST_CASE(value.boolean());

    value = fexists(tempFile.path());
    TEST_CASE(value.isBool());
    TEST_CASE(!value.boolean());

    value = fremove(tempFile.path());
    TEST_CASE(value.isBool());
    TEST_CASE(!value.boolean());
}

// -------------------------------------------------------------
DEFINE_TEST(testCodeNodeFileIO_WithFileRead) {
    Util::TemporaryFile tempFile("testCodeNodeFileIO_WithFileRead.txt",
                                 "12\n"
                                 "345\n"
                                 "6789\n");

    auto env = Environment::make();
    env->def("catText", Value(""));

    const auto fileVal = CodeNode::make<Define>(
        "file",
        CodeNode::make<FileOpen>(
            CodeNode::make<Literal>(Value(tempFile.path())),
            CodeNode::make<Literal>(Value('r'))))->eval(env);
    TEST_CASE(fileVal.isFile());
    TEST_CASE(fileVal.file().isOpen());

    const auto result = CodeNode::make<WithFile>(
        "f",
        CodeNode::make<Variable>("file"),
        CodeNode::make<Foreach>(
            "line",
            CodeNode::make<Variable>("f"),
            CodeNode::make<StringCat>(
                CodeNode::make<Variable>("catText"),
                CodeNode::make<Variable>("line"))))->eval(env);

    TEST_CASE(!fileVal.file().isOpen());

    TEST_CASE_MSG(result.isString(), "actual=" << result.typeToString());
    TEST_CASE_MSG(result.text() == "123456789", "actual=" << result.text());

    auto const &catText = env->get("catText");
    TEST_CASE_MSG(catText.isString(), "actual=" << catText.typeToString());
    TEST_CASE_MSG(catText.text() == "123456789", "actual=" << catText.text());
}
