#include <iostream>
#include <fstream>
#include <string>
#include <functional>
#include <stdexcept>

namespace {
    const std::string CodeBlock = "__CODE__";
    const std::string ExpectBlock = "__EXPECT__";

    std::string testFilename;
    std::ifstream testFile;
    bool seenCodeBlock = false;
    bool seenExpectBlock = false;
}

void makeAndThrowException(const std::string & first,
                           const std::string & second = "",
                           const std::string & third = "") {
    throw std::runtime_error(first + second + third);
}

void parseArgs(int argc, char ** argv) {
    if (argc != 2) {
        std::cerr << "Usage\n"
                  << "\t" << argv[0] << " <test_scenario_file>\n"
                  << std::endl;
        makeAndThrowException("Missing test filename");
    }

    testFilename = argv[1];
}

void openTestFile() {
    testFile.open(testFilename.c_str(), std::ios_base::in);
    if (!testFile.is_open()) {
        makeAndThrowException("Unable to open test file '", testFilename, "'");
    }
}

void closeTestFile() {
    if (testFile.is_open()) {
        testFile.close();
    }
}

bool checkBlock(const std::string & line, const std::string & block, bool & seenBlock) {
    if (line == block) {
        if (seenBlock) {
            makeAndThrowException("Multiple ", block, " lines");
        }
        seenBlock = true;
        return true;
    }
    return false;
}

void checkSeenBocks() {
    if (!seenCodeBlock) {
        makeAndThrowException("Missing ", CodeBlock);
    }

    if (!seenExpectBlock) {
        makeAndThrowException("Missing ", ExpectBlock);
    }
}

void readAndCopyBlock(std::ostream & outFile) {
    std::string line;
    while (std::getline(testFile, line)) {
        if (checkBlock(line, CodeBlock, seenCodeBlock)) {
            continue;
        }

        if (checkBlock(line, ExpectBlock, seenExpectBlock)) {
            break;
        }

        if (!line.empty()) {
            outFile << line << '\n';
        }
    }
}

void readAndCopyBlock(const std::string & outFilename) {
    std::ofstream outFile(outFilename, std::ios_base::out);
    if (!outFile.is_open()) {
        makeAndThrowException("Unable to open file '", outFilename, "' for writing");
    }

    try {
        readAndCopyBlock(outFile);
        outFile.close();
    }
    catch (...) {
        if (outFile.is_open()) {
            outFile.close();
        }
        throw;
    }
}

int main(int argc, char ** argv) {
    try {
        parseArgs(argc, argv);
        openTestFile();
        readAndCopyBlock(testFilename + ".code");
        readAndCopyBlock(testFilename + ".expect");
        checkSeenBocks();
        closeTestFile();
    }
    catch (const std::exception & ex) {
        closeTestFile();
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
