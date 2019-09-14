#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "environment.h"
#include "parser.h"

namespace Int {

    class Interpreter {
    public:
        Interpreter(bool batch);
        Interpreter(const Interpreter &) = delete;

        const Interpreter &operator=(const Interpreter &) = delete;

        bool readEvalPrintLoop();
        void loadFile(const std::string &filename);

    private:
        std::string readFile(const std::string &filename);

    private:
        struct ParserCB {
            ParserCB(Environment::SharedPtr env, std::string &lastResult, bool batch);
            void operator()(ByteCode::SharedPtr &code);

        private:
            Environment::SharedPtr  env;
            std::string            &lastResult;
            bool                    batch;
        };

    private:
        Parser                 parser_;
        Environment::SharedPtr env_;

        std::string prompt_;
        std::string contPrompt_;
        std::string lastResult_;

        ParserCB parserCB_;
    };

} // Int

#endif // INTERPRETER_H
