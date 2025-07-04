#ifndef ISHLANG_LAMBDA_H
#define ISHLANG_LAMBDA_H

#include "code_node.h"
#include "environment.h"
#include "value.h"

#include <algorithm>
#include <string>
#include <vector>

namespace Ishlang {

    class Lambda {
    public:
        using ParamList = std::vector<std::string>;
        using ArgList   = std::vector<Value>;

    public:
        Lambda() = default;
        Lambda(const ParamList &params, CodeNode::SharedPtr body, Environment::SharedPtr env);

        inline std::size_t paramsSize() const noexcept;

        Value exec(const ArgList &args) const;

        inline bool operator==(const Lambda &rhs) const;
        inline bool operator!=(const Lambda &rhs) const;

        inline bool operator<(const Lambda &rhs) const;
        inline bool operator>(const Lambda &rhs) const;
        inline bool operator<=(const Lambda &rhs) const;
        inline bool operator>=(const Lambda &rhs) const;

    private:
        static inline bool paramEqual(const ParamList &lhs, const ParamList &rhs);

    private:
        ParamList                      params_;
        CodeNode::SharedPtr            body_;
        mutable Environment::SharedPtr env_;
    };

    // --------------------------------------------------------------------------------
    // INLINE

    inline std::size_t Lambda::paramsSize() const noexcept {
        return params_.size();
    }

    inline bool Lambda::operator==(const Lambda &rhs) const {
        return paramEqual(params_, rhs.params_) && body_ == rhs.body_ && env_ == rhs.env_;
    }

    inline bool Lambda::operator!=(const Lambda &rhs) const {
        return !paramEqual(params_, rhs.params_) || body_ != rhs.body_ || env_ != rhs.env_;
    }

    inline bool Lambda::operator<(const Lambda &rhs) const {
        return params_.size() < rhs.params_.size();
    }

    inline bool Lambda::operator>(const Lambda &rhs) const {
        return params_.size() > rhs.params_.size();
    }

    inline bool Lambda::operator<=(const Lambda &rhs) const {
        return params_.size() <= rhs.params_.size();
    }

    inline bool Lambda::operator>=(const Lambda &rhs) const {
        return params_.size() >= rhs.params_.size();
    }

    inline bool Lambda::paramEqual(const ParamList &lhs, const ParamList &rhs) {
        return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

}

#endif // ISHLANG_LAMBDA_H
