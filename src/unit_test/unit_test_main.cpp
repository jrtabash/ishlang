#include "unit_test.h"

#include <cstdlib>
#include <iostream>
#include <string>

class Arguments {
public:
    Arguments(int argc, char **argv)
        : argc_(argc)
        , argv_(argv)
        , program()
        , verbose(false)
        , listTests(false)
        , testName()
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
                else if (arg == "-v") { verbose = true; }
                else if (arg == "-l") { listTests = true; }
                else if (arg == "-n") { testName = readArgValue("test_name", i); }
                else {
                    argError(std::string("Invalid option '") + arg + "'");
                }
            }
        }
    }

private:
    void usage() {
        std::cerr << "Usage:\n"
                  << '\t' << program << " [-h] [-v] [-l | -n test_name]\n"
                  << '\n'
                  << "Options:\n"
                  << '\t' << "-h : Print usage\n"
                  << '\t' << "-v : Use verbose mode\n"
                  << '\t' << "-l : List unit test names\n"
                  << '\t' << "-n : Run specified unit test\n"
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
    bool        verbose;
    bool        listTests;
    std::string testName;
};

int main(int argc, char** argv) {
    Arguments args(argc, argv);

    Ishlang::UnitTest unitTest;
    unitTest.setVerbose(args.verbose);
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

    return 0;
}
