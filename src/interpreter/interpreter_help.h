#ifndef ISHLANG_INTERPRETER_HELP_H
#define ISHLANG_INTERPRETER_HELP_H

#include <string>
#include <unordered_map>

namespace Ishlang {

    class HelpDict {
    public:
        using Dict = std::unordered_map<std::string, std::string>;

    public:
        HelpDict();

        std::string topics() const;
        const std::string &lookup(const std::string &topic) const;

    private:
        static void populateDict(Dict &dict);

        static const char *help_type();
        static const char *help_var();
        static const char *help_arith();
        static const char *help_logic();
        static const char *help_branch();
        static const char *help_loop();
        static const char *help_func();
        static const char *help_struct();
        static const char *help_cast();
        static const char *help_string();
        static const char *help_charop();
        static const char *help_pair();
        static const char *help_array();
        static const char *help_hashmap();
        static const char *help_range();
        static const char *help_generic();
        static const char *help_block();
        static const char *help_misc();
        static const char *help_import();
        static const char *help_file_io();
        static const char *help_repl();

    private:
        Dict dict_;
    };

} // Ishlang

#endif // ISHLANG_INTERPRETER_HELP_H
