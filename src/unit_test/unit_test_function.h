#ifndef ISHLANG_UNIT_TEST_FUNCTION_H
#define ISHLANG_UNIT_TEST_FUNCTION_H

#include "unit_test.h"
#include "value.h"

#include <iostream>
#include <source_location>

namespace UnitTestFtn {
    UnitTest unitTest;

    struct TestFtnBase {
        TestFtnBase(UnitTest & unitTest) : unitTest_(unitTest) {}

        inline UnitTest & unitTest() { return unitTest_; }

        inline void setFailed() { unitTest_.setFailed(); }

        inline bool parserTest(Ishlang::Parser &parser,
                               Ishlang::Environment::SharedPtr env,
                               const std::string &expr,
                               const Ishlang::Value &value,
                               bool success) {
            return unitTest_.parserTest(parser, env, expr, value, success);
        }

        static inline Ishlang::Value arrval(const auto &... vs) {
            return Ishlang::Value(Ishlang::Sequence({vs...}));
        }

        static inline Ishlang::Value arrval() {
            return Ishlang::Value(Ishlang::Sequence());
        }

    private:
        UnitTest & unitTest_;
    };
}

#define DEFINE_TEST(TESTNAME)                                           \
    namespace UnitTestFtn {                                             \
        struct TESTNAME##Struct : public TestFtnBase {                  \
            TESTNAME##Struct() : TestFtnBase(UnitTestFtn::unitTest) {   \
                unitTest().addTest(#TESTNAME, *this);                   \
            }                                                           \
                                                                        \
            void operator()();                                          \
        };                                                              \
        TESTNAME##Struct TESTNAME##Instance;                            \
    }                                                                   \
    void UnitTestFtn::TESTNAME##Struct::operator()()

#define PRINT_ERROR() std::cerr << std::source_location::current().file_name() << ':' << std::source_location::current().line() << ' '

#define TEST_CASE(EXPR) if (!(EXPR)) { PRINT_ERROR() << #EXPR << '\n'; setFailed(); }
#define TEST_CASE_MSG(EXPR, MSG) if (!(EXPR)) { PRINT_ERROR() << #EXPR << ": " << MSG << '\n'; setFailed(); }

#endif	// ISHLANG_UNIT_TEST_FUNCTION_H
