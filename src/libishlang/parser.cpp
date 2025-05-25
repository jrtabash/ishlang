#include "parser.h"
#include "exception.h"
#include "struct.h"
#include "util.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iterator>
#include <ranges>

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

    return CodeNode::make<Literal>(Value(expr));
}

// -------------------------------------------------------------
void Parser::readMulti(const std::string &expr, CallBack callback) {
    lexer_.read(expr);
    while (!lexer_.empty()) {
        if (!haveSExpression()) {
            return;
        }

        auto code = readExpr();
        if (code) {
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
                return CodeNode::make<Variable>(token.text);

            case Lexer::Unknown:
                throw UnknownTokenType(token.text, static_cast<char>(token.type));
                break;
            }
        }
    }

    return CodeNode::make<Literal>(Value::Null);
}

// -------------------------------------------------------------
CodeNode::SharedPtr Parser::makeLiteral(Lexer::TokenType type, const std::string &text) {
    switch (type) {
    case Lexer::Char:
        return CodeNode::make<Literal>(Value(text[1]));

    case Lexer::String:
        return CodeNode::make<Literal>(Value(std::string(text.c_str() + 1, text.size() - 2)));

    case Lexer::Int:
        return CodeNode::make<Literal>(Value(Value::Long(std::stoll(text, 0, 10))));

    case Lexer::Real:
        return CodeNode::make<Literal>(Value(std::stod(text, 0)));

    case Lexer::Bool:
        return CodeNode::make<Literal>((Value(Value::Bool(text == "true"))));

    default:
        break;
    }

    return CodeNode::make<Literal>(Value::Null);
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
                    return CodeNode::make<FunctionApp>(name, args);
                }
                else {
                    throw UnknownSymbol(token.text);
                }
            }
        }
    }
    return CodeNode::make<Literal>(Value::Null);
}

// -------------------------------------------------------------
CodeNode::SharedPtrList Parser::readExprList() {
    CodeNode::SharedPtrList forms;
    auto form = readExpr();
    while (form) {
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
CodeNode::SharedPtrList Parser::readAndCheckRangeExprList(const char *name, std::size_t minExpectedSize, std::optional<std::size_t> maxExpectedSize) {
    auto exprs(readExprList());
    if (exprs.size() < minExpectedSize || (maxExpectedSize && exprs.size() > *maxExpectedSize)) {
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
    while (cons.first && cons.second) {
        pairs.push_back(cons);
        if (ignoreLeftP(true)) { break; }
        cons.first = readExpr();
        cons.second = readExpr();
        ignoreRightP();
    }
    return pairs;
}

// -------------------------------------------------------------
CodeNode::NameSharedPtrs Parser::readNameExprPairs() {
    CodeNode::NameSharedPtrs nameExprs;
    std::pair<std::string, CodeNode::SharedPtr> cons;
    bool rpseen = false;
    if (lexer_.peek().type == Lexer::LeftP) {
        ignoreLeftP(false);
        cons.first = readName();
        cons.second = readExpr();
        ignoreRightP();
        while (cons.second) {
            nameExprs.push_back(cons);
            if (ignoreLeftP(true)) {
                rpseen = true;
                break;
            }
            cons.first = readName();
            cons.second = readExpr();
            ignoreRightP();
        }
    }
    if (!rpseen && lexer_.peek().type == Lexer::RightP) {
        ignoreRightP();
    }
    return nameExprs;
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
std::vector<std::string> Parser::readNames(const char *listName, std::size_t minExpectedSize) {
    std::vector<std::string> names;
    auto nameToken = lexer_.next();
    while (nameToken.type != Lexer::RightP) {
        if (nameToken.type != Lexer::Symbol) {
            throw UnexpectedExpression("name", nameToken.text);
        }
        names.push_back(std::move(nameToken.text));
        nameToken = lexer_.next();
    }

    if (names.size() < minExpectedSize) {
        throw TooManyOrFewForms(listName);
    }

    return names;
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
                  return CodeNode::make<ImportModule>(nameAndAsList[0].first,
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
                  return CodeNode::make<FromModuleImport>(name, nameAndAsList);
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
              return CodeNode::make<Define>(name, expr[0]);
          }
        },

        { "=",
          [this]() {
              const auto name(readName());
              auto expr(readAndCheckExprList("=", 1));
              return CodeNode::make<Assign>(name, expr[0]);
          }
        },

        { "?",
          [this]() {
              const auto name(readName());
              ignoreRightP();
              return CodeNode::make<Exists>(name);
          }
        },

        { "clone",
          [this]() {
              auto expr(readAndCheckExprList("clone", 1));
              return CodeNode::make<Clone>(expr[0]);
          }
        },

        { "+", MakeVariadicExpression<ArithOp>("+", *this, ArithOp::Add) },
        { "-", MakeVariadicExpression<ArithOp>("-", *this, ArithOp::Sub) },
        { "*", MakeVariadicExpression<ArithOp>("*", *this, ArithOp::Mul) },
        { "/", MakeVariadicExpression<ArithOp>("/", *this, ArithOp::Div) },
        { "%", MakeVariadicExpression<ArithOp>("%", *this, ArithOp::Mod) },
        { "^", MakeVariadicExpression<ArithOp>("^", *this, ArithOp::Pow) },

        { "==", MakeBinaryExpression<CompOp>("==", *this, CompOp::EQ) },
        { "!=", MakeBinaryExpression<CompOp>("!=", *this, CompOp::NE) },
        { "<",  MakeBinaryExpression<CompOp>("<",  *this, CompOp::LT) },
        { ">",  MakeBinaryExpression<CompOp>("<",  *this, CompOp::GT) },
        { "<=", MakeBinaryExpression<CompOp>("<=", *this, CompOp::LE) },
        { ">=", MakeBinaryExpression<CompOp>(">=", *this, CompOp::GE) },

        { "and", MakeVariadicExpression<LogicOp>("and", *this, LogicOp::Conjunction) },
        { "or",  MakeVariadicExpression<LogicOp>("or", *this, LogicOp::Disjunction) },

        { "not",
          [this]() {
              auto expr(readAndCheckExprList("not", 1));
              return CodeNode::make<Not>(expr[0]);
          }
        },

        { "neg",
          [this]() {
              auto expr(readAndCheckExprList("neg", 1));
              return CodeNode::make<NegativeOf>(expr[0]);
          }
        },

        { "progn",
          [this]() {
              auto exprs(readExprList());
              return CodeNode::make<ProgN>(exprs);
          }
        },

        { "block",
          [this]() {
              auto exprs(readExprList());
              return CodeNode::make<Block>(exprs);
          }
        },

        { "if",
          [this]() {
              auto exprs(readExprList());
              if (exprs.size() == 2) {
                  return CodeNode::make<If>(exprs[0], exprs[1]);
              }
              else if (exprs.size() == 3) {
                  return CodeNode::make<If>(exprs[0], exprs[1], exprs[2]);
              }
              else {
                  throw TooManyOrFewForms("if");
              }
          }
        },

        { "when",
          [this]() {
              auto exprs(readAndCheckExprList("when", 2));
              return CodeNode::make<If>(exprs[0], exprs[1]);
          }
        },

        { "unless",
          [this]() {
              auto exprs(readAndCheckExprList("unless", 2));
              return CodeNode::make<If>(CodeNode::make<Not>(exprs[0]), exprs[1]);
          }
        },

        { "cond",
          [this]() {
              auto pairs(readExprPairs());
              return CodeNode::make<Cond>(pairs);
          }
        },

        { "break",
          [this]() {
              ignoreRightP();
              return CodeNode::make<Break>();
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
                  return CodeNode::make<Loop>(decl, cond, next, body);
              }
              else if (forms.size() == 2) {
                  auto iter = forms.begin();
                  auto cond(*iter++);
                  auto body(*iter++);
                  return CodeNode::make<Loop>(cond, body);
              }
              else {
                  throw TooManyOrFewForms("loop");
              }
          }
        },

        { "foreach",
          [this]() {
              const auto name(readName());
              auto forms(readAndCheckExprList("foreach", 2));
              return CodeNode::make<Foreach>(name, forms[0], forms[1]);
          }
        },

        { "lambda",
          [this]() {
              auto params(readParams());
              auto exprs(readExprList());
              auto body(exprs.size() == 1
                        ? exprs[0]
                        : CodeNode::make<ProgN>(exprs));
              return CodeNode::make<LambdaExpr>(params, body);
          }
        },

        { "defun",
          [this]() {
              const auto name(readName());
              auto params(readParams());
              auto exprs(readExprList());
              auto body(exprs.size() == 1
                        ? exprs[0]
                        : CodeNode::make<ProgN>(exprs));
              return CodeNode::make<FunctionExpr>(name, params, body);
          }
        },

        { "(",
          [this]() {
              auto lambda(readApp("lambda"));
              auto args(readExprList());
              return CodeNode::make<LambdaApp>(lambda, args);
          }
        },

        { "istypeof",
          [this]() {
              auto form(readExpr());
              Value::TypeList types;
              std::ranges::transform(readNames("istypeof", 1), std::back_inserter(types), Value::stringToType);
              return CodeNode::make<IsType>(form, types);
          }
        },

        { "typename",
          [this]() {
              auto exprs(readAndCheckExprList("typename", 1));
              return CodeNode::make<TypeName>(exprs[0]);
          }
        },

        { "astype",
          [this]() {
              auto form(readExpr());
              auto type(Value::stringToType(readName()));
              ignoreRightP();
              return CodeNode::make<AsType>(form, type);
          }
        },

        { "assert",
          [this]() {
              const auto tag(readName());
              const auto exprs(readAndCheckExprList("assert", 1));
              return CodeNode::make<Assert>(tag, exprs[0]);
          }
        },

        { "print",
          [this]() {
              auto exprs(readAndCheckRangeExprList("print", 1, std::nullopt));
              return CodeNode::make<Print>(false, exprs);
          }
        },

        { "println",
          [this]() {
              auto exprs(readAndCheckRangeExprList("println", 1, std::nullopt));
              return CodeNode::make<Print>(true, exprs);
          }
        },

        { "read",
          [this]() {
              ignoreRightP();
              return CodeNode::make<Read>();
          }
        },

        { "struct",
          [this]() {
              const auto name(readName());
              const auto members(readParams());
              ignoreRightP();
              return CodeNode::make<StructExpr>(name, members);
          }
        },

        { "isstructname",
          [this]() {
              auto snExpr(readExpr());
              const auto name(readName());
              ignoreRightP();
              return CodeNode::make<IsStructName>(snExpr, name);
          }
        },

        { "structname",
          [this]() {
              auto exprs(readAndCheckExprList("structname", 1));
              return CodeNode::make<StructName>(exprs[0]);
          }
        },

        { "makeinstance",
          [this]() {
              const auto name(readName());
              const auto initArgs = readNameExprPairs();
              return CodeNode::make<MakeInstance>(name, initArgs);
          }
        },

        { "isinstanceof",
          [this]() {
              auto ioExpr(readExpr());
              const auto name(readName());
              ignoreRightP();
              return CodeNode::make<IsInstanceOf>(ioExpr, name);
          }
        },

        { "memget",
          [this]() {
              auto instExpr(readExpr());
              const auto name(readName());
              ignoreRightP();
              return CodeNode::make<GetMember>(instExpr, name);
          }
        },

        { "memset",
          [this]() {
              auto instExpr(readExpr());
              const auto name(readName());
              auto valueExpr(readExpr());
              ignoreRightP();
              return CodeNode::make<SetMember>(instExpr, name, valueExpr);
          }
        },

        { "strlen",
          [this]() {
              auto exprs(readAndCheckExprList("strlen", 1));
              return CodeNode::make<StringLen>(exprs[0]);
          }
        },

        { "strget",
          [this]() {
              auto exprs(readAndCheckExprList("strget", 2));
              return CodeNode::make<StringGet>(exprs[0], exprs[1]);
          }
        },

        { "strset",
          [this]() {
              auto exprs(readAndCheckExprList("strset", 3));
              return CodeNode::make<StringSet>(exprs[0], exprs[1], exprs[2]);
          }
        },

        { "strcat",
          [this]() {
              auto exprs(readAndCheckExprList("strcat", 2));
              return CodeNode::make<StringCat>(exprs[0], exprs[1]);
          }
        },

        { "substr",
          [this]() {
              auto exprs(readExprList());
              if (exprs.size() == 2) {
                  return CodeNode::make<SubString>(exprs[0], exprs[1]);
              }
              else if (exprs.size() == 3) {
                  return CodeNode::make<SubString>(exprs[0], exprs[1], exprs[2]);
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
                  return CodeNode::make<StringFind>(exprs[0], exprs[1]);
              }
              else if (exprs.size() == 3) {
                  return CodeNode::make<StringFind>(exprs[0], exprs[1], exprs[2]);
              }
              else {
                  throw TooManyOrFewForms("strfind");
              }
          }
        },

        { "strcount",
          [this]() {
              auto exprs(readAndCheckExprList("strcount", 2));
              return CodeNode::make<StringCount>(exprs[0], exprs[1]);
          }
        },

        { "strcmp",
          [this]() {
              auto exprs(readAndCheckExprList("strcmp", 2));
              return CodeNode::make<StringCompare>(exprs[0], exprs[1]);
          }
        },

        { "strsort",
          [this]() {
              auto exprs(readAndCheckRangeExprList("strsort", 1, 2));
              return CodeNode::make<StringSort>(exprs[0], exprs.size() == 2 ? exprs[1] : CodeNode::SharedPtr());
          }
        },

        { "strrev",
          [this] {
              auto exprs(readAndCheckExprList("strrev", 1));
              return CodeNode::make<StringReverse>(exprs[0]);
          }
        },

        { "strsplit",
          [this]() {
              auto exprs(readAndCheckExprList("strsplit", 2));
              return CodeNode::make<StringSplit>(exprs[0], exprs[1]);
          }
        },

        { "array",
          [this]() {
              auto valueExprs(readExprList());
              if (valueExprs.size() == 0) {
                  return CodeNode::make<MakeArray>();
              }
              else {
                  return CodeNode::make<MakeArray>(valueExprs);
              }
          }
        },

        { "arraysv",
          [this]() {
              auto exprs(readExprList());
              if (exprs.size() == 1) {
                  return CodeNode::make<MakeArraySV>(exprs[0]);
              }
              else if (exprs.size() == 2) {
                  return CodeNode::make<MakeArraySV>(exprs[0], exprs[1]);
              }
              else {
                  throw TooManyOrFewForms("arraysv");
              }
          }
        },

        { "arraysg",
          [this]() {
              auto exprs(readAndCheckExprList("arraysg", 2));
              return CodeNode::make<MakeArraySG>(exprs[0], exprs[1]);
          }
        },

        { "arrlen",
          [this]() {
              auto exprs(readAndCheckExprList("arrlen", 1));
              return CodeNode::make<ArrayLen>(exprs[0]);
          }
        },

        { "arrget",
          [this]() {
              auto exprs(readAndCheckExprList("arrget", 2));
              return CodeNode::make<ArrayGet>(exprs[0], exprs[1]);
          }
        },

        { "arrset",
          [this]() {
              auto exprs(readAndCheckExprList("arrset", 3));
              return CodeNode::make<ArraySet>(exprs[0], exprs[1], exprs[2]);
          }
        },

        { "arrpush",
          [this]() {
              auto exprs(readAndCheckExprList("arrpush", 2));
              return CodeNode::make<ArrayPush>(exprs[0], exprs[1]);
          }
        },

        { "arrpop",
          [this]() {
              auto exprs(readAndCheckExprList("arrpop", 1));
              return CodeNode::make<ArrayPop>(exprs[0]);
          }
        },

        { "arrfind",
          [this]() {
              auto exprs(readExprList());
              if (exprs.size() == 2) {
                  return CodeNode::make<ArrayFind>(exprs[0], exprs[1]);
              }
              else if (exprs.size() == 3) {
                  return CodeNode::make<ArrayFind>(exprs[0], exprs[1], exprs[2]);
              }
              else {
                  throw TooManyOrFewForms("arrfind");
              }
          }
        },

        { "arrcount",
          [this]() {
              auto exprs(readAndCheckExprList("arrcount", 2));
              return CodeNode::make<ArrayCount>(exprs[0], exprs[1]);
          }
        },

        { "arrsort",
          [this]() {
              auto exprs(readAndCheckRangeExprList("arrsort", 1, 2));
              return CodeNode::make<ArraySort>(exprs[0], exprs.size() == 2 ? exprs[1] : CodeNode::SharedPtr());
          }
        },

        { "arrrev",
          [this] {
              auto exprs(readAndCheckExprList("arrrev", 1));
              return CodeNode::make<ArrayReverse>(exprs[0]);
          }
        },

        { "arrclr",
          [this] {
              auto exprs(readAndCheckExprList("arrclr", 1));
              return CodeNode::make<ArrayClear>(exprs[0]);
          }
        },

        { "arrins",
          [this] {
              auto exprs(readAndCheckExprList("arrins", 3));
              return CodeNode::make<ArrayInsert>(exprs[0], exprs[1], exprs[2]);
          }
        },

        { "arrrem",
          [this] {
              auto exprs(readAndCheckExprList("arrrem", 2));
              return CodeNode::make<ArrayRemove>(exprs[0], exprs[1]);
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
              return CodeNode::make<Random>(exprs.size() == 1 ? exprs[0] : CodeNode::SharedPtr());
          }
        },

        { "hash",
          [this]() {
              auto exprs(readAndCheckExprList("hash", 1));
              return CodeNode::make<Hash>(exprs[0]);
          }
        },

        { "hashmap",
          [this]() {
              return CodeNode::make<MakeHashMap>(readExprList());
          }
        },

        { "hmlen",
          [this]() {
              auto exprs(readAndCheckExprList("hmlen", 1));
              return CodeNode::make<HashMapLen>(exprs[0]);
          }
        },

        { "hmhas",
          [this]() {
              auto exprs(readAndCheckExprList("hmhas", 2));
              return CodeNode::make<HashMapContains>(exprs[0], exprs[1]);
          }
        },

        { "hmget",
          [this]() {
              auto exprs(readAndCheckRangeExprList("hmget", 2, 3));
              return CodeNode::make<HashMapGet>(exprs[0], exprs[1], exprs.size() == 3 ? exprs[2] : CodeNode::SharedPtr());
          }
        },

        { "hmset",
          [this]() {
              auto exprs(readAndCheckExprList("hmget", 3));
              return CodeNode::make<HashMapSet>(exprs[0], exprs[1], exprs[2]);
          }
        },

        { "hmrem",
          [this]() {
              auto exprs(readAndCheckExprList("hmrem", 2));
              return CodeNode::make<HashMapRemove>(exprs[0], exprs[1]);
          }
        },

        { "hmclr",
          [this]() {
              auto exprs(readAndCheckExprList("hmclr", 1));
              return CodeNode::make<HashMapClear>(exprs[0]);
          }
        },

        { "hmfind",
          [this]() {
              auto exprs(readAndCheckExprList("hmfind", 2));
              return CodeNode::make<HashMapFind>(exprs[0], exprs[1]);
          }
        },

        { "hmcount",
          [this]() {
              auto exprs(readAndCheckExprList("hmcount", 2));
              return CodeNode::make<HashMapCount>(exprs[0], exprs[1]);
          }
        },

        { "hmkeys",
          [this]() {
              auto exprs(readAndCheckExprList("hmkeys", 1));
              return CodeNode::make<HashMapKeys>(exprs[0]);
          }
        },

        { "hmvals",
          [this]() {
              auto exprs(readAndCheckExprList("hmvals", 1));
              return CodeNode::make<HashMapValues>(exprs[0]);
          }
        },

        { "hmitems",
          [this]() {
              auto exprs(readAndCheckExprList("hmitems", 1));
              return CodeNode::make<HashMapItems>(exprs[0]);
          }
        },

        { "pair",
          [this]() {
              auto exprs(readAndCheckExprList("pair", 2));
              return CodeNode::make<MakePair>(exprs[0], exprs[1]);
          }
        },

        { "first",
          [this]() {
              auto exprs(readAndCheckExprList("first", 1));
              return CodeNode::make<PairFirst>(exprs[0]);
          }
        },

        { "second",
          [this]() {
              auto exprs(readAndCheckExprList("second", 1));
              return CodeNode::make<PairSecond>(exprs[0]);
          }
        },

        { "range",
          [this]() {
              auto exprs(readAndCheckRangeExprList("range", 1, 3));
              if (exprs.size() == 1) {
                  return CodeNode::make<MakeRange>(exprs[0]);
              }
              else {
                  return CodeNode::make<MakeRange>(exprs[0], exprs[1], exprs.size() == 3 ? exprs[2] : CodeNode::SharedPtr());
              }
          }
        },

        { "rngbegin",
          [this]() {
              auto rngExpr(readAndCheckExprList("rngbegin", 1));
              return CodeNode::make<RangeBegin>(rngExpr[0]);
          }
        },

        { "rngend",
          [this]() {
              auto rngExpr(readAndCheckExprList("rngend", 1));
              return CodeNode::make<RangeEnd>(rngExpr[0]);
          }
        },

        { "rngstep",
          [this]() {
              auto rngExpr(readAndCheckExprList("rngstep", 1));
              return CodeNode::make<RangeStep>(rngExpr[0]);
          }
        },

        { "rnglen",
          [this]() {
              auto rngExpr(readAndCheckExprList("rnglen", 1));
              return CodeNode::make<RangeLen>(rngExpr[0]);
          }
        },

        { "expand",
          [this]() {
              auto exprs(readAndCheckRangeExprList("expand", 1, std::nullopt));
              return CodeNode::make<Expand>(exprs);
          }
        },

        { "len",
          [this]() {
              auto exprs(readAndCheckExprList("len", 1));
              return CodeNode::make<GenericLen>(exprs[0]);
          }
        },

        { "empty",
          [this]() {
              auto exprs(readAndCheckExprList("empty", 1));
              return CodeNode::make<GenericEmpty>(exprs[0]);
          }
        },

        { "get",
          [this]() {
              auto exprs(readAndCheckRangeExprList("get", 2, 3));
              return CodeNode::make<GenericGet>(exprs[0], exprs[1], exprs.size() == 3 ? exprs[2] : CodeNode::SharedPtr());
          }
        },

        { "set",
          [this]() {
              auto exprs(readAndCheckExprList("get", 3));
              return CodeNode::make<GenericSet>(exprs[0], exprs[1], exprs[2]);
          }
        },

        { "clear",
          [this]() {
              auto exprs(readAndCheckExprList("clear", 1));
              return CodeNode::make<GenericClear>(exprs[0]);
          }
        },

        { "find",
          [this]() {
              auto exprs(readAndCheckRangeExprList("find", 2, 3));
              return CodeNode::make<GenericFind>(exprs[0], exprs[1], exprs.size() == 3 ? exprs[2] : CodeNode::SharedPtr());
          }
        },

        { "count",
          [this]() {
              auto exprs(readAndCheckExprList("count", 2));
              return CodeNode::make<GenericCount>(exprs[0], exprs[1]);
          }
        },

        { "sort",
          [this]() {
              auto exprs(readAndCheckRangeExprList("sort", 1, 2));
              return CodeNode::make<GenericSort>(exprs[0], exprs.size() == 2 ? exprs[1] : CodeNode::SharedPtr());
          }
        },

        { "reverse",
          [this]() {
              auto exprs(readAndCheckExprList("reverse", 1));
              return CodeNode::make<GenericReverse>(exprs[0]);
          }
        },

        { "sum",
          [this]() {
              auto exprs(readAndCheckExprList("sum", 1));
              return CodeNode::make<GenericSum>(exprs[0]);
          }
        },

        { "timeit",
          [this]() {
              auto exprs(readAndCheckRangeExprList("timeit", 1, 3));
              return CodeNode::make<TimeIt>(exprs[0],
                                            exprs.size() >= 2 ? exprs[1] : CodeNode::SharedPtr(),
                                            exprs.size() == 3 ? exprs[2] : CodeNode::SharedPtr());
          }
        },

        { "fopen",
          [this]() {
              auto exprs(readAndCheckExprList("fopen", 2));
              return CodeNode::make<FileOpen>(exprs[0], exprs[1]);
          }
        },

        { "fclose",
          [this]() {
              auto exprs(readAndCheckExprList("fclose", 1));
              return CodeNode::make<FileClose>(exprs[0]);
          }
        },

        { "fflush",
          [this]() {
              auto exprs(readAndCheckExprList("fflush", 1));
              return CodeNode::make<FileFlush>(exprs[0]);
          }
        },

        { "fisopen",
          [this]() {
              auto exprs(readAndCheckExprList("fisopen", 1));
              return CodeNode::make<FileIsOpen>(exprs[0]);
          }
        },

        { "fname",
          [this]() {
              auto exprs(readAndCheckExprList("fname", 1));
              return CodeNode::make<FileFName>(exprs[0]);
          }
        },

        { "fmode",
          [this]() {
              auto exprs(readAndCheckExprList("fmode", 1));
              return CodeNode::make<FileFMode>(exprs[0]);
          }
        },

        { "fread",
          [this]() {
              auto exprs(readAndCheckExprList("fread", 1));
              return CodeNode::make<FileRead>(exprs[0]);
          }
        },

        { "freadln",
          [this]() {
              auto exprs(readAndCheckExprList("freadln", 1));
              return CodeNode::make<FileReadLn>(exprs[0]);
          }
        },

        { "fwrite",
          [this]() {
              auto exprs(readAndCheckExprList("fwrite", 2));
              return CodeNode::make<FileWrite>(exprs[0], exprs[1]);
          }
        },

        { "fwriteln",
          [this]() {
              auto exprs(readAndCheckExprList("fwriteln", 2));
              return CodeNode::make<FileWriteLn>(exprs[0], exprs[1]);
          }
        },

        { "fexists",
          [this]() {
              auto exprs(readAndCheckExprList("fexists", 1));
              return CodeNode::make<FileExists>(exprs[0]);
          }
        },

        { "fremove",
          [this]() {
              auto exprs(readAndCheckExprList("fremove", 1));
              return CodeNode::make<FileRemove>(exprs[0]);
          }
        }
    };
}
