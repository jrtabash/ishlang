#ifndef PARSER_H
#define	PARSER_H

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

        bool hasIncompleteExpr() const { return !lexer_.empty(); }
        void clearIncompleteExpr() { lexer_.clear(); }

    private:
        CodeNode::SharedPtr readExpr();
        CodeNode::SharedPtr makeLiteral(Lexer::TokenType type, const std::string &text);
        CodeNode::SharedPtr readApp(const std::string &expected="");
        CodeNode::SharedPtrList readExprList();
        CodeNode::SharedPtrPairs readExprPairs();
        std::string readName();
        std::vector<std::string> readParams();
        bool ignoreLeftP(bool allowRightP);
        void ignoreRightP();

    private:
        bool haveSExpression() const;
        void initAppFtns();

    private:
        template <typename ExprType, typename ExprOp>
        struct MakeBinaryExpression {
            MakeBinaryExpression(Parser &parser, ExprOp exprOp)
                : parser_(parser)
                , exprOp_(exprOp)
            {}

            CodeNode::SharedPtr operator()() {
                CodeNode::SharedPtr lhs(parser_.readExpr());
                CodeNode::SharedPtr rhs(parser_.readExpr());
                parser_.ignoreRightP();
                return std::make_shared<ExprType>(exprOp_, lhs, rhs);
            }

        private:
            Parser &parser_;
            ExprOp exprOp_;
        };

    private:
        Lexer lexer_;
        std::unordered_map<std::string, std::function<CodeNode::SharedPtr ()>> appFtns_;
    };

} // Int

#endif // PARSER_H
