#ifndef LAMBDA_H
#define	LAMBDA_H

#include "value.h"
#include "environment.h"
#include "byte_code.h"

#include <string>
#include <vector>
#include <algorithm>

namespace Int {

    class Lambda {
    public:
        typedef std::vector<std::string> ParamList;
        typedef std::vector<Value>       ArgList;

    public:
        Lambda();
        Lambda(const ParamList &params, ByteCode::SharedPtr body, Environment::SharedPtr env);

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
        ByteCode::SharedPtr            body_;
        mutable Environment::SharedPtr env_;
    };

}

#endif // LAMBDA_H
