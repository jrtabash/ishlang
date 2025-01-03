#include "interpreter.h"
#include "module.h"
#include "sequence.h"
#include "util.h"

#include <cstdlib>

using namespace Ishlang;

// -------------------------------------------------------------
//                     PARSER CALLBACK
// -------------------------------------------------------------

// -------------------------------------------------------------
Interpreter::ParserCB::ParserCB(Environment::SharedPtr env, std::string &lastResult, bool &batch)
    : env(env)
    , lastResult(lastResult)
    , batch(batch)
{}

// -------------------------------------------------------------
void Interpreter::ParserCB::operator()(CodeNode::SharedPtr &code) {
    if (code) {
        Value result = code->eval(env);
        if (!batch) { std::cout << result << '\n'; }
        env->set(lastResult, result);
    }
}

// -------------------------------------------------------------
//                       BatchScope
// -------------------------------------------------------------

// -------------------------------------------------------------
Interpreter::BatchScope::BatchScope(bool &batch, bool newValue)
    : batch_(batch)
    , oldValue_(batch)
{
    batch_ = newValue;
}

// -------------------------------------------------------------
Interpreter::BatchScope::~BatchScope() {
    batch_ = oldValue_;
}

// -------------------------------------------------------------
//                       INTERPRETER
// -------------------------------------------------------------

// -------------------------------------------------------------
Interpreter::Interpreter(bool batch, const std::string &path)
    : parser_()
    , env_(Environment::make())
    , prompt_(">>")
    , contPrompt_("..")
    , lastResult_("*")
    , batch_(batch)
    , parserCB_(env_, lastResult_, batch_)
    , helpDict_()
{
    env_->def(lastResult_, Value::Null);
    initPath(path);
}

// -------------------------------------------------------------
bool Interpreter::readEvalPrintLoop() {
    std::string expr;
    for (;;) {
        try {
            std::cout << (parser_.hasIncompleteExpr() ? contPrompt_ : prompt_) << ' ';
            std::getline(std::cin, expr);
            if (std::cin.eof()) {
                std::cout << '\n';
                break;
            }
            else if (isREPLCommand(expr)) {
                handleREPLCommand(expr);
            }
            else {
                parser_.readMulti(expr, parserCB_);
            }
        }
        catch (const Exception &ex) {
            parser_.clearIncompleteExpr();
            ex.printError();
        }
        catch (const std::exception &ex) {
            std::cerr << "System error: " << ex.what() << std::endl;
            return false;
        }
    }
    return true;
}

// -------------------------------------------------------------
void Interpreter::loadFile(const std::string &filename) {
    auto _ = BatchScope(batch_, true);
    parser_.readFile(filename, parserCB_);
}

// -------------------------------------------------------------
bool Interpreter::evalExpr(const std::string &expression) {
    try {
        parser_.readMulti(expression, parserCB_);
        if (parser_.hasIncompleteExpr()) {
            throw IncompleteExpression(expression);
        }
    }
    catch (const Exception &ex) {
        parser_.clearIncompleteExpr();
        ex.printError();
        return false;
    }
    catch (const std::exception &ex) {
        std::cerr << "System error: " << ex.what() << std::endl;
        return false;
    }
    return true;
}

// -------------------------------------------------------------
void Interpreter::setArguments(char ** argv, int begin, int end) {
    Sequence arguments;
    for (int i = begin; i < end; ++i) {
        arguments.push(Value(argv[i]));
    }
    env_->def("argv", Value(arguments));
}

// -------------------------------------------------------------
bool Interpreter::isREPLCommand(const std::string &expr) const {
    return expr[0] == ':' && !parser_.hasIncompleteExpr();
}

// -------------------------------------------------------------
void Interpreter::handleREPLCommand(const std::string &expr) {
    Util::TokenList cmdTokens;

    const auto size = Util::tokenize(expr, cmdTokens);
    if (size < 1) {
        throw InvalidCommand("", "missing command");
    }

    const auto cmd = cmdTokens.front();
    cmdTokens.pop_front();

    if (cmd == ":exit" || cmd == ":quit") {
        if (size > 1) {
            throw InvalidCommand(cmd, "too many arguments");
        }
        std::exit(0);
    }
    else if (cmd == ":load") {
        if (size == 1) {
            throw InvalidCommand(cmd, "missing filename");
        }
        else if (size > 2) {
            throw InvalidCommand(cmd, "too many arguments");
        }
        loadFile(cmdTokens.front());
    }
    else if (cmd == ":batch") {
        if (size == 1) {
            batch_ = !batch_;
        }
        else if (size == 2) {
            if (!Util::setBoolFromString(batch_, cmdTokens.front())) {
                throw InvalidCommand(cmd, "expecting true or false");
            }
        }
        else {
            throw InvalidCommand(cmd, "too many arguments");
        }
    }
    else if (cmd == ":desc") {
        if (size == 1) {
            throw InvalidCommand(cmd, "missing struct or instance");
        }
        else if (size > 2) {
            throw InvalidCommand(cmd, "too many arguments");
        }
        describe(cmdTokens.front());
    }
    else if (cmd == ":help") {
        if (size == 1) {
            std::cout << helpDict_.topics() << std::endl;
        }
        else if (size == 2) {
            std::cout << helpDict_.lookup(cmdTokens.front()) << std::endl;
        }
        else {
            throw InvalidCommand(cmd, "too many arguments");
        }
    }
    else {
        throw InvalidCommand(cmd, "unknown command");
    }
}

// -------------------------------------------------------------
void Interpreter::describe(const std::string &name) const {
    const Value &value = env_->get(name);
    if (value.isUserType()) {
        value.userType().describe();
    }
    else if (value.isUserObject()) {
        value.userObject().describe();
    }
    else {
        std::cout << "Name '" << name << "' is not a user type or object\n";
    }
}

// -------------------------------------------------------------
void Interpreter::initPath(const std::string &path) {
    const auto envPath = std::getenv("ISHLANG_PATH");
    if (envPath) {
        const std::string envPathStr(envPath);
        if (!envPathStr.empty()) {
            ModuleStorage::addPaths(envPathStr);
        }
    }

    if (!path.empty()) {
        ModuleStorage::addPaths(path);
    }
}
