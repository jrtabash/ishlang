#include "module.h"
#include "exception.h"

using namespace Ishlang;

// -------------------------------------------------------------
Parser Module::parser_ = Parser();

// -------------------------------------------------------------
Module::Module(const std::string &name, const std::string &sourceFile)
    : name_(name)
    , sourceFile_(sourceFile)
    , env_(std::make_shared<Environment>())
{
}

// -------------------------------------------------------------
Value Module::load() {
    if (env_->empty()) {
        parser_.readFile(sourceFile_, [this](CodeNode::SharedPtr &code) { parserCallback(code); });
        return Value::True;
    }
    return Value::False;
}

// -------------------------------------------------------------
Value Module::loadFromString(const std::string &expr) {
    if (env_->empty() && sourceFile_.empty()) {
        parser_.readMulti(expr, [this](CodeNode::SharedPtr &code) { parserCallback(code); });

        if (parser_.hasIncompleteExpr()) {
            parser_.clearIncompleteExpr();
            throw IncompleteExpression("Module '" + name_ + "' loadFromString");
        }
        return Value::True;
    }
    return Value::False;
}

// -------------------------------------------------------------
Value Module::import(Environment::SharedPtr importEnv, const OptionalName &asName) {
    const std::string &importName = asName ? *asName : name_;
    env_->foreach(
        [this, &importEnv, &importName](const std::string &varName, const Value &varValue) {
            importEnv->def(importName + '.' + varName, varValue);
        });
    return Value::True;
}

// -------------------------------------------------------------
Value Module::alias(Environment::SharedPtr aliasEnv, const std::string &name, const OptionalName &asName) {
    if (env_->exists(name)) {
        const std::string &aliasName = asName ? *asName : name;
        aliasEnv->def(aliasName, env_->get(name));
        return Value::True;
    }
    return Value::False;
}

// -------------------------------------------------------------
void Module::parserCallback(CodeNode::SharedPtr & code) {
    if (code) {
        code->eval(env_);
    }
}
