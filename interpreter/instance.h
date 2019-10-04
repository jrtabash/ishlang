#ifndef INSTANCE_H
#define	INSTANCE_H

#include "struct.h"
#include "value.h"

#include <unordered_map>
#include <string>
#include <algorithm>

namespace Int {

    class Instance {
    public:
        using MemberTable = std::unordered_map<std::string, Value>;

        Instance();
        Instance(const Struct &type);

        bool operator==(const Instance &rhs) const { return type_ == rhs.type_ && membersEqual(members_, rhs.members_); }
        bool operator!=(const Instance &rhs) const { return type_ != rhs.type_ || !membersEqual(members_, rhs.members_); }

        bool operator<(const Instance &rhs)  const { return members_.size() < rhs.members_.size(); }
        bool operator>(const Instance &rhs)  const { return members_.size() > rhs.members_.size(); }
        bool operator<=(const Instance &rhs) const { return members_.size() <= rhs.members_.size(); }
        bool operator>=(const Instance &rhs) const { return members_.size() >= rhs.members_.size(); }

        const Struct &type() const { return type_; }

        const Value &get(const std::string &name) const;
        void set(const std::string &name, const Value &value);

    private:
        static bool membersEqual(const MemberTable &lhs, const MemberTable &rhs) {
            return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
        }
        
    private:
        const Struct type_;
        MemberTable  members_;
    };

}

#endif // STRUCT_H
