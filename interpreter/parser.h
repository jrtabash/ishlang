#ifndef PARSER_H
#define	PARSER_H

#include "byte_code.h"

#include <string>
#include <forward_list>
#include <unordered_set>
#include <functional>

namespace Int {

    class Parser {
    public:
        enum TokenType {
            Unknown = 'U',
            LeftP   = '(',
            RightP  = ')',
            Char    = 'C',
            String  = 'S',
            Int     = 'I',
            Real    = 'R',
            Bool    = 'B',
            Null    = 'N',
            Symbol  = 'L',
        };

        typedef std::function<void (ByteCode::SharedPtr &code)> CallBack;

    public:
        Parser();

        ByteCode::SharedPtr read(const std::string &expr);
        ByteCode::SharedPtr readLiteral(const std::string &expr);
        void readMulti(const std::string &expr, CallBack callback);

    private:
        ByteCode::SharedPtr readExpr(const std::string &expr, size_t &pos);
        ByteCode::SharedPtr makeLiteral(TokenType tokenType, const std::string & expr);
        ByteCode::SharedPtr readApp(const std::string &expr, size_t &pos, const std::string &expected="");
        ByteCode::SharedPtrList readExprList(const std::string &expr, size_t &pos);
        ByteCode::SharedPtrPairs readExprPairs(const std::string &expr, size_t &pos);
        std::vector<std::string> readParams(const std::string &expr, size_t &pos);
        bool ignoreLeftP(const std::string &expr, size_t &pos, bool allowRightP);
        void ignoreRightP(const std::string &expr, size_t &pos);

    private:
        static ArithOp::Type str2ArithOp(const std::string &token);
        static CompOp::Type str2CompOp(const std::string &token);
        static LogicOp::Type str2LogicOp(const std::string &token);

    public:
        static TokenType tokenType(const std::string &token);
    };

} // Int

#endif // PARSER_H
