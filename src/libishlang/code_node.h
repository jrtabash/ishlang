#ifndef BYTE_CODE_H
#define BYTE_CODE_H

#include "environment.h"
#include "struct.h"
#include "value.h"

#include <functional>
#include <memory>
#include <optional>
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
        
    public:
        CodeNode() {}
        virtual ~CodeNode() {}
        
        Value eval(Environment::SharedPtr env) {
            if (!env.get()) { throw NullEnvironment(); }
            return this->exec(env);
        }

    protected:
        virtual Value exec(Environment::SharedPtr env) = 0;
    };
    
    // -------------------------------------------------------------
    class Literal : public CodeNode {
    public:
        Literal(const Value &value) : CodeNode(), value_(value) {}
        virtual ~Literal() {}

    protected:
        virtual Value exec(Environment::SharedPtr /*env*/) override { return value_; }

    private:
        Value value_;
    };

    // -------------------------------------------------------------
    class IsType : public CodeNode {
    public:
        IsType(CodeNode::SharedPtr expr, Value::Type type);
        virtual ~IsType() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) override;

    private:
        CodeNode::SharedPtr expr_;
        Value::Type         type_;
    };

    // -------------------------------------------------------------
    class TypeName : public CodeNode {
    public:
        TypeName(CodeNode::SharedPtr expr);
        virtual ~TypeName() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) override;

    private:
        CodeNode::SharedPtr expr_;
    };

    // -------------------------------------------------------------
    class AsType : public CodeNode {
    public:
        AsType(CodeNode::SharedPtr expr, Value::Type type);
        virtual ~AsType() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) override;

    private:
        CodeNode::SharedPtr expr_;
        Value::Type         type_;
    };

    // -------------------------------------------------------------
    class Variable : public CodeNode {
    public:
        Variable(const std::string &name) : CodeNode(), name_(name) {}
        virtual ~Variable() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) override { return env->get(name_); }
        
    private:
        std::string name_;
    };

    // -------------------------------------------------------------
    class Define : public CodeNode {
    public:
        Define(const std::string &name, CodeNode::SharedPtr code) : CodeNode(), name_(name), code_(code) {}
        virtual ~Define() {}
        
    protected:
        virtual Value exec(Environment::SharedPtr env) override { return env->def(name_, code_ ? code_->eval(env) : Value::Null); }

    private:
        std::string         name_;
        CodeNode::SharedPtr code_;
    };
    
    // -------------------------------------------------------------
    class Assign : public CodeNode {
    public:
        Assign(const std::string &name, CodeNode::SharedPtr code) : CodeNode(), name_(name), code_(code) {}
        virtual ~Assign() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) override { return env->set(name_, code_ ? code_->eval(env) : Value::Null); }
        
    private:
        std::string         name_;
        CodeNode::SharedPtr code_;
    };

    // -------------------------------------------------------------
    class Exists : public CodeNode {
    public:
        Exists(const std::string &name) : name_(name) {}
        virtual ~Exists() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) override { return Value(env->exists(name_)); }

    private:
        std::string name_;
    };

    // -------------------------------------------------------------
    class Clone : public CodeNode {
    public:
        Clone(CodeNode::SharedPtr code) : CodeNode(), code_(code) {}
        virtual ~Clone() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) override { return code_->eval(env).clone(); }

    private:
        CodeNode::SharedPtr code_;
    };

    // -------------------------------------------------------------
    class BinaryOp : public CodeNode {
    public:
        BinaryOp(CodeNode::SharedPtr lhs, CodeNode::SharedPtr rhs) : lhs_(lhs), rhs_(rhs) {}
        virtual ~BinaryOp() {}
        
    protected:
        virtual Value exec(Environment::SharedPtr env) override = 0;
        
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
        
    protected:
        virtual Value exec(Environment::SharedPtr env) override;
        
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
        
    protected:
        virtual Value exec(Environment::SharedPtr env) override;
        
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
        
    protected:
        virtual Value exec(Environment::SharedPtr env) override;
        
    private:
        Type type_;
    };

    // -------------------------------------------------------------
    class Not : public CodeNode {
    public:
        Not(CodeNode::SharedPtr operand);
        virtual ~Not() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) override;

    private:
        CodeNode::SharedPtr operand_;
    };

    // -------------------------------------------------------------
    class NegativeOf : public CodeNode {
    public:
        NegativeOf(CodeNode::SharedPtr operand);
        virtual ~NegativeOf() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) override;

    private:
        CodeNode::SharedPtr operand_;
    };

    // -------------------------------------------------------------
    class ProgN : public CodeNode {
    public:
        ProgN(CodeNode::SharedPtrList exprs);
        virtual ~ProgN() {}
        
    protected:
        virtual Value exec(Environment::SharedPtr env) override;
        
    private:
        CodeNode::SharedPtrList exprs_;
    };

    // -------------------------------------------------------------
    class Block : public ProgN {
    public:
        Block(CodeNode::SharedPtrList exprs);
        virtual ~Block() {}
        
    protected:
        virtual Value exec(Environment::SharedPtr env) override;
    };

    // -------------------------------------------------------------
    class If : public CodeNode {
    public:
        If(CodeNode::SharedPtr pred, CodeNode::SharedPtr tCode);
        If(CodeNode::SharedPtr pred, CodeNode::SharedPtr tCode, CodeNode::SharedPtr fCode);
        virtual ~If() {}
        
    protected:
        virtual Value exec(Environment::SharedPtr env) override;
        
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
        
    protected:
        virtual Value exec(Environment::SharedPtr env) override;
        
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
        
    protected:
        virtual Value exec(Environment::SharedPtr /*env*/) override { throw Except(); }
    };

    // -------------------------------------------------------------
    class Loop : public CodeNode {
    public:
        Loop(CodeNode::SharedPtr decl, CodeNode::SharedPtr cond, CodeNode::SharedPtr next, CodeNode::SharedPtr body);
        Loop(CodeNode::SharedPtr cond, CodeNode::SharedPtr body);
        virtual ~Loop() {}
        
    protected:
        virtual Value exec(Environment::SharedPtr env) override;
        
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

    protected:
        virtual Value exec(Environment::SharedPtr env) override;

    private:
        ParamList           params_;
        CodeNode::SharedPtr body_;
    };

    // -------------------------------------------------------------
    class LambdaApp : public CodeNode {
    public:
        LambdaApp(CodeNode::SharedPtr closure, SharedPtrList args);
        virtual ~LambdaApp() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) override;

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

    protected:
        virtual Value exec(Environment::SharedPtr env) override;

    private:
        std::string name_;
    };

    // -------------------------------------------------------------
    class FunctionApp : public LambdaApp {
    public:
        FunctionApp(const std::string &name, SharedPtrList args);
        virtual ~FunctionApp() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) override;

    private:
        std::string name_;
    };

    // -------------------------------------------------------------
    class Print : public CodeNode {
    public:
        Print(bool newline, CodeNode::SharedPtr expr);
        virtual ~Print() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) override;

    private:
        bool                newline_;
        CodeNode::SharedPtr expr_;
    };

    // -------------------------------------------------------------
    class Read : public CodeNode {
    public:
        Read();
        virtual ~Read() {};

    protected:
        virtual Value exec(Environment::SharedPtr env) override;
    };

    // -------------------------------------------------------------
    class StructExpr : public CodeNode {
    public:
        StructExpr(const std::string &name, const Struct::MemberList &members);
        virtual ~StructExpr() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) override;

    private:
        Struct struct_;
    };

    // -------------------------------------------------------------
    class IsStructName : public CodeNode {
    public:
        IsStructName(CodeNode::SharedPtr expr, const std::string &name);
        virtual ~IsStructName() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) override;

    private:
        CodeNode::SharedPtr expr_;
        std::string         name_;
    };

    // -------------------------------------------------------------
    class StructName : public CodeNode {
    public:
        StructName(CodeNode::SharedPtr expr);
        virtual ~StructName() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) override;

    private:
        CodeNode::SharedPtr expr_;
    };

    // -------------------------------------------------------------
    class MakeInstance : public CodeNode {
    public:
        MakeInstance(const std::string &name);
        virtual ~MakeInstance() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) override;

    private:
        std::string name_;
    };

    // -------------------------------------------------------------
    class IsInstanceOf : public CodeNode {
    public:
        IsInstanceOf(CodeNode::SharedPtr expr, const std::string &name);
        virtual ~IsInstanceOf() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) override;

    private:
        CodeNode::SharedPtr expr_;
        std::string         name_;
    };

    // -------------------------------------------------------------
    class GetMember : public CodeNode {
    public:
        GetMember(CodeNode::SharedPtr expr, const std::string &name);
        virtual ~GetMember() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) override;

    private:
        CodeNode::SharedPtr expr_;
        std::string         name_;
    };

    // -------------------------------------------------------------
    class SetMember : public CodeNode {
    public:
        SetMember(CodeNode::SharedPtr expr, const std::string &name, CodeNode::SharedPtr newValExpr);
        virtual ~SetMember() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) override;

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

    protected:
        virtual Value exec(Environment::SharedPtr env) override;

    private:
        CodeNode::SharedPtr expr_;
    };

    // -------------------------------------------------------------
    class StringGet : public CodeNode {
    public:
        StringGet(CodeNode::SharedPtr str, CodeNode::SharedPtr pos);
        virtual ~StringGet() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) override;

    private:
        CodeNode::SharedPtr str_;
        CodeNode::SharedPtr pos_;
    };

    // -------------------------------------------------------------
    class StringSet : public CodeNode {
    public:
        StringSet(CodeNode::SharedPtr str, CodeNode::SharedPtr pos, CodeNode::SharedPtr val);
        virtual ~StringSet() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) override;

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

    protected:
        virtual Value exec(Environment::SharedPtr env) override;

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

    protected:
        virtual Value exec(Environment::SharedPtr env) override;

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

    protected:
        virtual Value exec(Environment::SharedPtr env) override;

    private:
        CodeNode::SharedPtr str_;
        CodeNode::SharedPtr chr_;
        CodeNode::SharedPtr pos_;
    };

    // -------------------------------------------------------------
    class StringCount : public CodeNode {
    public:
        StringCount(CodeNode::SharedPtr str, CodeNode::SharedPtr chr);
        virtual ~StringCount() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) override;

    private:
        CodeNode::SharedPtr str_;
        CodeNode::SharedPtr chr_;
    };

    // -------------------------------------------------------------
    class MakeArray : public CodeNode {
    public:
        MakeArray();
        MakeArray(CodeNode::SharedPtrList values);
        virtual ~MakeArray() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) override;

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

    protected:
        virtual Value exec(Environment::SharedPtr env) override;

    private:
        CodeNode::SharedPtr size_;
        CodeNode::SharedPtr initValue_;
    };

    // -------------------------------------------------------------
    class ArrayLen : public CodeNode {
    public:
        ArrayLen(CodeNode::SharedPtr expr);
        virtual ~ArrayLen() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) override;

    private:
        CodeNode::SharedPtr expr_;
    };

    // -------------------------------------------------------------
    class ArrayGet : public CodeNode {
    public:
        ArrayGet(CodeNode::SharedPtr arr, CodeNode::SharedPtr pos);
        virtual ~ArrayGet() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) override;

    private:
        CodeNode::SharedPtr arr_;
        CodeNode::SharedPtr pos_;
    };

    // -------------------------------------------------------------
    class ArraySet : public CodeNode {
    public:
        ArraySet(CodeNode::SharedPtr arr, CodeNode::SharedPtr pos, CodeNode::SharedPtr val);
        virtual ~ArraySet() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) override;

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

    protected:
        virtual Value exec(Environment::SharedPtr env) override;

    private:
        CodeNode::SharedPtr arr_;
        CodeNode::SharedPtr val_;
    };

    // -------------------------------------------------------------
    class ArrayFind : public CodeNode {
    public:
        ArrayFind(CodeNode::SharedPtr arr, CodeNode::SharedPtr val);
        ArrayFind(CodeNode::SharedPtr str, CodeNode::SharedPtr val, CodeNode::SharedPtr pos);
        virtual ~ArrayFind() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) override;

    private:
        CodeNode::SharedPtr arr_;
        CodeNode::SharedPtr val_;
        CodeNode::SharedPtr pos_;
    };

    // -------------------------------------------------------------
    class ArrayCount : public CodeNode {
    public:
        ArrayCount(CodeNode::SharedPtr arr, CodeNode::SharedPtr val);
        virtual ~ArrayCount() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) override;

    private:
        CodeNode::SharedPtr arr_;
        CodeNode::SharedPtr val_;
    };

    // -------------------------------------------------------------
    class StrCharCheck : public CodeNode {
    public:
        using CheckFtn = std::function<bool (unsigned char)>;

        enum Type {
            Upper = 'u',
            Lower = 'l',
            Alpha = 'a',
            Numer = 'n',
            Alnum = 'm',
            Punct = 'p',
            Space = 's',
        };

    public:
        StrCharCheck(Type type, CodeNode::SharedPtr operand);
        virtual ~StrCharCheck() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) override;

    private:
        static CheckFtn typeToCheckFtn(Type type);

    private:
        Type type_;
        CodeNode::SharedPtr operand_;
        CheckFtn ftn_;
    };

    // -------------------------------------------------------------
    class StrCharTransform : public CodeNode {
    public:
        using TransformFtn = std::function<char (unsigned char)>;

        enum Type {
            ToUpper = 'u',
            ToLower = 'l',
        };

    public:
        StrCharTransform(Type type, CodeNode::SharedPtr operand);
        virtual ~StrCharTransform() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) override;

    private:
        static TransformFtn typeToTransformFtn(Type type);

    private:
        Type type_;
        CodeNode::SharedPtr operand_;
        TransformFtn ftn_;
    };

    // -------------------------------------------------------------
    class ImportModule : public CodeNode {
    public:
        ImportModule(const std::string &name, const std::string &asName = "");
        virtual ~ImportModule() {}

        virtual Value exec(Environment::SharedPtr env) override;

    private:
        std::string name_;
        std::string asName_;
    };

    // -------------------------------------------------------------
    class FromModuleImport : public CodeNode {
    public:
        FromModuleImport(const std::string &name, const std::string &varName, const std::string &asName = "");
        FromModuleImport(const std::string &name, const NameAndAsList &aliasList);
        virtual ~FromModuleImport() {}

        virtual Value exec(Environment::SharedPtr env) override;

    private:
        std::string name_;
        NameAndAsList aliasList_;
    };

}

#endif	// BYTE_CODE_H
