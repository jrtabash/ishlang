#ifndef UNIT_TEST_H
#define	UNIT_TEST_H

#include "value.h"
#include "environment.h"
#include "lambda.h"
#include "struct.h"
#include "instance.h"
#include "byte_code.h"
#include "parser.h"
#include "lexer.h"

#include <string>
#include <map>
#include <functional>

namespace Int {

    class UnitTest {
    public:
        typedef std::function<void ()>          Function;
        typedef std::map<std::string, Function> Tests;
        
    public:
        UnitTest();
        
        bool run(const std::string &test="");
        
        void list() const;

        void setVerbose(bool flag) { verbose_ = flag; }
        
    private:
        void runTest(const std::string &name, Function ftn);
        
    private:
        void testToken();
        void testTokenType();
        void testValue();
        void testEnvironment();
        void testLambda();
        void testStruct();
        void testStructValue();
        void testInstance();
        void testInstanceValue();
        void testByteCodeBasic();
        void testByteCodeClone();
        void testByteCodeIsType();
        void testByteCodeArithOp();
        void testByteCodeCompOp();
        void testByteCodeLogicOp();
        void testByteCodeSequence();
        void testByteCodeCond();
        void testByteCodeLoop();
        void testByteCodeLambdaExpr();
        void testByteCodeLambdaApp();
        void testByteCodeFunctionExpr();
        void testByteCodeFunctionApp();
        void testByteCodeStruct();
        void testByteCodeIsStructName();
        void testByteCodeMakeInstance();
        void testByteCodeIsInstanceOf();
        void testByteCodeGetSetMember();
        void testByteCodeStringLen();
        void testByteCodeCharAt();
        void testParserBasic();
        void testParserIsType();
        void testParserVar();
        void testParserArith();
        void testParserComp();
        void testParserLogic();
        void testParserBlock();
        void testParserCond();
        void testParserLoop();
        void testParserLambda();
        void testParserFtn();
        void testParserStruct();
        void testParserIsStructName();
        void testParserMakeInstance();
        void testParserIsInstanceOf();
        void testParserStringLen();
        void testParserCharAt();
        
    private:
        bool  success_;
        bool  verbose_;
        Tests tests_;
    };

}

#endif	// UNIT_TEST_H

