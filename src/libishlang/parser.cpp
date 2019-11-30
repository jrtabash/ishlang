#include "parser.h"
#include "exception.h"
#include "struct.h"
#include "util.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>

using namespace Ishlang;

// -------------------------------------------------------------
Parser::Parser()
    : lexer_()
{
    initAppFtns();
}

// -------------------------------------------------------------
CodeNode::SharedPtr Parser::read(const std::string &expr) {
    lexer_.read(expr);
    return readExpr();
}

// -------------------------------------------------------------
CodeNode::SharedPtr Parser::readLiteral(const std::string &expr) {
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

        CodeNode::SharedPtr code = readExpr();
        if (code.get()) {
            callback(code);
        }
    }
}

// -------------------------------------------------------------
CodeNode::SharedPtr Parser::readExpr() {
    while (!lexer_.empty()) {
        if (!haveSExpression()) {
            return CodeNode::SharedPtr();
        }

        auto token = lexer_.next();

        if (!token.text.empty()) {
            switch (token.type) {
            case Lexer::LeftP:
                return readApp();

            case Lexer::RightP:
                return CodeNode::SharedPtr();

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
CodeNode::SharedPtr Parser::makeLiteral(Lexer::TokenType type, const std::string &text) {
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
CodeNode::SharedPtr Parser::readApp(const std::string &expected) {
    if (!lexer_.empty()) {
        auto token = lexer_.next();

        if (!token.text.empty()) {
            if (!expected.empty() && token.text != expected) {
                throw UnexpectedExpression("lambda", token.text);
            }

            auto iter = appFtns_.find(token.text);
            if (iter != appFtns_.end()) {
                return iter->second();
            }
            else {
                if (token.type == Lexer::Symbol) {
                    const std::string & name(token.text);
                    CodeNode::SharedPtrList args(readExprList());
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
CodeNode::SharedPtrList Parser::readExprList() {
    CodeNode::SharedPtrList forms;
    CodeNode::SharedPtr form = readExpr();
    while (form.get()) {
        forms.push_back(form);
        form = readExpr();
    }
    return forms;
}

// -------------------------------------------------------------
CodeNode::SharedPtrPairs Parser::readExprPairs() {
    CodeNode::SharedPtrPairs pairs;
    CodeNode::SharedPtrPair cons;
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
CodeNode::ParamList Parser::readParams() {
    CodeNode::ParamList params;
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
void Parser::initAppFtns() {
    appFtns_ = {
        { "var",
          [this]() {
                const std::string name(readName());
                CodeNode::SharedPtr code(readExpr());
                ignoreRightP();
                return std::make_shared<Define>(name, code);
          }
        },

        { "=",
          [this]() {
                const std::string name(readName());
                CodeNode::SharedPtr code(readExpr());
                ignoreRightP();
                return std::make_shared<Assign>(name, code);
          }
        },

        { "?",
          [this]() {
                const std::string name(readName());
                ignoreRightP();
                return std::make_shared<Exists>(name);
          }
        },

        { "clone",
          [this]() {
                CodeNode::SharedPtr code(readExpr());
                ignoreRightP();
                return std::make_shared<Clone>(code);
          }
        },

        { "+", MakeBinaryExpression<ArithOp, ArithOp::Type>(*this, ArithOp::Add) },
        { "-", MakeBinaryExpression<ArithOp, ArithOp::Type>(*this, ArithOp::Sub) },
        { "*", MakeBinaryExpression<ArithOp, ArithOp::Type>(*this, ArithOp::Mul) },
        { "/", MakeBinaryExpression<ArithOp, ArithOp::Type>(*this, ArithOp::Div) },
        { "%", MakeBinaryExpression<ArithOp, ArithOp::Type>(*this, ArithOp::Mod) },
        { "^", MakeBinaryExpression<ArithOp, ArithOp::Type>(*this, ArithOp::Pow) },

        { "==", MakeBinaryExpression<CompOp, CompOp::Type>(*this, CompOp::EQ) },
        { "!=", MakeBinaryExpression<CompOp, CompOp::Type>(*this, CompOp::NE) },
        { "<",  MakeBinaryExpression<CompOp, CompOp::Type>(*this, CompOp::LT) },
        { ">",  MakeBinaryExpression<CompOp, CompOp::Type>(*this, CompOp::GT) },
        { "<=", MakeBinaryExpression<CompOp, CompOp::Type>(*this, CompOp::LE) },
        { ">=", MakeBinaryExpression<CompOp, CompOp::Type>(*this, CompOp::GE) },

        { "and", MakeBinaryExpression<LogicOp, LogicOp::Type>(*this, LogicOp::Conjunction) },
        { "or",  MakeBinaryExpression<LogicOp, LogicOp::Type>(*this, LogicOp::Disjunction) },

        { "not",
          [this]() {
                CodeNode::SharedPtr operand(readExpr());
                ignoreRightP();
                return std::make_shared<Not>(operand);
          }
        },

        { "progn",
          [this]() {
                CodeNode::SharedPtrList exprs(readExprList());
                return std::make_shared<ProgN>(exprs);
          }
        },

        { "block",
          [this]() {
                CodeNode::SharedPtrList exprs(readExprList());
                return std::make_shared<Block>(exprs);
          }
        },

        { "if",
          [this]() {
                CodeNode::SharedPtrList exprs(readExprList());
                if (exprs.size() == 2) {
                    return std::make_shared<If>(exprs[0], exprs[1]);
                }
                else if (exprs.size() == 3) {
                    return std::make_shared<If>(exprs[0], exprs[1], exprs[2]);
                }
                else {
                    throw InvalidExpression("Too many/few forms in if");
                }
          }
        },

        { "when",
          [this]() {
                CodeNode::SharedPtrList exprs(readExprList());
                if (exprs.size() == 2) {
                    return std::make_shared<If>(exprs[0], exprs[1]);
                }
                else {
                    throw InvalidExpression("Too many/fiew forms in when");
                }
          }
        },

        { "unless",
          [this]() {
                CodeNode::SharedPtrList exprs(readExprList());
                if (exprs.size() == 2) {
                    return std::make_shared<If>(std::make_shared<Not>(exprs[0]), exprs[1]);
                }
                else {
                    throw InvalidExpression("Too many/fiew forms in unless");
                }
          }
        },

        { "cond",
          [this]() {
                CodeNode::SharedPtrPairs pairs(readExprPairs());
                return std::make_shared<Cond>(pairs);
          }
        },

        { "break",
          [this]() {
                ignoreRightP();
                return std::make_shared<Break>();
          }
        },

        { "loop",
          [this]() {
                CodeNode::SharedPtrList forms(readExprList());
                if (forms.size() == 4) {
                    CodeNode::SharedPtrList::iterator iter = forms.begin();
                    CodeNode::SharedPtr decl(*iter++);
                    CodeNode::SharedPtr cond(*iter++);
                    CodeNode::SharedPtr next(*iter++);
                    CodeNode::SharedPtr body(*iter++);
                    return std::make_shared<Loop>(decl, cond, next, body);
                }
                else if (forms.size() == 2) {
                    CodeNode::SharedPtrList::iterator iter = forms.begin();
                    CodeNode::SharedPtr cond(*iter++);
                    CodeNode::SharedPtr body(*iter++);
                    return std::make_shared<Loop>(cond, body);
                }
                else {
                    throw InvalidExpression("Too many/few forms in loop");
                }
          }
        },

        { "lambda",
          [this]() {
                CodeNode::ParamList params(readParams());
                CodeNode::SharedPtrList exprs(readExprList());
                CodeNode::SharedPtr body(exprs.size() == 1 ? exprs[0] : std::make_shared<ProgN>(exprs));
                return std::make_shared<LambdaExpr>(params, body);
          }
        },

        { "defun", MakeFunctionExpression(*this) },
        { "def",   MakeFunctionExpression(*this) },

        { "(",
          [this]() {
                CodeNode::SharedPtr lambda(readApp("lambda"));
                CodeNode::SharedPtrList args(readExprList());
                return std::make_shared<LambdaApp>(lambda, args);
          }
        },

        { "istypeof",
          [this]() {
                CodeNode::SharedPtr form(readExpr());
                Value::Type type(Value::stringToType(readName()));
                ignoreRightP();
                return std::make_shared<IsType>(form, type);
          }
        },

        { "typename",
          [this]() {
                CodeNode::SharedPtrList exprs(readExprList());
                if (exprs.size() == 1) {
                    return std::make_shared<TypeName>(exprs[0]);
                }
                else {
                    throw InvalidExpression("Too many/few arguments to typename");
                }
          }
        },

        { "print",
          [this]() {
                CodeNode::SharedPtr pExpr(readExpr());
                ignoreRightP();
                return std::make_shared<Print>(false, pExpr);
          }
        },

        { "println",
          [this]() {
                CodeNode::SharedPtr pExpr(readExpr());
                ignoreRightP();
                return std::make_shared<Print>(true, pExpr);
          }
        },

        { "read",
          [this]() {
                ignoreRightP();
                return std::make_shared<Read>();
          }
        },

        { "struct",
          [this]() {
                const std::string name(readName());
                const Struct::MemberList members(readParams());
                ignoreRightP();
                return std::make_shared<StructExpr>(name, members);
          }
        },

        { "isstructname",
          [this]() {
                CodeNode::SharedPtr snExpr(readExpr());
                const std::string name(readName());
                ignoreRightP();
                return std::make_shared<IsStructName>(snExpr, name);
          }
        },

        { "structname",
          [this]() {
                CodeNode::SharedPtrList exprs(readExprList());
                if (exprs.size() == 1) {
                    return std::make_shared<StructName>(exprs[0]);
                }
                else {
                    throw InvalidExpression("Too many/few arguments to structname");
                }
          }
        },

        { "makeinstance",
          [this]() {
                const std::string name(readName());
                ignoreRightP();
                return std::make_shared<MakeInstance>(name);
          }
        },

        { "isinstanceof",
          [this]() {
                CodeNode::SharedPtr ioExpr(readExpr());
                const std::string name(readName());
                ignoreRightP();
                return std::make_shared<IsInstanceOf>(ioExpr, name);
          }
        },

        { "get",
          [this]() {
                CodeNode::SharedPtr instExpr(readExpr());
                const std::string name(readName());
                ignoreRightP();
                return std::make_shared<GetMember>(instExpr, name);
          }
        },

        { "set",
          [this]() {
                CodeNode::SharedPtr instExpr(readExpr());
                const std::string name(readName());
                CodeNode::SharedPtr valueExpr(readExpr());
                ignoreRightP();
                return std::make_shared<SetMember>(instExpr, name, valueExpr);
          }
        },

        { "strlen",
          [this]() {
                CodeNode::SharedPtr strExpr(readExpr());
                ignoreRightP();
                return std::make_shared<StringLen>(strExpr);
          }
        },

        { "getchar",
          [this]() {
                CodeNode::SharedPtr strExpr(readExpr());
                CodeNode::SharedPtr posExpr(readExpr());
                ignoreRightP();
                return std::make_shared<GetCharAt>(strExpr, posExpr);
          }
        },

        { "setchar",
          [this]() {
                CodeNode::SharedPtr strExpr(readExpr());
                CodeNode::SharedPtr posExpr(readExpr());
                CodeNode::SharedPtr valExpr(readExpr());
                ignoreRightP();
                return std::make_shared<SetCharAt>(strExpr, posExpr, valExpr);
          }
        },

        { "strcat",
          [this]() {
                CodeNode::SharedPtr strExpr(readExpr());
                CodeNode::SharedPtr otherExpr(readExpr());
                ignoreRightP();
                return std::make_shared<StringCat>(strExpr, otherExpr);
          }
        },

        { "substr",
          [this]() {
                CodeNode::SharedPtrList exprs(readExprList());
                if (exprs.size() == 2) {
                    return std::make_shared<SubString>(exprs[0], exprs[1]);
                }
                else if (exprs.size() == 3) {
                    return std::make_shared<SubString>(exprs[0], exprs[1], exprs[2]);
                }
                else {
                    throw InvalidExpression("Too many/few arguments to substr");
                }
          }
        },

        { "strfind",
          [this]() {
                CodeNode::SharedPtrList exprs(readExprList());
                if (exprs.size() == 2) {
                    return std::make_shared<StringFind>(exprs[0], exprs[1]);
                }
                else if (exprs.size() == 3) {
                    return std::make_shared<StringFind>(exprs[0], exprs[1], exprs[2]);
                }
                else {
                    throw InvalidExpression("Too many/few arguments to strfind");
                }
          }
        },

        { "array",
          [this]() {
                CodeNode::SharedPtrList valueExprs(readExprList());
                if (valueExprs.size() == 0) {
                    return std::make_shared<MakeArray>();
                }
                else {
                    return std::make_shared<MakeArray>(valueExprs);
                }
          }
        },

        { "arraysv",
          [this]() {
                CodeNode::SharedPtrList exprs(readExprList());
                if (exprs.size() == 1) {
                    return std::make_shared<MakeArraySV>(exprs[0]);
                }
                else if (exprs.size() == 2) {
                    return std::make_shared<MakeArraySV>(exprs[0], exprs[1]);
                }
                else {
                    throw InvalidExpression("Too many/few arguments to arraysv");
                }
          }
        },

        { "arrlen",
          [this]() {
                CodeNode::SharedPtr strExpr(readExpr());
                ignoreRightP();
                return std::make_shared<ArrayLen>(strExpr);
          }
        },

        { "arrget",
          [this]() {
                CodeNode::SharedPtr arrExpr(readExpr());
                CodeNode::SharedPtr posExpr(readExpr());
                ignoreRightP();
                return std::make_shared<ArrayGet>(arrExpr, posExpr);
          }
        },

        { "arrset",
          [this]() {
                CodeNode::SharedPtr arrExpr(readExpr());
                CodeNode::SharedPtr posExpr(readExpr());
                CodeNode::SharedPtr valExpr(readExpr());
                ignoreRightP();
                return std::make_shared<ArraySet>(arrExpr, posExpr, valExpr);
          }
        },

        { "arradd",
          [this]() {
                CodeNode::SharedPtr arrExpr(readExpr());
                CodeNode::SharedPtr valExpr(readExpr());
                ignoreRightP();
                return std::make_shared<ArrayAdd>(arrExpr, valExpr);
          }
        }
    };
}
