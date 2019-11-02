#ifndef ENVIRONMENT_H
#define	ENVIRONMENT_H

#include <unordered_map>
#include <string>
#include <memory>

#include "value.h"
#include "exception.h"

namespace Ishlang {
    
    class Environment {
    public:
        using SharedPtr = std::shared_ptr<Environment>;
        
    public:
        Environment(SharedPtr parent=SharedPtr());
        
        const Value &def(const std::string &name, const Value &value);
        const Value &set(const std::string &name, const Value &value);
        const Value &get(const std::string &name) const;
        bool exists(const std::string &name) const;
        
    private:
        using Table = std::unordered_map<std::string, Value>;
        
        SharedPtr parent_;
        Table     table_;
    };
}

#endif	// ENVIRONMENT_H
