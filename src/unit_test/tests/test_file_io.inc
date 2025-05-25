#include "unit_test_function.h"

#include "exception.h"
#include "file_io.h"
#include "util.h"

#include <iostream>
#include <string>

using namespace Ishlang;

// -------------------------------------------------------------
DEFINE_TEST(testFileIO_FileMode) {
    TEST_CASE(FileModeNS::toChar(FileMode::Read) == 'r');
    TEST_CASE(FileModeNS::toChar(FileMode::Write) == 'w');
    TEST_CASE(FileModeNS::toChar(FileMode::Append) == 'a');

    TEST_CASE(FileModeNS::fromChar('r') == FileMode::Read);
    TEST_CASE(FileModeNS::fromChar('w') == FileMode::Write);
    TEST_CASE(FileModeNS::fromChar('a') == FileMode::Append);

    TEST_CASE(FileModeNS::toOpenMode(FileMode::Read) == std::ios_base::in);
    TEST_CASE(FileModeNS::toOpenMode(FileMode::Write) == std::ios_base::out);
    TEST_CASE(FileModeNS::toOpenMode(FileMode::Append) == std::ios_base::app);
}

// -------------------------------------------------------------
DEFINE_TEST(testFileIO_DefaultFile) {
    auto file = FileStruct();
    TEST_CASE(!file.isOpen());
}

// -------------------------------------------------------------
DEFINE_TEST(testFileIO_FailedToOpenFile) {
    try {
        auto file = FileStruct("doesNotExist.stuff", FileMode::Read);
        TEST_CASE(false);
    }
    catch (const FileIOError &ex) {
        TEST_CASE_MSG(std::string("FileIO Error doesNotExist.stuff - Failed to open file") == ex.what(),
                      "actual='" << ex.what() << "'");
    }
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testFileIO_OpenThenClose) {
    Util::TemporaryFile tempFile("testFileIO_OpenThenClose.txt");

    try {
        auto file = FileStruct(tempFile.path(), FileMode::Read);
        TEST_CASE(file.isOpen());

        TEST_CASE(file.filename() == tempFile.path());
        TEST_CASE(file.mode() == FileMode::Read);

        file.close();
        TEST_CASE(!file.isOpen());
    }
    catch (const FileIOError &ex) {
        TEST_CASE_MSG(false, ex.what());
    }
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testFileIO_ReadEmptyFile) {
    Util::TemporaryFile tempFile("testFileIO_ReadEmptyFile.txt");

    try {
        auto file = FileStruct(tempFile.path(), FileMode::Read);
        TEST_CASE(file.isOpen());

        TEST_CASE(file.read() == std::nullopt);
        TEST_CASE(file.readln() == std::nullopt);
    }
    catch (const FileIOError &ex) {
        TEST_CASE_MSG(false, ex.what());
    }
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testFileIO_Read_NonEmptyFile) {
    Util::TemporaryFile tempFile("testFileIO_Read_NonEmptyFile.txt", "12\n");

    try {
        auto file = FileStruct(tempFile.path(), FileMode::Read);
        TEST_CASE(file.isOpen());

        std::optional<char> optC;
        optC = file.read(); TEST_CASE(optC && *optC == '1');
        optC = file.read(); TEST_CASE(optC && *optC == '2');
        optC = file.read(); TEST_CASE(optC && *optC == '\n');
        optC = file.read(); TEST_CASE(optC == std::nullopt);
    }
    catch (const FileIOError &ex) {
        TEST_CASE_MSG(false, ex.what());
    }
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testFileIO_Readln_NonEmptyFile) {
    Util::TemporaryFile tempFile("testFileIO_Readln_NonEmptyFile.txt",
                                 "12 34\n"
                                 "56 78\n"
                                 "90\n");

    try {
        auto file = FileStruct(tempFile.path(), FileMode::Read);
        TEST_CASE(file.isOpen());

        std::optional<std::string> optS;
        optS = file.readln(); TEST_CASE_MSG(optS && *optS == "12 34", "actual=" << (optS ? *optS : std::string("NULL")));
        optS = file.readln(); TEST_CASE_MSG(optS && *optS == "56 78", "actual=" << (optS ? *optS : std::string("NULL")));
        optS = file.readln(); TEST_CASE_MSG(optS && *optS == "90", "actual=" << (optS ? *optS : std::string("NULL")));
        optS = file.readln(); TEST_CASE_MSG(optS == std::nullopt, "actual=" << (optS ? *optS : std::string("NULL")));
    }
    catch (const FileIOError &ex) {
        TEST_CASE_MSG(false, ex.what());
    }
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testFileIO_Write) {
    Util::TemporaryFile tempFile("testFileIO_Write.txt");

    try {
        auto file = FileStruct(tempFile.path(), FileMode::Write);
        TEST_CASE(file.isOpen());

        file.write('a');
        file.write('b');
        file.write("cde");
        file.write('\n');

        file.close();
        TEST_CASE(!file.isOpen());

        {
            std::ifstream ifs(tempFile.path().c_str());
            std::string line;
            std::getline(ifs, line); TEST_CASE_MSG(ifs && line == "abcde", "actual=" << line);
            std::getline(ifs, line); TEST_CASE(!ifs);
        }
    }
    catch (const FileIOError &ex) {
        TEST_CASE_MSG(false, ex.what());
    }
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testFileIO_Writeln) {
    Util::TemporaryFile tempFile("testFileIO_Writeln.txt");

    try {
        auto file = FileStruct(tempFile.path(), FileMode::Write);
        TEST_CASE(file.isOpen());

        file.writeln("abc");
        file.writeln("def");
        file.writeln("g");
        file.writeln('h');

        file.close();
        TEST_CASE(!file.isOpen());

        {
            std::ifstream ifs(tempFile.path().c_str());
            std::string line;
            std::getline(ifs, line); TEST_CASE_MSG(ifs && line == "abc", "actual=" << line);
            std::getline(ifs, line); TEST_CASE_MSG(ifs && line == "def", "actual=" << line);
            std::getline(ifs, line); TEST_CASE_MSG(ifs && line == "g", "actual=" << line);
            std::getline(ifs, line); TEST_CASE_MSG(ifs && line == "h", "actual=" << line);
            std::getline(ifs, line); TEST_CASE(!ifs);
        }
    }
    catch (const FileIOError &ex) {
        TEST_CASE_MSG(false, ex.what());
    }
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testFileIO_Append) {
    Util::TemporaryFile tempFile("testFileIO_Append.txt");

    try {
        {
            auto file = FileStruct(tempFile.path(), FileMode::Write);
            TEST_CASE(file.isOpen());

            file.writeln("abc");
            file.writeln("def");
            file.writeln("g");

            file.close();
            TEST_CASE(!file.isOpen());
        }

        {
            std::ifstream ifs(tempFile.path().c_str());
            std::string line;
            std::getline(ifs, line); TEST_CASE_MSG(ifs && line == "abc", "actual=" << line);
            std::getline(ifs, line); TEST_CASE_MSG(ifs && line == "def", "actual=" << line);
            std::getline(ifs, line); TEST_CASE_MSG(ifs && line == "g", "actual=" << line);
            std::getline(ifs, line); TEST_CASE(!ifs);
        }

        {
            auto file = FileStruct(tempFile.path(), FileMode::Append);
            TEST_CASE(file.isOpen());

            file.write('h');
            file.writeln("i");
            file.writeln("jk");

            file.close();
            TEST_CASE(!file.isOpen());
        }

        {
            std::ifstream ifs(tempFile.path().c_str());
            std::string line;
            std::getline(ifs, line); TEST_CASE_MSG(ifs && line == "abc", "actual=" << line);
            std::getline(ifs, line); TEST_CASE_MSG(ifs && line == "def", "actual=" << line);
            std::getline(ifs, line); TEST_CASE_MSG(ifs && line == "g", "actual=" << line);
            std::getline(ifs, line); TEST_CASE_MSG(ifs && line == "hi", "actual=" << line);
            std::getline(ifs, line); TEST_CASE_MSG(ifs && line == "jk", "actual=" << line);
            std::getline(ifs, line); TEST_CASE(!ifs);
        }
    }
    catch (const FileIOError &ex) {
        TEST_CASE_MSG(false, ex.what());
    }
    catch (...) {
        TEST_CASE(false);
    }
}
