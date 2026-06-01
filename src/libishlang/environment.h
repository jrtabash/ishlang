#ifndef ISHLANG_ENVIRONMENT_H
#define ISHLANG_ENVIRONMENT_H

#include <concepts>
#include <memory>
#include <string>
#include <unordered_map>

#include "exception.h"
#include "iden_table.h"
#include "value.h"

namespace Ishlang {

    template <typename Function>
    concept EnvForeachInvocable = std::invocable<Function, const std::string &, const Value &>;

    class Environment {
    public:
        using SharedPtr = std::shared_ptr<Environment>;

    public:
        Environment(SharedPtr parent=SharedPtr());

        const Value &def(IdenType iden, const Value &value);
        const Value &set(IdenType iden, const Value &value);
        const Value &get(IdenType iden) const;

        inline const Value &defByName(const std::string &name, const Value &value);
        inline const Value &setByName(const std::string &name, const Value &value);
        inline const Value &getByName(const std::string &name) const;

        inline bool exists(IdenType iden) const noexcept;
        inline bool exists(const std::string &name) const noexcept;

        inline bool empty() const noexcept;
        inline std::size_t size() const noexcept;
        inline void clear() noexcept;

        inline void foreach(EnvForeachInvocable auto && ftn) const;

    public:
        static inline SharedPtr make(SharedPtr parent=SharedPtr());

        static inline IdenTable & idenTable();

    private:
        static IdenTable idenTable_;

    private:
        using Table = std::unordered_map<IdenType, Value>;

        SharedPtr parent_;
        Table     table_;
    };

    // --------------------------------------------------------------------------------
    // INLINE

    inline const Value &Environment::defByName(const std::string &name, const Value &value) {
        return def(idenTable_.mapName(name), value);
    }

    inline const Value &Environment::setByName(const std::string &name, const Value &value) {
        return set(idenTable_.mapName(name), value);
    }

    inline const Value &Environment::getByName(const std::string &name) const {
        return get(idenTable_.mapName(name));
    }

    inline bool Environment::exists(IdenType iden) const noexcept {
        return table_.find(iden) != table_.end();
    }

    inline bool Environment::exists(const std::string &name) const noexcept {
        return exists(idenTable_.mapName(name));
    }

    inline bool Environment::empty() const noexcept {
        return table_.empty();
    }

    inline std::size_t Environment::size() const noexcept {
        return table_.size();
    }

    inline void Environment::clear() noexcept {
        // Do not clear parent
        table_.clear();
    }

    inline void Environment::foreach(EnvForeachInvocable auto && ftn) const {
        for (const auto & nameValue : table_) {
            ftn(idenTable_.getName(nameValue.first), nameValue.second);
        }
    }

    inline auto Environment::make(SharedPtr parent) -> SharedPtr {
        return std::make_shared<Environment>(parent);
    }

    inline IdenTable & Environment::idenTable() {
        return idenTable_;
    }

}

#endif	// ISHLANG_ENVIRONMENT_H
