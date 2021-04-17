#ifndef PARSER_H
#define PARSER_H

#include "code_node.h"
#include "lexer.h"

#include <forward_list>
#include <functional>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace Ishlang {

    class Parser {
    public:
        using CallBack = std::function<void (CodeNode::SharedPtr &code)>;

    public:
        Parser();

        CodeNode::SharedPtr read(const std::string &expr);
        CodeNode::SharedPtr readLiteral(const std::string &expr);
        void readMulti(const std::string &expr, CallBack callback);
        void readFile(const std::string &filename, CallBack callback);

        inline bool hasIncompleteExpr() const;
        inline void clearIncompleteExpr();

    private:
        CodeNode::SharedPtr readExpr();
        CodeNode::SharedPtr makeLiteral(Lexer::TokenType type, const std::string &text);
        CodeNode::SharedPtr readApp(const std::string &expected="");
        CodeNode::SharedPtrList readExprList();
        CodeNode::SharedPtrList readAndCheckExprList(const char *name, std::size_t expectedSize);
        CodeNode::SharedPtrList readAndCheckRangeExprList(const char *name, std::size_t minExpectedSize, std::size_t maxExpectedSize);
        CodeNode::SharedPtrPairs readExprPairs();
        CodeNode::NameSharedPtrs readNameExprPairs();
        std::string readName();
        CodeNode::NameAndAsList readNameAndAsList();
        std::vector<std::string> readParams();
        bool ignoreLeftP(bool allowRightP);
        void ignoreRightP();
        bool peekRightP() const;

    private:
        bool haveSExpression() const;
        void initAppFtns();

    private:
        template <typename ExprType, typename ExprOp>
        struct MakeBinaryExpression {
            inline MakeBinaryExpression(const std::string &name, Parser &parser, ExprOp exprOp);

            inline CodeNode::SharedPtr operator()();

        private:
            const std::string &name_;
            Parser &parser_;
            ExprOp exprOp_;
        };

    private:
        template <typename CodeNodeType>
        struct MakeStrCharOp {
            using OpType = typename CodeNodeType::Type;

            inline MakeStrCharOp(const std::string &name, Parser &parser, OpType opType);

            inline CodeNode::SharedPtr operator()();

        private:
            const std::string &name_;
            Parser &parser_;
            OpType opType_;
        };

    private:
        Lexer lexer_;
        std::unordered_map<std::string, std::function<CodeNode::SharedPtr ()>> appFtns_;
    };

    // --------------------------------------------------------------------------------
    // INLINE

    inline bool Parser::hasIncompleteExpr() const {
        return !lexer_.empty();
    }

    inline void Parser::clearIncompleteExpr() {
        lexer_.clear();
    }

    template <typename ExprType, typename ExprOp>
    inline Parser::MakeBinaryExpression<ExprType, ExprOp>::MakeBinaryExpression(const std::string &name, Parser &parser, ExprOp exprOp)
        : name_(name)
        , parser_(parser)
        , exprOp_(exprOp)
    {}

    template <typename ExprType, typename ExprOp>
    inline CodeNode::SharedPtr Parser::MakeBinaryExpression<ExprType, ExprOp>::operator()() {
        auto exprs(parser_.readAndCheckExprList(name_.c_str(), 2));
        return std::make_shared<ExprType>(exprOp_, exprs[0], exprs[1]);
    }

    template <typename CodeNodeType>
    inline Parser::MakeStrCharOp<CodeNodeType>::MakeStrCharOp(const std::string &name, Parser &parser, OpType opType)
        : name_(name)
        , parser_(parser)
        , opType_(opType)
    {}

    template <typename CodeNodeType>
    inline CodeNode::SharedPtr Parser::MakeStrCharOp<CodeNodeType>::operator()() {
        auto exprs(parser_.readAndCheckExprList(name_.c_str(), 1));
        return std::make_shared<CodeNodeType>(opType_, exprs[0]);
    }

} // Int

#endif // PARSER_H
