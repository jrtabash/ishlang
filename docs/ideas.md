## Ideas

### Add support for a standard library / set of standard modules
1. Add stdlib folder under src folder in ishlang repo
2. New ISHLANG_LIB_PATH vs use ISHLANG_PATH? With ISHLANG_LIB_PATH can enforce module search order
3. Use a default install location similar to default ishlang install location. Update top level Makefile as necessary.

### Add a math standard module
Initial Functions
- abs
- min
- max
- sign
- sqrt
- ceil
- floor

### Add a standard module for parsing command line arguments
- Idea:
  - Command:
    ```bash
    ishlang -f calc_sum.ish -a -v -n 10
    ```
  - Code Example:
    ```
    (import argsparser as ap)
    (var args (ap.make_args))
    (ap.add_arg args "-v" "verbose" bool)
    (ap.add_arg args "-n" "num" int)
    (var pargs (ap.parse_args args))
    (println (ap.get_arg pargs "verbose"))
    (println (ap.get_arg pargs "num"))
    ```
  - Functions:
    ```
    (argsparser.make_args)
    (argsparser.add_arg <arguments> <option_name> <variable_name> <option_type>)
    (argsparser.get_arg <parsed_arguments> <variable_name>)
    (argsparser.parse_args <arguments>)
    ```
  - Notes:
    1. Treat bool arguments as flags, e.g. verbose argument above
    2. parse_args will use the argv array that is set by the -a interpreter option
