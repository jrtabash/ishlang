#include "interpreter.h"

#include <cstdlib>
#include <iostream>
#include <string>

class Arguments {
public:
    Arguments(int argc, char **argv)
        : argc_(argc)
        , argv_(argv)
        , program()
        , interactive(false)
        , batch(false)
        , filename()
    {
        parse();
    }

private:
    void parse() {
        if (argc_ > 0) {
            program = argv_[0];

            for (int i = 1; i < argc_; ++i) {
                std::string arg(argv_[i]);
                if      (arg == "-h") { usage(); }
                else if (arg == "-i") { interactive = true; }
                else if (arg == "-b") { batch = true; }
                else if (arg == "-f") { filename = readArgValue("file", i); }
                else {
                    argError(std::string("Invalid option '") + arg + "'");
                }
            }
        }
    }

private:
    void usage() {
        std::cerr << "Usage:\n"
                  << '\t' << program << " [-h] [-i] [-b] [-f file]\n"
                  << '\n'
                  << "Options:\n"
                  << '\t' << "-h : Print usage\n"
                  << '\t' << "-i : Enter interactive mode\n"
                  << '\t' << "-b : Run in batch mode\n"
                  << '\t' << "-f : Run code file\n"
                  << std::endl;
        exit(1);
    }

    const char *readArgValue(const char *name, int &i) {
        if (++i < argc_) { return argv_[i]; }
        argError(std::string("Premature end of arguments - ") + name + "\n");
        return 0;
    }

    void argError(const std::string &msg) {
        std::cerr << "\nError: " << msg << "\n\n";
        usage();
    }

public:
    int    argc_;
    char **argv_;

    std::string program;
    bool        interactive;
    bool        batch;
    std::string filename;
};

int main(int argc, char** argv) {
    Arguments args(argc, argv);

    bool const forceInteractive = (args.filename.empty() && !args.interactive);
    bool const forceBatch = (!args.filename.empty() && !args.interactive);

    Ishlang::Interpreter interpreter(args.batch || forceBatch);
    if (!args.filename.empty()) {
        interpreter.loadFile(args.filename);
    }

    if (args.interactive || forceInteractive) {
        if (!interpreter.readEvalPrintLoop()) {
            return 1;
        }
    }

    return 0;
}
