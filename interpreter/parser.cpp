#include "parser.h"
#include "util.h"
#include "struct.h"
#include "exception.h"

#include <cctype>
#include <algorithm>
#include <cstdlib>

using namespace Int;

// -------------------------------------------------------------
Parser::Parser()
{}

// -------------------------------------------------------------
ByteCode::SharedPtr Parser::read(const std::string &expr) {
    size_t pos = 0;
    return readExpr(expr, pos);
}

// -------------------------------------------------------------
ByteCode::SharedPtr Parser::readLiteral(const std::string &expr) {
    auto const tokTyp = tokenType(expr);
    switch (tokTyp) {
    case Char:
    case String:
    case Int:
    case Real:
    case Bool:
    case Null:
        return makeLiteral(tokTyp, expr);

    default:
        break;
    }

    return std::make_shared<Literal>(Value(expr));
}

// -------------------------------------------------------------
void Parser::readMulti(const std::string &expr, CallBack callback) {
    size_t pos = 0;
    while (pos != std::string::npos) {
        ByteCode::SharedPtr code = readExpr(expr, pos);
        if (code.get()) {
            callback(code);
        }
    }
}

// -------------------------------------------------------------
ByteCode::SharedPtr Parser::readExpr(const std::string &expr, size_t &pos) {
    std::string token;
    while (pos != std::string::npos) {
        token = Util::nextToken(expr, pos);
        if (!token.empty()) {
            const TokenType tokTyp = tokenType(token);
            switch (tokTyp) {
            case LeftP:
                return readApp(expr, pos);

            case RightP:
                return ByteCode::SharedPtr();

            case Char:
            case String:
            case Int:
            case Real:
            case Bool:
            case Null:
                return makeLiteral(tokTyp, token);

            case Symbol:
                return std::make_shared<Variable>(token);

            default:
                throw UnknownTokenType(token, static_cast<char>(tokTyp));
                break;
            }
        }
    }
    return std::make_shared<Literal>(Value::Null);
}

// -------------------------------------------------------------
ByteCode::SharedPtr Parser::makeLiteral(TokenType tokenType, const std::string & token) {
    switch (tokenType) {
    case Char:
        return std::make_shared<Literal>(Value(token[1]));

    case String:
        return std::make_shared<Literal>(Value(std::string(token.c_str() + 1, token.size() - 2)));

    case Int:
        return std::make_shared<Literal>(Value(Value::Long(std::stoll(token, 0, 10))));

    case Real:
        return std::make_shared<Literal>(Value(std::stod(token, 0)));

    case Bool:
        return std::make_shared<Literal>((Value(Value::Bool(token == "true"))));

    default:
        break;
    }

    return std::make_shared<Literal>(Value::Null);
}

// -------------------------------------------------------------
ByteCode::SharedPtr Parser::readApp(const std::string &expr, size_t &pos, const std::string &expected) {
    std::string token;
    if (pos != std::string::npos) {
        token = Util::nextToken(expr, pos);
        if (!token.empty()) {
            if (!expected.empty() && token != expected) {
                throw UnexpectedExpression("lambda", token);
            }

            if (token == "var") {
                const std::string name(Util::nextToken(expr, pos));
                ByteCode::SharedPtr code(readExpr(expr, pos));
                ignoreRightP(expr, pos);
                return std::make_shared<Define>(name, code);
            }
            else if (token == "=") {
                const std::string name(Util::nextToken(expr, pos));
                ByteCode::SharedPtr code(readExpr(expr, pos));
                ignoreRightP(expr, pos);
                return std::make_shared<Assign>(name, code);
            }
            else if (token == "?") {
                const std::string name(Util::nextToken(expr, pos));
                ignoreRightP(expr, pos);
                return std::make_shared<Exists>(name);
            }
            else if (token == "clone") {
                ByteCode::SharedPtr code(readExpr(expr, pos));
                ignoreRightP(expr, pos);
                return std::make_shared<Clone>(code);
            }
            else if (token == "+" || token == "-" || token == "*" || token == "/" || token == "%") {
                const ArithOp::Type op(str2ArithOp(token));
                ByteCode::SharedPtr lhs(readExpr(expr, pos));
                ByteCode::SharedPtr rhs(readExpr(expr, pos));
                ignoreRightP(expr, pos);
                return std::make_shared<ArithOp>(op, lhs, rhs);
            }
            else if (token == "==" || token == "!=" || token == "<" || token == ">" || token == "<=" || token == ">=") {
                const CompOp::Type op(str2CompOp(token));
                ByteCode::SharedPtr lhs(readExpr(expr, pos));
                ByteCode::SharedPtr rhs(readExpr(expr, pos));
                ignoreRightP(expr, pos);
                return std::make_shared<CompOp>(op, lhs, rhs);
            }
            else if (token == "and" || token == "or") {
                const LogicOp::Type op(str2LogicOp(token));
                ByteCode::SharedPtr lhs(readExpr(expr, pos));
                ByteCode::SharedPtr rhs(readExpr(expr, pos));
                ignoreRightP(expr, pos);
                return std::make_shared<LogicOp>(op, lhs, rhs);
            }
            else if (token == "progn") {
                ByteCode::SharedPtrList exprs(readExprList(expr, pos));
                return std::make_shared<ProgN>(exprs);
            }
            else if (token == "block") {
                ByteCode::SharedPtrList exprs(readExprList(expr, pos));
                return std::make_shared<Block>(exprs);
            }
            else if (token == "if") {
                ByteCode::SharedPtr pred(readExpr(expr, pos));
                ByteCode::SharedPtr tCode(readExpr(expr, pos));
                ByteCode::SharedPtr fCode(readExpr(expr, pos));
                ignoreRightP(expr, pos);
                return std::make_shared<If>(pred, tCode, fCode);
            }
            else if (token == "cond") {
                ByteCode::SharedPtrPairs pairs(readExprPairs(expr, pos));
                return std::make_shared<Cond>(pairs);
            }
            else if (token == "break") {
                ignoreRightP(expr, pos);
                return std::make_shared<Break>();
            }
            else if (token == "loop") {
                ByteCode::SharedPtrList forms(readExprList(expr, pos));
                if (forms.size() == 4) {
                    ByteCode::SharedPtrList::iterator iter = forms.begin();
                    ByteCode::SharedPtr decl(*iter++);
                    ByteCode::SharedPtr cond(*iter++);
                    ByteCode::SharedPtr next(*iter++);
                    ByteCode::SharedPtr body(*iter++);
                    return std::make_shared<Loop>(decl, cond, next, body);
                }
                else if (forms.size() == 2) {
                    ByteCode::SharedPtrList::iterator iter = forms.begin();
                    ByteCode::SharedPtr cond(*iter++);
                    ByteCode::SharedPtr body(*iter++);
                    return std::make_shared<Loop>(cond, body);
                }
                else {
                    throw InvalidExpression("Too many/few forms in loop");
                }
            }
            else if (token == "lambda") {
                ByteCode::ParamList params(readParams(expr, pos));
                ByteCode::SharedPtr body(readExpr(expr, pos));
                ignoreRightP(expr, pos);
                return std::make_shared<LambdaExpr>(params, body);
            }
            else if (token == "def") {
                const std::string name(Util::nextToken(expr, pos));
                ByteCode::ParamList params(readParams(expr, pos));
                ByteCode::SharedPtr body(readExpr(expr, pos));
                ignoreRightP(expr, pos);
                return std::make_shared<FunctionExpr>(name, params, body);
            }
            else if (token == "(") {
                ByteCode::SharedPtr lambda(readApp(expr, pos, "lambda"));
                ByteCode::SharedPtrList args(readExprList(expr, pos));
                return std::make_shared<LambdaApp>(lambda, args);
            }
            else if (token == "istypeof") {
                ByteCode::SharedPtr form(readExpr(expr, pos));
                Value::Type type(Value::stringToType(Util::nextToken(expr, pos)));
                ignoreRightP(expr, pos);
                return std::make_shared<IsType>(form, type);
            }
            else if (token == "print" || token == "println") {
                ByteCode::SharedPtr pExpr(readExpr(expr, pos));
                ignoreRightP(expr, pos);
                return std::make_shared<Print>(token == "println", pExpr);
            }
            else if (token == "read") {
                ignoreRightP(expr, pos);
                return std::make_shared<Read>();
            }
            else if (token == "struct") {
                const std::string name(Util::nextToken(expr, pos));
                const Struct::MemberList members(readParams(expr, pos));
                ignoreRightP(expr, pos);
                return std::make_shared<StructExpr>(name, members);
            }
            else if (token == "isstructname") {
                ByteCode::SharedPtr snExpr(readExpr(expr, pos));
                const std::string name(Util::nextToken(expr, pos));
                ignoreRightP(expr, pos);
                return std::make_shared<IsStructName>(snExpr, name);
            }
            else if (token == "makeinstance") {
                const std::string name(Util::nextToken(expr, pos));
                ignoreRightP(expr, pos);
                return std::make_shared<MakeInstance>(name);
            }
            else if (token == "isinstanceof") {
                ByteCode::SharedPtr ioExpr(readExpr(expr, pos));
                const std::string name(Util::nextToken(expr, pos));
                ignoreRightP(expr, pos);
                return std::make_shared<IsInstanceOf>(ioExpr, name);
            }
            else if (token == "get") {
                ByteCode::SharedPtr instExpr(readExpr(expr, pos));
                const std::string name(Util::nextToken(expr, pos));
                ignoreRightP(expr, pos);
                return std::make_shared<GetMember>(instExpr, name);
            }
            else if (token == "set") {
                ByteCode::SharedPtr instExpr(readExpr(expr, pos));
                const std::string name(Util::nextToken(expr, pos));
                ByteCode::SharedPtr valueExpr(readExpr(expr, pos));
                ignoreRightP(expr, pos);
                return std::make_shared<SetMember>(instExpr, name, valueExpr);
            }
            else {
                if (tokenType(token) == Symbol) {
                    const std::string name(std::move(token));
                    ByteCode::SharedPtrList args(readExprList(expr, pos));
                    return std::make_shared<FunctionApp>(name, args);
                }
                else {
                    throw UnknownSymbol(token);
                }
            }
        }
    }
    return std::make_shared<Literal>(Value::Null);
}

// -------------------------------------------------------------
ByteCode::SharedPtrList Parser::readExprList(const std::string &expr, size_t &pos) {
    ByteCode::SharedPtrList forms;
    ByteCode::SharedPtr form = readExpr(expr, pos);
    while (form.get()) {
        forms.push_back(form);
        form = readExpr(expr, pos);
    }
    return forms;
}

// -------------------------------------------------------------
ByteCode::SharedPtrPairs Parser::readExprPairs(const std::string &expr, size_t &pos) {
    ByteCode::SharedPtrPairs pairs;
    ByteCode::SharedPtrPair cons;
    ignoreLeftP(expr, pos, false);
    cons.first = readExpr(expr, pos);
    cons.second = readExpr(expr, pos);
    ignoreRightP(expr, pos);
    while (cons.first.get() && cons.second.get()) {
        pairs.push_back(cons);
        if (ignoreLeftP(expr, pos, true)) { break; }
        cons.first = readExpr(expr, pos);
        cons.second = readExpr(expr, pos);
        ignoreRightP(expr, pos);
    }
    return pairs;
}

// -------------------------------------------------------------
ByteCode::ParamList Parser::readParams(const std::string &expr, size_t &pos) {
    ByteCode::ParamList params;
    std::string token = Util::nextToken(expr, pos);
    TokenType tokTyp = tokenType(token);
    if (tokTyp != LeftP) {
        throw InvalidExpression("Expecting ( beginning of param list");
    }

    token = Util::nextToken(expr, pos);
    tokTyp = tokenType(token);
    while (tokTyp != RightP) {
        if (tokTyp != Symbol) {
            throw UnexpectedTokenType(token, tokTyp, "paramList");
        }
        params.push_back(std::move(token));
        token = Util::nextToken(expr, pos);
        tokTyp = tokenType(token);
    }
    return params;
}

// -------------------------------------------------------------
bool Parser::ignoreLeftP(const std::string &expr, size_t &pos, bool allowRightP) {
    TokenType tokTyp(tokenType(Util::nextToken(expr, pos)));
    if (tokTyp == RightP && allowRightP) { return true; }
    if (tokTyp != LeftP) {
        throw ExpectedParenthesis('(');
    }
    return false;
}

// -------------------------------------------------------------
void Parser::ignoreRightP(const std::string &expr, size_t &pos) {
    if (tokenType(Util::nextToken(expr, pos)) != RightP) {
        throw ExpectedParenthesis(')');
    }
}

// -------------------------------------------------------------
ArithOp::Type Parser::str2ArithOp(const std::string &token) {
    ArithOp::Type type(ArithOp::Add);
    if      (token == "-") { type = ArithOp::Sub; }
    else if (token == "*") { type = ArithOp::Mul; }
    else if (token == "/") { type = ArithOp::Div; }
    else if (token == "%") { type = ArithOp::Mod; }
    return type;
}

// -------------------------------------------------------------
CompOp::Type Parser::str2CompOp(const std::string &token) {
    CompOp::Type type(CompOp::EQ);
    if      (token == "!=") { type = CompOp::NE; }
    else if (token == "<")  { type = CompOp::LT; }
    else if (token == ">")  { type = CompOp::GT; }
    else if (token == "<=") { type = CompOp::LE; }
    else if (token == ">=") { type = CompOp::GE; }
    return type;
}

// -------------------------------------------------------------
LogicOp::Type Parser::str2LogicOp(const std::string &token) {
    return token == "and" ? LogicOp::Conjunction : LogicOp::Disjunction;
}

// -------------------------------------------------------------
Parser::TokenType Parser::tokenType(const std::string &token) {
    static const std::unordered_set<char> singles({
            '-', '+', '*', '/', '%', '=', '<', '>'});

    static const std::unordered_set<char> notAllowed({
            '(', ')', '-', '+', '[', ']', '{', '}', '~', '!', '@',
            '#', '$', '%', '^', '&', '*', '=', '|', '\\', ',', '.', 
            '<', '>', '?', '`', '/', '\'', '"' });

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
