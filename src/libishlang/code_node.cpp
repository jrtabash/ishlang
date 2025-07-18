#include "code_node.h"
#include "code_node_util.h"
#include "exception.h"
#include "file_io.h"
#include "generic_functions.h"
#include "lambda.h"
#include "module.h"
#include "parser.h"
#include "sequence.h"
#include "util.h"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cmath>
#include <cstring>
#include <filesystem>
#include <functional>
#include <limits>
#include <iomanip>
#include <iostream>
#include <random>
#include <ranges>

using namespace Ishlang;

namespace fs = std::filesystem;

// -------------------------------------------------------------
IsType::IsType(CodeNode::SharedPtr expr, Value::TypeList types)
  : CodeNode()
  , expr_(expr)
  , types_(types)
{
    assert(types_.size() > 0);
}

Value IsType::exec(Environment::SharedPtr env) const {
    if (expr_) {
        const Value exprValue = expr_->eval(env);
        return Value(std::ranges::any_of(
                         types_,
                         [exprType = exprValue.type()](Value::Type type) {
                             return exprType == type;
                         }));
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
Assert::Assert(const std::string &tag, CodeNode::SharedPtr expr)
    : CodeNode()
    , tag_(tag)
    , expr_(expr)
{}

Value Assert::exec(Environment::SharedPtr env) const {
    if (expr_) {
        Value val = expr_->eval(env);

        if (!val.isBool()) {
            throw AssertFailed(tag_, "unable to check non-boolean expression");
        }

        if (!val.boolean()) {
            throw AssertFailed(tag_);
        }

        return Value::True;
    }
    return Value::Null;
}

// -------------------------------------------------------------
ArithOp::ArithOp(Type type, CodeNode::SharedPtrList operands)
    : VariadicOp(operands)
    , type_(type)
{
    assert(operands_.size() >= 2);
}

Value ArithOp::exec(Environment::SharedPtr env) const {
    if (!operands_.empty()) {
        const auto values = evalOperands(env, operands_, Value::eInteger, Value::eReal);

        const bool real = std::ranges::any_of(values, [](const Value & v) { return v.isReal(); });
        switch (type_) {
        case Add:
            if (real) { return accum<Value::Double>(values, std::plus<Value::Double>()); }
            else      { return accum<Value::Long>(values, std::plus<Value::Long>()); }
                
        case Sub:
            if (real) { return accum<Value::Double>(values, std::minus<Value::Double>()); }
            else      { return accum<Value::Long>(values, std::minus<Value::Long>()); }
                
        case Mul:
            if (real) { return accum<Value::Double>(values, std::multiplies<Value::Double>()); }
            else      { return accum<Value::Long>(values, std::multiplies<Value::Long>()); }
                
        case Div:
            if (real) {
                if (isDivByZero<Value::Double>(values)) { throw DivByZero(); }
                return accum<Value::Double>(values, std::divides<Value::Double>());
            }
            else {
                if (isDivByZero<Value::Long>(values)) { throw DivByZero(); }
                return accum<Value::Long>(values, std::divides<Value::Long>());
            }

        case Mod:
            if (real) {
                throw InvalidOperandType(Value::typeToString(Value::eInteger), Value::typeToString(Value::eReal));
            }
            if (isDivByZero<Value::Long>(values)) { throw DivByZero(); }
            return accum<Value::Long>(values, std::modulus<Value::Long>());

        case Pow:
            return accum<Value::Double>(values, power);
        }
    }
    return Value::Zero;
}

// -------------------------------------------------------------
ArithAssignOp::ArithAssignOp(Type type, const std::string &name, CodeNode::SharedPtr delta)
    : CodeNode()
    , type_(type)
    , name_(name)
    , delta_(delta)
{}

Value ArithAssignOp::exec(Environment::SharedPtr env) const {
    if (delta_) {
        const auto & nameVal = env->get(name_);
        if (!nameVal.isNumber()) {
            throw InvalidExpressionType(typesToString(Value::eInteger, Value::eReal), nameVal.typeToString());
        }

        const auto deltaVal = evalOperand(env, delta_, Value::eInteger, Value::eReal);
        const bool real = nameVal.isReal() || deltaVal.isReal();

        Value updtVal;
        switch (type_) {
        case Add:
            if (real) { updtVal = Value(nameVal.real() + deltaVal.real()); }
            else      { updtVal = Value(nameVal.integer() + deltaVal.integer()); }
            break;

        case Sub:
            if (real) { updtVal = Value(nameVal.real() - deltaVal.real()); }
            else      { updtVal = Value(nameVal.integer() - deltaVal.integer()); }
            break;

        case Mul:
            if (real) { updtVal = Value(nameVal.real() * deltaVal.real()); }
            else      { updtVal = Value(nameVal.integer() * deltaVal.integer()); }
            break;

        case Div:
            if (real) {
                if (Util::isZero(deltaVal.real())) { throw DivByZero(); }
                updtVal = Value(nameVal.real() / deltaVal.real());
            }
            else {
                if (deltaVal.integer() == 0) { throw DivByZero(); }
                updtVal = Value(nameVal.integer() / deltaVal.integer());
            }
            break;

        case Mod:
            if (real) {
                throw InvalidOperandType(Value::typeToString(Value::eInteger), Value::typeToString(Value::eReal));
            }
            if (deltaVal.integer() == 0) { throw DivByZero(); }
            updtVal = Value(nameVal.integer() % deltaVal.integer());
            break;

        case Pow:
            updtVal = Value(std::pow(nameVal.real(), deltaVal.real()));
            break;
        }
        return env->set(name_, updtVal);
    }
    return Value::Null;
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
                throw IncompatibleTypes(op2str(type_), lhsVal.typeToString(), rhsVal.typeToString());
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

const char * CompOp::op2str(Type type) {
    switch (type) {
    case EQ: return "==";
    case NE: return "!=";
    case LT: return "<";
    case GT: return ">";
    case LE: return "<=";
    case GE: return ">=";
    }

    assert(!"unknown operation");
    return "unknown";
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
    : UnaryOp(operand)
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
    : UnaryOp(operand)
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

        const Value pVal = evalExpression(ifEnv, pred_, Value::eBoolean);
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
            if (!iter->first) { throw InvalidExpression("Null condition"); }

            const Value pred = evalExpression(env, iter->first, Value::eBoolean);
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
                condVal = evalExpression(loopEnv, cond_, Value::eBoolean);
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
            case Value::eFile:    return implFile(loopEnv, contValue.file());
            default:
                throw InvalidExpressionType(
                    typesToString(Value::eString, Value::eArray, Value::eHashMap, Value::eRange, Value::eFile),
                    contValue.typeToString());
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

Value Foreach::implFile(Environment::SharedPtr loopEnv, FileStruct &file) const {
    Value result = Value::Null;
    while (auto optLine = file.readln()) {
        loopEnv->set(name_, Value(std::move(*optLine)));
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
        closureVar_ = evalExpression(env, closure_, Value::eClosure);
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
        throw InvalidExpressionType(Value::typeToString(Value::eClosure), lambdaVal.typeToString());
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
            throw InvalidExpressionType(
                typesToString(Value::eUserObject, Value::eUserType),
                value.typeToString());
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
        throw InvalidExpressionType(Value::typeToString(Value::eUserType), structValue.typeToString());
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
        const Value value = evalOperand(env, expr_, Value::eUserObject);
        return Generic::get(value.userObject(), name_);
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
    if (expr_ && newValExpr_) {
        Value instanceValue = evalOperand(env, expr_, Value::eUserObject);
        Value newValue = newValExpr_->eval(env);
        Generic::set(instanceValue.userObject(), name_, newValue);
        return newValue;
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
        return Generic::find(evalOperand(env, str_, Value::eString).text(),
                             chr_->eval(env),
                             pos_ ? pos_->eval(env) : Value::Zero);
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
        return Generic::count(evalOperand(env, str_, Value::eString).text(),
                              chr_->eval(env));
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
        Generic::sort(str.text(), desc_ ? desc_->eval(env) : Value::False);
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
        Generic::reverse(str.text());
        return str;
    }
    return Value::Null;
}

// -------------------------------------------------------------
StringSplit::StringSplit(CodeNode::SharedPtr str, CodeNode::SharedPtr delim)
    : CodeNode()
    , str_(str)
    , delim_(delim)
{}

Value StringSplit::exec(Environment::SharedPtr env) const {
    if (str_ && delim_) {
        const Value str = evalOperand(env, str_, Value::eString);
        const Value delim = evalOperand(env, delim_, Value::eCharacter);

        auto ret = Value(Sequence());
        auto & arr = ret.array();

        for (auto && token : Util::split(str.text(), delim.character())) {
            arr.push(Value(std::move(token)));
        }

        return ret;
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
MakeArraySG::MakeArraySG(CodeNode::SharedPtr size, CodeNode::SharedPtr genFtn)
    : CodeNode()
    , size_(size)
    , genFtn_(genFtn)
{}

Value MakeArraySG::exec(Environment::SharedPtr env) const {
    if (size_ && genFtn_) {
        const auto size = evalOperand(env, size_, Value::eInteger);
        const auto rawSize = size.integer();

        Sequence seq(rawSize, Value::Null);
        if (rawSize > 0) {
            const auto gclo = evalOperand(env, genFtn_, Value::eClosure);

            const auto & gftn = gclo.closure();
            const Lambda::ArgList args;

            for (Value::Long i = 0; i < rawSize; ++i) {
                seq.set(i, gftn.exec(args));
            }
        }
        return Value(std::move(seq));
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
        return Generic::find(evalOperand(env, arr_, Value::eArray).array(),
                             val_->eval(env),
                             pos_ ? pos_->eval(env) : Value::Zero);
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
        return Generic::count(evalOperand(env, arr_, Value::eArray).array(),
                              val_->eval(env));
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
        Generic::sort(arr.array(), desc_ ? desc_->eval(env) : Value::False);
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
        Generic::reverse(arr.array());
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
            return Value(std::ranges::all_of(str, ftn_));
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
    : UnaryOp(operand)
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
        return Generic::find(evalOperand(env, htExpr_, Value::eHashMap).hashMap(),
                             valueExpr_->eval(env));
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
        return Generic::count(evalOperand(env, htExpr_, Value::eHashMap).hashMap(),
                              valueExpr_->eval(env));
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
        case Value::ePair:    return Generic::length(objVal.pair());
        default:
            throw InvalidOperandType(
                typesToString(Value::eString, Value::eArray, Value::eHashMap, Value::eRange, Value::ePair),
                objVal.typeToString());
        }
    }
    return Value::Null;
}

// -------------------------------------------------------------
GenericEmpty::GenericEmpty(CodeNode::SharedPtr object)
    : CodeNode()
    , object_(object)
{}

Value GenericEmpty::exec(Environment::SharedPtr env) const {
    if (object_) {
        auto objVal = object_->eval(env);
        switch (objVal.type()) {
        case Value::eString:  return Generic::empty(objVal.text());
        case Value::eArray:   return Generic::empty(objVal.array());
        case Value::eHashMap: return Generic::empty(objVal.hashMap());
        case Value::eRange:   return Generic::empty(objVal.range());
        case Value::ePair:    return Generic::empty(objVal.pair());
        default:
            throw InvalidOperandType(
                typesToString(Value::eString, Value::eArray, Value::eHashMap, Value::eRange, Value::ePair),
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

        case Value::ePair:
            return Generic::get(objVal.pair(), key_->eval(env));

        default:
            throw InvalidOperandType(
                typesToString(Value::eString, Value::eArray, Value::eHashMap, Value::eUserObject, Value::ePair),
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

// -------------------------------------------------------------
GenericClear::GenericClear(CodeNode::SharedPtr object)
    : CodeNode()
    , object_(object)
{}

Value GenericClear::exec(Environment::SharedPtr env) const {
    if (object_) {
        auto objVal = object_->eval(env);
        switch (objVal.type()) {
        case Value::eString:  return Generic::clear(objVal.text());
        case Value::eArray:   return Generic::clear(objVal.array());
        case Value::eHashMap: return Generic::clear(objVal.hashMap());
        default:
            throw InvalidOperandType(
                typesToString(Value::eString, Value::eArray, Value::eHashMap),
                objVal.typeToString());
        }
    }
    return Value::Null;
}

// -------------------------------------------------------------
GenericFind::GenericFind(CodeNode::SharedPtr object, CodeNode::SharedPtr item, CodeNode::SharedPtr pos)
    : CodeNode()
    , object_(object)
    , item_(item)
    , pos_(pos)
{}

Value GenericFind::exec(Environment::SharedPtr env) const {
    if (object_ && item_) {
        auto objVal = object_->eval(env);
        auto itemVal = item_->eval(env);

        switch (objVal.type()) {
        case Value::eString:  return Generic::find(objVal.text(), itemVal, pos_ ? pos_->eval(env) : Value::Zero);
        case Value::eArray:   return Generic::find(objVal.array(), itemVal, pos_ ? pos_->eval(env) : Value::Zero);
        case Value::eHashMap: return Generic::find(objVal.hashMap(), itemVal);
        case Value::ePair:    return Generic::find(objVal.pair(), itemVal, pos_ ? pos_->eval(env) : Value::Zero);
        default:
            throw InvalidOperandType(
                typesToString(Value::eString, Value::eArray, Value::eHashMap, Value::ePair),
                objVal.typeToString());
        }
    }
    return Value::Null;
}

// -------------------------------------------------------------
GenericCount::GenericCount(CodeNode::SharedPtr object, CodeNode::SharedPtr item)
    : CodeNode()
    , object_(object)
    , item_(item)
{}

// -------------------------------------------------------------
Value GenericCount::exec(Environment::SharedPtr env) const {
    if (object_ && item_) {
        auto objVal = object_->eval(env);
        auto itemVal = item_->eval(env);

        switch (objVal.type()) {
        case Value::eString:  return Generic::count(objVal.text(), itemVal);
        case Value::eArray:   return Generic::count(objVal.array(), itemVal);
        case Value::eHashMap: return Generic::count(objVal.hashMap(), itemVal);
        case Value::ePair:    return Generic::count(objVal.pair(), itemVal);
        default:
            throw InvalidOperandType(
                typesToString(Value::eString, Value::eArray, Value::eHashMap, Value::ePair),
                objVal.typeToString());
        }
    }
    return Value::Null;
}

// -------------------------------------------------------------
GenericSort::GenericSort(CodeNode::SharedPtr obj, CodeNode::SharedPtr descending)
    : CodeNode()
    , obj_(obj)
    , desc_(descending)
{}

Value GenericSort::exec(Environment::SharedPtr env) const {
    if (obj_) {
        Value obj = obj_->eval(env);
        const Value desc = desc_ ? desc_->eval(env) : Value::False;
        switch (obj.type()) {
        case Value::eString: Generic::sort(obj.text(), desc);  break;
        case Value::eArray:  Generic::sort(obj.array(), desc); break;
        default:
            throw InvalidOperandType(
                typesToString(Value::eString, Value::eArray),
                obj.typeToString());
        }
        return obj;
    }
    return Value::Null;
}

// -------------------------------------------------------------
GenericReverse::GenericReverse(CodeNode::SharedPtr obj)
    : CodeNode()
    , obj_(obj)
{}

Value GenericReverse::exec(Environment::SharedPtr env) const {
    if (obj_) {
        Value obj = obj_->eval(env);
        switch (obj.type()) {
        case Value::eString: Generic::reverse(obj.text());  break;
        case Value::eArray:  Generic::reverse(obj.array()); break;
        default:
            throw InvalidOperandType(
                typesToString(Value::eString, Value::eArray),
                obj.typeToString());
        }
        return obj;
    }
    return Value::Null;
}

// -------------------------------------------------------------
GenericSum::GenericSum(CodeNode::SharedPtr obj)
    : CodeNode()
    , obj_(obj)
{}

Value GenericSum::exec(Environment::SharedPtr env) const {
    if (obj_) {
        Value obj = obj_->eval(env);
        switch (obj.type()) {
        case Value::eArray: return Generic::sum(obj.array());
        case Value::eRange: return Generic::sum(obj.range());
        case Value::ePair:  return Generic::sum(obj.pair());
        default:
            throw InvalidOperandType(
                typesToString(Value::eArray, Value::eRange, Value::ePair),
                obj.typeToString());
        }
    }
    return Value::Null;
}

// -------------------------------------------------------------
GenericApply::GenericApply(CodeNode::SharedPtr ftn, CodeNode::SharedPtr args)
    : CodeNode()
    , ftn_(ftn)
    , args_(args)
{}

Value GenericApply::exec(Environment::SharedPtr env) const {
    if (ftn_) {
        const auto ftnVal = evalOperand(env, ftn_, Value::eClosure);
        const auto argsVal = args_->eval(env);

        switch (argsVal.type()) {
        case Value::eArray: return Generic::apply(ftnVal.closure(), argsVal.array());
        case Value::ePair:  return Generic::apply(ftnVal.closure(), argsVal.pair());
        case Value::eRange: return Generic::apply(ftnVal.closure(), argsVal.range());
        default:
            throw InvalidOperandType(
                typesToString(Value::eArray, Value::ePair, Value::eRange),
                argsVal.typeToString());
        }
    }
    return Value::Null;
}

// -------------------------------------------------------------
TimeIt::TimeIt(CodeNode::SharedPtr expr, CodeNode::SharedPtr count, CodeNode::SharedPtr summary)
    : CodeNode()
    , expr_(expr)
    , count_(count)
    , summary_(summary)
{}

Value TimeIt::exec(Environment::SharedPtr env) const {
    if (expr_) {
        Value::Long sum = 0;
        Value::Long min = 0;
        Value::Long max = 0;

        const auto count = std::min(std::max(count_ ? evalOperand(env, count_, Value::eInteger).integer() : 1ll, 1ll), 1000ll);
        const auto summary = summary_ ? evalOperand(env, summary_, Value::eBoolean).boolean() : true;

        for (Value::Long i = 0; i < count; ++i) {
            auto tEnv = Environment::make(env);

            auto start = std::chrono::high_resolution_clock::now();
            expr_->eval(tEnv);
            auto end = std::chrono::high_resolution_clock::now();

            Value::Long const duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            if (sum > 0) {
                sum += duration;
                min = std::min(min, duration);
                max = std::max(max, duration);
            }
            else {
                sum = duration;
                min = duration;
                max = duration;
            }
        }

        const auto mean = sum / static_cast<double>(count);
        if (summary) {
            std::cout << "\nTimeIt Summary / Microseconds"
                      << "\n-----------------------------"
                      << "\n  count: " << count
                      << "\n    sum: " << sum
                      << "\n   mean: " << std::fixed << std::setprecision(3) << mean
                      << "\n    min: " << min
                      << "\n    max: " << max
                      << "\n"
                      << std::endl;
        }
        return Value(mean);
    }
    return Value::Null;
}

// -------------------------------------------------------------
FileOpen::FileOpen(CodeNode::SharedPtr filename, CodeNode::SharedPtr mode)
    : FileOp(filename)
    , mode_(mode)
{}

Value FileOpen::exec(Environment::SharedPtr env) const {
    if (file_ && mode_) {
        auto fname = evalOperand(env, file_, Value::eString).text();
        const auto fmode = evalOperand(env, mode_, Value::eCharacter).character();
        return Value(FileParams{.filename=std::move(fname), .mode=FileModeNS::fromChar(fmode)});
    }
    return Value::Null;
}

// -------------------------------------------------------------
FileClose::FileClose(CodeNode::SharedPtr file)
    : FileOp(file)
{}

Value FileClose::exec(Environment::SharedPtr env) const {
    if (file_) {
        auto fileVal = evalOperand(env, file_, Value::eFile);
        auto &file = fileVal.file();
        file.close();
        return Value(!file.isOpen());
    }
    return Value::Null;
}

// -------------------------------------------------------------
FileFlush::FileFlush(CodeNode::SharedPtr file)
    : FileOp(file)
{}

Value FileFlush::exec(Environment::SharedPtr env) const {
    if (file_) {
        auto fileVal = evalOperand(env, file_, Value::eFile);
        auto &file = fileVal.file();
        file.flush();
        return Value(file.isOpen());
    }
    return Value::Null;
}

// -------------------------------------------------------------
FileIsOpen::FileIsOpen(CodeNode::SharedPtr file)
    : FileOp(file)
{}

Value FileIsOpen::exec(Environment::SharedPtr env) const {
    if (file_) {
        const auto fileVal = evalOperand(env, file_, Value::eFile);
        return Value(fileVal.file().isOpen());
    }
    return Value::Null;
}

// -------------------------------------------------------------
FileFName::FileFName(CodeNode::SharedPtr file)
    : FileOp(file)
{}

Value FileFName::exec(Environment::SharedPtr env) const {
    if (file_) {
        const auto fileVal = evalOperand(env, file_, Value::eFile);
        return Value(fileVal.file().filename());
    }
    return Value::Null;
}

// -------------------------------------------------------------
FileFMode::FileFMode(CodeNode::SharedPtr file)
    : FileOp(file)
{}

Value FileFMode::exec(Environment::SharedPtr env) const {
    if (file_) {
        const auto fileVal = evalOperand(env, file_, Value::eFile);
        return Value(FileModeNS::toChar(fileVal.file().mode()));
    }
    return Value::Null;
}

// -------------------------------------------------------------
FileRead::FileRead(CodeNode::SharedPtr file)
    : FileOp(file)
{}

Value FileRead::exec(Environment::SharedPtr env) const {
    if (file_) {
        auto fileVal = evalOperand(env, file_, Value::eFile);
        auto optC = fileVal.file().read();
        if (optC) {
            return Value(*optC);
        }
    }
    return Value::Null;
}

// -------------------------------------------------------------
FileReadLn::FileReadLn(CodeNode::SharedPtr file)
    : FileOp(file)
{}

Value FileReadLn::exec(Environment::SharedPtr env) const {
    if (file_) {
        auto fileVal = evalOperand(env, file_, Value::eFile);
        auto optS = fileVal.file().readln();
        if (optS) {
            return Value(*optS);
        }
    }
    return Value::Null;
}

// -------------------------------------------------------------
FileWrite::FileWrite(CodeNode::SharedPtr file, CodeNode::SharedPtr charOrStr)
    : FileOp(file)
    , charOrStr_(charOrStr)
{}

Value FileWrite::exec(Environment::SharedPtr env) const {
    if (file_ && charOrStr_) {
        const auto cOrSVal = evalOperand(env, charOrStr_, Value::eCharacter, Value::eString);
        auto fileVal = evalOperand(env, file_, Value::eFile);
        if (cOrSVal.isString()) {
            fileVal.file().write(cOrSVal.text());
        }
        else {
            fileVal.file().write(cOrSVal.character());
        }
        return Value::True;
    }
    return Value::Null;
}

// -------------------------------------------------------------
FileWriteLn::FileWriteLn(CodeNode::SharedPtr file, CodeNode::SharedPtr str)
    : FileOp(file)
    , str_(str)
{}

Value FileWriteLn::exec(Environment::SharedPtr env) const {
    if (file_ && str_) {
        const auto cOrSVal = evalOperand(env, str_, Value::eCharacter, Value::eString);
        auto fileVal = evalOperand(env, file_, Value::eFile);
        if (cOrSVal.isString()) {
            fileVal.file().writeln(cOrSVal.text());
        }
        else {
            fileVal.file().writeln(cOrSVal.character());
        }
        return Value::True;
    }
    return Value::Null;
}

// -------------------------------------------------------------
FileExists::FileExists(CodeNode::SharedPtr file)
    : FileOp(file)
{}

Value FileExists::exec(Environment::SharedPtr env) const {
    if (file_) {
        const auto pathVal = evalOperand(env, file_, Value::eString);
        return Value(fs::exists(pathVal.text()));
    }
    return Value::Null;
}

// -------------------------------------------------------------
FileRemove::FileRemove(CodeNode::SharedPtr file)
    : FileOp(file)
{}

Value FileRemove::exec(Environment::SharedPtr env) const {
    if (file_) {
        const auto pathVal = evalOperand(env, file_, Value::eString);
        return Value(fs::remove(pathVal.text()));
    }
    return Value::Null;
}

// -------------------------------------------------------------
WithFile::WithFile(const std::string &name, CodeNode::SharedPtr file, CodeNode::SharedPtr body)
    : FileOp(file)
    , name_(name)
    , body_(body)
{}

Value WithFile::exec(Environment::SharedPtr env) const {
    if (file_ && body_) {
        auto fileVal = evalOperand(env, file_, Value::eFile);

        try {
            auto withEnv = Environment::make(env);
            withEnv->def(name_, fileVal);

            auto result = body_->eval(withEnv);

            fileVal.file().close();
            return result;
        }
        catch (...) {
            fileVal.file().close();
            throw;
        }
    }
    return Value::Null;
}
