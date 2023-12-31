#ifndef INSTANCE_H
#define INSTANCE_H

#include "struct.h"
#include "value.h"

#include <algorithm>
#include <ostream>
#include <string>
#include <unordered_map>

namespace Ishlang {

    class Instance {
    public:
        using MemberTable = std::unordered_map<std::string, Value>;
        using InitArgs = std::unordered_map<std::string, Value>;

        Instance() = default;
        Instance(const Struct &type, InitArgs const & initArgs = InitArgs());

        inline bool operator==(const Instance &rhs) const;
        inline bool operator!=(const Instance &rhs) const;

        inline bool operator<(const Instance &rhs)  const;
        inline bool operator>(const Instance &rhs)  const;
        inline bool operator<=(const Instance &rhs) const;
        inline bool operator>=(const Instance &rhs) const;

        inline const Struct &type() const;

        const Value &get(const std::string &name) const;
        void set(const std::string &name, const Value &value);

        void describe() const;

    public:
        friend std::ostream &operator<<(std::ostream &out, const Instance &inst) {
            out << "instance:" << inst.type().name();
            return out;
        }

    private:
        static inline bool membersEqual(const MemberTable &lhs, const MemberTable &rhs);
        static inline const Value &initArgOrNull(InitArgs const & initArgs, const std::string &name);
        
    private:
        Struct type_;
        MemberTable  members_;
    };

    // --------------------------------------------------------------------------------
    // INLINE

    inline bool Instance::operator==(const Instance &rhs) const {
        return type_ == rhs.type_ && membersEqual(members_, rhs.members_);
    }

    inline bool Instance::operator!=(const Instance &rhs) const {
        return type_ != rhs.type_ || !membersEqual(members_, rhs.members_);
    }

    inline bool Instance::operator<(const Instance &rhs) const {
        return members_.size() < rhs.members_.size();
    }

    inline bool Instance::operator>(const Instance &rhs) const {
        return members_.size() > rhs.members_.size();
    }

    inline bool Instance::operator<=(const Instance &rhs) const {
        return members_.size() <= rhs.members_.size();
    }

    inline bool Instance::operator>=(const Instance &rhs) const {
        return members_.size() >= rhs.members_.size();
    }

    inline const Struct &Instance::type() const {
        return type_;
    }

    inline bool Instance::membersEqual(const MemberTable &lhs, const MemberTable &rhs) {
        return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

    inline const Value &Instance::initArgOrNull(InitArgs const & initArgs, const std::string &name) {
        auto const iter = initArgs.find(name);
        return iter == initArgs.end() ? Value::Null : iter->second;
    }

}

#endif // STRUCT_H
