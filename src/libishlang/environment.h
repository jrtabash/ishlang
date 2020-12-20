#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

#include "exception.h"
#include "value.h"

namespace Ishlang {
    
    class Environment {
    public:
        using SharedPtr = std::shared_ptr<Environment>;
        using ForeachFtn = std::function<void (const std::string &, const Value &)>;
        
    public:
        Environment(SharedPtr parent=SharedPtr());
        
        const Value &def(const std::string &name, const Value &value);
        const Value &set(const std::string &name, const Value &value);
        const Value &get(const std::string &name) const;

        inline bool exists(const std::string &name) const noexcept;
        inline bool empty() const noexcept;
        inline std::size_t size() const noexcept;

        void foreach(ForeachFtn ftn) const;
        
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

}

#endif	// ENVIRONMENT_H
