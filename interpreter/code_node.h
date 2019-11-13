#ifndef BYTE_CODE_H
#define	BYTE_CODE_H

#include "environment.h"
#include "struct.h"
#include "value.h"

#include <memory>
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
        
    public:
        CodeNode() {}
        virtual ~CodeNode() {}
        
        Value eval(Environment::SharedPtr env) {
            if (!env.get()) { throw NullEnvironment(); }
            return this->exec(env);
        }
        
        virtual Value exec(Environment::SharedPtr env) = 0;
    };
    
    // -------------------------------------------------------------
    class Literal : public CodeNode {
    public:
        Literal(const Value &value) : CodeNode(), value_(value) {}
        virtual ~Literal() {}

        virtual Value exec(Environment::SharedPtr /*env*/) { return value_; }

    private:
        Value value_;
    };

    // -------------------------------------------------------------
    class IsType : public CodeNode {
    public:
        IsType(CodeNode::SharedPtr expr, Value::Type type);
        virtual ~IsType() {}

        virtual Value exec(Environment::SharedPtr env);

    private:
        CodeNode::SharedPtr expr_;
        Value::Type         type_;
    };

    // -------------------------------------------------------------
    class Variable : public CodeNode {
    public:
        Variable(const std::string &name) : CodeNode(), name_(name) {}
        virtual ~Variable() {}

        virtual Value exec(Environment::SharedPtr env) { return env->get(name_); }
        
    private:
        std::string name_;
    };

    // -------------------------------------------------------------
    class Define : public CodeNode {
    public:
        Define(const std::string &name, CodeNode::SharedPtr code) : CodeNode(), name_(name), code_(code) {}
        virtual ~Define() {}
        
        virtual Value exec(Environment::SharedPtr env) { return env->def(name_, code_ ? code_->exec(env) : Value::Null); }

    private:
        std::string         name_;
        CodeNode::SharedPtr code_;
    };
    
    // -------------------------------------------------------------
    class Assign : public CodeNode {
    public:
        Assign(const std::string &name, CodeNode::SharedPtr code) : CodeNode(), name_(name), code_(code) {}
        virtual ~Assign() {}

        virtual Value exec(Environment::SharedPtr env) { return env->set(name_, code_ ? code_->exec(env) : Value::Null); }
        
    private:
        std::string         name_;
        CodeNode::SharedPtr code_;
    };

    // -------------------------------------------------------------
    class Exists : public CodeNode {
    public:
        Exists(const std::string &name) : name_(name) {}
        virtual ~Exists() {}

        virtual Value exec(Environment::SharedPtr env) { return Value(env->exists(name_)); }

    private:
        std::string name_;
    };

    // -------------------------------------------------------------
    class Clone : public CodeNode {
    public:
        Clone(CodeNode::SharedPtr code) : CodeNode(), code_(code) {}
        virtual ~Clone() {}

        virtual Value exec(Environment::SharedPtr env) { return code_->exec(env).clone(); }

    private:
        CodeNode::SharedPtr code_;
    };

    // -------------------------------------------------------------
    class BinaryOp : public CodeNode {
    public:
        BinaryOp(CodeNode::SharedPtr lhs, CodeNode::SharedPtr rhs) : lhs_(lhs), rhs_(rhs) {}
        virtual ~BinaryOp() {}
        
        virtual Value exec(Environment::SharedPtr env) = 0;
        
    protected:
        CodeNode::SharedPtr lhs_;
        CodeNode::SharedPtr rhs_;
    };
    
    // -------------------------------------------------------------
    class ArithOp : public BinaryOp {
    public:
        enum Type {
            Add = '+',
            Sub = '-',
            Mul = '*',
            Div = '/',
            Mod = '%',
            Pow = '^',
        };
        
    public:
        ArithOp(Type type, CodeNode::SharedPtr lhs, CodeNode::SharedPtr rhs);
        virtual ~ArithOp() {}
        
        virtual Value exec(Environment::SharedPtr env);
        
    private:
        Type type_;
    };
    
    // -------------------------------------------------------------
    class CompOp : public BinaryOp {
    public:
        enum Type {
            EQ = '=',
            NE = '!',
            LT = '<',
            GT = '>',
            LE = 'l',
            GE = 'g',
        };
        
    public:
        CompOp(Type type, CodeNode::SharedPtr lhs, CodeNode::SharedPtr rhs);
        virtual ~CompOp() {}
        
        virtual Value exec(Environment::SharedPtr env);
        
    private:
        Type type_;
    };
    
    // -------------------------------------------------------------
    class LogicOp : public BinaryOp {
    public:
        enum Type {
            Conjunction = '&',
            Disjunction = '|',
        };
        
    public:
        LogicOp(Type type, CodeNode::SharedPtr lhs, CodeNode::SharedPtr rhs);
        virtual ~LogicOp() {}
        
        virtual Value exec(Environment::SharedPtr env);
        
    private:
        Type type_;
    };

    // -------------------------------------------------------------
    class Not : public CodeNode {
    public:
        Not(CodeNode::SharedPtr operand);
        virtual ~Not() {}

        virtual Value exec(Environment::SharedPtr env);

    private:
        CodeNode::SharedPtr operand_;
    };

    // -------------------------------------------------------------
    class ProgN : public CodeNode {
    public:
        ProgN(CodeNode::SharedPtrList exprs);
        virtual ~ProgN() {}
        
        virtual Value exec(Environment::SharedPtr env);
        
    private:
        CodeNode::SharedPtrList exprs_;
    };

    // -------------------------------------------------------------
    class Block : public ProgN {
    public:
        Block(CodeNode::SharedPtrList exprs);
        virtual ~Block() {}
        
        virtual Value exec(Environment::SharedPtr env);
    };

    // -------------------------------------------------------------
    class If : public CodeNode {
    public:
        If(CodeNode::SharedPtr pred, CodeNode::SharedPtr tCode);
        If(CodeNode::SharedPtr pred, CodeNode::SharedPtr tCode, CodeNode::SharedPtr fCode);
        virtual ~If() {}
        
        virtual Value exec(Environment::SharedPtr env);
        
    private:
        CodeNode::SharedPtr pred_;
        CodeNode::SharedPtr tCode_;
        CodeNode::SharedPtr fCode_;
    };
    
    // -------------------------------------------------------------
    class Cond : public CodeNode {
    public:
        Cond(CodeNode::SharedPtrPairs cases);
        virtual ~Cond() {}
        
        virtual Value exec(Environment::SharedPtr env);
        
    private:
        CodeNode::SharedPtrPairs cases_;
    };

    // -------------------------------------------------------------
    class Break : public CodeNode {
    public:
        struct Except {};
        
    public:
        Break() {}
        virtual ~Break() {}
        
        virtual Value exec(Environment::SharedPtr /*env*/) { throw Except(); }
    };

    // -------------------------------------------------------------
    class Loop : public CodeNode {
    public:
        Loop(CodeNode::SharedPtr decl, CodeNode::SharedPtr cond, CodeNode::SharedPtr next, CodeNode::SharedPtr body);
        Loop(CodeNode::SharedPtr cond, CodeNode::SharedPtr body);
        virtual ~Loop() {}
        
        virtual Value exec(Environment::SharedPtr env);
        
    private:
        CodeNode::SharedPtr decl_;
        CodeNode::SharedPtr cond_;
        CodeNode::SharedPtr next_;
        CodeNode::SharedPtr body_;
    };

    // -------------------------------------------------------------
    class LambdaExpr : public CodeNode {
    public:
        LambdaExpr(const ParamList &params, CodeNode::SharedPtr body);
        virtual ~LambdaExpr() {}

        virtual Value exec(Environment::SharedPtr env);

    private:
        ParamList           params_;
        CodeNode::SharedPtr body_;
    };

    // -------------------------------------------------------------
    class LambdaApp : public CodeNode {
    public:
        LambdaApp(CodeNode::SharedPtr closure, SharedPtrList args);
        virtual ~LambdaApp() {}

        virtual Value exec(Environment::SharedPtr env);

    private:
        CodeNode::SharedPtr closure_;
        SharedPtrList       argExprs_;

    protected:
        Value closureVar_;
    };

    // -------------------------------------------------------------
    class FunctionExpr : public LambdaExpr {
    public:
        FunctionExpr(const std::string &name, const ParamList &params, CodeNode::SharedPtr body);
        virtual ~FunctionExpr() {}

        virtual Value exec(Environment::SharedPtr env);

    private:
        std::string name_;
    };

    // -------------------------------------------------------------
    class FunctionApp : public LambdaApp {
    public:
        FunctionApp(const std::string &name, SharedPtrList args);
        virtual ~FunctionApp() {}

        virtual Value exec(Environment::SharedPtr env);

    private:
        std::string name_;
    };

    // -------------------------------------------------------------
    class Print : public CodeNode {
    public:
        Print(bool newline, CodeNode::SharedPtr expr);
        virtual ~Print() {}

        virtual Value exec(Environment::SharedPtr env);

    private:
        bool                newline_;
        CodeNode::SharedPtr expr_;
    };

    // -------------------------------------------------------------
    class Read : public CodeNode {
    public:
        Read();
        virtual ~Read() {};

        virtual Value exec(Environment::SharedPtr env);
    };

    // -------------------------------------------------------------
    class StructExpr : public CodeNode {
    public:
        StructExpr(const std::string &name, const Struct::MemberList &members);
        virtual ~StructExpr() {}

        virtual Value exec(Environment::SharedPtr env);

    private:
        Struct struct_;
    };

    // -------------------------------------------------------------
    class IsStructName : public CodeNode {
    public:
        IsStructName(CodeNode::SharedPtr expr, const std::string &name);
        virtual ~IsStructName() {}

        virtual Value exec(Environment::SharedPtr env);

    private:
        CodeNode::SharedPtr expr_;
        std::string         name_;
    };

    // -------------------------------------------------------------
    class MakeInstance : public CodeNode {
    public:
        MakeInstance(const std::string &name);
        virtual ~MakeInstance() {}

        virtual Value exec(Environment::SharedPtr env);

    private:
        std::string name_;
    };

    // -------------------------------------------------------------
    class IsInstanceOf : public CodeNode {
    public:
        IsInstanceOf(CodeNode::SharedPtr expr, const std::string &name);
        virtual ~IsInstanceOf() {}

        virtual Value exec(Environment::SharedPtr env);

    private:
        CodeNode::SharedPtr expr_;
        std::string         name_;
    };

    // -------------------------------------------------------------
    class GetMember : public CodeNode {
    public:
        GetMember(CodeNode::SharedPtr expr, const std::string &name);
        virtual ~GetMember() {}

        virtual Value exec(Environment::SharedPtr env);

    private:
        CodeNode::SharedPtr expr_;
        std::string         name_;
    };

    // -------------------------------------------------------------
    class SetMember : public CodeNode {
    public:
        SetMember(CodeNode::SharedPtr expr, const std::string &name, CodeNode::SharedPtr newValExpr);
        virtual ~SetMember() {}

        virtual Value exec(Environment::SharedPtr env);

    private:
        CodeNode::SharedPtr expr_;
        std::string         name_;
        CodeNode::SharedPtr newValExpr_;
    };

    // -------------------------------------------------------------
    class StringLen : public CodeNode {
    public:
        StringLen(CodeNode::SharedPtr expr);
        virtual ~StringLen() {}

        virtual Value exec(Environment::SharedPtr env);

    private:
        CodeNode::SharedPtr expr_;
    };

    // -------------------------------------------------------------
    class GetCharAt : public CodeNode {
    public:
        GetCharAt(CodeNode::SharedPtr str, CodeNode::SharedPtr pos);
        virtual ~GetCharAt() {}

        virtual Value exec(Environment::SharedPtr env);

    private:
        CodeNode::SharedPtr str_;
        CodeNode::SharedPtr pos_;
    };

    // -------------------------------------------------------------
    class SetCharAt : public CodeNode {
    public:
        SetCharAt(CodeNode::SharedPtr str, CodeNode::SharedPtr pos, CodeNode::SharedPtr val);
        virtual ~SetCharAt() {}

        virtual Value exec(Environment::SharedPtr env);

    private:
        CodeNode::SharedPtr str_;
        CodeNode::SharedPtr pos_;
        CodeNode::SharedPtr val_;
    };

    // -------------------------------------------------------------
    class StringCat : public CodeNode {
    public:
        StringCat(CodeNode::SharedPtr str, CodeNode::SharedPtr other);
        virtual ~StringCat() {}

        virtual Value exec(Environment::SharedPtr env);

    private:
        CodeNode::SharedPtr str_;
        CodeNode::SharedPtr other_;
    };

    // -------------------------------------------------------------
    class SubString : public CodeNode {
    public:
        SubString(CodeNode::SharedPtr str, CodeNode::SharedPtr pos);
        SubString(CodeNode::SharedPtr str, CodeNode::SharedPtr pos, CodeNode::SharedPtr len);
        virtual ~SubString() {}

        virtual Value exec(Environment::SharedPtr env);

    private:
        CodeNode::SharedPtr str_;
        CodeNode::SharedPtr pos_;
        CodeNode::SharedPtr len_;
    };

    // -------------------------------------------------------------
    class StringFind : public CodeNode {
    public:
        StringFind(CodeNode::SharedPtr str, CodeNode::SharedPtr chr);
        StringFind(CodeNode::SharedPtr str, CodeNode::SharedPtr chr, CodeNode::SharedPtr pos);
        virtual ~StringFind() {}

        virtual Value exec(Environment::SharedPtr env);

    private:
        CodeNode::SharedPtr str_;
        CodeNode::SharedPtr chr_;
        CodeNode::SharedPtr pos_;
    };

    // -------------------------------------------------------------
    class MakeArray : public CodeNode {
    public:
        MakeArray();
        MakeArray(CodeNode::SharedPtrList values);
        virtual ~MakeArray() {}

        virtual Value exec(Environment::SharedPtr env);

    private:
        CodeNode::SharedPtrList values_;
    };

    // -------------------------------------------------------------
    // Make array from size and value
    class MakeArraySV : public CodeNode {
    public:
        MakeArraySV(CodeNode::SharedPtr size);
        MakeArraySV(CodeNode::SharedPtr size, CodeNode::SharedPtr initValue);
        virtual ~MakeArraySV() {}

        virtual Value exec(Environment::SharedPtr env);

    private:
        CodeNode::SharedPtr size_;
        CodeNode::SharedPtr initValue_;
    };

    // -------------------------------------------------------------
    class ArrayLen : public CodeNode {
    public:
        ArrayLen(CodeNode::SharedPtr expr);
        virtual ~ArrayLen() {}

        virtual Value exec(Environment::SharedPtr env);

    private:
        CodeNode::SharedPtr expr_;
    };

    // -------------------------------------------------------------
    class ArrayGet : public CodeNode {
    public:
        ArrayGet(CodeNode::SharedPtr arr, CodeNode::SharedPtr pos);
        virtual ~ArrayGet() {}

        virtual Value exec(Environment::SharedPtr env);

    private:
        CodeNode::SharedPtr arr_;
        CodeNode::SharedPtr pos_;
    };

    // -------------------------------------------------------------
    class ArraySet : public CodeNode {
    public:
        ArraySet(CodeNode::SharedPtr arr, CodeNode::SharedPtr pos, CodeNode::SharedPtr val);
        virtual ~ArraySet() {}

        virtual Value exec(Environment::SharedPtr env);

    private:
        CodeNode::SharedPtr arr_;
        CodeNode::SharedPtr pos_;
        CodeNode::SharedPtr val_;
    };

    // -------------------------------------------------------------
    class ArrayAdd : public CodeNode {
    public:
        ArrayAdd(CodeNode::SharedPtr arr, CodeNode::SharedPtr val);
        virtual ~ArrayAdd() {}

        virtual Value exec(Environment::SharedPtr env);

    private:
        CodeNode::SharedPtr arr_;
        CodeNode::SharedPtr val_;
    };
}

#endif	// BYTE_CODE_H
