#ifndef ISHLANG_MODULE_H
#define ISHLANG_MODULE_H

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "code_node.h"
#include "environment.h"
#include "parser.h"

namespace Ishlang {

    // --------------------------------------------------------------------------------
    class Module {
    public:
        using SharedPtr = std::shared_ptr<Module>;
        using OptionalName = std::optional<std::string>;
        using AliasList = CodeNode::NameAndAsList;

    public:
        Module(const std::string &name, const std::string &sourceFile);

        Value load();
        Value loadFromString(const std::string &expr);
        Value import(Environment::SharedPtr importEnv, const OptionalName &asName = std::nullopt);
        Value alias(Environment::SharedPtr aliasEnv, const std::string &name, const OptionalName &asName = std::nullopt);
        Value aliases(Environment::SharedPtr aliasEnv, const AliasList &aliasList);

        inline const std::string &name() const noexcept;
        inline const std::string &sourceFile() const noexcept;

    private:
        void parserCallback(CodeNode::SharedPtr & code);

    private:
        static Parser parser_;

    private:
        std::string name_;
        std::string sourceFile_;
        Environment::SharedPtr env_;
    };

    // --------------------------------------------------------------------------------
    class ModuleStorage {
    public: // Path
        static bool addPath(const std::string &path);
        static bool addPaths(const std::string &path);

        static inline const std::vector<std::string> &paths() noexcept;

    public: // Storage
        static Module::SharedPtr getOrCreate(const std::string &name);
        static Module::SharedPtr add(const std::string &name, const std::string &sourceFile);
        static Module::SharedPtr get(const std::string &name);

        static inline bool exists(const std::string &name) noexcept;

    private:
        static std::string findModuleFile(const std::string &name);

    private:
        static std::vector<std::string> paths_;
        static std::unordered_map<std::string, Module::SharedPtr> storage_;
    };

    // --------------------------------------------------------------------------------
    // INLINE

    inline const std::string &Module::name() const noexcept {
        return name_;
    }

    inline const std::string &Module::sourceFile() const noexcept {
        return sourceFile_;
    }

    inline const std::vector<std::string> &ModuleStorage::paths() noexcept {
        return paths_;
    }

    inline bool ModuleStorage::exists(const std::string &name) noexcept {
        return storage_.find(name) != storage_.end();
    }

}

#endif // ISHLANG_MODULE_H
