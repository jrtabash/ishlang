# The following commands are available from REPL

## Exit from Interpreter

When on the REPL prompt, enter the command ':exit' or ':quit' to exit program.
Using CTRL-d also exits the REPL.

## Load Source File

The following command loads an Ishlang source file:
```
:load <filename>
```

#### Example
```
:load hello.ish
```

## Switch Batch Mode On/Off

The batch command allows for changing the interpreter's batch mode:
```
:batch [<value>]
```

* If used without an argument, the interpreter's batch mode is toggled.
* If used with an argument, the command expects true or false values.

#### Example
```
:batch true
```

## Access Last Result

Use * to access the result of the last evaulation.

#### Example
```
  >> (+ 5 10)
  15
  >> (+ * 10)
  25
  >> *
  25
  >> 
```

## Describe Struct or Instance

The following command describes structs and instances:
```
:desc <name>
```

#### Example
```
  >> (struct User (id name loc))
  struct:User
  >> (var usr (makeinstance User (id 125) (name "Jon") (loc "USA")))
  instance:User
  >> 
  >> :desc User
  Struct User
    id
    name
    loc
  >> 
  >> :desc usr
  Instance of User
     loc: "USA"
    name: "Jon"
      id: 125
  >> 
```

## Help Topics

The help command provides support for interactive help in REPL.
```
:help [<topic>]
```

When no topic is provided, the command will show usage info with
a list of avaiable help topics.

#### Example
```
>> :help
>> :help loop
```
