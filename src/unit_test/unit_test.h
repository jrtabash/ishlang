#ifndef ISHLANG_UNIT_TEST_H
#define ISHLANG_UNIT_TEST_H

#include "environment.h"
#include "parser.h"
#include "sequence.h"
#include "util.h"
#include "value.h"

#include <functional>
#include <map>
#include <string>

class UnitTest {
public:
    using Function = std::function<void ()>;
    using Tests    = std::map<std::string, Function>;
        
public:
    UnitTest();
        
    bool run(const std::string &test="");
        
    void list() const;

    void setVerbose(bool flag);

public: // Test function interface
    void addTest(const char *name, Function ftn);
    void setFailed();
        
    const std::string & defaultModuleCode();
    Ishlang::Util::TemporaryFile createTempModuleFile(const std::string &name, const std::string &code = "");

    bool parserTest(Ishlang::Parser &parser,
                    Ishlang::Environment::SharedPtr env,
                    const std::string &expr,
                    const Ishlang::Value &value,
                    bool success);

private:
    void runTest(const std::string &name, Function ftn);

private:
    bool  success_;
    bool  verbose_;
    Tests tests_;
};

#endif	// ISHLANG_UNIT_TEST_H
