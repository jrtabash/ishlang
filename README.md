# Ishlang
Ishlang is a fully parenthesized prefix notation programming language inspired by Lisp.

The name Ishlang (ish-lang) is shorthand for "Lisp'ish programming language", and was chosen to pay homage to Lisp. Though it is inspired by Lisp, the language diverts considerably from Lisp, and is not a lisp implementation.

The language supports a rich set of features, including branching, looping, functions, nested-functions, recursion, lambdas/closures, pairs, arrays, hashmaps, ranges, structs and modules.

More details about the language available in [docs/specification.md](docs/specification.md)

REPL command definitions available in [docs/repl_commands.md](docs/repl_commands.md)

List of todos available in [docs/todo.md](docs/todo.md)

Details on testing are provided in [docs/testing.md](docs/testing.md)

## Hello World
```
(println "Hello World")
```

## Building Ishlang
Build debug
```bash
cd src
make

# OR

cd src
make DEBUG=1
```

Build release
```bash
cd src
make DEBUG=0
```

## Running Ishlang
Before running setup library path appropriately:

On Linux
```bash
export LD_LIBRARY_PATH=<path>/<to>/<ishlang>/<binaries>
```

On Darwin
```bash
export DYLD_LIBRARY_PATH=<path>/<to>/<ishlang>/<binaries>
```

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
        ishlang [-h] [-i] [-b] [-p] [-f file] [-e expr] [-a arg1 ... argN]

Options:
        -h : Print usage
        -i : Enter interactive mode
        -b : Run in batch mode
        -p : Import path
        -f : Run code file
        -e : Execute expression, after running file, before entering interactive mode
        -a : Arguments passed to user. Must be last option. Available in argv array
```
