# Ishlang
Ishlang is a fully parenthesized prefix notation programming language inspired by Lisp.

The name Ishlang (ish-lang) is shorthand for "Lisp'ish programming language", and was chosen to pay homage to Lisp. Though it is inspired by Lisp, the language diverts considerably from Lisp, and is not a lisp implementation.

The language is still under development, but already supports a rich set of features, including branching, looping, functions, nested-functions, recursion, lambdas/closures, pairs, arrays, hashmaps, structs and modules.

More details about the language available in [docs/specification.txt](docs/specification.txt)

REPL command definitions available in [docs/repl_commands.md](docs/repl_commands.md)

List of todos available in [docs/todo.txt](docs/todo.txt)

Details on testing are provided in [docs/testing.md](docs/testing.md)

## Example: Hello World Program
```
(println "Hello World")
```

## Basic Commands:
Run source file:
```bash
ishlang -f <filename>
```

Run ishlang REPL:
```bash
ishlang
```

See usage for more information:
```bash
ishlang -h
```
## Ishlang Usage
```bash
Usage:
        ishlang [-h] [-i] [-b] [-p] [-f file]

Options:
        -h : Print usage
        -i : Enter interactive mode
        -b : Run in batch mode
        -p : Import path
        -f : Run code file
```
