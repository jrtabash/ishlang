#include <fstream>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {
    const std::string CodeBlock = "__CODE__";
    const std::string ExpectBlock = "__EXPECT__";

    std::string testFilename;
    std::ifstream testFile;
    bool seenCodeBlock = false;
    bool seenExpectBlock = false;
}

void parseArgs(int argc, char ** argv) {
    if (argc != 2) {
        std::cerr << "Usage\n"
                  << "\t" << argv[0] << " <test_scenario_file>\n"
                  << std::endl;
        throw std::runtime_error("Missing test filename");
    }

    testFilename = argv[1];
}

void openTestFile() {
    testFile.open(testFilename.c_str(), std::ios_base::in);
    if (!testFile.is_open()) {
        throw std::runtime_error("Unable to open test file '" + testFilename + "'");
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
            throw std::runtime_error("Multiple " + block + " lines");
        }
        seenBlock = true;
        return true;
    }
    return false;
}

void checkSeenBlocks() {
    if (!seenCodeBlock) {
        throw std::runtime_error("Missing " + CodeBlock);
    }

    if (!seenExpectBlock) {
        throw std::runtime_error("Missing " + ExpectBlock);
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
        throw std::runtime_error("Unable to open file '" + outFilename + "' for writing");
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
        checkSeenBlocks();
        closeTestFile();
    }
    catch (const std::exception & ex) {
        closeTestFile();
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
