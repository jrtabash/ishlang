#ifndef STRUCT_H
#define	STRUCT_H

#include <algorithm>
#include <ostream>
#include <string>
#include <vector>

namespace Ishlang {

    class Struct {
    public:
        using MemberList = std::vector<std::string>;

        Struct() = default;
        Struct(const std::string &name, const MemberList &members);

        bool operator==(const Struct &rhs) const { return name_ == rhs.name_ && membersEqual(members_, rhs.members_); }
        bool operator!=(const Struct &rhs) const { return name_ != rhs.name_ || !membersEqual(members_, rhs.members_); }

        bool operator<(const Struct &rhs)  const { return members_.size() < rhs.members_.size(); }
        bool operator>(const Struct &rhs)  const { return members_.size() > rhs.members_.size(); }
        bool operator<=(const Struct &rhs) const { return members_.size() <= rhs.members_.size(); }
        bool operator>=(const Struct &rhs) const { return members_.size() >= rhs.members_.size(); }

        const std::string &name() const { return name_; }
        const MemberList &members() const { return members_; }

        friend std::ostream &operator<<(std::ostream &out, const Struct &s) {
            out << "struct:" << s.name();
            return out;
        }

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
