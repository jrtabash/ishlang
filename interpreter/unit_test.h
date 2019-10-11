#ifndef UNIT_TEST_H
#define	UNIT_TEST_H

#include "value.h"
#include "environment.h"
#include "lambda.h"
#include "struct.h"
#include "instance.h"
#include "sequence.h"
#include "byte_code.h"
#include "parser.h"
#include "lexer.h"

#include <string>
#include <map>
#include <functional>

namespace Int {

    class UnitTest {
    public:
        using Function = std::function<void ()>;
        using Tests    = std::map<std::string, Function>;
        
    public:
        UnitTest();
        
        bool run(const std::string &test="");
        
        void list() const;

        void setVerbose(bool flag) { verbose_ = flag; }
        
    private:
        void runTest(const std::string &name, Function ftn);
        
    private:
        void testToken();
        void testComment();
        void testTokenType();
        void testValue();
        void testEnvironment();
        void testLambda();
        void testStruct();
        void testStructValue();
        void testInstance();
        void testInstanceValue();
        void testSequence();
        void testSequenceValue();
        void testSequencePrint();
        void testByteCodeBasic();
        void testByteCodeClone();
        void testByteCodeIsType();
        void testByteCodeArithOp();
        void testByteCodeCompOp();
        void testByteCodeLogicOp();
        void testByteCodeNot();
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
        void testByteCodeSetCharAt();
        void testByteCodeStringCat();
        void testByteCodeSubString();
        void testByteCodeStringFind();
        void testByteCodeMakeArray();
        void testByteCodeArrayLen();
        void testByteCodeArrayGet();
        void testByteCodeArraySet();
        void testByteCodeArrayAdd();
        void testParserBasic();
        void testParserIsType();
        void testParserVar();
        void testParserArith();
        void testParserComp();
        void testParserLogic();
        void testParserNot();
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
        void testParserGetChar();
        void testParserSetChar();
        void testParserStringCat();
        void testParserSubString();
        void testParserStringFind();
        void testParserMakeArray();
        void testParserArrayLen();
        void testParserArrayGet();
        void testParserArraySet();
        void testParserArrayAdd();
        
    private:
        bool  success_;
        bool  verbose_;
        Tests tests_;
    };

}

#endif	// UNIT_TEST_H

