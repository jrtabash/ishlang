#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <concepts>
#include <memory>
#include <string>
#include <unordered_map>

#include "exception.h"
#include "value.h"

namespace Ishlang {

    template <typename Function>
    concept EnvForeachInvocable = std::invocable<Function, const std::string &, const Value &>;

    class Environment {
    public:
        using SharedPtr = std::shared_ptr<Environment>;

    public:
        Environment(SharedPtr parent=SharedPtr());
        
        const Value &def(const std::string &name, const Value &value);
        const Value &set(const std::string &name, const Value &value);
        const Value &get(const std::string &name) const;

        inline bool exists(const std::string &name) const noexcept;
        inline bool empty() const noexcept;
        inline std::size_t size() const noexcept;

        inline void foreach(EnvForeachInvocable auto && ftn) const;
        
    private:
        using Table = std::unordered_map<std::string, Value>;
        
        SharedPtr parent_;
        Table     table_;
    };

    // --------------------------------------------------------------------------------
    // INLINE

    inline bool Environment::exists(const std::string &name) const noexcept {
        return table_.find(name) != table_.end();
    }

    inline bool Environment::empty() const noexcept {
        return table_.empty();
    }

    inline std::size_t Environment::size() const noexcept {
        return table_.size();
    }

    inline void Environment::foreach(EnvForeachInvocable auto && ftn) const {
        for (const auto & nameValue : table_) {
            ftn(nameValue.first, nameValue.second);
        }
    }

}

#endif	// ENVIRONMENT_H
