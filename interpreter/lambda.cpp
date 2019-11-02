#include "lambda.h"

using namespace Ishlang;

// -------------------------------------------------------------
Lambda::Lambda()
    : params_()
    , body_()
    , env_()
{}

// -------------------------------------------------------------
Lambda::Lambda(const ParamList &params, CodeNode::SharedPtr body, Environment::SharedPtr env)
    : params_(params)
    , body_(body)
    , env_(env)
{}

// -------------------------------------------------------------
Value Lambda::exec(const ArgList &args) const {
    if (body_) {
        if (params_.size() != args.size()) {
            throw InvalidArgsSize(params_.size(), args.size());
        }

        Environment::SharedPtr lambdaEnv(std::make_shared<Environment>(env_));

        ParamList::const_iterator pIter = params_.begin();
        ArgList::const_iterator aIter = args.begin();
        for (; pIter != params_.end() && aIter != args.end(); ++pIter, ++aIter) {
            lambdaEnv->def(*pIter, *aIter);
        }

        return body_->exec(lambdaEnv);
    }
    return Value::Null;
}
