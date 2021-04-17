#ifndef LEXER_H
#define LEXER_H

#include <deque>
#include <functional>
#include <string>
#include <unordered_set>

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

            inline Token(TokenType type, std::string &&text);
        };

        using Tokens = std::deque<Token>;

    public:
        Lexer();

        void read(const std::string & expr);

        Token next();
        const Token &peek() const;

        inline Tokens::const_iterator cbegin() const;
        inline Tokens::const_iterator cend() const;

        inline size_t size() const;
        inline bool empty() const;

        inline void clear();

    public:
        static TokenType tokenType(const std::string &token);

    private:
        Tokens tokens_;
    };

    // --------------------------------------------------------------------------------
    // INLINE

    inline Lexer::Token::Token(TokenType type, std::string &&text)
        : type(type)
        , text(text)
    {}

    inline auto Lexer::cbegin() const -> Tokens::const_iterator {
        return tokens_.begin();
    }

    inline auto Lexer::cend() const -> Tokens::const_iterator {
        return tokens_.end();
    }

    inline size_t Lexer::size() const {
        return tokens_.size();
    }

    inline bool Lexer::empty() const {
        return tokens_.empty();
    }

    inline void Lexer::clear() {
        tokens_.clear();
    }

}

#endif // LEXER_H
