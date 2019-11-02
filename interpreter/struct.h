#ifndef STRUCT_H
#define	STRUCT_H

#include <string>
#include <vector>
#include <algorithm>

namespace Ishlang {

    class Struct {
    public:
        using MemberList = std::vector<std::string>;

        Struct();
        Struct(const std::string &name, const MemberList &members);

        bool operator==(const Struct &rhs) const { return name_ == rhs.name_ && membersEqual(members_, rhs.members_); }
        bool operator!=(const Struct &rhs) const { return name_ != rhs.name_ || !membersEqual(members_, rhs.members_); }

        bool operator<(const Struct &rhs)  const { return members_.size() < rhs.members_.size(); }
        bool operator>(const Struct &rhs)  const { return members_.size() > rhs.members_.size(); }
        bool operator<=(const Struct &rhs) const { return members_.size() <= rhs.members_.size(); }
        bool operator>=(const Struct &rhs) const { return members_.size() >= rhs.members_.size(); }

        const std::string &name() const { return name_; }
        const MemberList &members() const { return members_; }

    private:
        static bool membersEqual(const MemberList &lhs, const MemberList &rhs) {
            return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
        }
        
    private:
        std::string name_;
        MemberList  members_;
    };

}

#endif // STRUCT_H
