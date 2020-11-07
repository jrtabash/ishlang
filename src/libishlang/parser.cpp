#include "parser.h"
#include "exception.h"
#include "struct.h"
#include "util.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <fstream>

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

        auto code = readExpr();
        if (code.get()) {
            callback(code);
        }
    }
}

// -------------------------------------------------------------
void Parser::readFile(const std::string &filename, CallBack callback) {
    std::ifstream ifs(filename.c_str());
    if (!ifs.is_open()) {
        throw UnknownFile(filename);
    }

    unsigned lineNo = 0;
    try {
        std::string line;
        while (std::getline(ifs, line)) {
            ++lineNo;
            readMulti(line, callback);
        }

        if (hasIncompleteExpr()) {
            clearIncompleteExpr();
            throw IncompleteExpression("Incomplete code at end of file " + filename);
        }

        ifs.close();
    }
    catch (Exception &ex) {
        ifs.close();
        ex.setFileContext(filename, lineNo);
        throw;
    }
    catch (...) {
        ifs.close();
        throw;
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
                    const auto & name(token.text);
                    auto args(readExprList());
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
    auto form = readExpr();
    while (form.get()) {
        forms.push_back(form);
        form = readExpr();
    }
    return forms;
}

// -------------------------------------------------------------
CodeNode::SharedPtrList Parser::readAndCheckExprList(const char *name, std::size_t expectedSize) {
    auto exprs(readExprList());
    if (exprs.size() != expectedSize) {
        throw TooManyOrFewForms(name);
    }
    return exprs;
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
CodeNode::NameAndAsList Parser::readNameAndAsList() {
    // Parse following format: "name [as asName] [name [as asName]]*"
    // Examples:
    //   1) foo
    //   2) foo as bar
    //   3) one two
    //   4) one as single two as double
    //   5) one two foo as bar

    CodeNode::NameAndAsList nameAndAsList;

    auto getNext =
        [this]() {
            auto token = lexer_.next();
            if (token.type != Lexer::Symbol) {
                throw UnexpectedTokenType(token.text, token.type, "name/as list");
            }
            return token;
        };

    auto name = getNext();

    while (name.type != Lexer::RightP) {
        if (name.text == "as") {
            throw InvalidExpression("Misformed name/as list");
        }

        auto maybeAs = lexer_.next();
        if (maybeAs.text == "as") {
            auto asName = getNext();
            nameAndAsList.emplace_back(name.text, asName.text);
            name = lexer_.next();
        }
        else {
            nameAndAsList.emplace_back(name.text, std::nullopt);
            name = maybeAs;
        }
    }

    return nameAndAsList;
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
        { "import",
          [this]() {
              const auto nameAndAsList = readNameAndAsList();
              if (nameAndAsList.size() == 1) {
                  return std::make_shared<ImportModule>(nameAndAsList[0].first,
                                                        nameAndAsList[0].second ? *nameAndAsList[0].second : "");
              }
              else {
                  throw InvalidExpression("Misformed import");
              }
          }
        },

        { "from",
          [this]() {
              const auto name = readName();
              const auto import = readName();
              if (import != "import") {
                  throw InvalidExpression("Misformed from/import");
              }

              const auto nameAndAsList = readNameAndAsList();
              if (nameAndAsList.size() > 0) {
                  return std::make_shared<FromModuleImport>(name, nameAndAsList);
              }
              else {
                  throw InvalidExpression("Misformed from/import");
              }
          }
        },

        { "var",
          [this]() {
                const auto name(readName());
                auto expr(readAndCheckExprList("var", 1));
                return std::make_shared<Define>(name, expr[0]);
          }
        },

        { "=",
          [this]() {
                const auto name(readName());
                auto expr(readAndCheckExprList("=", 1));
                return std::make_shared<Assign>(name, expr[0]);
          }
        },

        { "?",
          [this]() {
                const auto name(readName());
                ignoreRightP();
                return std::make_shared<Exists>(name);
          }
        },

        { "clone",
          [this]() {
                auto expr(readAndCheckExprList("clone", 1));
                return std::make_shared<Clone>(expr[0]);
          }
        },

        { "+", MakeBinaryExpression<ArithOp, ArithOp::Type>("+", *this, ArithOp::Add) },
        { "-", MakeBinaryExpression<ArithOp, ArithOp::Type>("-", *this, ArithOp::Sub) },
        { "*", MakeBinaryExpression<ArithOp, ArithOp::Type>("*", *this, ArithOp::Mul) },
        { "/", MakeBinaryExpression<ArithOp, ArithOp::Type>("/", *this, ArithOp::Div) },
        { "%", MakeBinaryExpression<ArithOp, ArithOp::Type>("%", *this, ArithOp::Mod) },
        { "^", MakeBinaryExpression<ArithOp, ArithOp::Type>("^", *this, ArithOp::Pow) },

        { "==", MakeBinaryExpression<CompOp, CompOp::Type>("==", *this, CompOp::EQ) },
        { "!=", MakeBinaryExpression<CompOp, CompOp::Type>("!=", *this, CompOp::NE) },
        { "<",  MakeBinaryExpression<CompOp, CompOp::Type>("<",  *this, CompOp::LT) },
        { ">",  MakeBinaryExpression<CompOp, CompOp::Type>("<",  *this, CompOp::GT) },
        { "<=", MakeBinaryExpression<CompOp, CompOp::Type>("<=", *this, CompOp::LE) },
        { ">=", MakeBinaryExpression<CompOp, CompOp::Type>(">=", *this, CompOp::GE) },

        { "and", MakeBinaryExpression<LogicOp, LogicOp::Type>("and", *this, LogicOp::Conjunction) },
        { "or",  MakeBinaryExpression<LogicOp, LogicOp::Type>("or",  *this, LogicOp::Disjunction) },

        { "not",
          [this]() {
                auto expr(readAndCheckExprList("not", 1));
                return std::make_shared<Not>(expr[0]);
          }
        },

        { "neg",
          [this]() {
              auto expr(readAndCheckExprList("neg", 1));
              return std::make_shared<NegativeOf>(expr[0]);
          }
        },

        { "progn",
          [this]() {
                auto exprs(readExprList());
                return std::make_shared<ProgN>(exprs);
          }
        },

        { "block",
          [this]() {
                auto exprs(readExprList());
                return std::make_shared<Block>(exprs);
          }
        },

        { "if",
          [this]() {
                auto exprs(readExprList());
                if (exprs.size() == 2) {
                    return std::make_shared<If>(exprs[0], exprs[1]);
                }
                else if (exprs.size() == 3) {
                    return std::make_shared<If>(exprs[0], exprs[1], exprs[2]);
                }
                else {
                    throw TooManyOrFewForms("if");
                }
          }
        },

        { "when",
          [this]() {
                auto exprs(readAndCheckExprList("when", 2));
                return std::make_shared<If>(exprs[0], exprs[1]);
          }
        },

        { "unless",
          [this]() {
                auto exprs(readAndCheckExprList("unless", 2));
                return std::make_shared<If>(std::make_shared<Not>(exprs[0]), exprs[1]);
          }
        },

        { "cond",
          [this]() {
                auto pairs(readExprPairs());
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
                auto forms(readExprList());
                if (forms.size() == 4) {
                    auto iter = forms.begin();
                    auto decl(*iter++);
                    auto cond(*iter++);
                    auto next(*iter++);
                    auto body(*iter++);
                    return std::make_shared<Loop>(decl, cond, next, body);
                }
                else if (forms.size() == 2) {
                    auto iter = forms.begin();
                    auto cond(*iter++);
                    auto body(*iter++);
                    return std::make_shared<Loop>(cond, body);
                }
                else {
                    throw TooManyOrFewForms("loop");
                }
          }
        },

        { "lambda",
          [this]() {
                auto params(readParams());
                auto exprs(readExprList());
                auto body(exprs.size() == 1
                          ? exprs[0]
                          : std::make_shared<ProgN>(exprs));
                return std::make_shared<LambdaExpr>(params, body);
          }
        },

        { "defun",
          [this]() {
                const auto name(readName());
                auto params(readParams());
                auto exprs(readExprList());
                auto body(exprs.size() == 1
                          ? exprs[0]
                          : std::make_shared<ProgN>(exprs));
                return std::make_shared<FunctionExpr>(name, params, body);
          }
        },

        { "(",
          [this]() {
                auto lambda(readApp("lambda"));
                auto args(readExprList());
                return std::make_shared<LambdaApp>(lambda, args);
          }
        },

        { "istypeof",
          [this]() {
                auto form(readExpr());
                auto type(Value::stringToType(readName()));
                ignoreRightP();
                return std::make_shared<IsType>(form, type);
          }
        },

        { "typename",
          [this]() {
                auto exprs(readAndCheckExprList("typename", 1));
                return std::make_shared<TypeName>(exprs[0]);
          }
        },

        { "astype",
          [this]() {
                auto form(readExpr());
                auto type(Value::stringToType(readName()));
                ignoreRightP();
                return std::make_shared<AsType>(form, type);
          }
        },

        { "print",
          [this]() {
                auto exprs(readAndCheckExprList("print", 1));
                return std::make_shared<Print>(false, exprs[0]);
          }
        },

        { "println",
          [this]() {
                auto exprs(readAndCheckExprList("println", 1));
                return std::make_shared<Print>(true, exprs[0]);
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
                const auto name(readName());
                const auto members(readParams());
                ignoreRightP();
                return std::make_shared<StructExpr>(name, members);
          }
        },

        { "isstructname",
          [this]() {
                auto snExpr(readExpr());
                const auto name(readName());
                ignoreRightP();
                return std::make_shared<IsStructName>(snExpr, name);
          }
        },

        { "structname",
          [this]() {
                auto exprs(readAndCheckExprList("structname", 1));
                return std::make_shared<StructName>(exprs[0]);
          }
        },

        { "makeinstance",
          [this]() {
                const auto name(readName());
                ignoreRightP();
                return std::make_shared<MakeInstance>(name);
          }
        },

        { "isinstanceof",
          [this]() {
                auto ioExpr(readExpr());
                const auto name(readName());
                ignoreRightP();
                return std::make_shared<IsInstanceOf>(ioExpr, name);
          }
        },

        { "memget",
          [this]() {
                auto instExpr(readExpr());
                const auto name(readName());
                ignoreRightP();
                return std::make_shared<GetMember>(instExpr, name);
          }
        },

        { "memset",
          [this]() {
                auto instExpr(readExpr());
                const auto name(readName());
                auto valueExpr(readExpr());
                ignoreRightP();
                return std::make_shared<SetMember>(instExpr, name, valueExpr);
          }
        },

        { "strlen",
          [this]() {
                auto exprs(readAndCheckExprList("strlen", 1));
                return std::make_shared<StringLen>(exprs[0]);
          }
        },

        { "strget",
          [this]() {
                auto exprs(readAndCheckExprList("strget", 2));
                return std::make_shared<StringGet>(exprs[0], exprs[1]);
          }
        },

        { "strset",
          [this]() {
                auto exprs(readAndCheckExprList("strset", 3));
                return std::make_shared<StringSet>(exprs[0], exprs[1], exprs[2]);
          }
        },

        { "strcat",
          [this]() {
                auto exprs(readAndCheckExprList("strcat", 2));
                return std::make_shared<StringCat>(exprs[0], exprs[1]);
          }
        },

        { "substr",
          [this]() {
                auto exprs(readExprList());
                if (exprs.size() == 2) {
                    return std::make_shared<SubString>(exprs[0], exprs[1]);
                }
                else if (exprs.size() == 3) {
                    return std::make_shared<SubString>(exprs[0], exprs[1], exprs[2]);
                }
                else {
                    throw TooManyOrFewForms("substr");
                }
          }
        },

        { "strfind",
          [this]() {
                auto exprs(readExprList());
                if (exprs.size() == 2) {
                    return std::make_shared<StringFind>(exprs[0], exprs[1]);
                }
                else if (exprs.size() == 3) {
                    return std::make_shared<StringFind>(exprs[0], exprs[1], exprs[2]);
                }
                else {
                    throw TooManyOrFewForms("strfind");
                }
          }
        },

        { "strcount",
          [this]() {
                auto exprs(readAndCheckExprList("strcount", 2));
                return std::make_shared<StringCount>(exprs[0], exprs[1]);
          }
        },

        { "array",
          [this]() {
                auto valueExprs(readExprList());
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
                auto exprs(readExprList());
                if (exprs.size() == 1) {
                    return std::make_shared<MakeArraySV>(exprs[0]);
                }
                else if (exprs.size() == 2) {
                    return std::make_shared<MakeArraySV>(exprs[0], exprs[1]);
                }
                else {
                    throw TooManyOrFewForms("arraysv");
                }
          }
        },

        { "arrlen",
          [this]() {
                auto exprs(readAndCheckExprList("arrlen", 1));
                return std::make_shared<ArrayLen>(exprs[0]);
          }
        },

        { "arrget",
          [this]() {
                auto exprs(readAndCheckExprList("arrget", 2));
                return std::make_shared<ArrayGet>(exprs[0], exprs[1]);
          }
        },

        { "arrset",
          [this]() {
                auto exprs(readAndCheckExprList("arrset", 3));
                return std::make_shared<ArraySet>(exprs[0], exprs[1], exprs[2]);
          }
        },

        { "arradd",
          [this]() {
                auto exprs(readAndCheckExprList("arradd", 2));
                return std::make_shared<ArrayAdd>(exprs[0], exprs[1]);
          }
        },

        { "arrfind",
          [this]() {
                auto exprs(readExprList());
                if (exprs.size() == 2) {
                    return std::make_shared<ArrayFind>(exprs[0], exprs[1]);
                }
                else if (exprs.size() == 3) {
                    return std::make_shared<ArrayFind>(exprs[0], exprs[1], exprs[2]);
                }
                else {
                    throw TooManyOrFewForms("arrfind");
                }
          }
        },

        { "arrcount",
          [this]() {
                auto exprs(readAndCheckExprList("arrcount", 2));
                return std::make_shared<ArrayCount>(exprs[0], exprs[1]);
          }
        },

        { "isupper", MakeStrCharOp<StrCharCheck>("isupper", *this, StrCharCheck::Upper) },
        { "islower", MakeStrCharOp<StrCharCheck>("islower", *this, StrCharCheck::Lower) },
        { "isalpha", MakeStrCharOp<StrCharCheck>("isalpha", *this, StrCharCheck::Alpha) },
        { "isnumer", MakeStrCharOp<StrCharCheck>("isnumer", *this, StrCharCheck::Numer) },
        { "isalnum", MakeStrCharOp<StrCharCheck>("isalnum", *this, StrCharCheck::Alnum) },
        { "ispunct", MakeStrCharOp<StrCharCheck>("ispunct", *this, StrCharCheck::Punct) },
        { "isspace", MakeStrCharOp<StrCharCheck>("isspace", *this, StrCharCheck::Space) },

        { "toupper", MakeStrCharOp<StrCharTransform>("toupper", *this, StrCharTransform::ToUpper) },
        { "tolower", MakeStrCharOp<StrCharTransform>("tolower", *this, StrCharTransform::ToLower) },

        { "rand",
          [this]() {
              auto exprs(readExprList());
              if (exprs.size() > 1) {
                  throw TooManyOrFewForms("rand");
              }
              return std::make_shared<Random>(exprs.size() == 1 ? exprs[0] : CodeNode::SharedPtr());
          }
        },

        { "hash",
          [this]() {
              auto exprs(readAndCheckExprList("hash", 1));
              return std::make_shared<Hash>(exprs[0]);
          }
        }
    };
}
