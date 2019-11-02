#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <deque>
#include <unordered_set>
#include <functional>

namespace Ishlang {

    class Lexer {
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

        struct Token {
            TokenType type;
            std::string text;

            Token(TokenType type, std::string &&text)
                : type(type)
                , text(text)
            {}
        };

        using Tokens = std::deque<Token>;

    public:
        Lexer();

        void read(const std::string & expr);

        Token next();

        Tokens::const_iterator cbegin() const { return tokens_.begin(); }
        Tokens::const_iterator cend() const { return tokens_.end(); }

        size_t size() const { return tokens_.size(); }
        bool empty() const { return tokens_.empty(); }

        void clear() { tokens_.clear(); }

    public:
        static TokenType tokenType(const std::string &token);

    private:
        Tokens tokens_;
    };
}

#endif // LEXER_H
