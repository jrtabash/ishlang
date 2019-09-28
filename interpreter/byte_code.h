#ifndef BYTE_CODE_H
#define	BYTE_CODE_H

#include "environment.h"
#include "value.h"
#include "struct.h"

#include <vector>
#include <memory>

namespace Int {

    // -------------------------------------------------------------
    class ByteCode {
    public:
        typedef std::shared_ptr<ByteCode>       SharedPtr;
        typedef std::vector<SharedPtr>          SharedPtrList;
        typedef std::pair<SharedPtr, SharedPtr> SharedPtrPair;
        typedef std::vector<SharedPtrPair>      SharedPtrPairs;
        typedef std::vector<std::string>        ParamList;
        
    public:
        ByteCode() {}
        virtual ~ByteCode() {}
        
        Value eval(Environment::SharedPtr env) {
            if (!env.get()) { throw NullEnvironment(); }
            return this->exec(env);
        }
        
        virtual Value exec(Environment::SharedPtr env) = 0;
    };
    
    // -------------------------------------------------------------
    class Literal : public ByteCode {
    public:
        Literal(const Value &value) : ByteCode(), value_(value) {}
        virtual ~Literal() {}

        virtual Value exec(Environment::SharedPtr env) { return value_; }

    private:
        Value value_;
    };

    // -------------------------------------------------------------
    class IsType : public ByteCode {
    public:
        IsType(ByteCode::SharedPtr expr, Value::Type type);
        virtual ~IsType() {}

        virtual Value exec(Environment::SharedPtr env);

    private:
        ByteCode::SharedPtr expr_;
        Value::Type         type_;
    };

    // -------------------------------------------------------------
    class Variable : public ByteCode {
    public:
        Variable(const std::string &name) : ByteCode(), name_(name) {}
        virtual ~Variable() {}

        virtual Value exec(Environment::SharedPtr env) { return env->get(name_); }
        
    private:
        std::string name_;
    };

    // -------------------------------------------------------------
    class Define : public ByteCode {
    public:
        Define(const std::string &name, ByteCode::SharedPtr code) : ByteCode(), name_(name), code_(code) {}
        virtual ~Define() {}
        
        virtual Value exec(Environment::SharedPtr env) { return env->def(name_, code_ ? code_->exec(env) : Value::Null); }

    private:
        std::string         name_;
        ByteCode::SharedPtr code_;
    };
    
    // -------------------------------------------------------------
    class Assign : public ByteCode {
    public:
        Assign(const std::string &name, ByteCode::SharedPtr code) : ByteCode(), name_(name), code_(code) {}
        virtual ~Assign() {}

        virtual Value exec(Environment::SharedPtr env) { return env->set(name_, code_ ? code_->exec(env) : Value::Null); }
        
    private:
        std::string         name_;
        ByteCode::SharedPtr code_;
    };

    // -------------------------------------------------------------
    class Exists : public ByteCode {
    public:
        Exists(const std::string &name) : name_(name) {}
        virtual ~Exists() {}

        virtual Value exec(Environment::SharedPtr env) { return Value(env->exists(name_)); }

    private:
        std::string name_;
    };

    // -------------------------------------------------------------
    class Clone : public ByteCode {
    public:
        Clone(ByteCode::SharedPtr code) : ByteCode(), code_(code) {}
        virtual ~Clone() {}

        virtual Value exec(Environment::SharedPtr env) { return code_->exec(env).clone(); }

    private:
        ByteCode::SharedPtr code_;
    };

    // -------------------------------------------------------------
    class BinaryOp : public ByteCode {
    public:
        BinaryOp(ByteCode::SharedPtr lhs, ByteCode::SharedPtr rhs) : lhs_(lhs), rhs_(rhs) {}
        virtual ~BinaryOp() {}
        
        virtual Value exec(Environment::SharedPtr env) = 0;
        
    protected:
        ByteCode::SharedPtr lhs_;
        ByteCode::SharedPtr rhs_;
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
        };
        
    public:
        ArithOp(Type type, ByteCode::SharedPtr lhs, ByteCode::SharedPtr rhs);
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
        CompOp(Type type, ByteCode::SharedPtr lhs, ByteCode::SharedPtr rhs);
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
        LogicOp(Type type, ByteCode::SharedPtr lhs, ByteCode::SharedPtr rhs);
        virtual ~LogicOp() {}
        
        virtual Value exec(Environment::SharedPtr env);
        
    private:
        Type type_;
    };
    
    // -------------------------------------------------------------
    class ProgN : public ByteCode {
    public:
        ProgN(ByteCode::SharedPtrList exprs);
        virtual ~ProgN() {}
        
        virtual Value exec(Environment::SharedPtr env);
        
    private:
        ByteCode::SharedPtrList exprs_;
    };

    // -------------------------------------------------------------
    class Block : public ProgN {
    public:
        Block(ByteCode::SharedPtrList exprs);
        virtual ~Block() {}
        
        virtual Value exec(Environment::SharedPtr env);
    };

    // -------------------------------------------------------------
    class If : public ByteCode {
    public:
        If(ByteCode::SharedPtr pred, ByteCode::SharedPtr tCode, ByteCode::SharedPtr fCode);
        virtual ~If() {}
        
        virtual Value exec(Environment::SharedPtr env);
        
    private:
        ByteCode::SharedPtr pred_;
        ByteCode::SharedPtr tCode_;
        ByteCode::SharedPtr fCode_;
    };
    
    // -------------------------------------------------------------
    class Cond : public ByteCode {
    public:
        Cond(ByteCode::SharedPtrPairs cases);
        virtual ~Cond() {}
        
        virtual Value exec(Environment::SharedPtr env);
        
    private:
        ByteCode::SharedPtrPairs cases_;
    };

    // -------------------------------------------------------------
    class Break : public ByteCode {
    public:
        struct Except {};
        
    public:
        Break() {}
        virtual ~Break() {}
        
        virtual Value exec(Environment::SharedPtr env) { throw Except(); }
    };

    // -------------------------------------------------------------
    class Loop : public ByteCode {
    public:
        Loop(ByteCode::SharedPtr decl, ByteCode::SharedPtr cond, ByteCode::SharedPtr next, ByteCode::SharedPtr body);
        Loop(ByteCode::SharedPtr cond, ByteCode::SharedPtr body);
        virtual ~Loop() {}
        
        virtual Value exec(Environment::SharedPtr env);
        
    private:
        ByteCode::SharedPtr decl_;
        ByteCode::SharedPtr cond_;
        ByteCode::SharedPtr next_;
        ByteCode::SharedPtr body_;
    };

    // -------------------------------------------------------------
    class LambdaExpr : public ByteCode {
    public:
        LambdaExpr(const ParamList &params, ByteCode::SharedPtr body);
        virtual ~LambdaExpr() {}

        virtual Value exec(Environment::SharedPtr env);

    private:
        ParamList           params_;
        ByteCode::SharedPtr body_;
    };

    // -------------------------------------------------------------
    class LambdaApp : public ByteCode {
    public:
        LambdaApp(ByteCode::SharedPtr closure, SharedPtrList args);
        virtual ~LambdaApp() {}

        virtual Value exec(Environment::SharedPtr env);

    private:
        ByteCode::SharedPtr closure_;
        SharedPtrList       argExprs_;

    protected:
        Value closureVar_;
    };

    // -------------------------------------------------------------
    class FunctionExpr : public LambdaExpr {
    public:
        FunctionExpr(const std::string &name, const ParamList &params, ByteCode::SharedPtr body);
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
    class Print : public ByteCode {
    public:
        Print(bool newline, ByteCode::SharedPtr expr);
        virtual ~Print() {}

        virtual Value exec(Environment::SharedPtr env);

    private:
        bool                newline_;
        ByteCode::SharedPtr expr_;
    };

    // -------------------------------------------------------------
    class Read : public ByteCode {
    public:
        Read();
        virtual ~Read() {};

        virtual Value exec(Environment::SharedPtr env);
    };

    // -------------------------------------------------------------
    class StructExpr : public ByteCode {
    public:
        StructExpr(const std::string &name, const Struct::MemberList &members);
        virtual ~StructExpr() {}

        virtual Value exec(Environment::SharedPtr env);

    private:
        Struct struct_;
    };

    // -------------------------------------------------------------
    class IsStructName : public ByteCode {
    public:
        IsStructName(ByteCode::SharedPtr expr, const std::string &name);
        virtual ~IsStructName() {}

        virtual Value exec(Environment::SharedPtr env);

    private:
        ByteCode::SharedPtr expr_;
        std::string         name_;
    };

    // -------------------------------------------------------------
    class MakeInstance : public ByteCode {
    public:
        MakeInstance(const std::string &name);
        virtual ~MakeInstance() {}

        virtual Value exec(Environment::SharedPtr env);

    private:
        std::string name_;
    };

    // -------------------------------------------------------------
    class IsInstanceOf : public ByteCode {
    public:
        IsInstanceOf(ByteCode::SharedPtr expr, const std::string &name);
        virtual ~IsInstanceOf() {}

        virtual Value exec(Environment::SharedPtr env);

    private:
        ByteCode::SharedPtr expr_;
        std::string         name_;
    };

    // -------------------------------------------------------------
    class GetMember : public ByteCode {
    public:
        GetMember(ByteCode::SharedPtr expr, const std::string &name);
        virtual ~GetMember() {}

        virtual Value exec(Environment::SharedPtr env);

    private:
        ByteCode::SharedPtr expr_;
        std::string         name_;
    };

    // -------------------------------------------------------------
    class SetMember : public ByteCode {
    public:
        SetMember(ByteCode::SharedPtr expr, const std::string &name, ByteCode::SharedPtr newValExpr);
        virtual ~SetMember() {}

        virtual Value exec(Environment::SharedPtr env);

    private:
        ByteCode::SharedPtr expr_;
        std::string         name_;
        ByteCode::SharedPtr newValExpr_;
    };

    // -------------------------------------------------------------
    class StringLen : public ByteCode {
    public:
        StringLen(ByteCode::SharedPtr expr);
        virtual ~StringLen() {}

        virtual Value exec(Environment::SharedPtr env);

    private:
        ByteCode::SharedPtr expr_;
    };

    // -------------------------------------------------------------
    class GetCharAt : public ByteCode {
    public:
        GetCharAt(ByteCode::SharedPtr str, ByteCode::SharedPtr pos);
        virtual ~GetCharAt() {}

        virtual Value exec(Environment::SharedPtr env);

    private:
        ByteCode::SharedPtr str_;
        ByteCode::SharedPtr pos_;
    };

    // -------------------------------------------------------------
    class SetCharAt : public ByteCode {
    public:
        SetCharAt(ByteCode::SharedPtr str, ByteCode::SharedPtr pos, ByteCode::SharedPtr val);
        virtual ~SetCharAt() {}

        virtual Value exec(Environment::SharedPtr env);

    private:
        ByteCode::SharedPtr str_;
        ByteCode::SharedPtr pos_;
        ByteCode::SharedPtr val_;
    };
}

#endif	// BYTE_CODE_H
