#ifndef ISHLANG_UNIT_TEST_H
#define ISHLANG_UNIT_TEST_H

#include "code_node.h"
#include "environment.h"
#include "hashtable.h"
#include "instance.h"
#include "integer_range.h"
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
        inline Value arrval(const auto &... vs) { return Value(Sequence({vs...})); }
        inline Value arrval() { return Value(Sequence()); }

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
        void testSequenceSort();
        void testSequenceReverse();
        void testSequenceInsert();
        void testSequenceErase();
        void testSequenceClear();
        void testSequenceValue();
        void testSequencePrint();
        void testSequenceGenerate();
        void testHashtable();
        void testHashtableFind();
        void testHashtableCount();
        void testHashtableKeysValues();
        void testIntegerRange();
        void testIntegerRangeComparison();
        void testIntegerRangeStartNext();
        void testIntegerRangeGenerator();
        void testModule();
        void testModuleStorage();
        void testCodeNodeUtil();
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
        void testCodeNodeForeach();
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
        void testCodeNodeStringReverse();
        void testCodeNodeMakeArray();
        void testCodeNodeMakeArraySV();
        void testCodeNodeArrayLen();
        void testCodeNodeArrayGet();
        void testCodeNodeArraySet();
        void testCodeNodeArrayPush();
        void testCodeNodeArrayPop();
        void testCodeNodeArrayFind();
        void testCodeNodeArrayCount();
        void testCodeNodeArraySort();
        void testCodeNodeArrayReverse();
        void testCodeNodeArrayClear();
        void testCodeNodeArrayInsert();
        void testCodeNodeArrayRemove();
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
        void testCodeNodeMakeRange();
        void testCodeNodeRangeGetters();
        void testCodeNodeExpand();
        void testCodeNodeGenericLen();
        void testCodeNodeGenericGet();
        void testCodeNodeGenericSet();
        void testParserBasic();
        void testParserClone();
        void testParserIsType();
        void testParserTypeName();
        void testParserAsType();
        void testParserVar();
        void testParserArith();
        void testParserComp();
        void testParserLogicOp();
        void testParserNot();
        void testParserNegativeOf();
        void testParserBlock();
        void testParserCond();
        void testParserLoop();
        void testParserForeach();
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
        void testParserStringReverse();
        void testParserMakeArray();
        void testParserMakeArraySV();
        void testParserArrayLen();
        void testParserArrayGet();
        void testParserArraySet();
        void testParserArrayPush();
        void testParserArrayPop();
        void testParserArrayFind();
        void testParserArrayCount();
        void testParserArraySort();
        void testParserArrayReverse();
        void testParserArrayClear();
        void testParserArrayInsert();
        void testParserArrayRemove();
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
        void testParserMakeRange();
        void testParserRangeGetters();
        void testParserExpand();
        void testParserGenericLen();
        void testParserGenericGet();
        void testParserGenericSet();
        
    private:
        bool  success_;
        bool  verbose_;
        Tests tests_;
    };

}

#endif	// ISHLANG_UNIT_TEST_H
