#ifndef PARSER_H
#define	PARSER_H

#include "byte_code.h"
#include "lexer.h"

#include <string>
#include <forward_list>
#include <unordered_set>
#include <functional>

namespace Int {

    class Parser {
    public:
        using CallBack = std::function<void (ByteCode::SharedPtr &code)>;

    public:
        Parser();

        ByteCode::SharedPtr read(const std::string &expr);
        ByteCode::SharedPtr readLiteral(const std::string &expr);
        void readMulti(const std::string &expr, CallBack callback);

        bool hasIncompleteExpr() const { return !lexer_.empty(); }
        void clearIncompleteExpr() { lexer_.clear(); }

    private:
        ByteCode::SharedPtr readExpr();
        ByteCode::SharedPtr makeLiteral(Lexer::TokenType type, const std::string &text);
        ByteCode::SharedPtr readApp(const std::string &expected="");
        ByteCode::SharedPtrList readExprList();
        ByteCode::SharedPtrPairs readExprPairs();
        std::string readName();
        std::vector<std::string> readParams();
        bool ignoreLeftP(bool allowRightP);
        void ignoreRightP();

    private:
        bool haveSExpression() const;

    private:
        static ArithOp::Type str2ArithOp(const std::string &token);
        static CompOp::Type str2CompOp(const std::string &token);
        static LogicOp::Type str2LogicOp(const std::string &token);

    private:
        Lexer lexer_;
    };

} // Int

#endif // PARSER_H
