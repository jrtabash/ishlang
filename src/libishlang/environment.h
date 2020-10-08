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

        bool exists(const std::string &name) const noexcept;
        bool empty() const noexcept;
        std::size_t size() const noexcept;

        void foreach(ForeachFtn ftn) const;
        
    private:
        using Table = std::unordered_map<std::string, Value>;
        
        SharedPtr parent_;
        Table     table_;
    };
}

#endif	// ENVIRONMENT_H
