#include "module.h"
#include "exception.h"
#include "util.h"

using namespace Ishlang;

// -------------------------------------------------------------
// MODULE
// -------------------------------------------------------------

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
Value Module::aliases(Environment::SharedPtr aliasEnv, const AliasList &aliasList) {
    std::size_t count = 0;
    for (auto const & varAndAs : aliasList) {
        if (alias(aliasEnv, varAndAs.first, varAndAs.second) == Value::True) {
            ++count;
        }
    }
    return Value(count == aliasList.size());
}

// -------------------------------------------------------------
void Module::parserCallback(CodeNode::SharedPtr & code) {
    if (code) {
        code->eval(env_);
    }
}

// -------------------------------------------------------------
// MODULE STORAGE
// -------------------------------------------------------------

// -------------------------------------------------------------
std::vector<std::string> ModuleStorage::paths_;
std::unordered_map<std::string, Module::SharedPtr> ModuleStorage::storage_;

// -------------------------------------------------------------
bool ModuleStorage::addPath(const std::string &path) {
    if (Util::pathExists(path) && Util::isDirectory(path)) {
        paths_.push_back(path);
        return true;
    }
    return false;
}

// -------------------------------------------------------------
bool ModuleStorage::addPaths(const std::string &path) {
    bool ret = true;
    for (auto const & p : Util::split(path, ':')) {
        if (!addPath(p)) {
            ret = false;
        }
    }
    return ret;
}

// -------------------------------------------------------------
Module::SharedPtr ModuleStorage::getOrCreate(const std::string &name) {
    if (exists(name)) {
        return get(name);
    }
    else {
        auto ptr = add(name, findModuleFile(name));
        if (ptr && !ptr->sourceFile().empty()) {
            ptr->load();
        }
        return ptr;
    }
}

// -------------------------------------------------------------
Module::SharedPtr ModuleStorage::add(const std::string &name, const std::string &sourceFile) {
    if (exists(name)) {
        throw ModuleError(name, "Failed to add duplicate module name to module storage");
    }

    auto [iter, success] = storage_.emplace(name, std::make_shared<Module>(name, sourceFile));
    if (!success) {
        throw ModuleError(name, "Failed to add module to module storage");
    }

    return iter->second;
}

// -------------------------------------------------------------
Module::SharedPtr ModuleStorage::get(const std::string &name) {
    auto iter = storage_.find(name);
    if (iter == storage_.end()) {
        throw ModuleError(name, "Failed to find module in module storage");
    }

    return iter->second;
}

// -------------------------------------------------------------
std::string ModuleStorage::findModuleFile(const std::string &name) {
    const auto filename = name + ".ish";
    auto filePath = Util::findFilePath(Util::currentPath(), filename);

    if (!filePath) {
        for (auto const & path : paths_) {
            filePath = Util::findFilePath(path, filename);
            if (filePath) {
                break;
            }
        }

        if (!filePath) {
            throw ModuleError(name, std::string("Cannot find module source file '") + filename + "'");
        }
    }

    return filePath->string();
}
