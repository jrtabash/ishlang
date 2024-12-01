# Language Summary

Ishlang is a fully parenthesized prefix notation programming language inspired by Lisp.

The name Ishlang (ish-lang) is shorthand for "Lisp'ish programming language", and was chosen to pay homage to Lisp. Though it is inspired by Lisp, the language diverts considerably from Lisp, and is not a lisp implementation.

#### Hello world Program
```
(println "Hello World")
```

## Values
Every expression in ishlang evaluates to a value. A value can hold any of the following types:
- none
- int
- real
- char
- bool
- string
- pair
- array
- hashmap
- range
- closure
- usertype
- userobject

## Literals
Can be none, char, int, real, bool, and string.

Examples:
- none:   null
- char:   'a'
- string: "hello"
- int:    30
- real:   25.25
- bool:   true

Reserved literal keywords: null, true, false

## Variables
### Defining
```
(var <name> <expression>)
```

#### Example
```
(var number 5)
```

### Assignment
```
(= <name> <expression>)
```

#### Example
```
(= number 10)
```

### Exists check
```
(? <name>)
```

#### Example
```
(? number)
```

## Input / Output
### Printing
```
(print <expression> ...)
(println <expression> ...)
```

#### Examples
```
(print "Hello ")
(println "World")
(println "Hello " "World")
(println "Hello World")
```

The functions print and println evaluate to null value

### Reading
```
(read)
```

#### Example
```
(var something (read))
```

## Arithmetic
### Variadic operations: + - * / % ^
```
(<op> <expression> <expression> [<expression> ...])
```

When more than 2 operands are given to op, the expression is equivalent to a binary left to right aggregation:
```
(<op> <1> <2> <3> <4>)
<=>
(<op> (<op> (<op> <1> <2>) <3>) <4>)
```

#### Examples
```
(+ 10 12)
(* 2 5)
(+ 1 2 3 4 5)
(* 2 4 6)
```

### Unary operation:
```
(neg <expression>)
```

The neg operator returns the negative of expression.

#### Example
```
(neg 10)
```

## Logical
### Binary operations: == != < > <= >=
```
(op <expression> <expression>)
```

### Variadic operations: and or
```
(op <expression> <expression> [<expression> ...])
```

#### Examples
```
(== x y)
(or (== x y) (> y 10))
(and (== (% x 2) 0) (> x y) (> y 10))
```

### Unary operation
```
(not <expression>)
```

#### Examples
```
(not true)
```

## If Statement
```
(if <condition_expression> <true_expression> [<false_expression>])
```

### Example
```
(if (> x 0) 1 0)
```

## When Statement
```
(when <condition_expression> <true_expression>)
```

The when form is syntactic sugar for the following if/true statement:
```
(if <condition_expression> <true_expression>)
```

## Example
```
(when (> x 0) 1)
```

## Unless Statement
```
(unless <condition_expression> <false_expression>)
```

The unless form is syntactic sugar for the following if/not statement:
```
(if (not <condition_expression>) <true_expression>)
```

### Example
```
(unless (<= x 0) 1)
```

## Cond Statement
```
(cond
  (<condition> <expression>)
  ...
  (true <expression>))
```

The (true ...) case acts as a default catch all case, and is optional

### Example
```
(var check1 false)
(var check2 false)
...
(cond
 (check1 (println "Check1"))
 (check2 (println "Check2"))
 (true   (println "Neither Check")))
```

## Progn and Block
Progn and block define a sequence of expression, and groups them into one expression.
```
(progn
  <expression>
  ...)

(block
  <expression>
  ...)
```

Difference between progn and block is that block defines an new environment and progn does not.

The value of executing a sequence is the result of evaluating the last expression.

### Example
```
(progn
  (var x 10)
  (= x (+ x 5))
  (println x))

(block
  (var x 10)
  (= x (+ x 5))
  (pringln x))

```
In the progn case the variable x is defined in the enclosing environment, and as such
exists outside the boundary of progn. In the block case, the variable x is defined in
the block environment, and is not available outside the scope of block.

## Looping
General form:
```
(loop <decl_expression> <cond_expression> <next_expression>
  <body_expression>)
```

Conditional only form:
```
(loop <cond_expression>
  <body_expression>)
```

### Example - general form
```
(var count 0)
(loop (var i 1) (<= i 10) (= i (+ i 1))
  (= count (+ count i)))
```

### Example - conditional form
```
(var count 0)
(var term 0)
(loop (< count 55)
  (progn
    (= term (+ term 1))
    (= count (+ count term))))
```

## Foreach
```
(foreach <var> <iterable_expression>
  <body_expression>)
```

### Notes
- Loop over each element in `<iterable_expression>`
- Iterable expression can be a string, array, hashmap or range
- The `<var>` variable is read-only and cannot directly modify iterable elemets

### Example - sum array elements
```
(var sum 0)
(foreach x (array 1 2 3 4 5)
  (= sum (+ sum x)))
```

### Example - sum hashmap values
```
(var sum 0)
(foreach kv (hashmap (pair 1 10) (pair 2 20) (pair 3 30))
  (= sum (+ sum (second kv))))
```

### Example - sum range
```
(var sum 0)
(foreach i (range 10)
  (= sum (+ sum i)))
```

## Functions
User defined functions:
```
(defun <name> <param_list>
  <body>)
```

Functions can be nested, i.e defined inside other functions.

Language supports recursion.

### Example
```
(defun hello ()
  (println "Hello world"))

(defun add (x y)
  (+ x y))

(defun foobar (x y)
  (var z (+ x y))
  (if (> z 10)
      (/ z 2)
    z)))
```

## Lambda
The lambda expression defines a closure.
```
(lambda <param_list>
  <body>)
```

The closure environment is the enclosing environment where the lambda is defined.

When a lambda function is executed, the lambda execution environment will inherit the closure environment.

### Examples
```
(lambda (x y)
  (+ x y))

(var myAdd (lambda (x y) (+ x y)))
(println (myAdd 2 3))

(var x 0)
(var addOne (lambda () (= x (+ x 1))))
(addOne)
(println x)
```

## Structs
Define a user type:
```
(struct <name>
  <member_list>)
```

Creating an instance:
```
(makeinstance <name> [(member value) [(member value) ...]])
```

Setting / Getting member fields:
```
(memget <instance_expression> <member_name>)
(memset <instance_expression> <member_name> <expression>)
```

### Example
```
(struct Person
  (name
   age))

(var p (makeinstance Person (name "John")))
(memset p age 25)
(println (memget p name))
(println (memget p age))
```

## Type Checking
Is type of?
```
(istypeof <expression> <type> [<type> ...])
```

Check expression type matches any of provided types.

`<type>` must be one of: none, int, real, char, bool, string, pair, array, hashmap, range, closure, usertype, userobject

### Example
```
(istypeof 10 int)
(istypeof 10 int real)
(istypeof Person usertype)
```

Is struct name?
```
(isstructname <expression> <struct>)
```

### Example
```
(var s Person)
(isstructname s Person)
```

Is instance of?
```
(isinstanceof <expression> <struct>)
```

### Example
```
(var p (makeinstance Person))
(isinstanceof p Person)
```

## Type Name
Get type name of:
```
(typename <expression>)
```

Returns the name of the type of the expression.

### Example
```
(typename 10)
(typename Person)
```

Get struct name:
```
(structname <expression>)
```

Returns struct name, expression must be an instance or a struct.

### Example
```
(struct Person (name))
(var p (makeinstance Person))
(structname p)
```

## Type Conversion
Convert value to value of another type:
```
(astype <expr> <type>)
```

Supports the following type conversions:
| From |  To  |   Equivalent to |
| --- | --- | --- |
| int | real | float(from) |
| int | char | ascii(from) |
| int | bool | from != 0 |
| int | string | string(from) |
| real | int | int(from) |
| real | char | ascii(int(from)) |
| real | bool | int(from) != 0 |
| real | string | string(from) |
| char | int | ascii_value(from) |
| char | real | ascii_value(int(from)) |
| char | bool | from == 't' -> true, from == 'f' -> false |
| char | string | string(from) |
| bool | int | from == true -> 1, from == false -> 0 |
| bool | real | from == true -> 1.0, from == false -> 0.0 |
| bool | char | from == true -> 't', from == false -> 'f' |
| bool | string | from == true -> "true", from == false -> "false" |
| string | int | int(from) |
| string | real | real(from) |
| string | char | from[0] or '\0' if empty |
| string | bool | from == "true" -> true, from == "false" -> false |

### Example
```
(astype true int)
(astype 25.12 int)
(astype 25 real)
(astype "10" int)
(astype 't' bool)
(astype 99 char)
(astype 12.34 string)
```

## Asserting
Assert expression is true
```
(assert <tag> <expression>)
```

### Example
```
(assert Check1 (== 1 1)) ;; Success
(assert Check2 (>= 3 2)) ;; Success
(assert Check3 (<= 3 2)) ;; Error: Assert Check3 failed
(assert Check4 (+ 1 2))  ;; Error: Assert Check4 failed (unable to check non-boolean expression)
```

## Cloning
Clone a value:
```
(clone <expression>)
```

will clone the value resulting from executing expression

### Example
```
(var p (makeinstance Person (name "John") (age 25)))
(var q (clone p))
```

## String Operations
**strlen**: Compute string length
```
(strlen <string>)
```

**strget**: Return character at position
```
(strget <string> <position>)
```

**strset**: Set character at position
```
(strset <string> <position> <character>)
```

**strcat**: Concatenate string or character to string
```
(strcat <string> <other>)
```

**substr**: Return substring from given string
```
(substr <string> <position> [<length>]
```

**strfind**: Find position of character in string
```
(strfind <string> <character> [<position>])
```

**strcount**: Count number of occurrences of characeter in string
```
(strcount <string> <character>)
```

**strcmp**: Compare two strings lexicographically
```
(strcmp <string1> <string2>)
```

**strsort**: Sort string
```
(strsort <string> [<descending>])
```

**strrev**: Reverse string
```
(strrev <string>)
```

**strsplit**: Split string
```
(strsplit <string> <delimiter>)
```

### Examples
```
(var str "Hello!")
(println (strlen str))
(println (strget str 3))
(strset str 5 ' ')
(strcat str "World!")
(println (substr str 0 5))
(println (strfind str 'W'))
(println (strcount str 'l'))
(println (strcmp "ab" "bc"))
(println (strsort str))
(println (strsort str true))
(println (strsort str false))
(println (strrev str))
(println (strsplit "1 2 3 4 5" ' '))
```

## String / Character Operations
**isupper**: Check upper case
```
(isupper <character>)
(isupper <string>)
```

**islower**: Check lower case
```
(islower <character>)
(islower <string>)
```

**isalpha**: Check alphabet
```
(isalpha <character>)
(isalpha <string>)
```

**isnumer**: Check numeric/digit
```
(isnumer <character>)
(isnumer <string>)
```

**isalnum**: Check alphabet or numeric/digit
```
(isalnum <character>)
(isalnum <string>)
```

**ispunct**: Check punctuation
```
(ispunct <character>)
(ispunct <string>)
```

**isspace**: Check space
```
(isspace <character>)
(isspace <string>)
```

The string check operations will return true if
the check is true for every character in the string.

### Examples
```
(var c 'U')
(var s "UPPER")
(var n "124")
(var p '?')
(var a ' ')
(isupper c)
(isupper s)
(islower c)
(islower s)
(isalpha s)
(isnumer n)
(isalnum n)
(ispunct p)
(isspace a)
```

## Pair Operations
**pair**: Make a pair
```
(pair <first> <second>)
```

**first**: Get first item
```
(first <pair>)
```

**second**: Get second item
```
(second <pair>)
```

### Examples
```
(var p (pair 1 2))
(first p)
(second p)
```

## Array Operations
**array**: Make an array
```
(array [<element> [<element ...]])
```

**arraysv**: Make an array from size and inital value. Default initial value is null
```
(arraysv <size> [<inital_value>])
```

**arrlen**: Return array length
```
(arrlen <array>)
```

**arrget**: Return value at position
```
(arrget <array> <position>)
```

**arrset**: Set value at position
```
(arrset <array> <position> <value>)
```

**arrpush**: Push element to end of array
```
(arrpush <array> <value>)
```

**arrpop**: Pop element from end of array
```
(arrpop <array>)
```

**arrfind**: Find position of value in array
```
(arrfind <array> <value> [<position>])
```

**arrcount**: Count number of occurrences of value in array
```
(arrcount <array> <value>)
```

**arrsort**: Sort array
```
(arrsort <array> [<descending>])
```

**arrrev**: Reverse array
```
(arrrev <array>)
```

**arrclr**: Clear array
```
(arrclr <array>)
```

**arrins**: Insert item into array before position
```
(arrins <array> <pos> <item>)
```

**arrrem**: Remove item from array at position
```
(arrrem <array> <pos>)
```

### Examples
```
(var a (array))
(arrpush a 1)
(arrpush a 2)
(println (arrlen a))
(println (arrget a 0))
(arrset a 1 20)
(println (arrget a 0))
(println (arrget a 1))
(println (arraysv 5))
(println (arraysv 10 0))
(println (arrfind a 2))
(println (arrcount a 2))
(println (arrsort a))
(println (arrsort a true))
(println (arrsort a false))
(println (arrrev a))
(arrins a 0 100)
(arrins a 1 200)
(arrclr a)
```

## Hashmap Operations

**hashmap**: Make a hashmap
```
(hashmap [(pair <key> <value>) [(pair <key> <value>) ...]])
(hashmap [(array <key> <value>) [(array <key> <value>) ...]])
```
Note: Using pair and array can be mixed in same hashmap expression. See example below.

**hmlen**: Return hashmap length
```
(hmlen <hashmap>)
```

**hmhas**: Check if key is in hashmap
```
(hmhas <hashmap> <key>)
```

**hmget**: Get hashmap value for given key. Returns default_return when key is not found. Default default_return value is null
```
(hmget <hashmap> <key> [<default_return])
```

**hmset**: Set hashmap value for given key
```
(hmset <hashmap> <key> <value>)
```

**hmrem**: Remove hashmap value with given key
```
(hmrem <hashmap> <key>)
```

**hmclr**: Clear hashmap
```
(hmclr <hashmap>)
```

**hmfind**: Find hashmap key with given value
```
(hmfind <hashmap> <value>)
```

**hmcount**: Count number of occurrences of value in hashmap
```
(hmcount <hashmap> <value>)
```

**hmkeys**: Return array of hashmap keys
```
(hmkeys <hashmap>)
```

**hmvals**: Return array of hashmap values
```
(hmvals <hashmap>)
```

**hmitems**: Return array of hashmap key/value pairs
```
(hmitems <hashmap>)
```

### Examples
```
(var ht (hashmap (array "one" 1) (array "two" 2) (array "four" 4)))
(var ht2 (hashmap (pair "one" 1) (pair "two" 2)))
(var ht3 (hashmap (pair "one" 1) (array "two" 2) (pair "three" 3)))
(hmlen ht)
(hmhas ht "one")
(hmget ht "two")
(hmget ht "two" 0)
(hmset ht "three" 3)
(hmrem ht "three")
(hmfind ht 2)
(hmcount ht 1)
(hmkeys ht)
(hmvals ht)
(hmitems ht)
(hmclr ht)
```

## Range Operations

**range**: Make a range
```
(range <end>)
(range <begin> <end> [<step>])
```

### Notes
- Range includes begin and excludes end
- Use foreach to loop over ranges
- Use expand to expand ranges to an array

**rngbegin**: Get range begin
```
(rngbegin <range>)
```

**rngend**: Get range end
```
(rngend <range>)
```

**rngstep**: Get range step
```
(rngstep <range>)
```

**rnglen**: Get range length
```
(rnglen <range>)
```

### Examples
```
(var rng (range 10))
(var rng2 (range 2 10 2))
(rngbegin rng)
(rngend rng)
(rngstep rng)
(rnglen rng)
(foreach i rng (println i))
(expand rng)
```

## Expand
```
(expand <range> [<range> [<range> ...]])
```

### Example
```
(expand (range 10))
(expand (range 10) (range 10 100 10))
```

## Generic Functions
**len**: Length of string, array, hashmap or range
```
(len <object>)
```

**empty**: Is string, array, hashmap or range empty?
```
(empty <object>)
```

**get**: Get value at index, key or member from string, array, hashmap or userobject
```
(get <object> <key> [<default_return>])
```

### Notes
- For string and array, key must be an integer
- For userobject, key must be a member name/symbol or a string
- The parameter default_return applies to hashmap and is ignored otherwise

**set**: Set value at index, key or member for string, array, hashmap or userobject
```
(set <object> <key> <value>)
```

### Notes
- For string and array, key must be an integer
- For userobject, key must be a member name/symbol or a string

**clear**: Clear string, array or hashmap
```
(clear <object>)
```

**find**: Get position or key of value in string, array or hashmap
```
(find <object> <item> [<position>])
```

### Notes
- For string, item must be a character
- The parameter position applies to string and array and is ignored otherwise
- When provided, position must be an integer, and is used as the search start index

**count**: Count number of times value occurs in string, array or hashmap
```
(count <object> <item>)
```

### Notes
- For string, item must be a character

**sort**: Sort string or array
```
(sort <obj> [<descending>])
```

### Notes
- When provided, descending must be a boolean

### Examples
```
(struct Person (name age))
(var txt "hello")
(var seq (array 1 2 3))
(var tab (hashmap (pair 1 10) (pair 2 20)))
(var obj (makeinstance Person (name "Jon") (age 25)))
(bar rng (range 10))

(len txt)
(len seq)
(len tab)
(len rng)
(empty txt)
(empty seq)
(empty tab)
(empty rng)
(get txt 0)
(get seq 1)
(get tab 2 200)
(get obj name)
(get obj "name")
(find txt 'l')
(find txt 'l' 3)
(find seq 3)
(find tab 20)
(count txt 'l')
(count seq 2)
(count tab 20)
(set txt 0 'H')
(set seq 1 20)
(set tab 3 30)
(set obj age 26)
(set obj "age" 27)
(clear txt)
(clear seq)
(clear tab)
```

## Comments
Comments start with ;; and continue for the remainder of the line.
Comments can be occupy full or partial lines.
```
  ;; <comment text>
```

### Examples
```
;; This is a function
(defun x2 (x) ;; Double x
  (* 2 x))
```

## Imports and Aliases
Import full ishlang files with import command, or import select components using from/import command.
```
(import <module> [as <asName>])
(from <module> import name [as <asName] [name [as <asName>]]*)
```

A module name must have a corresponding source file `<module>.ish`.

Ishlang searches for a module source file according to the following rule:
1. Look in current working directory
2. Then look in directories specified in ISHLANG_PATH environment variable
3. Then look in directories specified in ishlang command line -p option

Paths specified in environment variable ISHLANG_PATH or command line -p option must
be delimited using : character.

### Examples
- Assuming the following module is defined:
  - name: arith.ish
  - code:
   ```
   (defun add (x y) (+ x y))
   (defun sub (x y) (- x y))
   ```
- Module uses:
  - Import:
    ```
    (import arith)
    (println (arith.add 1 2))
    ```
  - Import as
    ```
    (import arith as math)
    (println (math.sub 10 5))
    ```
  - From/Import
    ```
    (from arith import add)
    (println (add 1 2))
    ```
  - From/Import as
    ```
    (from arith import sub as minus)
    (println (minus 10 5))
    ```
  - From/Import ...
    ```
    (from arith import add sub)
    (println (add 1 2))
    (println (sub 10 5))
    ```
  - From/Import as ...
    ```
    (from arith import add as plus sub as minus)
    (println (plus 1 2))
    (println (minus 10 5))
    ```

## Random Function
Return a random integer between 0 and max.
```
(rand [<max>])
```

The return range of rand is [0, max] if max is specified.

If max is not specified, then the return range is [0, 4294967295].

If max is greater than 4294967295, then 4294967295 is used. Essentially,
the call is equivalent to not specifying max.

If max is 0, then function returns 0.

### Examples
```
(rand)
(rand 10)
```

## Hash Function
Return a hash of operand.
```
(hash <operand>)
```

The return range of hash is [0, 9223372036854775807].

Operand can be any ishlang value.

Examples:
```
(struct Foo (bar))
(var inst (makeinstance Foo))
(defun ftn () 5)
(var a (array 1 2))

(hash 100)
(hash "string")
(hash 'a')
(hash true)
(hash null)
(hash 3.14)
(hash Foo)
(hash inst)
(hash ftn)
(hash a)
```
