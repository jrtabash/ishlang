#include "interpreter_help.h"

using namespace Ishlang;

HelpDict::HelpDict()
    : dict_()
{
    populateDict(dict_);
}

std::string HelpDict::topics() const {
    std::string topics =
        "\n"
        ":help [<topic>]\n"
        "\n"
        "Available topics:\n"
        " ";

    unsigned i = 0;
    for (auto && [key, _] : dict_) {
        topics += '\t' + key;
        if (++i > 3) {
            topics += "\n ";
            i = 0;
        }
    }

    if (i > 0) {
        topics += '\n';
    }

    return topics;
}

const std::string &HelpDict::lookup(const std::string &topic) const {
    static const std::string UnknownTopic = "\nUnknown help topic\n";

    auto iter = dict_.find(topic);
    return iter != dict_.end() ? iter->second : UnknownTopic;
}

void HelpDict::populateDict(Dict &dict) {
    Dict tmp;
    tmp.emplace("type", help_type());
    tmp.emplace("var", help_var());
    tmp.emplace("arith", help_arith());
    tmp.emplace("logic", help_logic());
    tmp.emplace("branch", help_branch());
    tmp.emplace("loop", help_loop());
    tmp.emplace("func", help_func());
    tmp.emplace("struct", help_struct());
    tmp.emplace("cast", help_cast());
    tmp.emplace("string", help_string());
    tmp.emplace("charop", help_charop());
    tmp.emplace("pair", help_pair());
    tmp.emplace("array", help_array());
    tmp.emplace("hashmap", help_hashmap());
    tmp.emplace("range", help_range());
    tmp.emplace("generic", help_generic());
    tmp.emplace("block", help_block());
    tmp.emplace("misc", help_misc());
    tmp.emplace("import", help_import());
    tmp.emplace("fileio", help_file_io());
    tmp.emplace("repl", help_repl());
    dict.swap(tmp);
}

const char *HelpDict::help_type() {
    return R"(
Types
-----
Every expression in ishlang evaluates to a value.
A value can hold any of the following types:

  none int real char bool string pair
  array hashmap range file closure
  usertype userobject

Examples:
       none: null
        int: 25
       real: 12.34
       char: 'a'
       bool: true
     string: "hello"
       pair: (pair 1 2)
      array: (array 1 2)
    hashmap: (hashmap (pair 1 100))
      range: (range 10)
       file: (fopen "path/to/file.txt" 'r')
    closure: (lambda () 42)
   usertype: (struct Foo (bar))
 userobject: (makeinstance Foo)

Type checking functions:
  istypeof - check expression type matches any of provided types
             (istypeof <expr> <type> [<type> ...])

  typename - Get name of expression type
             (typename <expr>)

See ":help struct" for information on struct type checking functions.
)";
}

const char *HelpDict::help_var() {
    return R"(
Variables
---------
Create variable:
  (var <name> <expr>)

Assign variable:
  (= <name> <expr>)

Lookup variable:
  <name>

Check variable exists:
  (? <name>)
)";
}

const char *HelpDict::help_arith() {
    return R"(
Arithmetic
----------
Variadic Operations: + - * / % ^
  (<op> <expr> <expr> [<expr> ...])

  When more than 2 operands are given to <op>, the expression
  is equivalent to a binary left to right aggregation.
  Example:
    (+ 1 2 3) <=> (+ (+ 1 2) 3)

Binary operations: += -= *= /= %= ^=
  (<op> <name> <expression>)

  Apply arithmetic operation and assign result to <name>.
  The variable referenced by <name> must be defined.

Negation Operation:
  (neg <expr>)
)";
}

const char *HelpDict::help_logic() {
    return R"(
Logical
-------
Binary Operations: == != < > <= >=
  (<op> <expr> <expr>)

Variadic Operations: and or
  (<op> <expr> <expr> [<expr> ...])

  * Evaluate all expressions left to right
  * When op is and, stop on and return first false expression, otherwise return true
  * When op is or, stop on and return first true expression, otherwise return false

Not Operation:
  (not <expr>)
)";
}

const char *HelpDict::help_branch() {
    return R"(
Branching
---------
If Expression:
  (if <condition> <true_expre> [<false_expr>])

When Expression - syntactic sugar for if/true expression:
  (when <condition> <true_expr>)

  Example
    (when (> x 0) 10) <=> (if (> x 0) 10)

Unless Expression - syntactic sugar for if/not expression:
  (unless <condition> <false_expr>)

  Example
    (unless (<= x 0) 10) <=> (if (not (<= x 0)) 10)

Cond Expression:
  (cond
   (<cond1> <expr1>)
   ...
   (true <else_expr>))

  Example
    (var x (read))
    (cond
      ((< x 0) (neg x))
      ((> x 0) (* 2 x))
      (true x))
)";
}

const char *HelpDict::help_loop() {
    return R"(
Looping
-------
General Loop Expression:
  (loop <decl_expr> <cond_expr> <next_expr>
    <body_expr>)

  Example
    (var agg 0)
    (loop (var i 1) (<= i 10) (= i (+ i 1))
      (= agg (+ agg i)))

Conditional Loop Expression:
  (loop <cond_expr>
    <body_expr>)

  Example
    (var agg 0)
    (var trm 1)
    (loop (<= trm 10)
      (= agg (+ agg trm))
      (= trm (+ trm 1)))

Foreach Expression:
  (foreach <var> <iterable_expr>
    <body_expr>)

  Example
    (var agg 0)
    (foreach i (range 11)
      (= agg (+ agg i)))
)";
}

const char *HelpDict::help_func() {
    return R"(
Functions
---------
User Defined Functions:
  (defun <name> <param_list>
    <body>)

  * Functions can be nested
  * Functions can be recursive

  Example
    (defun add (x y)
      (+ x y))

Lambda Expressions / Closures:
  (lambda <param_list>
    <body>)

  * The closure environment is the enclosing
    environment where the lambda is defined
  * When a lambda is executed, the lambda execution
    environment will inherit the closure environment

  Example
    (lambda (x) (+ x 1))
)";
}

const char *HelpDict::help_struct() {
    return R"(
Structs
-------
Defining User Types:
  (struct <name>
    <member_list>)

  Example
    (struct Person
      (name
       age))

Creating User Type Instances:
  (makeinstance <name> [(member value) [(member value) ...]])

  Example
    (makeinstance Person (name \Jon\) (age 25))

Getting / Setting Member Fields:
  (memget <instance> <mem_name>)
  (memset <instance> <mem_name> <value_expr>)

  Example
    (memget p name)
    (memset p age 26)

Struct Type Functions:
  isstructname - Check expression struct name
                 (isstructname <expr> <struct>)

  isinstanceof - Check expression is instance of struct
                 (isinstanceof <expr> <struct>)

    structname - Get struct name of struct or instance
                 (structname <expr>)

See ":help generic" for information on struct generic functions support
)";
}

const char *HelpDict::help_cast() {
    return R"(
Type Conversion
---------------
Convert value to value of another type:
  (astype <expr> <type>)

Following type conversions are supported:
  From  |   To   | Equivalent to
--------|--------|---------------
 int    | real   | float(from)
 int    | char   | ascii(from)
 int    | bool   | from != 0
 int    | string | string(from)
 real   | int    | int(from)
 real   | char   | ascii(int(from))
 real   | bool   | int(from) != 0
 real   | string | string(from)
 char   | int    | ascii_value(from)
 char   | real   | ascii_value(int(from))
 char   | bool   | from == 't' -> true, from == 'f' -> false
 char   | string | string(from)
 bool   | int    | from == true -> 1, from == false -> 0
 bool   | real   | from == true -> 1.0, from == false -> 0.0
 bool   | char   | from == true -> 't', from == false -> 'f'
 bool   | string | from == true -> "true", from == false -> "false"
 string | int    | int(from)
 string | real   | real(from)
 string | char   | from[0] or '\0' if empty
 string | bool   | from == "true" -> true, from == "false" -> false
)";
}

const char *HelpDict::help_string() {
    return R"(
String Operations
-----------------
    strlen - Get string length
             (strlen <string>)

    strget - Return character at position
             (strget <string> <position>)

    strset - Set character at position
             (strset <string> <position> <character>)

    strcat - Concatenate string or character to string
             (strcat <string> <other>)

    substr - Return substring from given string
             (substr <string> <position> [<length>])

   strfind - Find position of character in string
             (strfind <string> <character> [<position>])

  strcount - Count number of occurrences of characeter in string
             (strcount <string> <character>)

    strcmp - Compare two strings lexicographically
             (strcmp <string1> <string2>)

   strsort - Sort string
             (strsort <string> [<descending>])

    strrev - Reverse string
             (strrev <string>)

  strsplit - Split string
             (strsplit <string> <delimiter>)

See ":help generic" for information on string generic functions support.
)";
}

const char *HelpDict::help_charop() {
    return R"(
Character / String Operations
-----------------------------
  isupper - Check upper case
            (isupper <character>)
            (isupper <string>)

  islower - Check lower case
            (islower <character>)
            (islower <string>)

  isalpha - Check alphabet
            (isalpha <character>)
            (isalpha <string>)

  isnumer - Check numeric/digit
            (isnumer <character>)
            (isnumer <string>)

  isalnum - Check alphabet or numeric/digit
            (isalnum <character>)
            (isalnum <string>)

  ispunct - Check punctuation
            (ispunct <character>)
            (ispunct <string>)

  isspace - Check space
            (isspace <character>)
            (isspace <string>)

The string check operations will return true if the
check is true for every character in the string.
)";
}

const char *HelpDict::help_pair() {
    return R"(
Pair Operations
---------------
    pair - Make a pair
           (pair <first> <second>)

   first - Get first item
           (first <pair>)

  second - Get second item
           (second <pair>)

See ":help generic" for information on pair generic functions support.
)";
}

const char *HelpDict::help_array() {
    return R"(
Array Operations
----------------
     array - Make an array
             (array [<element> [<element ...]])

   arraysv - Make an array from size and inital value
             (arraysv <size> [<inital_value>])

             * Default initial value is null

   arraysg - Make an array from size and generator function
             (arraysg <size> <generator_function>)

    arrlen - Return array length
             (arrlen <array>)

    arrget - Return value at position
             (arrget <array> <position>)

    arrset - Set value at position
             (arrset <array> <position> <value>)

   arrpush - Push element to end of array
             (arrpush <array> <value>)

    arrpop - Pop element from end of array
             (arrpop <array>)

   arrfind - Find position of value in array
             (arrfind <array> <value> [<position>])

  arrcount - Count number of occurrences of value in array
             (arrcount <array> <value>)

   arrsort - Sort array
             (arrsort <array> [<descending>])

    arrrev - Reverse array
             (arrrev <array>)

    arrclr - Clear array
             (arrclr <array>)

    arrins - Insert item into array before position
             (arrins <array> <pos> <item>)

    arrrem - Remove item from array at position
             (arrrem <array> <pos>)

See ":help generic" for information on array generic functions support.
)";
}

const char *HelpDict::help_hashmap() {
    return R"(
Hashmap Operations
------------------
  hashmap - Make a hashmap
            (hashmap [(pair <key> <value>) [(pair <key> <value>) ...]])
            (hashmap [(array <key> <value>) [(array <key> <value>) ...]])

            * Using pair and array can be mixed in same hashmap expression

    hmlen - Return hashmap length
            (hmlen <hashmap>)

    hmhas - Check if key is in hashmap
            (hmhas <hashmap> <key>)

    hmget - Get hashmap value for given key
            (hmget <hashmap> <key> [<default_return])

            * Returns default_return when key is not found
            * Default default_return value is null

    hmset - Set hashmap value for given key
            (hmset <hashmap> <key> <value>)

    hmrem - Remove hashmap value with given key
            (hmrem <hashmap> <key>)

    hmclr - Clear hashmap
            (hmclr <hashmap>)

   hmfind - Find hashmap key with given value
            (hmfind <hashmap> <value>)

  hmcount - Count number of occurrences of value in hashmap
            (hmcount <hashmap> <value>)

   hmkeys - Return array of hashmap keys
            (hmkeys <hashmap>)

   hmvals - Return array of hashmap values
            (hmvals <hashmap>)

  hmitems - Return array of hashmap key/value pairs
            (hmitems <hashmap>)

See ":help generic" for information on hashmap generic functions support.
)";
}

const char *HelpDict::help_range() {
    return R"(
Range Operations
----------------
     range - Make a range
             (range <end>)
             (range <begin> <end> [<step>])

             * Range includes begin and excludes end

  rngbegin - Get range begin
             (rngbegin <range>)

    rngend - Get range end
             (rngend <range>)

   rngstep - Get range step
             (rngstep <range>)

    rnglen - Get range length
             (rnglen <range>)

Expand
------
Expand range(s) into an array
  (expand <range> [<range> [<range> ...]])

Loop
----
Use foreach to loop over a range
  (foreach <var> <range>
    <body>)

See ":help generic" for information on range generic functions support.
)";
}

const char *HelpDict::help_generic() {
    return R"(
Generic Functions
-----------------
      len - Length of string, array, hashmap, pair or range
            (len <object>)

    empty - Is string, array, hashmap, pair or range empty?
            (empty <object>)

      get - Get value at index, key or member from string, array, hashmap, pair or userobject
            (get <object> <key> [<default_return>])

            * For string, pair and array, key must be an integer
            * For userobject, key must be a member name/symbol or a string
            * The parameter default_return applies to hashmap and is ignored otherwise

      set - Set value at index, key or member for string, array, hashmap or userobject
            (set <object> <key> <value>)

            * For string and array, key must be an integer
            * For userobject, key must be a member name/symbol or a string

    clear - Clear string, array or hashmap
            (clear <object>)

     find - Get position or key of value in string, array, pair or hashmap
            (find <object> <item> [<position>])

            * For string, item must be a character
            * The parameter position applies to string, pair and array and is ignored otherwise
            * When provided, position must be an integer, and is used as the search start index

    count - Count number of times value occurs in string, array, pair or hashmap
            (count <object> <item>)

            * For string, item must be a character

     sort - Sort string or array
            (sort <obj> [<descending>])

            * When provided, descending must be a boolean

  reverse - Reverse string or array
            (reverse <obj>)

      sum - Sum array, pair or range
            (sum <obj>)

    apply - Apply function to array, pair or range
            (apply <ftn> <obj>)

            * Length of <ftn> parameters must match <obj> length
)";
}

const char *HelpDict::help_block() {
    return R"(
Progn and Block
---------------
Progn and block define a sequence of expressions,
and groups them into one expression.

  (progn
    <expression>
    ...)

  (block
    <expression>
    ...)

Difference between progn and block is that block
defines an new environment and progn does not.

The value of executing a sequence is the result of
evaluating the last expression.

Example:
  (block
    (var x 10)
    (= x (+ x 5))
    (pringln x))
)";
}

const char *HelpDict::help_misc() {
    return R"(
Misc Functions
--------------
  assert - Assert expression is true
           (assert <tag> <expression>)

   clone - Clone a value
           (clone <object>)

    hash - Return a hash of value
           (hash <object>)

           * The return range of hash is [0, 9223372036854775807]
           * Object can be any ishlang value

  random - Return a random integer between 0 and max
           (rand [<max>])

           * The return range of rand is [0, max] if max is specified
           * If max is not specified, then the return range is [0, 4294967295]
           * If max is greater than 4294967295, then 4294967295 is used. Essentially,
             the call is equivalent to not specifying max.
           * If max is 0, then function returns 0

  timeit - Return average time in microseconds to evaluate expression
           (timeit <expr> [<count>] [<summary>])

           * The count is number of times to repeat evaluation, defaults to 1
           * Allowed count range is [1, 1000]
           * The summary is a flag to print a time summary, defaults to true
           * The summary will show count, sum, mean, min and max
)";
}

const char *HelpDict::help_import() {
    return R"(
Imports and Aliases
-------------------
Import full ishlang modules with import command, or import select components using from/import command

  (import <module> [as <asName>])
  (from <module> import name [as <asName] [name [as <asName>]]*)

A module name must have a corresponding source file `<module>.ish`

Ishlang searches for a module source file according to the following rule:
* Look in current working directory
* Then look in directories specified in ISHLANG_PATH environment variable
* Then look in directories specified in ishlang command line -p option

Paths specified in environment variable ISHLANG_PATH or command line -p option must
be delimited using : character

Example
  # Assume module arith.ish exists and is in path
  # and assume arith contains functions add and sub

  # Import
  (import arith)

  # Import as
  (import arith as math)

  # From/Import
  (from arith import add)

  # From/Import as
  (from arith import sub as minus)

  # From/Import
  (from arith import add sub)

  # From/Import as
  (from arith import add as plus sub as minus)
)";
}

const char *HelpDict::help_file_io() {
  return R"(
File IO
-------
     fopen - Open a file for reading or writing
             (fopen <filename> <mode>])

             * Mode must be one of 'r' (read), 'w' (write) or 'a' (append)
             * Use foreach to iterate over lines in a file
             * Use withfile to open a file, do work and close file

    fclose - Close file
             (fclose <file>)

    fflush - Flush file
             (fflush <file>)

   fisopen - Is file open?
             (fisopen <file>)

     fname - Get filename
             (fname <file>)

     fmode - Get mode
             (fmode <file>)

     fread - Read character
             (fread <file>)

   freadln - Read line
             (freadln <file>)

    fwrite - Write character or string
             (fwrite <file> <char_or_str>)

  fwriteln - Write character or string followed by newline
             (fwriteln <file> <char_or_str>)

   fexists - Does file exist?
             (fexists <filename>)

   fremove - Delete file
             (fremove <filename>)
)";
}

const char *HelpDict::help_repl() {
    return R"(
REPL Commands
-------------
    :exit - Quit interpreter
    :quit - Quit interpreter
       ^D - Quit interpreter

        * - Access result of last evaulation

    :load - Load source file
            :load <filename>

   :batch - Switch batch mode on/off
            :batch [<bool>]

    :desc - Describe struct or instance
            :desc <name>

    :help - Help topics
            :help [<topic>]
)";
}
