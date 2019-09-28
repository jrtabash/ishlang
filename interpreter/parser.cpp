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
    : lexer_()
{}

// -------------------------------------------------------------
ByteCode::SharedPtr Parser::read(const std::string &expr) {
    lexer_.read(expr);
    return readExpr();
}

// -------------------------------------------------------------
ByteCode::SharedPtr Parser::readLiteral(const std::string &expr) {
    const auto tokTyp = Lexer::tokenType(expr);
    switch (tokTyp) {
    case Lexer::Char:
    case Lexer::String:
    case Lexer::Int:
    case Lexer::Real:
    case Lexer::Bool:
    case Lexer::Null:
        return makeLiteral(tokTyp, expr);

    default:
        break;
    }

    return std::make_shared<Literal>(Value(expr));
}

// -------------------------------------------------------------
void Parser::readMulti(const std::string &expr, CallBack callback) {
    lexer_.read(expr);
    while (!lexer_.empty()) {
        if (!haveSExpression()) {
            return;
        }

        ByteCode::SharedPtr code = readExpr();
        if (code.get()) {
            callback(code);
        }
    }
}

// -------------------------------------------------------------
ByteCode::SharedPtr Parser::readExpr() {
    while (!lexer_.empty()) {
        if (!haveSExpression()) {
            return ByteCode::SharedPtr();
        }

        auto token = lexer_.next();

        if (!token.text.empty()) {
            switch (token.type) {
            case Lexer::LeftP:
                return readApp();

            case Lexer::RightP:
                return ByteCode::SharedPtr();

            case Lexer::Char:
            case Lexer::String:
            case Lexer::Int:
            case Lexer::Real:
            case Lexer::Bool:
            case Lexer::Null:
                return makeLiteral(token.type, token.text);

            case Lexer::Symbol:
                return std::make_shared<Variable>(token.text);

            case Lexer::Unknown:
                throw UnknownTokenType(token.text, static_cast<char>(token.type));
                break;
            }
        }
    }

    return std::make_shared<Literal>(Value::Null);
}

// -------------------------------------------------------------
ByteCode::SharedPtr Parser::makeLiteral(Lexer::TokenType type, const std::string &text) {
    switch (type) {
    case Lexer::Char:
        return std::make_shared<Literal>(Value(text[1]));

    case Lexer::String:
        return std::make_shared<Literal>(Value(std::string(text.c_str() + 1, text.size() - 2)));

    case Lexer::Int:
        return std::make_shared<Literal>(Value(Value::Long(std::stoll(text, 0, 10))));

    case Lexer::Real:
        return std::make_shared<Literal>(Value(std::stod(text, 0)));

    case Lexer::Bool:
        return std::make_shared<Literal>((Value(Value::Bool(text == "true"))));

    default:
        break;
    }

    return std::make_shared<Literal>(Value::Null);
}

// -------------------------------------------------------------
ByteCode::SharedPtr Parser::readApp(const std::string &expected) {
    if (!lexer_.empty()) {
        auto token = lexer_.next();

        if (!token.text.empty()) {
            if (!expected.empty() && token.text != expected) {
                throw UnexpectedExpression("lambda", token.text);
            }

            if (token.text == "var") {
                const std::string name(readName());
                ByteCode::SharedPtr code(readExpr());
                ignoreRightP();
                return std::make_shared<Define>(name, code);
            }
            else if (token.text == "=") {
                const std::string name(readName());
                ByteCode::SharedPtr code(readExpr());
                ignoreRightP();
                return std::make_shared<Assign>(name, code);
            }
            else if (token.text == "?") {
                const std::string name(readName());
                ignoreRightP();
                return std::make_shared<Exists>(name);
            }
            else if (token.text == "clone") {
                ByteCode::SharedPtr code(readExpr());
                ignoreRightP();
                return std::make_shared<Clone>(code);
            }
            else if (token.text == "+" || token.text == "-" || token.text == "*" || token.text == "/" || token.text == "%") {
                const ArithOp::Type op(str2ArithOp(token.text));
                ByteCode::SharedPtr lhs(readExpr());
                ByteCode::SharedPtr rhs(readExpr());
                ignoreRightP();
                return std::make_shared<ArithOp>(op, lhs, rhs);
            }
            else if (token.text == "==" || token.text == "!=" || token.text == "<" || token.text == ">" || token.text == "<=" || token.text == ">=") {
                const CompOp::Type op(str2CompOp(token.text));
                ByteCode::SharedPtr lhs(readExpr());
                ByteCode::SharedPtr rhs(readExpr());
                ignoreRightP();
                return std::make_shared<CompOp>(op, lhs, rhs);
            }
            else if (token.text == "and" || token.text == "or") {
                const LogicOp::Type op(str2LogicOp(token.text));
                ByteCode::SharedPtr lhs(readExpr());
                ByteCode::SharedPtr rhs(readExpr());
                ignoreRightP();
                return std::make_shared<LogicOp>(op, lhs, rhs);
            }
            else if (token.text == "progn") {
                ByteCode::SharedPtrList exprs(readExprList());
                return std::make_shared<ProgN>(exprs);
            }
            else if (token.text == "block") {
                ByteCode::SharedPtrList exprs(readExprList());
                return std::make_shared<Block>(exprs);
            }
            else if (token.text == "if") {
                ByteCode::SharedPtr pred(readExpr());
                ByteCode::SharedPtr tCode(readExpr());
                ByteCode::SharedPtr fCode(readExpr());
                ignoreRightP();
                return std::make_shared<If>(pred, tCode, fCode);
            }
            else if (token.text == "cond") {
                ByteCode::SharedPtrPairs pairs(readExprPairs());
                return std::make_shared<Cond>(pairs);
            }
            else if (token.text == "break") {
                ignoreRightP();
                return std::make_shared<Break>();
            }
            else if (token.text == "loop") {
                ByteCode::SharedPtrList forms(readExprList());
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
            else if (token.text == "lambda") {
                ByteCode::ParamList params(readParams());
                ByteCode::SharedPtr body(readExpr());
                ignoreRightP();
                return std::make_shared<LambdaExpr>(params, body);
            }
            else if (token.text == "def") {
                const std::string name(readName());
                ByteCode::ParamList params(readParams());
                ByteCode::SharedPtr body(readExpr());
                ignoreRightP();
                return std::make_shared<FunctionExpr>(name, params, body);
            }
            else if (token.text == "(") {
                ByteCode::SharedPtr lambda(readApp("lambda"));
                ByteCode::SharedPtrList args(readExprList());
                return std::make_shared<LambdaApp>(lambda, args);
            }
            else if (token.text == "istypeof") {
                ByteCode::SharedPtr form(readExpr());
                Value::Type type(Value::stringToType(readName()));
                ignoreRightP();
                return std::make_shared<IsType>(form, type);
            }
            else if (token.text == "print" || token.text == "println") {
                ByteCode::SharedPtr pExpr(readExpr());
                ignoreRightP();
                return std::make_shared<Print>(token.text == "println", pExpr);
            }
            else if (token.text == "read") {
                ignoreRightP();
                return std::make_shared<Read>();
            }
            else if (token.text == "struct") {
                const std::string name(readName());
                const Struct::MemberList members(readParams());
                ignoreRightP();
                return std::make_shared<StructExpr>(name, members);
            }
            else if (token.text == "isstructname") {
                ByteCode::SharedPtr snExpr(readExpr());
                const std::string name(readName());
                ignoreRightP();
                return std::make_shared<IsStructName>(snExpr, name);
            }
            else if (token.text == "makeinstance") {
                const std::string name(readName());
                ignoreRightP();
                return std::make_shared<MakeInstance>(name);
            }
            else if (token.text == "isinstanceof") {
                ByteCode::SharedPtr ioExpr(readExpr());
                const std::string name(readName());
                ignoreRightP();
                return std::make_shared<IsInstanceOf>(ioExpr, name);
            }
            else if (token.text == "get") {
                ByteCode::SharedPtr instExpr(readExpr());
                const std::string name(readName());
                ignoreRightP();
                return std::make_shared<GetMember>(instExpr, name);
            }
            else if (token.text == "set") {
                ByteCode::SharedPtr instExpr(readExpr());
                const std::string name(readName());
                ByteCode::SharedPtr valueExpr(readExpr());
                ignoreRightP();
                return std::make_shared<SetMember>(instExpr, name, valueExpr);
            }
            else if (token.text == "strlen") {
                ByteCode::SharedPtr strExpr(readExpr());
                ignoreRightP();
                return std::make_shared<StringLen>(strExpr);
            }
            else if (token.text == "getchar") {
                ByteCode::SharedPtr strExpr(readExpr());
                ByteCode::SharedPtr posExpr(readExpr());
                ignoreRightP();
                return std::make_shared<GetCharAt>(strExpr, posExpr);
            }
            else if (token.text == "setchar") {
                ByteCode::SharedPtr strExpr(readExpr());
                ByteCode::SharedPtr posExpr(readExpr());
                ByteCode::SharedPtr valExpr(readExpr());
                ignoreRightP();
                return std::make_shared<SetCharAt>(strExpr, posExpr, valExpr);
            }
            else {
                if (token.type == Lexer::Symbol) {
                    const std::string & name(token.text);
                    ByteCode::SharedPtrList args(readExprList());
                    return std::make_shared<FunctionApp>(name, args);
                }
                else {
                    throw UnknownSymbol(token.text);
                }
            }
        }
    }
    return std::make_shared<Literal>(Value::Null);
}

// -------------------------------------------------------------
ByteCode::SharedPtrList Parser::readExprList() {
    ByteCode::SharedPtrList forms;
    ByteCode::SharedPtr form = readExpr();
    while (form.get()) {
        forms.push_back(form);
        form = readExpr();
    }
    return forms;
}

// -------------------------------------------------------------
ByteCode::SharedPtrPairs Parser::readExprPairs() {
    ByteCode::SharedPtrPairs pairs;
    ByteCode::SharedPtrPair cons;
    ignoreLeftP(false);
    cons.first = readExpr();
    cons.second = readExpr();
    ignoreRightP();
    while (cons.first.get() && cons.second.get()) {
        pairs.push_back(cons);
        if (ignoreLeftP(true)) { break; }
        cons.first = readExpr();
        cons.second = readExpr();
        ignoreRightP();
    }
    return pairs;
}

// -------------------------------------------------------------
std::string Parser::readName() {
    auto nameToken = lexer_.next();
    if (nameToken.type != Lexer::Symbol) {
        throw UnexpectedExpression("name", nameToken.text);
    }
    return nameToken.text;
}

// -------------------------------------------------------------
ByteCode::ParamList Parser::readParams() {
    ByteCode::ParamList params;
    auto token = lexer_.next();
    if (token.type != Lexer::LeftP) {
        throw InvalidExpression("Expecting ( beginning of param list");
    }

    token = lexer_.next();
    while (token.type != Lexer::RightP) {
        if (token.type != Lexer::Symbol) {
            throw UnexpectedTokenType(token.text, token.type, "paramList");
        }
        params.push_back(std::move(token.text));
        token = lexer_.next();
    }
    return params;
}

// -------------------------------------------------------------
bool Parser::ignoreLeftP(bool allowRightP) {
    auto token = lexer_.next();
    if (token.type == Lexer::RightP && allowRightP) { return true; }
    if (token.type != Lexer::LeftP) {
        throw ExpectedParenthesis('(');
    }
    return false;
}

// -------------------------------------------------------------
void Parser::ignoreRightP() {
    if (lexer_.next().type != Lexer::RightP) {
        throw ExpectedParenthesis(')');
    }
}

// -------------------------------------------------------------
bool Parser::haveSExpression() const {
    size_t openParenthesis = 0;
    for (auto iter = lexer_.cbegin(); iter != lexer_.cend(); ++iter) {
        const auto & token = *iter;
        if (token.type == Lexer::LeftP) {
            ++openParenthesis;
        }
        else if (token.type == Lexer::RightP) {
            if (openParenthesis > 0) {
                --openParenthesis;
            }
        }

        if (openParenthesis == 0) {
            return true;
        }
    }

    return openParenthesis == 0;
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
