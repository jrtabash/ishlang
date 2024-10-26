#include "code_node.h"
#include "code_node_util.h"
#include "exception.h"
#include "generic_functions.h"
#include "lambda.h"
#include "module.h"
#include "parser.h"
#include "sequence.h"
#include "util.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstring>
#include <functional>
#include <limits>
#include <random>

using namespace Ishlang;

// -------------------------------------------------------------
IsType::IsType(CodeNode::SharedPtr expr, Value::Type type)
  : CodeNode()
  , expr_(expr)
  , type_(type)
{}

Value IsType::exec(Environment::SharedPtr env) const {
    if (expr_) {
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

Value TypeName::exec(Environment::SharedPtr env) const {
    if (expr_) {
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

Value AsType::exec(Environment::SharedPtr env) const {
    if (expr_) {
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

Value ArithOp::exec(Environment::SharedPtr env) const {
    if (lhs_ && rhs_) {
        const Value lhsVal = evalOperand(env, lhs_, Value::eInteger, Value::eReal);
        const Value rhsVal = evalOperand(env, rhs_, Value::eInteger, Value::eReal);
        
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
            if (real) {
                throw InvalidOperandType(
                    Value::typeToString(Value::eReal),
                    Value::typeToString(Value::eInteger));
            }
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

Value CompOp::exec(Environment::SharedPtr env) const {
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
LogicOp::LogicOp(Type type, CodeNode::SharedPtrList operands)
    : VariadicOp(operands)
    , type_(type)
{}

Value LogicOp::exec(Environment::SharedPtr env) const {
    if (!operands_.empty()) {
        switch (type_) {
        case Conjunction:
            for (const auto &operand : operands_) {
                if (!evalOperand(env, operand, Value::eBoolean).boolean()) {
                    return Value::False;
                }
            }
            return Value::True;

        case Disjunction:
            for (const auto &operand : operands_) {
                if (evalOperand(env, operand, Value::eBoolean).boolean()) {
                    return Value::True;
                }
            }
            return Value::False;
        }
    }
    return Value::False;
}

// -------------------------------------------------------------
Not::Not(CodeNode::SharedPtr operand)
    : CodeNode()
    , operand_(operand)
{}

Value Not::exec(Environment::SharedPtr env) const {
    if (operand_) {
        const Value operand = evalOperand(env, operand_, Value::eBoolean);
        return Value(!operand.boolean());
    }
    return Value::False;
}

// -------------------------------------------------------------
NegativeOf::NegativeOf(CodeNode::SharedPtr operand)
    : CodeNode()
    , operand_(operand)
{}

Value NegativeOf::exec(Environment::SharedPtr env) const {
    if (operand_) {
        const Value operand = evalOperand(env, operand_, Value::eInteger, Value::eReal);
        return operand.isInt() ? Value(-operand.integer()) : Value(-operand.real());
    }
    return Value::Null;
}

// -------------------------------------------------------------
ProgN::ProgN(CodeNode::SharedPtrList exprs)
    : CodeNode()
    , exprs_(exprs)
{}

Value ProgN::exec(Environment::SharedPtr env) const {
    if (!exprs_.empty()) {
        Value result;
        for (CodeNode::SharedPtrList::const_iterator iter = exprs_.begin(); iter != exprs_.end(); ++iter) {
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

Value Block::exec(Environment::SharedPtr env) const {
    auto blockEnv = Environment::make(env);
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

Value If::exec(Environment::SharedPtr env) const {
    if (pred_) {
        auto ifEnv = Environment::make(env);

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

Value Cond::exec(Environment::SharedPtr env) const {
    const size_t numCases = cases_.size();
    if (numCases > 0) {
        for (CodeNode::SharedPtrPairs::const_iterator iter = cases_.begin(); iter != cases_.end(); ++iter) {
            if (!iter->first) { throw InvalidExpressionType("Boolean", "Null"); }

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

Value Loop::exec(Environment::SharedPtr env) const {
    if (cond_ && body_) {
        auto loopEnv = Environment::make(env);
        
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
Foreach::Foreach(const std::string &name, CodeNode::SharedPtr container, CodeNode::SharedPtr body)
    : CodeNode()
    , name_(name)
    , container_(container)
    , body_(body)
{}

Value Foreach::exec(Environment::SharedPtr env) const {
    if (container_ && body_) {
        auto loopEnv = Environment::make(env);
        loopEnv->def(name_, Value::Null);

        try {
            auto contValue = container_->eval(loopEnv);
            switch (contValue.type()) {
            case Value::eString:  return impl(loopEnv, contValue.text());
            case Value::eArray:   return impl(loopEnv, contValue.array());
            case Value::eHashMap: return impl(loopEnv, contValue.hashMap());
            case Value::eRange:   return implRange(loopEnv, contValue.range());
            default:
                throw InvalidExpressionType("String, Array, HashMap or Range", contValue.typeToString());
            }
        }
        catch (const Break::Except &) { return Value::Null; }
        catch (...) { throw; }
    }
    return Value::Null;
}

template <typename Container>
Value Foreach::impl(Environment::SharedPtr loopEnv, const Container &container) const {
    Value result = Value::Null;
    for (const auto &item : container) {
        if constexpr (std::is_same_v<Container, Hashtable>) {
            loopEnv->set(name_, Value(ValuePair(item.first, item.second)));
        }
        else {
            loopEnv->set(name_, item);
        }
        result = body_->eval(loopEnv);
    }
    return result;
}

Value Foreach::implRange(Environment::SharedPtr loopEnv, const IntegerRange &range) const {
    Value result = Value::Null;
    auto gen = range.generator();
    while (auto i = gen.next()) {
        loopEnv->set(name_, Value(*i));
        result = body_->eval(loopEnv);
    }
    return result;
}

// -------------------------------------------------------------
LambdaExpr::LambdaExpr(const ParamList &params, CodeNode::SharedPtr body)
  : CodeNode()
  , params_(params)
  , body_(body)
{}

Value LambdaExpr::exec(Environment::SharedPtr env) const {
    return Value(Lambda(params_, body_, env));
}

// -------------------------------------------------------------
LambdaApp::LambdaApp(CodeNode::SharedPtr closure, SharedPtrList args)
    : CodeNode()
    , closure_(closure)
    , argExprs_(args)
    , closureVar_(Value::Null)
{}

Value LambdaApp::exec(Environment::SharedPtr env) const {
    if (!closureVar_.isClosure() && closure_) {
        closureVar_ = closure_->eval(env);
    }

    if (!closureVar_.isClosure()) {
        throw InvalidExpressionType("Closure", closureVar_.typeToString());
    }

    Lambda::ArgList args(argExprs_.size());
    std::transform(argExprs_.begin(), argExprs_.end(), args.begin(), [&env](auto const & arg) { return arg->eval(env); });

    return closureVar_.closure().exec(args);
}

// -------------------------------------------------------------
FunctionExpr::FunctionExpr(const std::string &name, const ParamList &params, CodeNode::SharedPtr body)
    : LambdaExpr(params, body)
    , name_(name)
{}

Value FunctionExpr::exec(Environment::SharedPtr env) const {
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

Value FunctionApp::exec(Environment::SharedPtr env) const {
    closureVar_ = env->get(name_);
    return LambdaApp::exec(env);
}

// -------------------------------------------------------------
Print::Print(bool newline, CodeNode::SharedPtrList exprs)
    : CodeNode()
    , newline_(newline)
    , exprs_(exprs)
{}

Value Print::exec(Environment::SharedPtr env) const {
    for (const auto &expr : exprs_) {
        Value::print(expr->eval(env));
    }
    if (newline_) { std::cout << std::endl; }
    return Value::Null;
}

// -------------------------------------------------------------
Read::Read()
    : CodeNode()
{}

Value Read::exec(Environment::SharedPtr env) const {
    std::string input;
    std::getline(std::cin, input);
    return Parser().readLiteral(input)->eval(env);
}

// -------------------------------------------------------------
StructExpr::StructExpr(const std::string &name, const Struct::MemberList &members)
    : CodeNode()
    , struct_(name, members)
{}

Value StructExpr::exec(Environment::SharedPtr env) const {
    Value structValue(struct_);
    return env->def(struct_.name(), structValue);
}

// -------------------------------------------------------------
IsStructName::IsStructName(CodeNode::SharedPtr expr, const std::string &name)
    : CodeNode()
    , expr_(expr)
    , name_(name)
{}

Value IsStructName::exec(Environment::SharedPtr env) const {
    if (expr_) {
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
{}

Value StructName::exec(Environment::SharedPtr env) const {
    if (expr_) {
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
MakeInstance::MakeInstance(const std::string &name, const NameSharedPtrs &initList)
    : CodeNode()
    , name_(name)
    , initList_(initList)
{}

Value MakeInstance::exec(Environment::SharedPtr env) const {
    const Value structValue = env->get(name_);
    if (!structValue.isUserType()) {
        throw InvalidExpressionType("UserType", structValue.typeToString());
    }
    return Value(Instance(structValue.userType(), makeInitArgs(env)));
}

Instance::InitArgs MakeInstance::makeInitArgs(Environment::SharedPtr &env) const {
    Instance::InitArgs initArgs;
    for (const auto &nameSharedPtr : initList_) {
        initArgs.emplace(nameSharedPtr.first, nameSharedPtr.second->eval(env));
    }
    return initArgs;
}

// -------------------------------------------------------------
IsInstanceOf::IsInstanceOf(CodeNode::SharedPtr expr, const std::string &name)
    : CodeNode()
    , expr_(expr)
    , name_(name)
{}

Value IsInstanceOf::exec(Environment::SharedPtr env) const {
    if (expr_) {
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

Value GetMember::exec(Environment::SharedPtr env) const {
    if (expr_) {
        Value value = expr_->eval(env);
        if (value.isUserObject()) {
            return Generic::get(value.userObject(), name_);
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

Value SetMember::exec(Environment::SharedPtr env) const {
    if (expr_) {
        Value instanceValue = expr_->eval(env);
        if (instanceValue.isUserObject()) {
            if (newValExpr_) {
                Value newValue = newValExpr_->eval(env);
                Generic::set(instanceValue.userObject(), name_, newValue);
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

Value StringLen::exec(Environment::SharedPtr env) const {
    if (expr_) {
        const Value str = evalOperand(env, expr_, Value::eString);
        return Generic::length(str.text());
    }
    return Value::Zero;
}

// -------------------------------------------------------------
StringGet::StringGet(CodeNode::SharedPtr str, CodeNode::SharedPtr pos)
    : CodeNode()
    , str_(str)
    , pos_(pos)
{}

Value StringGet::exec(Environment::SharedPtr env) const {
    if (str_ && pos_) {
        const Value str = evalOperand(env, str_, Value::eString);
        return Generic::get(str.text(), pos_->eval(env));
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

Value StringSet::exec(Environment::SharedPtr env) const {
    if (str_ && pos_ && val_) {
        Value str = evalOperand(env, str_, Value::eString);
        Value val = val_->eval(env);
        Generic::set(str.text(), pos_->eval(env), val);
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

Value StringCat::exec(Environment::SharedPtr env) const {
    if (str_ && other_) {
        Value str = evalOperand(env, str_, Value::eString);
        const Value other = evalOperand(env, other_, Value::eString, Value::eCharacter);

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
{}

SubString::SubString(CodeNode::SharedPtr str, CodeNode::SharedPtr pos, CodeNode::SharedPtr len)
    : CodeNode()
    , str_(str)
    , pos_(pos)
    , len_(len)
{}

Value SubString::exec(Environment::SharedPtr env) const {
    if (str_ && pos_) {
        const Value str = evalOperand(env, str_, Value::eString);
        const Value pos = evalOperand(env, pos_, Value::eInteger);
        const Value len = len_ ? evalOperand(env, len_, Value::eInteger) : Value::Zero;

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
{}

StringFind::StringFind(CodeNode::SharedPtr str, CodeNode::SharedPtr chr, CodeNode::SharedPtr pos)
    : CodeNode()
    , str_(str)
    , chr_(chr)
    , pos_(pos)
{}

Value StringFind::exec(Environment::SharedPtr env) const {
    if (str_ && chr_) {
        const Value str = evalOperand(env, str_, Value::eString);
        const Value chr = evalOperand(env, chr_, Value::eCharacter);
        const Value pos = pos_ ? evalOperand(env, pos_, Value::eInteger) : Value::Zero;

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
{}

Value StringCount::exec(Environment::SharedPtr env) const {
    if (str_ && chr_) {
        const Value str = evalOperand(env, str_, Value::eString);
        const Value chr = evalOperand(env, chr_, Value::eCharacter);

        const auto &rawStr = str.text();

        return Value(Value::Long(std::count(rawStr.begin(), rawStr.end(), chr.character())));
    }
    return Value::Null;
}

// -------------------------------------------------------------
StringCompare::StringCompare(CodeNode::SharedPtr lhs, CodeNode::SharedPtr rhs)
    : CodeNode()
    , lhs_(lhs)
    , rhs_(rhs)
{}

Value StringCompare::exec(Environment::SharedPtr env) const {
    if (lhs_ && rhs_) {
        const Value lhs = evalOperand(env, lhs_, Value::eString);
        const Value rhs = evalOperand(env, rhs_, Value::eString);

        auto const & rawLhs = lhs.text();
        auto const & rawRhs = rhs.text();
        auto const maxSize = std::max(rawLhs.size(), rawRhs.size());

        return Value(Value::Long(std::strncmp(rawLhs.c_str(), rawRhs.c_str(), maxSize)));
    }
    return Value::Null;
}

// -------------------------------------------------------------
StringSort::StringSort(CodeNode::SharedPtr str, CodeNode::SharedPtr descending)
    : CodeNode()
    , str_(str)
    , desc_(descending)
{}

Value StringSort::exec(Environment::SharedPtr env) const {
    if (str_) {
        Value str = evalOperand(env, str_, Value::eString);
        const Value desc = desc_ ? evalOperand(env, desc_, Value::eBoolean) : Value::False;

        auto & rawStr = str.text();

        if (desc.boolean()) {
            std::sort(rawStr.begin(), rawStr.end(), std::greater<char>());
        }
        else {
            std::sort(rawStr.begin(), rawStr.end());
        }
        return str;
    }
    return Value::Null;
}

// -------------------------------------------------------------
StringReverse::StringReverse(CodeNode::SharedPtr str)
    : CodeNode()
    , str_(str)
{}

Value StringReverse::exec(Environment::SharedPtr env) const {
    if (str_) {
        Value str = evalOperand(env, str_, Value::eString);

        auto & rawStr = str.text();

        std::reverse(rawStr.begin(), rawStr.end());
        return str;
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

Value MakeArray::exec(Environment::SharedPtr env) const {
    if (values_.empty()) {
        return Value(Sequence());
    }
    else {
        std::vector<Value> values;
        for (const auto &valueCode : values_) {
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

Value MakeArraySV::exec(Environment::SharedPtr env) const {
    if (size_) {
        const Value size = evalOperand(env, size_, Value::eInteger);
        const Value initValue = initValue_ ? initValue_->eval(env) : Value::Null;
        return Value(Sequence(size.integer(), initValue));
    }
    return Value::Null;
}

// -------------------------------------------------------------
ArrayLen::ArrayLen(CodeNode::SharedPtr expr)
    : CodeNode()
    , expr_(expr)
{}

Value ArrayLen::exec(Environment::SharedPtr env) const {
    if (expr_) {
        const Value arr = evalOperand(env, expr_, Value::eArray);
        return Generic::length(arr.array());
    }
    return Value::Zero;
}

// -------------------------------------------------------------
ArrayGet::ArrayGet(CodeNode::SharedPtr arr, CodeNode::SharedPtr pos)
    : CodeNode()
    , arr_(arr)
    , pos_(pos)
{}

Value ArrayGet::exec(Environment::SharedPtr env) const {
    if (arr_ && pos_) {
        const Value arr = evalOperand(env, arr_, Value::eArray);
        return Generic::get(arr.array(), pos_->eval(env));
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

Value ArraySet::exec(Environment::SharedPtr env) const {
    if (arr_ && pos_ && val_) {
        Value arr = evalOperand(env, arr_, Value::eArray);
        Value val = val_->eval(env);
        Generic::set(arr.array(), pos_->eval(env), val);
        return val;
    }
    return Value::Null;
}

// -------------------------------------------------------------
ArrayPush::ArrayPush(CodeNode::SharedPtr arr, CodeNode::SharedPtr val)
    : CodeNode()
    , arr_(arr)
    , val_(val)
{}

Value ArrayPush::exec(Environment::SharedPtr env) const {
    if (arr_ && val_) {
        Value arr = evalOperand(env, arr_, Value::eArray);
        arr.array().push(val_->eval(env));
        return arr;
    }
    return Value::Null;
}

// -------------------------------------------------------------
ArrayPop::ArrayPop(CodeNode::SharedPtr arr)
    : CodeNode()
    , arr_(arr)
{}

Value ArrayPop::exec(Environment::SharedPtr env) const {
    if (arr_) {
        Value arr = evalOperand(env, arr_, Value::eArray);

        auto &rawArray = arr.array();
        auto const rawSize = rawArray.size();

        if (rawSize == 0) { throw OutOfRange("Pop empty array"); }

        Value back = rawArray.get(rawSize - 1);
        rawArray.pop();
        return back;
    }
    return Value::Null;
}

// -------------------------------------------------------------
ArrayFind::ArrayFind(CodeNode::SharedPtr arr, CodeNode::SharedPtr chr)
    : CodeNode()
    , arr_(arr)
    , val_(chr)
    , pos_()
{}

ArrayFind::ArrayFind(CodeNode::SharedPtr arr, CodeNode::SharedPtr chr, CodeNode::SharedPtr pos)
    : CodeNode()
    , arr_(arr)
    , val_(chr)
    , pos_(pos)
{}

Value ArrayFind::exec(Environment::SharedPtr env) const {
    if (arr_ && val_) {
        const Value arr = evalOperand(env, arr_, Value::eArray);
        const Value val = val_->eval(env);
        const Value pos = pos_ ? evalOperand(env, pos_, Value::eInteger) : Value::Zero;

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
{}

Value ArrayCount::exec(Environment::SharedPtr env) const {
    if (arr_ && val_) {
        const Value arr = evalOperand(env, arr_, Value::eArray);
        return Value(Value::Long(arr.array().count(val_->eval(env))));
    }
    return Value::Null;
}

// -------------------------------------------------------------
ArraySort::ArraySort(CodeNode::SharedPtr arr, CodeNode::SharedPtr descending)
    : CodeNode()
    , arr_(arr)
    , desc_(descending)
{}

Value ArraySort::exec(Environment::SharedPtr env) const {
    if (arr_) {
        Value arr = evalOperand(env, arr_, Value::eArray);
        const Value desc = desc_ ? evalOperand(env, desc_, Value::eBoolean) : Value::False;

        auto & rawArr = arr.array();

        rawArr.sort(desc.boolean());
        return arr;
    }
    return Value::Null;
}

// -------------------------------------------------------------
ArrayReverse::ArrayReverse(CodeNode::SharedPtr arr)
    : CodeNode()
    , arr_(arr)
{}

Value ArrayReverse::exec(Environment::SharedPtr env) const {
    if (arr_) {
        Value arr = evalOperand(env, arr_, Value::eArray);

        auto & rawArr = arr.array();

        rawArr.reverse();
        return arr;
    }
    return Value::Null;
}

// -------------------------------------------------------------
ArrayInsert::ArrayInsert(CodeNode::SharedPtr arr, CodeNode::SharedPtr pos, CodeNode::SharedPtr item)
    : CodeNode()
    , arr_(arr)
    , pos_(pos)
    , item_(item)
{}

Value ArrayInsert::exec(Environment::SharedPtr env) const {
    if (arr_ && pos_ && item_) {
        Value arr = evalOperand(env, arr_, Value::eArray);
        const Value pos = evalOperand(env, pos_, Value::eInteger);
        Value item = item_->eval(env);

        const auto rawPos = pos.integer();
        auto & rawArr = arr.array();

        if (rawPos < 0 || static_cast<std::size_t>(rawPos) > rawArr.size()) {
            throw OutOfRange("Array insert access");
        }

        rawArr.insert(static_cast<std::size_t>(rawPos), item);
        return item;
    }
    return Value::Null;
}

// -------------------------------------------------------------
ArrayRemove::ArrayRemove(CodeNode::SharedPtr arr, CodeNode::SharedPtr pos)
    : CodeNode()
    , arr_(arr)
    , pos_(pos)
{}

Value ArrayRemove::exec(Environment::SharedPtr env) const {
    if (arr_ && pos_) {
        Value arr = evalOperand(env, arr_, Value::eArray);
        const Value pos = evalOperand(env, pos_, Value::eInteger);

        const auto rawPos = pos.integer();
        auto & rawArr = arr.array();

        if (rawPos < 0 || static_cast<std::size_t>(rawPos) >= rawArr.size()) {
            throw OutOfRange("Array remove access");
        }

        rawArr.erase(static_cast<std::size_t>(rawPos));
    }
    return Value::Null;
}

// -------------------------------------------------------------
ArrayClear::ArrayClear(CodeNode::SharedPtr arr)
    : CodeNode()
    , arr_(arr)
{}

Value ArrayClear::exec(Environment::SharedPtr env) const {
    if (arr_) {
        Value arr = evalOperand(env, arr_, Value::eArray);
        arr.array().clear();
    }
    return Value::Null;
}

// -------------------------------------------------------------
StrCharCheck::StrCharCheck(Type type, CodeNode::SharedPtr operand)
    : CodeNode()
    , operand_(operand)
    , ftn_(typeToCheckFtn(type))
{}

Value StrCharCheck::exec(Environment::SharedPtr env) const {
    if (operand_) {
        const Value value = evalOperand(env, operand_, Value::eCharacter, Value::eString);

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
    , operand_(operand)
    , ftn_(typeToTransformFtn(type))
{}

Value StrCharTransform::exec(Environment::SharedPtr env) const {
    if (operand_) {
        const Value value = evalOperand(env, operand_, Value::eCharacter, Value::eString);

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

// -------------------------------------------------------------
ImportModule::ImportModule(const std::string &name, const std::string &asName)
    : CodeNode()
    , name_(name)
    , asName_(asName)
{}

Value ImportModule::exec(Environment::SharedPtr env) const {
    auto modulePtr = ModuleStorage::getOrCreate(name_);
    if (modulePtr) {
        return modulePtr->import(env, asName_.empty() ? Module::OptionalName() : Module::OptionalName(asName_));
    }
    return Value::False;
}

// -------------------------------------------------------------
FromModuleImport::FromModuleImport(const std::string &name, const std::string &varName, const std::string &asName)
    : CodeNode()
    , name_(name)
    , aliasList_()
{
    aliasList_.push_back({ varName, asName.empty() ? Module::OptionalName() : Module::OptionalName(asName) });
}

FromModuleImport::FromModuleImport(const std::string &name, const NameAndAsList &aliasList)
    : CodeNode()
    , name_(name)
    , aliasList_(aliasList)
{}

Value FromModuleImport::exec(Environment::SharedPtr env) const {
    auto modulePtr = ModuleStorage::getOrCreate(name_);
    if (modulePtr) {
        return modulePtr->aliases(env, aliasList_);
    }
    return Value::False;
}

// -------------------------------------------------------------
Random::Random(CodeNode::SharedPtr max)
    : CodeNode()
    , max_(max)
{}

Value Random::exec(Environment::SharedPtr env) const {
    static auto randFtn = std::mt19937(std::random_device()());
    static const auto maxRand = static_cast<Value::Long>(randFtn.max());

    Value::Long rand = static_cast<Value::Long>(randFtn());
    if (max_) {
        const Value maxValue = evalOperand(env, max_, Value::eInteger);

        if (maxValue.integer() < 0) { throw InvalidExpression("max negative"); }

        if (maxValue.integer() < maxRand) {
            rand = rand % (maxValue.integer() + 1);
        }
    }

    return Value(rand);
}

// -------------------------------------------------------------
Hash::Hash(CodeNode::SharedPtr operand)
    : CodeNode()
    , operand_(operand)
{}

Value Hash::exec(Environment::SharedPtr env) const {
    static const Value::Hash hashFtn;
    static const std::size_t maxLongPlus1 =
        std::size_t(std::numeric_limits<Value::Long>::max()) + 1;

    return (operand_
            ? Value(Value::Long(hashFtn(operand_->eval(env)) % maxLongPlus1))
            : Value::Zero);
}

// -------------------------------------------------------------
MakeHashMap::MakeHashMap(CodeNode::SharedPtrList pairs)
    : CodeNode()
    , pairs_(pairs)
{}

Value MakeHashMap::exec(Environment::SharedPtr env) const {
    if (pairs_.empty()) {
        return Value(Hashtable());
    }
    else {
        Hashtable::Table table;
        for (const auto &pairCode : pairs_) {
            auto pairValue = pairCode->eval(env);
            if (pairValue.isPair()) {
                append(table, pairValue.pair());
            }
            else if (pairValue.isArray()) {
                append(table, pairValue.array());
            }
            else {
                throw InvalidOperandType(
                    typesToString(Value::ePair, Value::eArray),
                    pairValue.typeToString());
            }
        }
        return Value(Hashtable(std::move(table)));
    }
}

void MakeHashMap::append(Hashtable::Table & table, const Sequence &arr) {
    if (arr.size() != 2) { throw InvalidExpression("Wrong array size, expecting 2"); }
    table.emplace(arr.get(0), arr.get(1));
}

void MakeHashMap::append(Hashtable::Table & table, const ValuePair &pair) {
    table.emplace(pair.first(), pair.second());
}

// -------------------------------------------------------------
HashMapLen::HashMapLen(CodeNode::SharedPtr htExpr)
    : CodeNode()
    , htExpr_(htExpr)
{}

Value HashMapLen::exec(Environment::SharedPtr env) const {
    if (htExpr_) {
        const Value hm = evalOperand(env, htExpr_, Value::eHashMap);
        return Generic::length(hm.hashMap());
    }
    return Value::Zero;
}

// -------------------------------------------------------------
HashMapContains::HashMapContains(CodeNode::SharedPtr htExpr, CodeNode::SharedPtr keyExpr)
    : CodeNode()
    , htExpr_(htExpr)
    , keyExpr_(keyExpr)
{}

Value HashMapContains::exec(Environment::SharedPtr env) const {
    if (htExpr_ && keyExpr_) {
        const Value hm = evalOperand(env, htExpr_, Value::eHashMap);
        return Value(hm.hashMap().exists(keyExpr_->eval(env)));
    }
    return Value::False;
}

// -------------------------------------------------------------
HashMapGet::HashMapGet(CodeNode::SharedPtr htExpr, CodeNode::SharedPtr keyExpr, CodeNode::SharedPtr defaultExpr)
    : CodeNode()
    , htExpr_(htExpr)
    , keyExpr_(keyExpr)
    , defaultExpr_(defaultExpr)
{}

Value HashMapGet::exec(Environment::SharedPtr env) const {
    if (htExpr_ && keyExpr_) {
        const Value hm = evalOperand(env, htExpr_, Value::eHashMap);
        const Value key = keyExpr_->eval(env);
        const Value defaultRet = defaultExpr_ ? defaultExpr_->eval(env) : Value::Null;
        return Generic::get(hm.hashMap(), key, defaultRet);
    }
    return Value::Null;
}

// -------------------------------------------------------------
HashMapSet::HashMapSet(CodeNode::SharedPtr htExpr, CodeNode::SharedPtr keyExpr, CodeNode::SharedPtr valueExpr)
    : CodeNode()
    , htExpr_(htExpr)
    , keyExpr_(keyExpr)
    , valueExpr_(valueExpr)
{}

Value HashMapSet::exec(Environment::SharedPtr env) const {
    if (htExpr_ && keyExpr_ && valueExpr_) {
        Value hm = evalOperand(env, htExpr_, Value::eHashMap);
        Value value = valueExpr_->eval(env);
        Generic::set(hm.hashMap(), keyExpr_->eval(env), value);
        return value;
    }
    return Value::Null;
}

// -------------------------------------------------------------
HashMapRemove::HashMapRemove(CodeNode::SharedPtr htExpr, CodeNode::SharedPtr keyExpr)
    : CodeNode()
    , htExpr_(htExpr)
    , keyExpr_(keyExpr)
{}

Value HashMapRemove::exec(Environment::SharedPtr env) const {
    if (htExpr_ && keyExpr_) {
        Value hm = evalOperand(env, htExpr_, Value::eHashMap);
        hm.hashMap().remove(keyExpr_->eval(env));
    }
    return Value::Null;
}

// -------------------------------------------------------------
HashMapClear::HashMapClear(CodeNode::SharedPtr htExpr)
    : CodeNode()
    , htExpr_(htExpr)
{}

Value HashMapClear::exec(Environment::SharedPtr env) const {
    if (htExpr_) {
        Value hm = evalOperand(env, htExpr_, Value::eHashMap);
        hm.hashMap().clear();
    }
    return Value::Null;
}

// -------------------------------------------------------------
HashMapFind::HashMapFind(CodeNode::SharedPtr htExpr, CodeNode::SharedPtr valueExpr)
    : CodeNode()
    , htExpr_(htExpr)
    , valueExpr_(valueExpr)
{}

Value HashMapFind::exec(Environment::SharedPtr env) const {
    if (htExpr_ && valueExpr_) {
        const Value hm = evalOperand(env, htExpr_, Value::eHashMap);
        return hm.hashMap().find(valueExpr_->eval(env));
    }
    return Value::Null;
}

// -------------------------------------------------------------
HashMapCount::HashMapCount(CodeNode::SharedPtr htExpr, CodeNode::SharedPtr valueExpr)
    : CodeNode()
    , htExpr_(htExpr)
    , valueExpr_(valueExpr)
{}

Value HashMapCount::exec(Environment::SharedPtr env) const {
    if (htExpr_ && valueExpr_) {
        const Value hm = evalOperand(env, htExpr_, Value::eHashMap);
        return Value(Value::Long(hm.hashMap().count(valueExpr_->eval(env))));
    }
    return Value::Zero;
}

// -------------------------------------------------------------
HashMapKeys::HashMapKeys(CodeNode::SharedPtr htExpr)
    : CodeNode()
    , htExpr_(htExpr)
{}

Value HashMapKeys::exec(Environment::SharedPtr env) const {
    if (htExpr_) {
        const Value hm = evalOperand(env, htExpr_, Value::eHashMap);
        return Value(hm.hashMap().keys());
    }
    return Value::Null;
}

// -------------------------------------------------------------
HashMapValues::HashMapValues(CodeNode::SharedPtr htExpr)
    : CodeNode()
    , htExpr_(htExpr)
{}

Value HashMapValues::exec(Environment::SharedPtr env) const {
    if (htExpr_) {
        const Value hm = evalOperand(env, htExpr_, Value::eHashMap);
        return Value(hm.hashMap().values());
    }
    return Value::Null;
}

// -------------------------------------------------------------
HashMapItems::HashMapItems(CodeNode::SharedPtr htExpr)
    : CodeNode()
    , htExpr_(htExpr)
{}

Value HashMapItems::exec(Environment::SharedPtr env) const {
    if (htExpr_) {
        const Value hm = evalOperand(env, htExpr_, Value::eHashMap);
        return Value(hm.hashMap().items());
    }
    return Value::Null;
}

// -------------------------------------------------------------
MakePair::MakePair(CodeNode::SharedPtr firstExpr, CodeNode::SharedPtr secondExpr)
    : CodeNode()
    , firstExpr_(firstExpr)
    , secondExpr_(secondExpr)
{}

Value MakePair::exec(Environment::SharedPtr env) const {
    if (firstExpr_ && secondExpr_) {
        const auto first = firstExpr_->eval(env);
        const auto second = secondExpr_->eval(env);

        return Value(ValuePair(first, second));
    }
    return Value::EmptyPair;
}

// -------------------------------------------------------------
PairFirst::PairFirst(CodeNode::SharedPtr pairExpr)
    : CodeNode()
    , pairExpr_(pairExpr)
{}

Value PairFirst::exec(Environment::SharedPtr env) const {
    if (pairExpr_) {
        const auto pairValue = evalOperand(env, pairExpr_, Value::ePair);
        return pairValue.pair().first();
    }
    return Value::Null;
}

// -------------------------------------------------------------
PairSecond::PairSecond(CodeNode::SharedPtr pairExpr)
    : CodeNode()
    , pairExpr_(pairExpr)
{}

Value PairSecond::exec(Environment::SharedPtr env) const {
    if (pairExpr_) {
        const auto pairValue = evalOperand(env, pairExpr_, Value::ePair);
        return pairValue.pair().second();
    }
    return Value::Null;
}

// -------------------------------------------------------------
MakeRange::MakeRange(CodeNode::SharedPtr end)
    : CodeNode()
    , begin_(CodeNode::SharedPtr())
    , end_(end)
    , step_(CodeNode::SharedPtr())
{}

MakeRange::MakeRange(CodeNode::SharedPtr begin, CodeNode::SharedPtr end, CodeNode::SharedPtr step)
    : CodeNode()
    , begin_(begin)
    , end_(end)
    , step_(step)
{}

Value MakeRange::exec(Environment::SharedPtr env) const {
    if (!end_) {
        throw InvalidExpression("Missing required range end");
    }

    const auto end = evalOperand(env, end_, Value::eInteger).integer();
    const auto begin = begin_ ? evalOperand(env, begin_, Value::eInteger).integer() : 0ll;
    const auto step = step_ ? evalOperand(env, step_, Value::eInteger).integer() : 1ll;
    return Value(IntegerRange(begin, end, step));
}

// -------------------------------------------------------------
template <typename R>
RangeGetter<R>::RangeGetter(CodeNode::SharedPtr rng, Getter && getter)
    : CodeNode()
    , rng_(rng)
    , getter_(std::move(getter))
{}

template <typename R>
Value RangeGetter<R>::exec(Environment::SharedPtr env) const {
    if (rng_) {
        const auto rng = evalOperand(env, rng_, Value::eRange);

        if constexpr (std::is_same_v<R, Value>) {
            return getter_(rng.range());
        }
        else {
            return Value(getter_(rng.range()));
        }
    }
    return Value::Null;
}

// -------------------------------------------------------------
RangeBegin::RangeBegin(CodeNode::SharedPtr rng)
    : RangeGetter<Value::Long>(rng, &IntegerRange::begin)
{}

// -------------------------------------------------------------
RangeEnd::RangeEnd(CodeNode::SharedPtr rng)
    : RangeGetter<Value::Long>(rng, &IntegerRange::end)
{}

// -------------------------------------------------------------
RangeStep::RangeStep(CodeNode::SharedPtr rng)
    : RangeGetter<Value::Long>(rng, &IntegerRange::step)
{}

// -------------------------------------------------------------
RangeLen::RangeLen(CodeNode::SharedPtr rng)
    : RangeGetter<Value>(
        rng,
        [](const IntegerRange & r) {
            return Generic::length(r);
        })
{}

// -------------------------------------------------------------
Expand::Expand(CodeNode::SharedPtrList exprs)
    : CodeNode()
    , exprs_(exprs)
{}

Value Expand::exec(Environment::SharedPtr env) const {
    std::vector<Value> values;
    for (const auto &expr : exprs_) {
        const auto val = evalOperand(env, expr, Value::eRange);

        auto gen = val.range().generator();
        while (auto i = gen.next()) {
            values.push_back(Value(*i));
        }
    }
    return Value(Sequence(std::move(values)));
}

// -------------------------------------------------------------
GenericLen::GenericLen(CodeNode::SharedPtr object)
    : CodeNode()
    , object_(object)
{}

Value GenericLen::exec(Environment::SharedPtr env) const {
    if (object_) {
        auto objVal = object_->eval(env);
        switch (objVal.type()) {
        case Value::eString:  return Generic::length(objVal.text());
        case Value::eArray:   return Generic::length(objVal.array());
        case Value::eHashMap: return Generic::length(objVal.hashMap());
        case Value::eRange:   return Generic::length(objVal.range());
        default:
            throw InvalidOperandType(
                typesToString(Value::eString, Value::eArray, Value::eHashMap, Value::eRange),
                objVal.typeToString());
        }
    }
    return Value::Null;
}

// -------------------------------------------------------------
GenericGet::GenericGet(CodeNode::SharedPtr object, CodeNode::SharedPtr key, CodeNode::SharedPtr defaultRet)
    : CodeNode()
    , object_(object)
    , key_(key)
    , defaultRet_(defaultRet)
{}

Value GenericGet::exec(Environment::SharedPtr env) const {
    if (object_ && key_) {
        auto objVal = object_->eval(env);
        switch (objVal.type()) {
        case Value::eString:
            return Generic::get(objVal.text(), key_->eval(env));

        case Value::eArray:
            return Generic::get(objVal.array(), key_->eval(env));

        case Value::eHashMap:
            return Generic::get(objVal.hashMap(), key_->eval(env), defaultRet_ ? defaultRet_->eval(env) : Value::Null);

        case Value::eUserObject:
            return key_->isIdentifier()
                ? Generic::get(objVal.userObject(), key_->identifierName())
                : Generic::get(objVal.userObject(), key_->eval(env));

        default:
            throw InvalidOperandType(
                typesToString(Value::eString, Value::eArray, Value::eHashMap, Value::eUserObject),
                objVal.typeToString());
        }
    }
    return Value::Null;
}

// -------------------------------------------------------------
GenericSet::GenericSet(CodeNode::SharedPtr object, CodeNode::SharedPtr key, CodeNode::SharedPtr value)
    : CodeNode()
    , object_(object)
    , key_(key)
    , value_(value)
{}

Value GenericSet::exec(Environment::SharedPtr env) const {
    if (object_ && key_ && value_) {
        auto objVal = object_->eval(env);
        auto value = value_->eval(env);

        switch (objVal.type()) {
        case Value::eString:
            Generic::set(objVal.text(), key_->eval(env), value);
            break;

        case Value::eArray:
            Generic::set(objVal.array(), key_->eval(env), value);
            break;

        case Value::eHashMap:
            Generic::set(objVal.hashMap(), key_->eval(env), value);
            break;

        case Value::eUserObject:
            if (key_->isIdentifier()) {
                Generic::set(objVal.userObject(), key_->identifierName(), value);
            }
            else {
                Generic::set(objVal.userObject(), key_->eval(env), value);
            }
            break;

        default:
            throw InvalidOperandType(
                typesToString(Value::eString, Value::eArray, Value::eHashMap, Value::eUserObject),
                objVal.typeToString());
        }

        return value;
    }
    return Value::Null;
}
