#ifndef ISHLANG_PARSER_H
#define ISHLANG_PARSER_H

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
        CodeNode::SharedPtrList readAndCheckRangeExprList(const char *name, std::size_t minExpectedSize, std::optional<std::size_t> maxExpectedSize);
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
        template <typename ExprType>
        struct MakeBinaryExpression {
            using ExprOp = typename ExprType::Type;

            inline MakeBinaryExpression(const std::string &name, Parser &parser, ExprOp exprOp);

            inline CodeNode::SharedPtr operator()();

        private:
            const std::string &name_;
            Parser &parser_;
            ExprOp exprOp_;
        };

    private:
        template <typename ExprType>
        struct MakeVariadicExpression {
            using ExprOp = typename ExprType::Type;

            inline MakeVariadicExpression(const std::string &name, Parser &parser, ExprOp exprOp);

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

    template <typename ExprType>
    inline Parser::MakeBinaryExpression<ExprType>::MakeBinaryExpression(const std::string &name, Parser &parser, ExprOp exprOp)
        : name_(name)
        , parser_(parser)
        , exprOp_(exprOp)
    {}

    template <typename ExprType>
    inline CodeNode::SharedPtr Parser::MakeBinaryExpression<ExprType>::operator()() {
        auto exprs(parser_.readAndCheckExprList(name_.c_str(), 2));
        return CodeNode::make<ExprType>(exprOp_, exprs[0], exprs[1]);
    }

    template <typename ExprType>
    inline Parser::MakeVariadicExpression<ExprType>::MakeVariadicExpression(const std::string &name, Parser &parser, ExprOp exprOp)
        : name_(name)
        , parser_(parser)
        , exprOp_(exprOp)
    {}

    template <typename ExprType>
    inline CodeNode::SharedPtr Parser::MakeVariadicExpression<ExprType>::operator()() {
        auto operands(parser_.readAndCheckRangeExprList(name_.c_str(), 2, std::nullopt));
        return CodeNode::make<ExprType>(exprOp_, operands);
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
        return CodeNode::make<CodeNodeType>(opType_, exprs[0]);
    }

} // Int

#endif // ISHLANG_PARSER_H
