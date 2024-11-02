#include "interpreter.h"

#include <cstdlib>
#include <iostream>
#include <string>

class Arguments {
public:
    Arguments(int argc, char **argv)
        : argc(argc)
        , argv(argv)
        , program()
        , interactive(false)
        , batch(false)
        , filename()
        , expression()
        , argsBegin(argc)
    {
        parse();
    }

private:
    void parse() {
        if (argc > 0) {
            program = argv[0];

            for (int i = 1; i < argc; ++i) {
                std::string arg(argv[i]);
                if      (arg == "-h") { usage(); }
                else if (arg == "-i") { interactive = true; }
                else if (arg == "-b") { batch = true; }
                else if (arg == "-p") { path = readArgValue("path", i); }
                else if (arg == "-f") { filename = readArgValue("file", i); }
                else if (arg == "-e") { expression = readArgValue("expression", i); }
                else if (arg == "-a") {
                    argsBegin = i + 1;
                    break;
                }
                else {
                    argError(std::string("Invalid option '") + arg + "'");
                }
            }
        }
    }

private:
    void usage() {
        std::cerr << "Usage:\n"
                  << '\t' << program << " [-h] [-i] [-b] [-p] [-f file] [-e expr] [-a arg1 ... argN]\n"
                  << '\n'
                  << "Options:\n"
                  << '\t' << "-h : Print usage\n"
                  << '\t' << "-i : Enter interactive mode\n"
                  << '\t' << "-b : Run in batch mode\n"
                  << '\t' << "-p : Import path\n"
                  << '\t' << "-f : Run code file\n"
                  << '\t' << "-e : Execute expression, after running file, before entering interactive mode\n"
                  << '\t' << "-a : Arguments passed to user. Must be last option. Available in argv array"
                  << std::endl;
        exit(1);
    }

    const char *readArgValue(const char *name, int &i) {
        if (++i < argc) { return argv[i]; }
        argError(std::string("Premature end of arguments - ") + name + "\n");
        return 0;
    }

    void argError(const std::string &msg) {
        std::cerr << "\nError: " << msg << "\n\n";
        usage();
    }

public:
    int    argc;
    char **argv;

    std::string program;
    bool        interactive;
    bool        batch;
    std::string path;
    std::string filename;
    std::string expression;
    int         argsBegin;
};

int main(int argc, char** argv) {
    Arguments args(argc, argv);

    bool const forceInteractive = (args.filename.empty() && args.expression.empty() && !args.interactive);
    bool const forceBatch = (!args.filename.empty() && !args.interactive);

    Ishlang::Interpreter interpreter(args.batch || forceBatch, args.path);
    interpreter.setArguments(args.argv, args.argsBegin, args.argc);

    if (!args.filename.empty()) {
        try {
            interpreter.loadFile(args.filename);
        }
        catch (const Ishlang::Exception &ex) {
            ex.printError();
            return 1;
        }
        catch (const std::exception &ex) {
            std::cerr << "System error: " << ex.what() << std::endl;
            return 1;
        }
    }

    if (!args.expression.empty()) {
        interpreter.evalExpr(args.expression);
    }

    if (args.interactive || forceInteractive) {
        if (!interpreter.readEvalPrintLoop()) {
            return 1;
        }
    }

    return 0;
}
