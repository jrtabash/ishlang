#ifndef LAMBDA_H
#define LAMBDA_H

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

        Value exec(const ArgList &args) const;

        bool operator==(const Lambda &rhs) const {
            return paramEqual(params_, rhs.params_) && body_.get() == rhs.body_.get() && env_.get() == rhs.env_.get();
        }

        bool operator!=(const Lambda &rhs) const {
            return !paramEqual(params_, rhs.params_) || body_.get() != rhs.body_.get() || env_.get() != rhs.env_.get();
        }

        bool operator<(const Lambda &rhs)  const { return params_.size() < rhs.params_.size(); }
        bool operator>(const Lambda &rhs)  const { return params_.size() > rhs.params_.size(); }
        bool operator<=(const Lambda &rhs) const { return params_.size() <= rhs.params_.size(); }
        bool operator>=(const Lambda &rhs) const { return params_.size() >= rhs.params_.size(); }

    private:
        static bool paramEqual(const ParamList &lhs, const ParamList &rhs) {
            return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
        }

    private:
        ParamList                      params_;
        CodeNode::SharedPtr            body_;
        mutable Environment::SharedPtr env_;
    };

}

#endif // LAMBDA_H
