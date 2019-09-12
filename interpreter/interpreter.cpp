#include "interpreter.h"

#include <fstream>

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
            std::cout << prompt_ << ' ';
            std::getline(std::cin, expr);
            if (std::cin.eof()) {
                std::cout << '\n';
                break;
            }
            else if (expr == "exit" || expr == "quit") {
                break;
            }

            parser_.readMulti(
                expr.substr(0, 4) == "load" ? readFile(expr.substr(5)) : expr,
                parserCB_);
        }
        catch (const Exception &ex) {
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
