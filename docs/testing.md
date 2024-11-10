## Unit Tests

### Run all:
```bash
ishlang_unit_test
```

### Run one test:
```bash
ishlang_unit_test -n <test_name>
```

### List all unit tests:
```bash
ishlang_unit_test -l
```

The option -v can be added to all above commands for verbose mode.

## Test Runner Scenarios

### Run all scenarios in a given folder:
```bash
ishlang_run_tests.sh [-v] <ishlang_binary> <ishlang_test_files_binary> <scenarios_folder>
```

### Scenario files
Scenario files must have the extension .test, and should be divided into
two sections, a __CODE__ section and an __EXPECT__ section. Add ishlang
code to the __CODE__ section, and add expected standard output to the
__EXPECT__ section. Several scenarios are provided in the src/test_scenarios folder.

#### Example scenario file (example.test)
```
__CODE__
(var x (+ 1 2))
(println 3)

__EXPECT__
3
```
