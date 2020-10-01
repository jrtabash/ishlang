#include "lexer.h"
#include "exception.h"
#include "util.h"

using namespace Ishlang;

// -------------------------------------------------------------
Lexer::Lexer()
    : tokens_()
{
}

// -------------------------------------------------------------
void Lexer::read(const std::string & expr) {
    size_t pos = 0;
    TokenType type = Unknown;
    std::string text;
    while (pos != std::string::npos) {
        text = Util::nextToken(expr, pos);
        if (!text.empty()) {
            type = tokenType(text);
            if (type == Unknown) {
                throw UnknownTokenType(text, static_cast<char>(type));
            }
            tokens_.emplace_back(type, std::move(text));
        }
    }
}

// -------------------------------------------------------------
Lexer::Token Lexer::next() {
    if (tokens_.empty()) {
        throw InvalidExpression("incomplete form");
    }
    auto token = tokens_.front();
    tokens_.pop_front();
    return token;
}

// -------------------------------------------------------------
Lexer::TokenType Lexer::tokenType(const std::string &token) {
    static const std::unordered_set<char> singles({
            '-', '+', '*', '/', '%', '^', '=', '<', '>', '?'});

    static const std::unordered_set<char> notAllowed({
            '(', ')', '-', '+', '[', ']', '{', '}', '~', '!', '@',
            '#', '$', '%', '^', '&', '*', '=', '|', '\\', ',', '.',
            '<', '>', '?', '`', '/', '\'', '"', ':' });

    const size_t size(token.size());
    if (token[0] == '(') {
        if (size == 1) { return LeftP; }
    }
    else if (token[0] == ')') {
        if (size == 1) { return RightP; }
    }
    else if (size == 1 && singles.find(token[0]) != singles.end()) {
        return Symbol;
    }
    else if (token == "==" || token == "!=" || token == "<=" || token == ">=") {
        return Symbol;
    }
    else if (token[0] == '\'') {
        if (size == 3 && token[size - 1] == '\'') { return Char; }
    }
    else if (token[0] == '"') {
        if (size >= 2) {
            if (token[size - 1] == '"') { return String; }
        }
    }
    else if (token[0] == '-' || token[0] == '+' || token[0] == '.' || std::isdigit(token[0])) {
        bool seenDot = token[0] == '.';
        unsigned count = std::isdigit(token[0]) ? 1 : 0;
        const bool isNum(
            std::all_of(token.begin() + 1, token.end(), [&seenDot, &count](char c) {
                    if (c == '.') {
                        if (seenDot) { return false; }
                        return seenDot = true;
                    }
                    ++count;
                    return static_cast<bool>(std::isdigit(c));
                }));
        if (isNum && count > 0) { // Must have at least a single digit
            return seenDot ? Real : Int;
        }
    }
    else if (token == "true" || token == "false") {
        return Bool;
    }
    else if (token == "null") {
        return Null;
    }
    else if (std::isalpha(token[0])) {
        if (std::all_of(token.begin() + 1, token.end(), [](char c) { return std::isprint(c) && notAllowed.find(c) == notAllowed.end(); })) {
            return Symbol;
        }
    }
    return Unknown;
}
