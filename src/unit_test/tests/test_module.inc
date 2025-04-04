#include "unit_test_function.h"

#include "exception.h"
#include "module.h"
#include "value.h"

#include <string>

using namespace Ishlang;

// -------------------------------------------------------------
DEFINE_TEST(testModule) {
    const std::string moduleCode = unitTest().defaultModuleCode();

    { // Test import and alias
        auto testEnv = Environment::make();
        Module::SharedPtr module(new Module("test", ""));

        TEST_CASE_MSG(module->name() == "test", "name actual=" << module->name());
        TEST_CASE_MSG(module->sourceFile() == "", "sourceFile actual=" << module->sourceFile());

        Value value = module->loadFromString(moduleCode);
        TEST_CASE_MSG(value == Value::True, "loadFromString moduleCode actual=" << value);

        // Module is already loaded.
        value = module->loadFromString(moduleCode);
        TEST_CASE_MSG(value == Value::False, "loadFromString moduleCode actual=" << value);

        value = module->import(testEnv);
        TEST_CASE_MSG(value == Value::True, "import actual=" << value);
        TEST_CASE_MSG(testEnv->size() == 3, "size actual=" << testEnv->size());
        TEST_CASE(testEnv->exists("test.PI"));
        TEST_CASE(testEnv->exists("test.add"));
        TEST_CASE(testEnv->exists("test.sub"));

        value = module->alias(testEnv, "add");
        TEST_CASE_MSG(value == Value::True, "alias add actual=" << value);
        TEST_CASE_MSG(testEnv->size() == 4, "size actual=" << testEnv->size());
        TEST_CASE(testEnv->exists("test.PI"));
        TEST_CASE(testEnv->exists("test.add"));
        TEST_CASE(testEnv->exists("test.sub"));
        TEST_CASE(testEnv->exists("add"));

        value = module->alias(testEnv, "mul");
        TEST_CASE_MSG(value == Value::False, "alias mul actual=" << value);
        TEST_CASE_MSG(testEnv->size() == 4, "size actual=" << testEnv->size());
        TEST_CASE(testEnv->exists("test.PI"));
        TEST_CASE(testEnv->exists("test.add"));
        TEST_CASE(testEnv->exists("test.sub"));
        TEST_CASE(testEnv->exists("add"));
        TEST_CASE(!testEnv->exists("mul"));
    }

    { // Test alias list
        auto testEnv = Environment::make();
        Module::SharedPtr module(new Module("test", ""));

        Value value = module->loadFromString(moduleCode);
        TEST_CASE_MSG(value == Value::True, "loadFromString moduleCode actual=" << value);

        value = module->aliases(testEnv, { { "add", Module::OptionalName() }, { "sub", "minus" } });
        TEST_CASE_MSG(value == Value::True, "alias add actual=" << value);
        TEST_CASE_MSG(testEnv->size() == 2, "size actual=" << testEnv->size());
        TEST_CASE(testEnv->exists("add"));
        TEST_CASE(testEnv->exists("minus"));
    }

    { // Test import and alias with as name
        auto testEnv = Environment::make();
        Module::SharedPtr module(new Module("test", ""));

        Value value = module->loadFromString(moduleCode);
        TEST_CASE_MSG(value == Value::True, "loadFromString moduleCode actual=" << value);

        value = module->import(testEnv, "astest");
        TEST_CASE_MSG(value == Value::True, "import actual=" << value);
        TEST_CASE_MSG(testEnv->size() == 3, "size actual=" << testEnv->size());
        TEST_CASE(testEnv->exists("astest.PI"));
        TEST_CASE(testEnv->exists("astest.add"));
        TEST_CASE(testEnv->exists("astest.sub"));

        value = module->alias(testEnv, "add", "asadd");
        TEST_CASE_MSG(value == Value::True, "alias add actual=" << value);
        TEST_CASE_MSG(testEnv->size() == 4, "size actual=" << testEnv->size());
        TEST_CASE(testEnv->exists("astest.PI"));
        TEST_CASE(testEnv->exists("astest.add"));
        TEST_CASE(testEnv->exists("astest.sub"));
        TEST_CASE(testEnv->exists("asadd"));
    }

    {
        Module::SharedPtr module(new Module("test", "module.ish"));
        TEST_CASE_MSG(module->name() == "test", "name actual=" << module->name());
        TEST_CASE_MSG(module->sourceFile() == "module.ish", "sourceFile actual=" << module->sourceFile());

        // Module has a source file, cannot load from string.
        Value value = module->loadFromString(moduleCode);
        TEST_CASE_MSG(value == Value::False, "actual=" << value);
    }

    try {
        Module::SharedPtr module(new Module("test", ""));
        const std::string incompleteCode = "(defun mul (x y)";
        module->loadFromString(incompleteCode);
        TEST_CASE(false);
    }
    catch (const IncompleteExpression &ex) {
        TEST_CASE_MSG(ex.what() == std::string("Incomplete expression: Module 'test' loadFromString"), "actual=" << ex.what());
    }
    catch (...) {
        TEST_CASE(false);
    }
}

// -------------------------------------------------------------
DEFINE_TEST(testModuleStorage) {
    const std::string name = "test";
    const std::string name2 = "test2";

    // Test basic exists, add and get
    TEST_CASE(!ModuleStorage::exists(name));
    TEST_CASE(ModuleStorage::add(name, ""));
    TEST_CASE(ModuleStorage::exists(name));
    TEST_CASE(ModuleStorage::get(name));

    // Test adding a duplicate module name
    try {
        ModuleStorage::add(name, "");
        TEST_CASE(false);
    }
    catch (const ModuleError &ex) {
        TEST_CASE(std::string("Module 'test' - Failed to add duplicate module name to module storage") == ex.what());
    }
    catch (...) {
        TEST_CASE(false);
    }

    // Test getting a non-existing module name
    try {
        ModuleStorage::get(name2);
        TEST_CASE(false);
    }
    catch (const ModuleError &ex) {
        TEST_CASE(std::string("Module 'test2' - Failed to find module in module storage") == ex.what());
    }
    catch (...) {
        TEST_CASE(false);
    }

    // Test adding another module
    TEST_CASE(!ModuleStorage::exists(name2));
    TEST_CASE(ModuleStorage::add(name2, ""));
    TEST_CASE(ModuleStorage::get(name2));
    TEST_CASE(ModuleStorage::exists(name2));

    // Test get returns the correct module
    TEST_CASE(ModuleStorage::get(name)->name() == name);
    TEST_CASE(ModuleStorage::get(name2)->name() == name2);
}
