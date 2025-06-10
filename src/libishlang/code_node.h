#ifndef ISHLANG_CODE_NODE_H
#define ISHLANG_CODE_NODE_H

#include "code_node_bases.h"
#include "environment.h"
#include "hashtable.h"
#include "instance.h"
#include "integer_range.h"
#include "struct.h"
#include "value.h"
#include "value_pair.h"

#include <cassert>
#include <functional>
#include <numeric>

namespace Ishlang {

    // -------------------------------------------------------------
    class Literal : public CodeNode {
    public:
        Literal(const Value &value) : CodeNode(), value_(value) {}
        virtual ~Literal() {}

    protected:
        virtual Value exec(Environment::SharedPtr /*env*/) const override { return value_.clone(); }

    private:
        Value value_;
    };

    // -------------------------------------------------------------
    class IsType : public CodeNode {
    public:
        IsType(CodeNode::SharedPtr expr, Value::TypeList types);
        virtual ~IsType() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        CodeNode::SharedPtr expr_;
        Value::TypeList     types_;
    };

    // -------------------------------------------------------------
    class TypeName : public CodeNode {
    public:
        TypeName(CodeNode::SharedPtr expr);
        virtual ~TypeName() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        CodeNode::SharedPtr expr_;
    };

    // -------------------------------------------------------------
    class AsType : public CodeNode {
    public:
        AsType(CodeNode::SharedPtr expr, Value::Type type);
        virtual ~AsType() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        CodeNode::SharedPtr expr_;
        Value::Type         type_;
    };

    // -------------------------------------------------------------
    class Assert : public CodeNode {
    public:
        Assert(const std::string &tag, CodeNode::SharedPtr expr);
        virtual ~Assert() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        std::string tag_;
        CodeNode::SharedPtr expr_;
    };

    // -------------------------------------------------------------
    class Variable : public CodeNode {
    public:
        Variable(const std::string &name) : CodeNode(), name_(name) {}
        virtual ~Variable() {}

    public:
        virtual bool isIdentifier() const override {
            return true;
        }

        virtual const std::string &identifierName() const override {
            return name_;
        }

    protected:
        virtual Value exec(Environment::SharedPtr env) const override { return env->get(name_); }

    private:
        std::string name_;
    };

    // -------------------------------------------------------------
    class Define : public CodeNode {
    public:
        Define(const std::string &name, CodeNode::SharedPtr code) : CodeNode(), name_(name), code_(code) {}
        virtual ~Define() {}
        
    protected:
        virtual Value exec(Environment::SharedPtr env) const override { return env->def(name_, code_ ? code_->eval(env) : Value::Null); }

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
        virtual Value exec(Environment::SharedPtr env) const override { return env->set(name_, code_ ? code_->eval(env) : Value::Null); }
        
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
        virtual Value exec(Environment::SharedPtr env) const override { return Value(env->exists(name_)); }

    private:
        std::string name_;
    };

    // -------------------------------------------------------------
    class Clone : public CodeNode {
    public:
        Clone(CodeNode::SharedPtr code) : CodeNode(), code_(code) {}
        virtual ~Clone() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override { return code_->eval(env).clone(); }

    private:
        CodeNode::SharedPtr code_;
    };

    // -------------------------------------------------------------
    class ArithOp : public VariadicOp {
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
        ArithOp(Type type, CodeNode::SharedPtrList operands);
        virtual ~ArithOp() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        template <typename NumType, typename AccumOp>
        static inline Value accum(const std::vector<Value> &vals, AccumOp op) {
            if constexpr (std::is_same_v<NumType, Value::Double>) {
                return Value(std::accumulate(
                                 vals.begin() + 1,
                                 vals.end(),
                                 vals.front().real(),
                                 [op](NumType a, const Value &v) { return op(a, v.real()); }));
            }
            else {
                static_assert(std::is_same_v<NumType, Value::Long>);
                return Value(std::accumulate(
                                 vals.begin() + 1,
                                 vals.end(),
                                 vals.front().integer(),
                                 [op](NumType a, const Value &v) { return op(a, v.integer()); }));
            }
        }

        template <typename NumType>
        static inline bool isDivByZero(const std::vector<Value> &vals) {
            if constexpr (std::is_same_v<NumType, Value::Double>) {
                return std::any_of(vals.begin() + 1, vals.end(), [](const Value &v) { return Util::isZero(v.real()); });
            }
            else {
                static_assert(std::is_same_v<NumType, Value::Long>);
                return std::any_of(vals.begin() + 1, vals.end(), [](const Value &v) { return v.integer() == 0; });
            }
        }

        static inline Value::Double power(Value::Double a, Value::Double v) {
            return std::pow(a, v);
        }

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
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        static const char * op2str(Type type);
        
    private:
        Type type_;
    };
    
    // -------------------------------------------------------------
    class LogicOp : public VariadicOp {
    public:
        enum Type {
            Conjunction = '&',
            Disjunction = '|',
        };
        
    public:
        LogicOp(Type type, CodeNode::SharedPtrList operands);
        virtual ~LogicOp() {}
        
    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        Type type_;
    };

    // -------------------------------------------------------------
    class Not : public UnaryOp {
    public:
        Not(CodeNode::SharedPtr operand);
        virtual ~Not() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;
    };

    // -------------------------------------------------------------
    class NegativeOf : public UnaryOp {
    public:
        NegativeOf(CodeNode::SharedPtr operand);
        virtual ~NegativeOf() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;
    };

    // -------------------------------------------------------------
    class ProgN : public CodeNode {
    public:
        ProgN(CodeNode::SharedPtrList exprs);
        virtual ~ProgN() {}
        
    protected:
        virtual Value exec(Environment::SharedPtr env) const override;
        
    private:
        CodeNode::SharedPtrList exprs_;
    };

    // -------------------------------------------------------------
    class Block : public ProgN {
    public:
        Block(CodeNode::SharedPtrList exprs);
        virtual ~Block() {}
        
    protected:
        virtual Value exec(Environment::SharedPtr env) const override;
    };

    // -------------------------------------------------------------
    class If : public CodeNode {
    public:
        If(CodeNode::SharedPtr pred, CodeNode::SharedPtr tCode);
        If(CodeNode::SharedPtr pred, CodeNode::SharedPtr tCode, CodeNode::SharedPtr fCode);
        virtual ~If() {}
        
    protected:
        virtual Value exec(Environment::SharedPtr env) const override;
        
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
        virtual Value exec(Environment::SharedPtr env) const override;
        
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
        virtual Value exec(Environment::SharedPtr /*env*/) const override { throw Except(); }
    };

    // -------------------------------------------------------------
    class Loop : public CodeNode {
    public:
        Loop(CodeNode::SharedPtr decl, CodeNode::SharedPtr cond, CodeNode::SharedPtr next, CodeNode::SharedPtr body);
        Loop(CodeNode::SharedPtr cond, CodeNode::SharedPtr body);
        virtual ~Loop() {}
        
    protected:
        virtual Value exec(Environment::SharedPtr env) const override;
        
    private:
        CodeNode::SharedPtr decl_;
        CodeNode::SharedPtr cond_;
        CodeNode::SharedPtr next_;
        CodeNode::SharedPtr body_;
    };

    // -------------------------------------------------------------
    class Foreach : public CodeNode {
    public:
        Foreach(const std::string &name, CodeNode::SharedPtr container, CodeNode::SharedPtr body);
        virtual ~Foreach() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        template <typename Container>
        Value impl(Environment::SharedPtr loopEnv, const Container &container) const;

        Value implRange(Environment::SharedPtr loopEnv, const IntegerRange &range) const;
        Value implFile(Environment::SharedPtr loopEnv, FileStruct &file) const;

    private:
        std::string         name_;
        CodeNode::SharedPtr container_;
        CodeNode::SharedPtr body_;
    };

    // -------------------------------------------------------------
    class LambdaExpr : public CodeNode {
    public:
        LambdaExpr(const ParamList &params, CodeNode::SharedPtr body);
        virtual ~LambdaExpr() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

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
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        CodeNode::SharedPtr closure_;
        SharedPtrList       argExprs_;

    protected:
        mutable Value closureVar_;
    };

    // -------------------------------------------------------------
    class FunctionExpr : public LambdaExpr {
    public:
        FunctionExpr(const std::string &name, const ParamList &params, CodeNode::SharedPtr body);
        virtual ~FunctionExpr() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        std::string name_;
    };

    // -------------------------------------------------------------
    class FunctionApp : public LambdaApp {
    public:
        FunctionApp(const std::string &name, SharedPtrList args);
        virtual ~FunctionApp() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        std::string name_;
    };

    // -------------------------------------------------------------
    class Print : public CodeNode {
    public:
        Print(bool newline, CodeNode::SharedPtrList exprs);
        virtual ~Print() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        bool                    newline_;
        CodeNode::SharedPtrList exprs_;
    };

    // -------------------------------------------------------------
    class Read : public CodeNode {
    public:
        Read();
        virtual ~Read() {};

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;
    };

    // -------------------------------------------------------------
    class StructExpr : public CodeNode {
    public:
        StructExpr(const std::string &name, const Struct::MemberList &members);
        virtual ~StructExpr() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        Struct struct_;
    };

    // -------------------------------------------------------------
    class IsStructName : public CodeNode {
    public:
        IsStructName(CodeNode::SharedPtr expr, const std::string &name);
        virtual ~IsStructName() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

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
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        CodeNode::SharedPtr expr_;
    };

    // -------------------------------------------------------------
    class MakeInstance : public CodeNode {
    public:
        MakeInstance(const std::string &name, const NameSharedPtrs &initList = NameSharedPtrs());
        virtual ~MakeInstance() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        Instance::InitArgs makeInitArgs(Environment::SharedPtr &env) const;

    private:
        std::string name_;
        NameSharedPtrs initList_;
    };

    // -------------------------------------------------------------
    class IsInstanceOf : public CodeNode {
    public:
        IsInstanceOf(CodeNode::SharedPtr expr, const std::string &name);
        virtual ~IsInstanceOf() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

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
        virtual Value exec(Environment::SharedPtr env) const override;

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
        virtual Value exec(Environment::SharedPtr env) const override;

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
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        CodeNode::SharedPtr expr_;
    };

    // -------------------------------------------------------------
    class StringGet : public CodeNode {
    public:
        StringGet(CodeNode::SharedPtr str, CodeNode::SharedPtr pos);
        virtual ~StringGet() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

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
        virtual Value exec(Environment::SharedPtr env) const override;

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
        virtual Value exec(Environment::SharedPtr env) const override;

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
        virtual Value exec(Environment::SharedPtr env) const override;

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
        virtual Value exec(Environment::SharedPtr env) const override;

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
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        CodeNode::SharedPtr str_;
        CodeNode::SharedPtr chr_;
    };

    // -------------------------------------------------------------
    class StringCompare : public CodeNode {
    public:
        StringCompare(CodeNode::SharedPtr lhs, CodeNode::SharedPtr rhs);
        virtual ~StringCompare() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        CodeNode::SharedPtr lhs_;
        CodeNode::SharedPtr rhs_;
    };

    // -------------------------------------------------------------
    class StringSort : public CodeNode {
    public:
        StringSort(CodeNode::SharedPtr str, CodeNode::SharedPtr descending);
        virtual ~StringSort() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        CodeNode::SharedPtr str_;
        CodeNode::SharedPtr desc_;
    };

    // -------------------------------------------------------------
    class StringReverse : public CodeNode {
    public:
        StringReverse(CodeNode::SharedPtr str);
        virtual ~StringReverse() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        CodeNode::SharedPtr str_;
    };

    // -------------------------------------------------------------
    class StringSplit : public CodeNode {
    public:
        StringSplit(CodeNode::SharedPtr str, CodeNode::SharedPtr delim);
        virtual ~StringSplit() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        CodeNode::SharedPtr str_;
        CodeNode::SharedPtr delim_;
    };

    // -------------------------------------------------------------
    class MakeArray : public CodeNode {
    public:
        MakeArray();
        MakeArray(CodeNode::SharedPtrList values);
        virtual ~MakeArray() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

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
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        CodeNode::SharedPtr size_;
        CodeNode::SharedPtr initValue_;
    };

    // -------------------------------------------------------------
    // Make array from size and generator function
    class MakeArraySG : public CodeNode {
    public:
        MakeArraySG(CodeNode::SharedPtr size, CodeNode::SharedPtr genFtn);
        virtual ~MakeArraySG() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        CodeNode::SharedPtr size_;
        CodeNode::SharedPtr genFtn_;
    };

    // -------------------------------------------------------------
    class ArrayLen : public CodeNode {
    public:
        ArrayLen(CodeNode::SharedPtr expr);
        virtual ~ArrayLen() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        CodeNode::SharedPtr expr_;
    };

    // -------------------------------------------------------------
    class ArrayGet : public CodeNode {
    public:
        ArrayGet(CodeNode::SharedPtr arr, CodeNode::SharedPtr pos);
        virtual ~ArrayGet() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

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
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        CodeNode::SharedPtr arr_;
        CodeNode::SharedPtr pos_;
        CodeNode::SharedPtr val_;
    };

    // -------------------------------------------------------------
    class ArrayPush : public CodeNode {
    public:
        ArrayPush(CodeNode::SharedPtr arr, CodeNode::SharedPtr val);
        virtual ~ArrayPush() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        CodeNode::SharedPtr arr_;
        CodeNode::SharedPtr val_;
    };

    // -------------------------------------------------------------
    class ArrayPop : public CodeNode {
    public:
        ArrayPop(CodeNode::SharedPtr arr);
        virtual ~ArrayPop() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        CodeNode::SharedPtr arr_;
    };

    // -------------------------------------------------------------
    class ArrayFind : public CodeNode {
    public:
        ArrayFind(CodeNode::SharedPtr arr, CodeNode::SharedPtr val);
        ArrayFind(CodeNode::SharedPtr str, CodeNode::SharedPtr val, CodeNode::SharedPtr pos);
        virtual ~ArrayFind() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

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
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        CodeNode::SharedPtr arr_;
        CodeNode::SharedPtr val_;
    };

    // -------------------------------------------------------------
    class ArraySort : public CodeNode {
    public:
        ArraySort(CodeNode::SharedPtr arr, CodeNode::SharedPtr descending);
        virtual ~ArraySort() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        CodeNode::SharedPtr arr_;
        CodeNode::SharedPtr desc_;
    };

    // -------------------------------------------------------------
    class ArrayReverse : public CodeNode {
    public:
        ArrayReverse(CodeNode::SharedPtr arr);
        virtual ~ArrayReverse() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        CodeNode::SharedPtr arr_;
    };

    // -------------------------------------------------------------
    class ArrayInsert : public CodeNode {
    public:
        ArrayInsert(CodeNode::SharedPtr arr, CodeNode::SharedPtr pos, CodeNode::SharedPtr item);
        virtual ~ArrayInsert() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        CodeNode::SharedPtr arr_;
        CodeNode::SharedPtr pos_;
        CodeNode::SharedPtr item_;
    };

    // -------------------------------------------------------------
    class ArrayRemove : public CodeNode {
    public:
        ArrayRemove(CodeNode::SharedPtr arr, CodeNode::SharedPtr pos);
        virtual ~ArrayRemove() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        CodeNode::SharedPtr arr_;
        CodeNode::SharedPtr pos_;
    };

    // -------------------------------------------------------------
    class ArrayClear : public CodeNode {
    public:
        ArrayClear(CodeNode::SharedPtr arr);
        virtual ~ArrayClear() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        CodeNode::SharedPtr arr_;
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
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        static CheckFtn typeToCheckFtn(Type type);

    private:
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
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        static TransformFtn typeToTransformFtn(Type type);

    private:
        CodeNode::SharedPtr operand_;
        TransformFtn ftn_;
    };

    // -------------------------------------------------------------
    class ImportModule : public CodeNode {
    public:
        ImportModule(const std::string &name, const std::string &asName = "");
        virtual ~ImportModule() {}

        virtual Value exec(Environment::SharedPtr env) const override;

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

        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        std::string name_;
        NameAndAsList aliasList_;
    };

    // -------------------------------------------------------------
    class Random : public CodeNode {
    public:
        Random(CodeNode::SharedPtr max = CodeNode::SharedPtr());
        virtual ~Random() {}

        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        CodeNode::SharedPtr max_;
    };

    // -------------------------------------------------------------
    class Hash : public UnaryOp {
    public:
        Hash(CodeNode::SharedPtr operand);
        virtual ~Hash() {}

        virtual Value exec(Environment::SharedPtr env) const override;
    };

    // -------------------------------------------------------------
    class MakeHashMap : public CodeNode {
    public:
        MakeHashMap(CodeNode::SharedPtrList pairs = CodeNode::SharedPtrList());
        virtual ~MakeHashMap() {}

        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        static void append(Hashtable::Table &table, const Sequence &arr);
        static void append(Hashtable::Table &table, const ValuePair &pair);

    private:
        CodeNode::SharedPtrList pairs_;
    };

    // -------------------------------------------------------------
    class HashMapLen : public CodeNode {
    public:
        HashMapLen(CodeNode::SharedPtr htExpr);
        virtual ~HashMapLen() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        CodeNode::SharedPtr htExpr_;
    };

    // -------------------------------------------------------------
    class HashMapContains : public CodeNode {
    public:
        HashMapContains(CodeNode::SharedPtr htExpr, CodeNode::SharedPtr keyExpr);
        virtual ~HashMapContains() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        CodeNode::SharedPtr htExpr_;
        CodeNode::SharedPtr keyExpr_;
    };

    // -------------------------------------------------------------
    class HashMapGet : public CodeNode {
    public:
        HashMapGet(CodeNode::SharedPtr htExpr, CodeNode::SharedPtr keyExpr, CodeNode::SharedPtr defaultExpr = CodeNode::SharedPtr());
        virtual ~HashMapGet() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        CodeNode::SharedPtr htExpr_;
        CodeNode::SharedPtr keyExpr_;
        CodeNode::SharedPtr defaultExpr_;
    };

    // -------------------------------------------------------------
    class HashMapSet : public CodeNode {
    public:
        HashMapSet(CodeNode::SharedPtr htExpr, CodeNode::SharedPtr keyExpr, CodeNode::SharedPtr valueExpr);
        virtual ~HashMapSet() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        CodeNode::SharedPtr htExpr_;
        CodeNode::SharedPtr keyExpr_;
        CodeNode::SharedPtr valueExpr_;
    };

    // -------------------------------------------------------------
    class HashMapRemove : public CodeNode {
    public:
        HashMapRemove(CodeNode::SharedPtr htExpr, CodeNode::SharedPtr keyExpr);
        virtual ~HashMapRemove() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        CodeNode::SharedPtr htExpr_;
        CodeNode::SharedPtr keyExpr_;
    };

    // -------------------------------------------------------------
    class HashMapClear : public CodeNode {
    public:
        HashMapClear(CodeNode::SharedPtr htExpr);
        virtual ~HashMapClear() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        CodeNode::SharedPtr htExpr_;
    };

    // -------------------------------------------------------------
    class HashMapFind : public CodeNode {
    public:
        HashMapFind(CodeNode::SharedPtr htExpr, CodeNode::SharedPtr valueExpr);
        virtual ~HashMapFind() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        CodeNode::SharedPtr htExpr_;
        CodeNode::SharedPtr valueExpr_;
    };

    // -------------------------------------------------------------
    class HashMapCount : public CodeNode {
    public:
        HashMapCount(CodeNode::SharedPtr htExpr, CodeNode::SharedPtr valueExpr);
        virtual ~HashMapCount() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        CodeNode::SharedPtr htExpr_;
        CodeNode::SharedPtr valueExpr_;
    };

    // -------------------------------------------------------------
    class HashMapKeys : public CodeNode {
    public:
        HashMapKeys(CodeNode::SharedPtr htExpr);
        virtual ~HashMapKeys() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        CodeNode::SharedPtr htExpr_;
    };

    // -------------------------------------------------------------
    class HashMapValues : public CodeNode {
    public:
        HashMapValues(CodeNode::SharedPtr htExpr);
        virtual ~HashMapValues() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        CodeNode::SharedPtr htExpr_;
    };

    // -------------------------------------------------------------
    class HashMapItems : public CodeNode {
    public:
        HashMapItems(CodeNode::SharedPtr htExpr);
        virtual ~HashMapItems() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        CodeNode::SharedPtr htExpr_;
    };

    // -------------------------------------------------------------
    class MakePair : public CodeNode {
    public:
        MakePair(CodeNode::SharedPtr firstExpr, CodeNode::SharedPtr secondExpr);
        virtual ~MakePair() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        CodeNode::SharedPtr firstExpr_;
        CodeNode::SharedPtr secondExpr_;
    };

    // -------------------------------------------------------------
    class PairFirst : public CodeNode {
    public:
        PairFirst(CodeNode::SharedPtr pairExpr);
        virtual ~PairFirst() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        CodeNode::SharedPtr pairExpr_;
    };

    // -------------------------------------------------------------
    class PairSecond : public CodeNode {
    public:
        PairSecond(CodeNode::SharedPtr pairExpr);
        virtual ~PairSecond() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        CodeNode::SharedPtr pairExpr_;
    };

    // -------------------------------------------------------------
    class MakeRange : public CodeNode {
    public:
        MakeRange(CodeNode::SharedPtr end);
        MakeRange(CodeNode::SharedPtr begin, CodeNode::SharedPtr end, CodeNode::SharedPtr step);
        virtual ~MakeRange() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        CodeNode::SharedPtr begin_;
        CodeNode::SharedPtr end_;
        CodeNode::SharedPtr step_;
    };

    // -------------------------------------------------------------
    template <typename R>
    class RangeGetter : public CodeNode {
    public:
        using Getter = std::function<R (const IntegerRange &)>;

    public:
        RangeGetter(CodeNode::SharedPtr rng, Getter && getter);
        virtual ~RangeGetter() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        CodeNode::SharedPtr rng_;
        Getter getter_;
    };

    // -------------------------------------------------------------
    class RangeBegin : public RangeGetter<Value::Long> {
    public:
        RangeBegin(CodeNode::SharedPtr rng);
        virtual ~RangeBegin() {}
    };

    // -------------------------------------------------------------
    class RangeEnd : public RangeGetter<Value::Long> {
    public:
        RangeEnd(CodeNode::SharedPtr rng);
        virtual ~RangeEnd() {}
    };

    // -------------------------------------------------------------
    class RangeStep : public RangeGetter<Value::Long> {
    public:
        RangeStep(CodeNode::SharedPtr rng);
        virtual ~RangeStep() {}
    };

    // -------------------------------------------------------------
    class RangeLen : public RangeGetter<Value> {
    public:
        RangeLen(CodeNode::SharedPtr rng);
        virtual ~RangeLen() {}
    };

    // -------------------------------------------------------------
    // Expand
    class Expand : public CodeNode {
    public:
        Expand(CodeNode::SharedPtrList exprs);
        virtual ~Expand() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        CodeNode::SharedPtrList exprs_;
    };

    // -------------------------------------------------------------
    class GenericLen : public CodeNode {
    public:
        GenericLen(CodeNode::SharedPtr object);
        virtual ~GenericLen() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        CodeNode::SharedPtr object_;
    };

    // -------------------------------------------------------------
    class GenericEmpty : public CodeNode {
    public:
        GenericEmpty(CodeNode::SharedPtr object);
        virtual ~GenericEmpty() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        CodeNode::SharedPtr object_;
    };

    // -------------------------------------------------------------
    class GenericGet : public CodeNode {
    public:
        GenericGet(CodeNode::SharedPtr object, CodeNode::SharedPtr key, CodeNode::SharedPtr defaultRet = CodeNode::SharedPtr());
        virtual ~GenericGet() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        CodeNode::SharedPtr object_;
        CodeNode::SharedPtr key_;
        CodeNode::SharedPtr defaultRet_;
    };

    // -------------------------------------------------------------
    class GenericSet : public CodeNode {
    public:
        GenericSet(CodeNode::SharedPtr object, CodeNode::SharedPtr key, CodeNode::SharedPtr value);
        virtual ~GenericSet() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        CodeNode::SharedPtr object_;
        CodeNode::SharedPtr key_;
        CodeNode::SharedPtr value_;
    };

    // -------------------------------------------------------------
    class GenericClear : public CodeNode {
    public:
        GenericClear(CodeNode::SharedPtr object);
        virtual ~GenericClear() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        CodeNode::SharedPtr object_;
    };

    // -------------------------------------------------------------
    class GenericFind : public CodeNode {
    public:
        GenericFind(CodeNode::SharedPtr object, CodeNode::SharedPtr item, CodeNode::SharedPtr pos = CodeNode::SharedPtr());
        virtual ~GenericFind() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        CodeNode::SharedPtr object_;
        CodeNode::SharedPtr item_;
        CodeNode::SharedPtr pos_;
    };

    // -------------------------------------------------------------
    class GenericCount : public CodeNode {
    public:
        GenericCount(CodeNode::SharedPtr object, CodeNode::SharedPtr item);
        virtual ~GenericCount() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        CodeNode::SharedPtr object_;
        CodeNode::SharedPtr item_;
    };

    // -------------------------------------------------------------
    class GenericSort : public CodeNode {
    public:
        GenericSort(CodeNode::SharedPtr obj, CodeNode::SharedPtr descending);
        virtual ~GenericSort() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        CodeNode::SharedPtr obj_;
        CodeNode::SharedPtr desc_;
    };

    // -------------------------------------------------------------
    class GenericReverse : public CodeNode {
    public:
        GenericReverse(CodeNode::SharedPtr obj);
        virtual ~GenericReverse() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        CodeNode::SharedPtr obj_;
    };

    // -------------------------------------------------------------
    class GenericSum : public CodeNode {
    public:
        GenericSum(CodeNode::SharedPtr obj);
        virtual ~GenericSum() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        CodeNode::SharedPtr obj_;
    };

    // -------------------------------------------------------------
    class TimeIt : public CodeNode {
    public:
        TimeIt(CodeNode::SharedPtr expr, CodeNode::SharedPtr ntimes = CodeNode::SharedPtr(), CodeNode::SharedPtr showSummary = CodeNode::SharedPtr());
        virtual ~TimeIt() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        CodeNode::SharedPtr expr_;
        CodeNode::SharedPtr count_;
        CodeNode::SharedPtr summary_;
    };

    // -------------------------------------------------------------
    class FileOpen : public FileOp {
    public:
        FileOpen(CodeNode::SharedPtr filename, CodeNode::SharedPtr mode);
        virtual ~FileOpen() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        CodeNode::SharedPtr mode_;
    };

    // -------------------------------------------------------------
    class FileClose : public FileOp {
    public:
        FileClose(CodeNode::SharedPtr file);
        virtual ~FileClose() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;
    };

    // -------------------------------------------------------------
    class FileFlush : public FileOp {
    public:
        FileFlush(CodeNode::SharedPtr file);
        virtual ~FileFlush() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;
    };

    // -------------------------------------------------------------
    class FileIsOpen : public FileOp {
    public:
        FileIsOpen(CodeNode::SharedPtr file);
        virtual ~FileIsOpen() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;
    };

    // -------------------------------------------------------------
    class FileFName : public FileOp {
    public:
        FileFName(CodeNode::SharedPtr file);
        virtual ~FileFName() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;
    };

    // -------------------------------------------------------------
    class FileFMode : public FileOp {
    public:
        FileFMode(CodeNode::SharedPtr file);
        virtual ~FileFMode() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;
    };

    // -------------------------------------------------------------
    class FileRead : public FileOp {
    public:
        FileRead(CodeNode::SharedPtr file);
        virtual ~FileRead() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;
    };

    // -------------------------------------------------------------
    class FileReadLn : public FileOp {
    public:
        FileReadLn(CodeNode::SharedPtr file);
        virtual ~FileReadLn() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;
    };

    // -------------------------------------------------------------
    class FileWrite : public FileOp {
    public:
        FileWrite(CodeNode::SharedPtr file, CodeNode::SharedPtr charOrStr);
        virtual ~FileWrite() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        CodeNode::SharedPtr charOrStr_;
    };

    // -------------------------------------------------------------
    class FileWriteLn : public FileOp {
    public:
        FileWriteLn(CodeNode::SharedPtr file, CodeNode::SharedPtr str);
        virtual ~FileWriteLn() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;

    private:
        CodeNode::SharedPtr str_;
    };

    // -------------------------------------------------------------
    class FileExists : public FileOp {
    public:
        FileExists(CodeNode::SharedPtr file);
        virtual ~FileExists() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;
    };

    // -------------------------------------------------------------
    class FileRemove : public FileOp {
    public:
        FileRemove(CodeNode::SharedPtr file);
        virtual ~FileRemove() {}

    protected:
        virtual Value exec(Environment::SharedPtr env) const override;
    };

}

#endif	// ISHLANG_CODE_NODE_H
