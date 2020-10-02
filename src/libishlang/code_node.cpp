#include "code_node.h"
#include "exception.h"
#include "instance.h"
#include "lambda.h"
#include "parser.h"
#include "sequence.h"
#include "util.h"

#include <cmath>
#include <algorithm>
#include <cctype>

using namespace Ishlang;

// -------------------------------------------------------------
IsType::IsType(CodeNode::SharedPtr expr, Value::Type type)
  : CodeNode()
  , expr_(expr)
  , type_(type)
{}

Value IsType::exec(Environment::SharedPtr env) {
    if (expr_.get()) {
        Value exprValue = expr_->eval(env);
        return exprValue.type() == type_ ? Value::True : Value::False;
    }
    return Value::False;
}

// -------------------------------------------------------------
TypeName::TypeName(CodeNode::SharedPtr expr)
  : CodeNode()
  , expr_(expr)
{}

Value TypeName::exec(Environment::SharedPtr env) {
    if (expr_.get()) {
        Value exprValue = expr_->eval(env);
        return Value(Value::typeToString(exprValue.type()));
    }
    return Value::Null;
}

// -------------------------------------------------------------
AsType::AsType(CodeNode::SharedPtr expr, Value::Type type)
  : CodeNode()
  , expr_(expr)
  , type_(type)
{}

Value AsType::exec(Environment::SharedPtr env) {
    if (expr_.get()) {
        Value exprValue = expr_->eval(env);
        return exprValue.asType(type_);
    }
    return Value::False;
}

// -------------------------------------------------------------
ArithOp::ArithOp(Type type, CodeNode::SharedPtr lhs, CodeNode::SharedPtr rhs)
    : BinaryOp(lhs, rhs)
    , type_(type)
{}

Value ArithOp::exec(Environment::SharedPtr env) {
    if (lhs_ && rhs_) {
        const Value lhsVal = lhs_->eval(env);
        const Value rhsVal = rhs_->eval(env);
        
        if (!lhsVal.isNumber()) { throw InvalidOperandType("Number", lhsVal.typeToString()); }
        if (!rhsVal.isNumber()) { throw InvalidOperandType("Number", rhsVal.typeToString()); }

        const bool real = lhsVal.isReal() || rhsVal.isReal();
        switch (type_) {
        case Add:
            if (real) { return Value(lhsVal.real() + rhsVal.real()); }
            else      { return Value(lhsVal.integer() + rhsVal.integer()); }
            break;
                
        case Sub:
            if (real) { return Value(lhsVal.real() - rhsVal.real()); }
            else      { return Value(lhsVal.integer() - rhsVal.integer()); }
            break;
                
        case Mul:
            if (real) { return Value(lhsVal.real() * rhsVal.real()); }
            else      { return Value(lhsVal.integer() * rhsVal.integer()); }
            break;
                
        case Div:
            if (real) {
                if (Util::isZero(rhsVal.real())) { throw DivByZero(); }
                return Value(lhsVal.real() / rhsVal.real());
            }
            else {
                if (rhsVal.integer() == 0) { throw DivByZero(); }
                return Value(lhsVal.integer() / rhsVal.integer());
            }
            break;

        case Mod:
            if (real) { throw InvalidOperandType("Real", "Integer"); }
            return Value(lhsVal.integer() % rhsVal.integer());
            break;

        case Pow:
            return std::pow(lhsVal.real(), rhsVal.isReal() ? rhsVal.real() : rhsVal.integer());
            break;
        }
    }
    return Value::Zero;
}

// -------------------------------------------------------------
CompOp::CompOp(Type type, CodeNode::SharedPtr lhs, CodeNode::SharedPtr rhs)
    : BinaryOp(lhs, rhs)
    , type_(type)
{}

Value CompOp::exec(Environment::SharedPtr env) {
    if (lhs_ && rhs_) {
        const Value lhsVal = lhs_->eval(env);
        const Value rhsVal = rhs_->eval(env);
        
        if (lhsVal.type() != rhsVal.type()) {
            if (!lhsVal.isNumber() || !rhsVal.isNumber()) {
                throw IncompatibleTypes(lhsVal.typeToString(), rhsVal.typeToString());
            }
        }

        switch (type_) {
            case EQ: return Value(lhsVal == rhsVal);
            case NE: return Value(lhsVal != rhsVal);
            case LT: return Value(lhsVal < rhsVal);
            case GT: return Value(lhsVal > rhsVal);
            case LE: return Value(lhsVal <= rhsVal);
            case GE: return Value(lhsVal >= rhsVal);
        }
    }
    return Value::False;
}

// -------------------------------------------------------------
LogicOp::LogicOp(Type type, CodeNode::SharedPtr lhs, CodeNode::SharedPtr rhs)
    : BinaryOp(lhs, rhs)
    , type_(type)
{}

Value LogicOp::exec(Environment::SharedPtr env) {
    if (lhs_ && rhs_) {
        const Value lhsVal = lhs_->eval(env);
        const Value rhsVal = rhs_->eval(env);
        
        if (!lhsVal.isBool()) { throw InvalidOperandType("Boolean", lhsVal.typeToString()); }
        if (!rhsVal.isBool()) { throw InvalidOperandType("Boolean", rhsVal.typeToString()); }

        switch (type_) {
            case Conjunction: return Value(lhsVal.boolean() && rhsVal.boolean());
            case Disjunction: return Value(lhsVal.boolean() || rhsVal.boolean());
        }
    }
    return Value::False;
}

// -------------------------------------------------------------
Not::Not(CodeNode::SharedPtr operand)
    : CodeNode()
    , operand_(operand)
{
}

Value Not::exec(Environment::SharedPtr env) {
    if (operand_.get()) {
        Value operand = operand_->eval(env);

        if (!operand.isBool()) { throw InvalidOperandType("Boolean", operand.typeToString()); }

        return !operand.boolean();
    }
    return Value::False;
}

// -------------------------------------------------------------
NegativeOf::NegativeOf(CodeNode::SharedPtr operand)
    : CodeNode()
    , operand_(operand)
{
}

Value NegativeOf::exec(Environment::SharedPtr env) {
    if (operand_.get()) {
        Value operand = operand_->eval(env);

        if (!operand.isNumber()) { throw InvalidOperandType("Number", operand.typeToString()); }

        return operand.isInt() ? Value(-operand.integer()) : Value(-operand.real());
    }
    return Value::Null;
}

// -------------------------------------------------------------
ProgN::ProgN(CodeNode::SharedPtrList exprs)
    : CodeNode()
    , exprs_(exprs)
{}

Value ProgN::exec(Environment::SharedPtr env) {
    if (!exprs_.empty()) {
        Value result;
        for (CodeNode::SharedPtrList::iterator iter = exprs_.begin(); iter != exprs_.end(); ++iter) {
            result = (*iter)->eval(env);
        }
        return result;
    }
    return Value::Null;
}

// -------------------------------------------------------------
Block::Block(CodeNode::SharedPtrList exprs)
    : ProgN(exprs)
{}

Value Block::exec(Environment::SharedPtr env) {
    Environment::SharedPtr blockEnv(std::make_shared<Environment>(env));
    return ProgN::exec(blockEnv);
}

// -------------------------------------------------------------
If::If(CodeNode::SharedPtr pred, CodeNode::SharedPtr tCode)
    : CodeNode()
    , pred_(pred)
    , tCode_(tCode)
    , fCode_()
{}

If::If(CodeNode::SharedPtr pred, CodeNode::SharedPtr tCode, CodeNode::SharedPtr fCode)
    : CodeNode()
    , pred_(pred)
    , tCode_(tCode)
    , fCode_(fCode)
{}

Value If::exec(Environment::SharedPtr env) {
    if (pred_) {
        Environment::SharedPtr ifEnv(std::make_shared<Environment>(env));

        const Value pVal = pred_->eval(ifEnv);
        if (!pVal.isBool()) { throw InvalidExpressionType("Boolean", pVal.typeToString()); }

        if (pVal.boolean()) {
            if (tCode_) { return tCode_->eval(ifEnv); }
        }
        else {
            if (fCode_) { return fCode_->eval(ifEnv); }
        }
    }
    return Value::Null;
}

// -------------------------------------------------------------
Cond::Cond(CodeNode::SharedPtrPairs cases)
    : CodeNode()
    , cases_(cases)
{}

Value Cond::exec(Environment::SharedPtr env) {
    const size_t numCases = cases_.size();
    if (numCases > 0) {
        for (CodeNode::SharedPtrPairs::iterator iter = cases_.begin(); iter != cases_.end(); ++iter) {
            if (!iter->first.get()) { throw InvalidExpressionType("Boolean", "Null"); }

            const Value pred = iter->first->eval(env);
            if (!pred.isBool()) { throw InvalidExpressionType("Boolean", pred.typeToString()); }
            if (pred.boolean()) {
                return iter->second ? iter->second->eval(env) : Value::Null;
            }
        }
    }
    return Value::Null;
}

// -------------------------------------------------------------
Loop::Loop(CodeNode::SharedPtr decl, CodeNode::SharedPtr cond, CodeNode::SharedPtr next, CodeNode::SharedPtr body)
    : CodeNode()
    , decl_(decl)
    , cond_(cond)
    , next_(next)
    , body_(body)
{}

Loop::Loop(CodeNode::SharedPtr cond, CodeNode::SharedPtr body)
    : CodeNode()
    , decl_()
    , cond_(cond)
    , next_()
    , body_(body)
{}

Value Loop::exec(Environment::SharedPtr env) {
    if (cond_ && body_) {
        Environment::SharedPtr loopEnv(std::make_shared<Environment>(env));
        
        Value result;
        Value condVal;

        try {
            if (decl_) { decl_->eval(loopEnv); }
            while (true) {
                condVal = cond_->eval(loopEnv);
                if (!condVal.isBool()) { throw InvalidExpressionType("Boolean", condVal.typeToString()); }
                if (!condVal.boolean()) { break; }

                result = body_->eval(loopEnv);
                if (next_) { next_->eval(loopEnv); }
            }
        }
        catch (const Break::Except &) { return Value::Null; }
        catch (...) { throw; }
        
        return result;
    }
    return Value::Null;
}

// -------------------------------------------------------------
LambdaExpr::LambdaExpr(const ParamList &params, CodeNode::SharedPtr body)
  : CodeNode()
  , params_(params)
  , body_(body)
{}

Value LambdaExpr::exec(Environment::SharedPtr env) {
    return Value(Lambda(params_, body_, env));
}

// -------------------------------------------------------------
LambdaApp::LambdaApp(CodeNode::SharedPtr closure, SharedPtrList args)
    : CodeNode()
    , closure_(closure)
    , argExprs_(args)
    , closureVar_(Value::Null)
{}

Value LambdaApp::exec(Environment::SharedPtr env) {
    if (!closureVar_.isClosure() && closure_.get()) {
        closureVar_ = closure_->eval(env);
    }

    if (!closureVar_.isClosure()) {
        throw InvalidExpressionType("Closure", closureVar_.typeToString());
    }

    Environment::SharedPtr appEnv(std::make_shared<Environment>(env));
    Lambda::ArgList args;
    for (SharedPtrList::iterator iter = argExprs_.begin(); iter != argExprs_.end(); ++iter) {
        args.push_back((*iter)->eval(appEnv));
    }
    
    return closureVar_.closure().exec(args);
}

// -------------------------------------------------------------
FunctionExpr::FunctionExpr(const std::string &name, const ParamList &params, CodeNode::SharedPtr body)
    : LambdaExpr(params, body)
    , name_(name)
{}

Value FunctionExpr::exec(Environment::SharedPtr env) {
    Value lambdaVal = LambdaExpr::exec(env);
    if (!lambdaVal.isClosure()) {
        throw InvalidExpressionType("Closure", lambdaVal.typeToString());
    }
    return env->def(name_, lambdaVal);
}

// -------------------------------------------------------------
FunctionApp::FunctionApp(const std::string &name, SharedPtrList args)
    : LambdaApp(CodeNode::SharedPtr(), args)
    , name_(name)
{}

Value FunctionApp::exec(Environment::SharedPtr env) {
    closureVar_ = env->get(name_);
    return LambdaApp::exec(env);
}

// -------------------------------------------------------------
Print::Print(bool newline, CodeNode::SharedPtr expr)
    : CodeNode()
    , newline_(newline)
    , expr_(expr)
{}

Value Print::exec(Environment::SharedPtr env) {
    Value::print(expr_->eval(env));
    if (newline_) { std::cout << std::endl; }
    return Value::Null;
}

// -------------------------------------------------------------
Read::Read()
    : CodeNode()
{
}

Value Read::exec(Environment::SharedPtr env)
{
    std::string input;
    std::getline(std::cin, input);
    return Parser().readLiteral(input)->eval(env);
}

// -------------------------------------------------------------
StructExpr::StructExpr(const std::string &name, const Struct::MemberList &members)
    : CodeNode()
    , struct_(name, members)
{}

Value StructExpr::exec(Environment::SharedPtr env) {
    Value structValue(struct_);
    return env->def(struct_.name(), structValue);
}

// -------------------------------------------------------------
IsStructName::IsStructName(CodeNode::SharedPtr expr, const std::string &name)
    : CodeNode()
    , expr_(expr)
    , name_(name)
{}

Value IsStructName::exec(Environment::SharedPtr env) {
    if (expr_.get()) {
        Value value = expr_->eval(env);
        if (value.isUserType() && value.userType().name() == name_) {
            return Value::True;
        }
    }
    return Value::False;
}

// -------------------------------------------------------------
StructName::StructName(CodeNode::SharedPtr expr)
    : CodeNode()
    , expr_(expr)
{
}

Value StructName::exec(Environment::SharedPtr env) {
    if (expr_.get()) {
        Value value = expr_->eval(env);
        if (value.isUserObject()) {
            return Value(value.userObject().type().name());
        }
        else if (value.isUserType()) {
            return Value(value.userType().name());
        }
        else {
            throw InvalidExpressionType("(UserObject or UserType)", value.typeToString());
        }
    }
    return Value::Null;
}

// -------------------------------------------------------------
MakeInstance::MakeInstance(const std::string &name)
    : CodeNode()
    , name_(name)
{}

Value MakeInstance::exec(Environment::SharedPtr env) {
    const Value structValue = env->get(name_);
    if (!structValue.isUserType()) {
        throw InvalidExpressionType("UserType", structValue.typeToString());
    }
    return Value(Instance(structValue.userType()));
}

// -------------------------------------------------------------
IsInstanceOf::IsInstanceOf(CodeNode::SharedPtr expr, const std::string &name)
    : CodeNode()
    , expr_(expr)
    , name_(name)
{}

Value IsInstanceOf::exec(Environment::SharedPtr env) {
    if (expr_.get()) {
        Value value = expr_->eval(env);
        if (value.isUserObject() && value.userObject().type().name() == name_) {
            return Value::True;
        }
    }
    return Value::False;
}

// -------------------------------------------------------------
GetMember::GetMember(CodeNode::SharedPtr expr, const std::string &name)
    : CodeNode()
    , expr_(expr)
    , name_(name)
{}

Value GetMember::exec(Environment::SharedPtr env) {
    if (expr_.get()) {
        Value value = expr_->eval(env);
        if (value.isUserObject()) {
            return value.userObject().get(name_);
        }
        else {
            throw InvalidExpressionType("UserObject", value.typeToString());
        }
    }
    return Value::Null;
}

// -------------------------------------------------------------
SetMember::SetMember(CodeNode::SharedPtr expr, const std::string &name, CodeNode::SharedPtr newValExpr)
    : CodeNode()
    , expr_(expr)
    , name_(name)
    , newValExpr_(newValExpr)
{}

Value SetMember::exec(Environment::SharedPtr env) {
    if (expr_.get()) {
        Value instanceValue = expr_->eval(env);
        if (instanceValue.isUserObject()) {
            if (newValExpr_.get()) {
                Value newValue = newValExpr_->eval(env);
                instanceValue.userObject().set(name_, newValue);
                return newValue;
            }
        }
        else {
            throw InvalidExpressionType("UserObject", instanceValue.typeToString());
        }
    }
    return Value::Null;
}

// -------------------------------------------------------------
StringLen::StringLen(CodeNode::SharedPtr expr)
    : CodeNode()
    , expr_(expr)
{}

Value StringLen::exec(Environment::SharedPtr env) {
    if (expr_.get()) {
        Value str = expr_->eval(env);

        if (!str.isString()) { throw InvalidOperandType("String", str.typeToString()); }

        return Value(Value::Long(str.text().size()));
    }
    return Value::Zero;
}

// -------------------------------------------------------------
StringGet::StringGet(CodeNode::SharedPtr str, CodeNode::SharedPtr pos)
    : CodeNode()
    , str_(str)
    , pos_(pos)
{}

Value StringGet::exec(Environment::SharedPtr env) {
    if (str_.get() && pos_.get()) {
        Value str = str_->eval(env);
        Value pos = pos_->eval(env);

        if (!str.isString()) { throw InvalidOperandType("String", str.typeToString()); }
        if (!pos.isInt()) { throw InvalidOperandType("Integer", pos.typeToString()); }

        const auto &rawStr = str.text();
        const auto rawPos = pos.integer();
        if (rawPos < 0 || static_cast<std::size_t>(rawPos) >= rawStr.size()) {
            throw OutOfRange("string strget access");
        }

        return Value(rawStr[rawPos]);
    }
    return Value::Null;
}

// -------------------------------------------------------------
StringSet::StringSet(CodeNode::SharedPtr str, CodeNode::SharedPtr pos, CodeNode::SharedPtr val)
    : CodeNode()
    , str_(str)
    , pos_(pos)
    , val_(val)
{}

Value StringSet::exec(Environment::SharedPtr env) {
    if (str_.get() && pos_.get() && val_.get()) {
        Value str = str_->eval(env);
        Value pos = pos_->eval(env);
        Value val = val_->eval(env);

        if (!str.isString()) { throw InvalidOperandType("String", str.typeToString()); }
        if (!pos.isInt()) { throw InvalidOperandType("Integer", pos.typeToString()); }
        if (!val.isChar()) { throw InvalidOperandType("Character", val.typeToString()); }

        auto &rawStr = str.text();
        const auto rawPos = pos.integer();
        if (rawPos < 0 || static_cast<std::size_t>(rawPos) >= rawStr.size()) {
            throw OutOfRange("string strset access");
        }

        rawStr[rawPos] = val.character();
        return val;
    }
    return Value::Null;
}

// -------------------------------------------------------------
StringCat::StringCat(CodeNode::SharedPtr str, CodeNode::SharedPtr other)
    : CodeNode()
    , str_(str)
    , other_(other)
{}

Value StringCat::exec(Environment::SharedPtr env) {
    if (str_.get() && other_.get()) {
        Value str = str_->eval(env);
        Value other = other_->eval(env);

        if (!str.isString()) { throw InvalidOperandType("String", str.typeToString()); }
        if (!other.isString() and !other.isChar()) { throw InvalidOperandType("(String or Character)", other.typeToString()); }

        auto &rawStr = str.text();

        if (other.isString()) {
            rawStr.append(other.text());
        }
        else {
            rawStr.append(1, other.character());
        }

        return str;
    }
    return Value::Null;
}

// -------------------------------------------------------------
SubString::SubString(CodeNode::SharedPtr str, CodeNode::SharedPtr pos)
    : CodeNode()
    , str_(str)
    , pos_(pos)
    , len_()
{
}

SubString::SubString(CodeNode::SharedPtr str, CodeNode::SharedPtr pos, CodeNode::SharedPtr len)
    : CodeNode()
    , str_(str)
    , pos_(pos)
    , len_(len)
{
}

Value SubString::exec(Environment::SharedPtr env) {
    if (str_.get() && pos_.get()) {
        Value str = str_->eval(env);
        Value pos = pos_->eval(env);
        Value len = len_ ? len_->eval(env) : Value::Zero;

        if (!str.isString()) { throw InvalidOperandType("String", str.typeToString()); }
        if (!pos.isInt()) { throw InvalidOperandType("Integer", pos.typeToString()); }
        if (!len.isInt()) { throw InvalidOperandType("Integer", len.typeToString()); }

        const auto &rawStr = str.text();

        const auto rawPos = pos.integer();
        if (rawPos < 0 || static_cast<std::size_t>(rawPos) >= rawStr.size()) {
            throw OutOfRange("substring position access");
        }

        const auto rawLen = (len_ ? len.integer() : Value::Long(rawStr.size()));
        if (rawLen < 0 || static_cast<std::size_t>(rawLen) > rawStr.size()) {
            throw OutOfRange("substring length access");
        }

        return Value(rawStr.substr(rawPos, rawLen));
    }
    return Value::Null;
}

// -------------------------------------------------------------
StringFind::StringFind(CodeNode::SharedPtr str, CodeNode::SharedPtr chr)
    : CodeNode()
    , str_(str)
    , chr_(chr)
    , pos_()
{
}

StringFind::StringFind(CodeNode::SharedPtr str, CodeNode::SharedPtr chr, CodeNode::SharedPtr pos)
    : CodeNode()
    , str_(str)
    , chr_(chr)
    , pos_(pos)
{
}

Value StringFind::exec(Environment::SharedPtr env) {
    if (str_.get() && chr_.get()) {
        Value str = str_->eval(env);
        Value chr = chr_->eval(env);
        Value pos = pos_ ? pos_->eval(env) : Value::Zero;

        if (!str.isString()) { throw InvalidOperandType("String", str.typeToString()); }
        if (!chr.isChar()) { throw InvalidOperandType("Character", chr.typeToString()); }
        if (!pos.isInt()) { throw InvalidOperandType("Integer", pos.typeToString()); }

        const auto &rawStr = str.text();

        const auto rawPos = pos.integer();
        if (rawPos < 0 || static_cast<std::size_t>(rawPos) >= rawStr.size()) {
            throw OutOfRange("strfind position access");
        }

        auto result = rawStr.find(chr.character(), rawPos);
        return result != std::string::npos ? Value(Value::Long(result)) : Value(-1ll);
    }
    return Value::Null;
}

// -------------------------------------------------------------
StringCount::StringCount(CodeNode::SharedPtr str, CodeNode::SharedPtr chr)
    : CodeNode()
    , str_(str)
    , chr_(chr)
{
}

Value StringCount::exec(Environment::SharedPtr env) {
    if (str_.get() && chr_.get()) {
        Value str = str_->eval(env);
        Value chr = chr_->eval(env);

        if (!str.isString()) { throw InvalidOperandType("String", str.typeToString()); }
        if (!chr.isChar()) { throw InvalidOperandType("Character", chr.typeToString()); }

        const auto &rawStr = str.text();

        return Value(Value::Long(std::count(rawStr.begin(), rawStr.end(), chr.character())));
    }
    return Value::Null;
}

// -------------------------------------------------------------
MakeArray::MakeArray()
    : CodeNode()
    , values_()
{}

MakeArray::MakeArray(CodeNode::SharedPtrList values)
    : CodeNode()
    , values_(values)
{}

Value MakeArray::exec(Environment::SharedPtr env) {
    if (values_.empty()) {
        return Value(Sequence());
    }
    else {
        std::vector<Value> values;
        for (auto & valueCode : values_) {
            values.push_back(valueCode->eval(env));
        }
        return Value(Sequence(std::move(values)));
    }
}

// -------------------------------------------------------------
MakeArraySV::MakeArraySV(CodeNode::SharedPtr size)
    : CodeNode()
    , size_(size)
    , initValue_()
{}

MakeArraySV::MakeArraySV(CodeNode::SharedPtr size, CodeNode::SharedPtr initValue)
    : CodeNode()
    , size_(size)
    , initValue_(initValue)
{}

Value MakeArraySV::exec(Environment::SharedPtr env) {
    if (size_.get()) {
        Value size = size_->eval(env);
        Value initValue = initValue_.get() ? initValue_->eval(env) : Value::Null;

        if (!size.isInt()) { throw InvalidOperandType("Integer", size.typeToString()); }

        return Value(Sequence(size.integer(), initValue));
    }
    return Value::Null;
}

// -------------------------------------------------------------
ArrayLen::ArrayLen(CodeNode::SharedPtr expr)
    : CodeNode()
    , expr_(expr)
{}

Value ArrayLen::exec(Environment::SharedPtr env) {
    if (expr_.get()) {
        Value arr = expr_->eval(env);

        if (!arr.isArray()) { throw InvalidOperandType("Array", arr.typeToString()); }

        return Value(Value::Long(arr.array().size()));
    }
    return Value::Zero;
}

// -------------------------------------------------------------
ArrayGet::ArrayGet(CodeNode::SharedPtr arr, CodeNode::SharedPtr pos)
    : CodeNode()
    , arr_(arr)
    , pos_(pos)
{}

Value ArrayGet::exec(Environment::SharedPtr env) {
    if (arr_.get() && pos_.get()) {
        Value arr = arr_->eval(env);
        Value pos = pos_->eval(env);

        if (!arr.isArray()) { throw InvalidOperandType("Array", arr.typeToString()); }
        if (!pos.isInt()) { throw InvalidOperandType("Integer", pos.typeToString()); }

        const auto &rawArr = arr.array();
        const auto rawPos = pos.integer();
        if (rawPos < 0 || static_cast<std::size_t>(rawPos) >= rawArr.size()) {
            throw OutOfRange("array get access");
        }

        return Value(rawArr.get(rawPos));
    }
    return Value::Null;
}

// -------------------------------------------------------------
ArraySet::ArraySet(CodeNode::SharedPtr arr, CodeNode::SharedPtr pos, CodeNode::SharedPtr val)
    : CodeNode()
    , arr_(arr)
    , pos_(pos)
    , val_(val)
{}

Value ArraySet::exec(Environment::SharedPtr env) {
    if (arr_.get() && pos_.get() && val_.get()) {
        Value arr = arr_->eval(env);
        Value pos = pos_->eval(env);
        Value val = val_->eval(env);

        if (!arr.isArray()) { throw InvalidOperandType("Array", arr.typeToString()); }
        if (!pos.isInt()) { throw InvalidOperandType("Integer", pos.typeToString()); }

        auto &rawArr = arr.array();
        const auto rawPos = pos.integer();
        if (rawPos < 0 || static_cast<std::size_t>(rawPos) >= rawArr.size()) {
            throw OutOfRange("array set access");
        }

        rawArr.set(rawPos, val);
        return val;
    }
    return Value::Null;
}

// -------------------------------------------------------------
ArrayAdd::ArrayAdd(CodeNode::SharedPtr arr, CodeNode::SharedPtr val)
    : CodeNode()
    , arr_(arr)
    , val_(val)
{}

Value ArrayAdd::exec(Environment::SharedPtr env) {
    if (arr_.get() && val_.get()) {
        Value arr = arr_->eval(env);
        Value val = val_->eval(env);

        if (!arr.isArray()) { throw InvalidOperandType("Array", arr.typeToString()); }

        arr.array().add(val);
        return arr;
    }
    return Value::Null;
}

// -------------------------------------------------------------
ArrayFind::ArrayFind(CodeNode::SharedPtr arr, CodeNode::SharedPtr chr)
    : CodeNode()
    , arr_(arr)
    , val_(chr)
    , pos_()
{
}

ArrayFind::ArrayFind(CodeNode::SharedPtr arr, CodeNode::SharedPtr chr, CodeNode::SharedPtr pos)
    : CodeNode()
    , arr_(arr)
    , val_(chr)
    , pos_(pos)
{
}

Value ArrayFind::exec(Environment::SharedPtr env) {
    if (arr_.get() && val_.get()) {
        Value arr = arr_->eval(env);
        Value val = val_->eval(env);
        Value pos = pos_ ? pos_->eval(env) : Value::Zero;

        if (!arr.isArray()) { throw InvalidOperandType("Array", arr.typeToString()); }
        if (!pos.isInt()) { throw InvalidOperandType("Integer", pos.typeToString()); }

        const auto &rawArray = arr.array();

        const auto rawPos = pos.integer();
        if (rawPos < 0 || static_cast<std::size_t>(rawPos) >= rawArray.size()) {
            throw OutOfRange("arrfind position access");
        }

        auto result = rawArray.find(val, rawPos);
        return result ? Value(Value::Long(*result)) : Value(-1ll);
    }
    return Value::Null;
}

// -------------------------------------------------------------
ArrayCount::ArrayCount(CodeNode::SharedPtr arr, CodeNode::SharedPtr val)
    : CodeNode()
    , arr_(arr)
    , val_(val)
{
}

Value ArrayCount::exec(Environment::SharedPtr env) {
    if (arr_.get() && val_.get()) {
        Value arr = arr_->eval(env);
        Value val = val_->eval(env);

        if (!arr.isArray()) { throw InvalidOperandType("Array", arr.typeToString()); }

        return Value(Value::Long(arr.array().count(val)));
    }
    return Value::Null;
}

// -------------------------------------------------------------
StrCharCheck::StrCharCheck(Type type, CodeNode::SharedPtr operand)
    : CodeNode()
    , type_(type)
    , operand_(operand)
    , ftn_(typeToCheckFtn(type))
{
}

Value StrCharCheck::exec(Environment::SharedPtr env) {
    if (operand_.get()) {
        const Value value = operand_->eval(env);

        if (!value.isChar() && !value.isString()) { throw InvalidOperandType("Character|String", value.typeToString()); }

        if (value.isString()) {
            auto const & str = value.text();
            return Value(std::all_of(str.begin(), str.end(), ftn_));
        }
        else {
            return Value(ftn_(value.character()));
        }
    }
    return Value::Null;
}

StrCharCheck::CheckFtn StrCharCheck::typeToCheckFtn(Type type) {
    switch (type) {
    case Upper: return [](unsigned char c) { return std::isupper(c); };
    case Lower: return [](unsigned char c) { return std::islower(c); };
    case Alpha: return [](unsigned char c) { return std::isalpha(c); };
    case Numer: return [](unsigned char c) { return std::isdigit(c); };
    case Alnum: return [](unsigned char c) { return std::isalnum(c); };
    case Punct: return [](unsigned char c) { return std::ispunct(c); };
    case Space: return [](unsigned char c) { return std::isspace(c); };
    }

    throw InvalidExpression("unknown string/character check type", std::string(1, char(type)));
}

// -------------------------------------------------------------
StrCharTransform::StrCharTransform(Type type, CodeNode::SharedPtr operand)
    : CodeNode()
    , type_(type)
    , operand_(operand)
    , ftn_(typeToTransformFtn(type))
{
}

Value StrCharTransform::exec(Environment::SharedPtr env) {
    if (operand_.get()) {
        const Value value = operand_->eval(env);

        if (!value.isChar() && !value.isString()) { throw InvalidOperandType("Character|String", value.typeToString()); }

        if (value.isString()) {
            Value retValue = value.clone();
            auto & str = retValue.text();
            std::transform(str.begin(), str.end(), str.begin(), ftn_);
            return retValue;
        }
        else {
            return Value(ftn_(value.character()));
        }
    }
    return Value::Null;
}

auto StrCharTransform::typeToTransformFtn(Type type) -> TransformFtn {
    switch (type) {
    case ToUpper: return [](unsigned char c) { return static_cast<char>(std::toupper(c)); };
    case ToLower: return [](unsigned char c) { return static_cast<char>(std::tolower(c)); };
    }

    throw InvalidExpression("unknown string/character translate type", std::string(1, char(type)));
}
