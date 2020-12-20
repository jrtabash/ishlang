#ifndef STRUCT_H
#define STRUCT_H

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

        inline bool operator==(const Struct &rhs) const;
        inline bool operator!=(const Struct &rhs) const;

        inline bool operator<(const Struct &rhs) const;
        inline bool operator>(const Struct &rhs) const;
        inline bool operator<=(const Struct &rhs) const;
        inline bool operator>=(const Struct &rhs) const;

        inline const std::string &name() const;
        inline const MemberList &members() const;

    public:
        friend std::ostream &operator<<(std::ostream &out, const Struct &s) {
            out << "struct:" << s.name();
            return out;
        }

    private:
        static inline bool membersEqual(const MemberList &lhs, const MemberList &rhs);
        
    private:
        std::string name_;
        MemberList  members_;
    };

    // --------------------------------------------------------------------------------
    // INLINE

    inline bool Struct::operator==(const Struct &rhs) const {
        return name_ == rhs.name_ && membersEqual(members_, rhs.members_);
    }

    inline bool Struct::operator!=(const Struct &rhs) const {
        return name_ != rhs.name_ || !membersEqual(members_, rhs.members_);
    }

    inline bool Struct::operator<(const Struct &rhs) const {
        return members_.size() < rhs.members_.size();
    }

    inline bool Struct::operator>(const Struct &rhs) const {
        return members_.size() > rhs.members_.size();
    }

    inline bool Struct::operator<=(const Struct &rhs) const {
        return members_.size() <= rhs.members_.size();
    }

    inline bool Struct::operator>=(const Struct &rhs) const {
        return members_.size() >= rhs.members_.size();
    }

    inline const std::string &Struct::name() const {
        return name_;
    }

    inline auto Struct::members() const -> const MemberList & {
        return members_;
    }

    inline bool Struct::membersEqual(const MemberList &lhs, const MemberList &rhs) {
        return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

}

#endif // STRUCT_H
