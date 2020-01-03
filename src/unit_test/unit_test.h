#ifndef UNIT_TEST_H
#define	UNIT_TEST_H

#include "code_node.h"
#include "environment.h"
#include "instance.h"
#include "lambda.h"
#include "lexer.h"
#include "parser.h"
#include "sequence.h"
#include "struct.h"
#include "value.h"

#include <functional>
#include <map>
#include <string>

namespace Ishlang {

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
        void testValueAsType();
        void testEnvironment();
        void testLambda();
        void testStruct();
        void testStructValue();
        void testInstance();
        void testInstanceValue();
        void testSequence();
        void testSequenceValue();
        void testSequencePrint();
        void testCodeNodeBasic();
        void testCodeNodeClone();
        void testCodeNodeIsType();
        void testCodeNodeTypeName();
        void testCodeNodeAsType();
        void testCodeNodeArithOp();
        void testCodeNodeCompOp();
        void testCodeNodeLogicOp();
        void testCodeNodeNot();
        void testCodeNodeSequence();
        void testCodeNodeCond();
        void testCodeNodeLoop();
        void testCodeNodeLambdaExpr();
        void testCodeNodeLambdaApp();
        void testCodeNodeFunctionExpr();
        void testCodeNodeFunctionApp();
        void testCodeNodeStruct();
        void testCodeNodeIsStructName();
        void testCodeNodeMakeInstance();
        void testCodeNodeIsInstanceOf();
        void testCodeNodeStructName();
        void testCodeNodeGetSetMember();
        void testCodeNodeStringLen();
        void testCodeNodeCharAt();
        void testCodeNodeSetCharAt();
        void testCodeNodeStringCat();
        void testCodeNodeSubString();
        void testCodeNodeStringFind();
        void testCodeNodeMakeArray();
        void testCodeNodeMakeArraySV();
        void testCodeNodeArrayLen();
        void testCodeNodeArrayGet();
        void testCodeNodeArraySet();
        void testCodeNodeArrayAdd();
        void testParserBasic();
        void testParserIsType();
        void testParserTypeName();
        void testParserAsType();
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
        void testParserStructName();
        void testParserStringLen();
        void testParserGetChar();
        void testParserSetChar();
        void testParserStringCat();
        void testParserSubString();
        void testParserStringFind();
        void testParserMakeArray();
        void testParserMakeArraySV();
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
