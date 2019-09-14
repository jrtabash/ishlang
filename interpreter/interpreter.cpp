#include "interpreter.h"
#include "util.h"

#include <fstream>
#include <cstdlib>

using namespace Int;

// -------------------------------------------------------------
//                     PARSER CALLBACK
// -------------------------------------------------------------

// -------------------------------------------------------------
Interpreter::ParserCB::ParserCB(Environment::SharedPtr env, std::string &lastResult, bool batch)
    : env(env)
    , lastResult(lastResult)
    , batch(batch)
{}

// -------------------------------------------------------------
void Interpreter::ParserCB::operator()(ByteCode::SharedPtr &code) {
    if (code.get()) {
        Value result = code->eval(env);
        if (!batch) { std::cout << result << '\n'; }
        env->set(lastResult, result);
    }
}

// -------------------------------------------------------------
//                       INTERPRETER
// -------------------------------------------------------------

// -------------------------------------------------------------
Interpreter::Interpreter(bool batch)
    : parser_()
    , env_(std::make_shared<Environment>())
    , prompt_(">>")
    , contPrompt_("..")
    , lastResult_("*")
    , parserCB_(env_, lastResult_, batch)
{
    env_->def(lastResult_, Value::Null);
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
            std::cerr << "Error: " << ex.what() << std::endl;
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
    try {
        parser_.readMulti(readFile(filename), parserCB_);
    }
    catch (const Exception &ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
    }
    catch (const std::exception &ex) {
        std::cerr << "System error: " << ex.what() << std::endl;
    }
}

// -------------------------------------------------------------
std::string Interpreter::readFile(const std::string &filename) {
    // TODO - Hacky and inefficient... 
    // redo with stream, which will require a rework of the parser.
    
    std::ifstream ifs(filename.c_str());
    if (!ifs.is_open()) {
        throw UnknownFile(filename);
    }

    std::string result;
    try {
        std::string line;
        while (std::getline(ifs, line)) {
            result += line;
        }
        ifs.close();
    }
    catch (...) {
        ifs.close();
        throw;
    }
    return result;
}

bool Interpreter::isREPLCommand(const std::string &expr) const {
    return expr[0] == ':' && !parser_.hasIncompleteExpr();
}

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
        parser_.readMulti(readFile(cmdTokens.front()), parserCB_);
    }
    else {
        throw InvalidCommand(cmd, "unknown command");
    }
}
