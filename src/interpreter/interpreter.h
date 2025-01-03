#ifndef ISHLANG_INTERPRETER_H
#define ISHLANG_INTERPRETER_H

#include "environment.h"
#include "interpreter_help.h"
#include "parser.h"

namespace Ishlang {

    class Interpreter {
    public:
        Interpreter(bool batch, const std::string &path = "");
        Interpreter(const Interpreter &) = delete;

        const Interpreter &operator=(const Interpreter &) = delete;

        bool readEvalPrintLoop();
        void loadFile(const std::string &filename);
        bool evalExpr(const std::string &expression);

        void setArguments(char ** argv, int begin, int end);

    private:
        bool isREPLCommand(const std::string &expr) const;
        void handleREPLCommand(const std::string &expr);
        void describe(const std::string &name) const;

    private:
        void initPath(const std::string &path);

    private:
        struct ParserCB {
            ParserCB(Environment::SharedPtr env, std::string &lastResult, bool &batch);
            void operator()(CodeNode::SharedPtr &code);

        private:
            Environment::SharedPtr  env;
            std::string            &lastResult;
            const bool             &batch;
        };

    private:
        struct BatchScope {
            BatchScope(bool &batch, bool newValue);
            ~BatchScope();

            BatchScope(const BatchScope &) = delete;
            BatchScope & operator=(const BatchScope &) = delete;

        private:
            bool &batch_;
            bool oldValue_;
        };

    private:
        Parser                 parser_;
        Environment::SharedPtr env_;

        std::string prompt_;
        std::string contPrompt_;
        std::string lastResult_;
        bool        batch_;

        ParserCB parserCB_;

        HelpDict helpDict_;
    };

} // Int

#endif // ISHLANG_INTERPRETER_H
