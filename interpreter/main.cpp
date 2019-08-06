#include "unit_test.h"
#include "interpreter.h"

#include <iostream>
#include <string>
#include <cstdlib>

class Arguments {
public:
    Arguments(int argc, char **argv)
        : argc_(argc)
        , argv_(argv)
        , program()
        , test(false)
        , verboseTest(false)
        , listTests(false)
        , testName()
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
                else if (arg == "-t") { test = true; }
                else if (arg == "-v") { verboseTest = true; }
                else if (arg == "-l") { listTests = true; }
                else if (arg == "-n") { testName = readArgValue("test_name", i); }
                else if (arg == "-i") { interactive = true; }
                else if (arg == "-b") { batch = true; }
                else if (arg == "-f") { filename = readArgValue("file", i); }
            }
        }
    }

private:
    void usage() {
        std::cerr << "Usage:\n"
                  << '\t' << program << " [-h] [-i] [-b] [-f file] [-t [-v] [-l | -n test_name]]\n"
                  << '\n'
                  << "Options:\n"
                  << '\t' << "-h : Print usage\n"
                  << '\t' << "-i : Enter interactive mode\n"
                  << '\t' << "-b : Run in batch mode\n"
                  << '\t' << "-f : Run code file\n"
                  << '\t' << "-t : Run unit tests\n"
                  << '\t' << "-v : With -t, use verbose mode\n"
                  << '\t' << "-l : With -t, list unit test names\n"
                  << '\t' << "-n : With -t, run specified unit test\n"
                  << std::endl;
        exit(1);
    }

    const char *readArgValue(const char *name, int &i) {
        if (++i < argc_) { return argv_[i]; }
        argError(std::string("Premature end of arguments - ") + name + "\n");
        return 0;
    }

    void argError(const std::string &msg) {
        std::cerr << "\nError: " << msg << '\n';
        usage();
    }

public:
    int    argc_;
    char **argv_;

    std::string program;
    bool        test;
    bool        verboseTest;
    bool        listTests;
    std::string testName;
    bool        interactive;
    bool        batch;
    std::string filename;
};

int main(int argc, char** argv) {
    Arguments args(argc, argv);

    if (args.test) {
        Int::UnitTest unitTest;
        unitTest.setVerbose(args.verboseTest);
        if (args.listTests) {
            unitTest.list();
        }
        else {
            try {
                if (!unitTest.run(args.testName)) {
                    return 1;
                }
            }
            catch (const std::exception &ex) {
                std::cerr << "Error: " << ex.what() << std::endl;
                return 1;
            }
        }
    }
    else {
        Int::Interpreter interpreter(args.batch);
        if (!args.filename.empty()) {
            interpreter.loadFile(args.filename);
        }

        if (args.interactive) {
            if (!interpreter.readEvalPrintLoop()) {
                return 1;
            }
        }
    }

    return 0;
}
