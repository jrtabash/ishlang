#ifndef ISHLANG_CODE_NODE_BASES_H
#define ISHLANG_CODE_NODE_BASES_H

#include "environment.h"

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace Ishlang {

    // -------------------------------------------------------------
    class CodeNode {
    public:
        using SharedPtr      = std::shared_ptr<CodeNode>;
        using SharedPtrList  = std::vector<SharedPtr>;
        using SharedPtrPair  = std::pair<SharedPtr, SharedPtr>;
        using SharedPtrPairs = std::vector<SharedPtrPair>;
        using ParamList      = std::vector<std::string>;
        using NameAndAsList  = std::vector<std::pair<std::string, std::optional<std::string>>>;
        using NameSharedPtrs = std::vector<std::pair<std::string, SharedPtr>>;

    public:
        CodeNode() {}
        virtual ~CodeNode() {}

        Value eval(Environment::SharedPtr env) {
            if (!env) { throw NullEnvironment(); }
            return this->exec(env);
        }

    public:
        template <typename CodeType, typename ... Args>
        static SharedPtr make(Args && ... args) {
            return std::make_shared<CodeType>(std::forward<Args>(args)...);
        }

    public:
        virtual bool isIdentifier() const {
            return false;
        }

        virtual const std::string &identifierName() const {
            static const std::string Empty;
            return Empty;
        }

    protected:
        virtual Value exec(Environment::SharedPtr env) = 0;
    };

    // -------------------------------------------------------------
    class BinaryOp : public CodeNode {
    public:
        BinaryOp(CodeNode::SharedPtr lhs, CodeNode::SharedPtr rhs)
            : CodeNode()
            , lhs_(lhs)
            , rhs_(rhs)
        {}

        virtual ~BinaryOp() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) override = 0;

    protected:
        CodeNode::SharedPtr lhs_;
        CodeNode::SharedPtr rhs_;
    };

    // -------------------------------------------------------------
    class VariadicOp : public CodeNode {
    public:
        VariadicOp(CodeNode::SharedPtrList operands)
            : CodeNode()
            , operands_(operands)
        {}

        virtual ~VariadicOp() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) override = 0;

    protected:
        CodeNode::SharedPtrList operands_;
    };

}

#endif	// ISHLANG_CODE_NODE_BASES_H
