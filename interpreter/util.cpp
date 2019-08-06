#include "util.h"
#include "exception.h"

#include <cmath>
#include <cctype>

using namespace Int;

const double Util::RealThreshold = 0.0000000001;

// -------------------------------------------------------------
bool Util::isZero(double val) { return std::fabs(val) <= RealThreshold; }

// -------------------------------------------------------------
bool Util::isEqual(double lhs, double rhs) { return std::fabs(lhs - rhs) <= RealThreshold; }

// -------------------------------------------------------------
std::string nextToken(std::istream &is) {
    std::string token;
    while (is.good()) {
        const char c = static_cast<char>(is.peek());
        if (isspace(c)) {
            if (!token.empty()) {
                is.get();
                break;
            }
            else { continue; }
        }
        else if (c == '\'') {
            if (token.empty()) {
                token += static_cast<char>(is.get());
                token += static_cast<char>(is.get());
                if (!is.good() || static_cast<char>(is.peek()) != '\'') {
                    throw InvalidExpression("character closing tick expected");
                }
                token += static_cast<char>(is.get());
            }
            break;
        }
        else if (c == '"') {
            if (token.empty()) {
                token += static_cast<char>(is.get());
                while (is.good() && static_cast<char>(is.peek()) != '"') {
                    token += static_cast<char>(is.get());
                }
                if (!is.good()) {
                    throw InvalidExpression("incomplete string");
                }
                token += static_cast<char>(is.get());
            }
            break;
        }
        else if (c == '(' || c == ')') {
            if (token.empty()) {
                token += static_cast<char>(is.get());
            }
            break;
        }
        else {
            token += static_cast<char>(is.get());
        }
    }
    return token;
}

// -------------------------------------------------------------
std::string Util::nextToken(const std::string &str, size_t &pos) {
    std::string token;
    for (; pos < str.size(); ++pos) {
        if (isspace(str[pos])) {
            if (!token.empty()) {
                ++pos;
                break;
            }
            else { continue; }
        }
        else if (str[pos] == '\'') {
            if (token.empty()) {
                token += str[pos++];
                if (pos >= str.size()) {
                    throw InvalidExpression("character literal expected");
                }
                token += str[pos++];
                if (pos >= str.size() || str[pos] != '\'') {
                    throw InvalidExpression("character closing tick expected");
                }
                token += str[pos++];
            }
            break;
        }
        else if (str[pos] == '"') {
            if (token.empty()) {
                token += str[pos++];
                if (pos >= str.size()) {
                    throw InvalidExpression("string character expected");
                }
                while (str[pos] != '"') {
                    token += str[pos++];
                    if (pos >= str.size()) {
                        throw InvalidExpression("incomplete string");
                    }
                }
                if (str[pos] != '"') {
                    throw InvalidExpression("string closing double quotes expected");
                }
                token += str[pos++];
            }
            break;
        }
        else if (str[pos] == '(' || str[pos] == ')') {
            if (token.empty()) {
                token += str[pos];
                ++pos;
            }
            break;
        }
        else {
            token += str[pos];
        }
    }
    if (pos >= str.size()) { pos = std::string::npos; }
    return token;
}

// -------------------------------------------------------------
size_t Util::tokenize(const std::string &str, TokenList &tokens) {
    tokens.clear();
    TokenList::iterator iter = tokens.before_begin();
    std::string token;
    size_t size = 0;
    size_t pos = 0;
    while (pos != std::string::npos) {
        token = nextToken(str, pos);
        if (!token.empty()) {
            iter = tokens.insert_after(iter, std::move(token));
            ++size;
        }
    }
    return size;
}
