#ifndef UNIT_TEST_H
#define UNIT_TEST_H

#include "code_node.h"
#include "environment.h"
#include "hashtable.h"
#include "instance.h"
#include "lambda.h"
#include "lexer.h"
#include "parser.h"
#include "sequence.h"
#include "struct.h"
#include "value.h"
#include "value_pair.h"
#include "module.h"
#include "util.h"

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

        void setVerbose(bool flag);
        
    private:
        void runTest(const std::string &name, Function ftn);
        const std::string & defaultModuleCode();
        Util::TemporaryFile createTempModuleFile(const std::string &name, const std::string &code = "");

    private:
        void testToken();
        void testUtilSplit();
        void testUtilPrintContainer();
        void testComment();
        void testTokenType();
        void testValue();
        void testValueAsType();
        void testValuePair();
        void testEnvironment();
        void testEnvironmentForeach();
        void testLambda();
        void testStruct();
        void testStructValue();
        void testInstance();
        void testInstanceValue();
        void testSequence();
        void testSequenceFind();
        void testSequenceCount();
        void testSequenceValue();
        void testSequencePrint();
        void testSequenceGenerate();
        void testHashtable();
        void testHashtableFind();
        void testHashtableCount();
        void testHashtableKeysValues();
        void testModule();
        void testModuleStorage();
        void testCodeNodeBasic();
        void testCodeNodeClone();
        void testCodeNodeIsType();
        void testCodeNodeTypeName();
        void testCodeNodeAsType();
        void testCodeNodeArithOp();
        void testCodeNodeCompOp();
        void testCodeNodeLogicOp();
        void testCodeNodeNot();
        void testCodeNodeNegativeOf();
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
        void testCodeNodeMakeInstanceWithInitArgs();
        void testCodeNodeIsInstanceOf();
        void testCodeNodeStructName();
        void testCodeNodeGetSetMember();
        void testCodeNodeStringLen();
        void testCodeNodeStringGet();
        void testCodeNodeStringSet();
        void testCodeNodeStringCat();
        void testCodeNodeSubString();
        void testCodeNodeStringFind();
        void testCodeNodeStringCount();
        void testCodeNodeStringCompare();
        void testCodeNodeStringSort();
        void testCodeNodeMakeArray();
        void testCodeNodeMakeArraySV();
        void testCodeNodeArrayLen();
        void testCodeNodeArrayGet();
        void testCodeNodeArraySet();
        void testCodeNodeArrayAdd();
        void testCodeNodeArrayFind();
        void testCodeNodeArrayCount();
        void testCodeNodeStrCharCheck();
        void testCodeNodeStrCharTransform();
        void testCodeNodeImportModule();
        void testCodeNodeFromModuleImport();
        void testCodeNodeRandom();
        void testCodeNodeHash();
        void testCodeNodeMakeHashMap();
        void testCodeNodeHashMapLen();
        void testCodeNodeHashMapContains();
        void testCodeNodeHashMapGet();
        void testCodeNodeHashMapSet();
        void testCodeNodeHashMapRemove();
        void testCodeNodeHashMapClear();
        void testCodeNodeHashMapFind();
        void testCodeNodeHashMapCount();
        void testCodeNodeHashMapKeys();
        void testCodeNodeHashMapValues();
        void testCodeNodeHashMapItems();
        void testCodeNodePairOperations();
        void testParserBasic();
        void testParserClone();
        void testParserIsType();
        void testParserTypeName();
        void testParserAsType();
        void testParserVar();
        void testParserArith();
        void testParserComp();
        void testParserLogic();
        void testParserNot();
        void testParserNegativeOf();
        void testParserBlock();
        void testParserCond();
        void testParserLoop();
        void testParserLambda();
        void testParserFtn();
        void testParserStruct();
        void testParserIsStructName();
        void testParserMakeInstance();
        void testParserMakeInstanceWithInitArgs();
        void testParserIsInstanceOf();
        void testParserStructName();
        void testParserGetSetMember();
        void testParserStringLen();
        void testParserStrGet();
        void testParserStrSrt();
        void testParserStringCat();
        void testParserSubString();
        void testParserStringFind();
        void testParserStringCount();
        void testParserStringCompare();
        void testParserStringSort();
        void testParserMakeArray();
        void testParserMakeArraySV();
        void testParserArrayLen();
        void testParserArrayGet();
        void testParserArraySet();
        void testParserArrayAdd();
        void testParserArrayFind();
        void testParserArrayCount();
        void testParserStrCharCheck();
        void testParserStrCharTransform();
        void testParserImportModule();
        void testParserFromModuleImport();
        void testParserRandom();
        void testParserHash();
        void testParserMakeHashMap();
        void testParserHashMapLen();
        void testParserHashMapContains();
        void testParserHashMapGet();
        void testParserHashMapSet();
        void testParserHashMapRemove();
        void testParserHashMapClear();
        void testParserHashMapFind();
        void testParserHashMapCount();
        void testParserHashMapKeys();
        void testParserHashMapVals();
        void testParserHashMapItems();
        void testParserPairOperations();
        
    private:
        bool  success_;
        bool  verbose_;
        Tests tests_;
    };

}

#endif	// UNIT_TEST_H
