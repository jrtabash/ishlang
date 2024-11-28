#include "unit_test.h"
#include "module.h"

#include <iostream>

using namespace Ishlang;

// -------------------------------------------------------------
UnitTest::UnitTest()
    : success_(true)
    , verbose_(false)
    , tests_()
{
    ModuleStorage::addPath(Util::temporaryPath());
}

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
void UnitTest::addTest(const char *name, Function ftn) {
    const auto [_, success] = tests_.emplace(name, ftn);
    if (!success) {
        throw std::runtime_error(std::string("Failed to add duplicate test '") + name + "'");
    }
}

// -------------------------------------------------------------
void UnitTest::setFailed() {
    success_ = false;
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
bool UnitTest::parserTest(Parser &parser,
                          Environment::SharedPtr env,
                          const std::string &expr,
                          const Value &value,
                          bool success) {
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
void UnitTest::runTest(const std::string &name, Function ftn) {
    if (verbose_) { std::cout << "Testing: " << name << '\n'; }
    ftn();
}
