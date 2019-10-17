#include "byte_code.h"
#include "lambda.h"
#include "instance.h"
#include "sequence.h"
#include "parser.h"
#include "exception.h"
#include "util.h"

using namespace Int;

// -------------------------------------------------------------
IsType::IsType(ByteCode::SharedPtr expr, Value::Type type)
  : ByteCode()
  , expr_(expr)
  , type_(type)
{}

Value IsType::exec(Environment::SharedPtr env) {
    if (expr_.get()) {
        Value exprValue = expr_->exec(env);
        return exprValue.type() == type_ ? Value::True : Value::False;
    }
    return Value::False;
}

// -------------------------------------------------------------
ArithOp::ArithOp(Type type, ByteCode::SharedPtr lhs, ByteCode::SharedPtr rhs)
    : BinaryOp(lhs, rhs)
    , type_(type)
{}

Value ArithOp::exec(Environment::SharedPtr env) {
    if (lhs_ && rhs_) {
        const Value lhsVal = lhs_->exec(env);
        const Value rhsVal = rhs_->exec(env);
        
        if (!lhsVal.isNumber()) { throw InvalidOperandType("Number", lhsVal.typeToString()); }
        if (!rhsVal.isNumber()) { throw InvalidOperandType("Number", rhsVal.typeToString()); }

        const bool real = lhsVal.isReal() || rhsVal.isReal() ? true : false;
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
        }
    }
    return Value::Zero;
}

// -------------------------------------------------------------
CompOp::CompOp(Type type, ByteCode::SharedPtr lhs, ByteCode::SharedPtr rhs)
    : BinaryOp(lhs, rhs)
    , type_(type)
{}

Value CompOp::exec(Environment::SharedPtr env) {
    if (lhs_ && rhs_) {
        const Value lhsVal = lhs_->exec(env);
        const Value rhsVal = rhs_->exec(env);
        
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
LogicOp::LogicOp(Type type, ByteCode::SharedPtr lhs, ByteCode::SharedPtr rhs)
    : BinaryOp(lhs, rhs)
    , type_(type)
{}

Value LogicOp::exec(Environment::SharedPtr env) {
    if (lhs_ && rhs_) {
        const Value lhsVal = lhs_->exec(env);
        const Value rhsVal = rhs_->exec(env);
        
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
Not::Not(ByteCode::SharedPtr operand)
    : ByteCode()
    , operand_(operand)
{
}

Value Not::exec(Environment::SharedPtr env) {
    if (operand_.get()) {
        Value operand = operand_->exec(env);

        if (!operand.isBool()) { throw InvalidOperandType("Boolean", operand.typeToString()); }

        return !operand.boolean();
    }
    return Value::False;
}

// -------------------------------------------------------------
ProgN::ProgN(ByteCode::SharedPtrList exprs)
    : ByteCode()
    , exprs_(exprs)
{}

Value ProgN::exec(Environment::SharedPtr env) {
    if (!exprs_.empty()) {
        Value result;
        for (ByteCode::SharedPtrList::iterator iter = exprs_.begin(); iter != exprs_.end(); ++iter) {
            result = (*iter)->exec(env);
        }
        return result;
    }
    return Value::Null;
}

// -------------------------------------------------------------
Block::Block(ByteCode::SharedPtrList exprs)
    : ProgN(exprs)
{}

Value Block::exec(Environment::SharedPtr env) {
    Environment::SharedPtr blockEnv(std::make_shared<Environment>(env));
    return ProgN::exec(std::make_shared<Environment>(blockEnv));
}

// -------------------------------------------------------------
If::If(ByteCode::SharedPtr pred, ByteCode::SharedPtr tCode)
    : ByteCode()
    , pred_(pred)
    , tCode_(tCode)
    , fCode_()
{}

If::If(ByteCode::SharedPtr pred, ByteCode::SharedPtr tCode, ByteCode::SharedPtr fCode)
    : ByteCode()
    , pred_(pred)
    , tCode_(tCode)
    , fCode_(fCode)
{}

Value If::exec(Environment::SharedPtr env) {
    if (pred_) {
        Environment::SharedPtr ifEnv(std::make_shared<Environment>(env));

        const Value pVal = pred_->exec(ifEnv);
        if (!pVal.isBool()) { throw InvalidExpressionType("Boolean", pVal.typeToString()); }

        if (pVal.boolean()) {
            if (tCode_) { return tCode_->exec(ifEnv); }
        }
        else {
            if (fCode_) { return fCode_->exec(ifEnv); }
        }
    }
    return Value::Null;
}

// -------------------------------------------------------------
Cond::Cond(ByteCode::SharedPtrPairs cases)
    : ByteCode()
    , cases_(cases)
{}

Value Cond::exec(Environment::SharedPtr env) {
    const size_t numCases = cases_.size();
    if (numCases > 0) {
        for (ByteCode::SharedPtrPairs::iterator iter = cases_.begin(); iter != cases_.end(); ++iter) {
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
Loop::Loop(ByteCode::SharedPtr decl, ByteCode::SharedPtr cond, ByteCode::SharedPtr next, ByteCode::SharedPtr body)
    : ByteCode()
    , decl_(decl)
    , cond_(cond)
    , next_(next)
    , body_(body)
{}

Loop::Loop(ByteCode::SharedPtr cond, ByteCode::SharedPtr body)
    : ByteCode()
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
            if (decl_) { decl_->exec(loopEnv); }
            while (true) {
                condVal = cond_->exec(loopEnv);
                if (!condVal.isBool()) { throw InvalidExpressionType("Boolean", condVal.typeToString()); }
                if (!condVal.boolean()) { break; }

                result = body_->exec(loopEnv);
                if (next_) { next_->exec(loopEnv); }
            }
        }
        catch (const Break::Except &) { return Value::Null; }
        catch (...) { throw; }
        
        return result;
    }
    return Value::Null;
}

// -------------------------------------------------------------
LambdaExpr::LambdaExpr(const ParamList &params, ByteCode::SharedPtr body)
  : ByteCode()
  , params_(params)
  , body_(body)
{}

Value LambdaExpr::exec(Environment::SharedPtr env) {
    return Value(Lambda(params_, body_, env));
}

// -------------------------------------------------------------
LambdaApp::LambdaApp(ByteCode::SharedPtr closure, SharedPtrList args)
    : ByteCode()
    , closure_(closure)
    , argExprs_(args)
    , closureVar_(Value::Null)
{}

Value LambdaApp::exec(Environment::SharedPtr env) {
    if (!closureVar_.isClosure() && closure_.get()) {
        closureVar_ = closure_->exec(env);
    }

    if (!closureVar_.isClosure()) {
        throw InvalidExpressionType("Closure", closureVar_.typeToString());
    }

    Environment::SharedPtr appEnv(std::make_shared<Environment>(env));
    Lambda::ArgList args;
    for (SharedPtrList::iterator iter = argExprs_.begin(); iter != argExprs_.end(); ++iter) {
        args.push_back((*iter)->exec(appEnv));
    }
    
    return closureVar_.closure().exec(args);
}

// -------------------------------------------------------------
FunctionExpr::FunctionExpr(const std::string &name, const ParamList &params, ByteCode::SharedPtr body)
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
    : LambdaApp(ByteCode::SharedPtr(), args)
    , name_(name)
{}

Value FunctionApp::exec(Environment::SharedPtr env) {
    closureVar_ = env->get(name_);
    return LambdaApp::exec(env);
}

// -------------------------------------------------------------
Print::Print(bool newline, ByteCode::SharedPtr expr)
    : ByteCode()
    , newline_(newline)
    , expr_(expr)
{}

Value Print::exec(Environment::SharedPtr env) {
    Value::print(expr_->exec(env));
    if (newline_) { std::cout << std::endl; }
    return Value::Null;
}

// -------------------------------------------------------------
Read::Read()
    : ByteCode()
{
}

Value Read::exec(Environment::SharedPtr env)
{
    std::string input;
    std::getline(std::cin, input);
    return Parser().readLiteral(input)->exec(env);
}

// -------------------------------------------------------------
StructExpr::StructExpr(const std::string &name, const Struct::MemberList &members)
    : ByteCode()
    , struct_(name, members)
{}

Value StructExpr::exec(Environment::SharedPtr env) {
    Value structValue(struct_);
    return env->def(struct_.name(), structValue);
}

// -------------------------------------------------------------
IsStructName::IsStructName(ByteCode::SharedPtr expr, const std::string &name)
    : ByteCode()
    , expr_(expr)
    , name_(name)
{}

Value IsStructName::exec(Environment::SharedPtr env) {
    if (expr_.get()) {
        Value value = expr_->exec(env);
        if (value.isUserType() && value.userType().name() == name_) {
            return Value::True;
        }
    }
    return Value::False;
}

// -------------------------------------------------------------
MakeInstance::MakeInstance(const std::string &name)
    : ByteCode()
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
IsInstanceOf::IsInstanceOf(ByteCode::SharedPtr expr, const std::string &name)
    : ByteCode()
    , expr_(expr)
    , name_(name)
{}

Value IsInstanceOf::exec(Environment::SharedPtr env) {
    if (expr_.get()) {
        Value value = expr_->exec(env);
        if (value.isUserObject() && value.userObject().type().name() == name_) {
            return Value::True;
        }
    }
    return Value::False;
}

// -------------------------------------------------------------
GetMember::GetMember(ByteCode::SharedPtr expr, const std::string &name)
    : ByteCode()
    , expr_(expr)
    , name_(name)
{}

Value GetMember::exec(Environment::SharedPtr env) {
    if (expr_.get()) {
        Value value = expr_->exec(env);
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
SetMember::SetMember(ByteCode::SharedPtr expr, const std::string &name, ByteCode::SharedPtr newValExpr)
    : ByteCode()
    , expr_(expr)
    , name_(name)
    , newValExpr_(newValExpr)
{}

Value SetMember::exec(Environment::SharedPtr env) {
    if (expr_.get()) {
        Value instanceValue = expr_->exec(env);
        if (instanceValue.isUserObject()) {
            if (newValExpr_.get()) {
                Value newValue = newValExpr_->exec(env);
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
StringLen::StringLen(ByteCode::SharedPtr expr)
    : ByteCode()
    , expr_(expr)
{}

Value StringLen::exec(Environment::SharedPtr env) {
    if (expr_.get()) {
        Value str = expr_->exec(env);

        if (!str.isString()) { throw InvalidOperandType("String", str.typeToString()); }

        return Value(Value::Long(str.text().size()));
    }
    return Value::Zero;
}

// -------------------------------------------------------------
GetCharAt::GetCharAt(ByteCode::SharedPtr str, ByteCode::SharedPtr pos)
    : ByteCode()
    , str_(str)
    , pos_(pos)
{}

Value GetCharAt::exec(Environment::SharedPtr env) {
    if (str_.get() && pos_.get()) {
        Value str = str_->exec(env);
        Value pos = pos_->exec(env);

        if (!str.isString()) { throw InvalidOperandType("String", str.typeToString()); }
        if (!pos.isInt()) { throw InvalidOperandType("Integer", pos.typeToString()); }

        const auto &rawStr = str.text();
        const auto rawPos = pos.integer();
        if (rawPos < 0 || rawPos >= rawStr.size()) {
            throw OutOfRange("string getchar access");
        }

        return Value(rawStr[rawPos]);
    }
    return Value::Null;
}

// -------------------------------------------------------------
SetCharAt::SetCharAt(ByteCode::SharedPtr str, ByteCode::SharedPtr pos, ByteCode::SharedPtr val)
    : ByteCode()
    , str_(str)
    , pos_(pos)
    , val_(val)
{}

Value SetCharAt::exec(Environment::SharedPtr env) {
    if (str_.get() && pos_.get() && val_.get()) {
        Value str = str_->exec(env);
        Value pos = pos_->exec(env);
        Value val = val_->exec(env);

        if (!str.isString()) { throw InvalidOperandType("String", str.typeToString()); }
        if (!pos.isInt()) { throw InvalidOperandType("Integer", pos.typeToString()); }
        if (!val.isChar()) { throw InvalidOperandType("Character", val.typeToString()); }

        auto &rawStr = str.text();
        const auto rawPos = pos.integer();
        if (rawPos < 0 || rawPos >= rawStr.size()) {
            throw OutOfRange("string setchar access");
        }

        rawStr[rawPos] = val.character();
        return val;
    }
    return Value::Null;
}

// -------------------------------------------------------------
StringCat::StringCat(ByteCode::SharedPtr str, ByteCode::SharedPtr other)
    : ByteCode()
    , str_(str)
    , other_(other)
{}

Value StringCat::exec(Environment::SharedPtr env) {
    if (str_.get() && other_.get()) {
        Value str = str_->exec(env);
        Value other = other_->exec(env);

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
SubString::SubString(ByteCode::SharedPtr str, ByteCode::SharedPtr pos)
    : ByteCode()
    , str_(str)
    , pos_(pos)
    , len_()
{
}

SubString::SubString(ByteCode::SharedPtr str, ByteCode::SharedPtr pos, ByteCode::SharedPtr len)
    : ByteCode()
    , str_(str)
    , pos_(pos)
    , len_(len)
{
}

Value SubString::exec(Environment::SharedPtr env) {
    if (str_.get() && pos_.get()) {
        Value str = str_->exec(env);
        Value pos = pos_->exec(env);
        Value len = len_ ? len_->exec(env) : Value::Zero;

        if (!str.isString()) { throw InvalidOperandType("String", str.typeToString()); }
        if (!pos.isInt()) { throw InvalidOperandType("Integer", pos.typeToString()); }
        if (!len.isInt()) { throw InvalidOperandType("Integer", len.typeToString()); }

        const auto &rawStr = str.text();

        const auto rawPos = pos.integer();
        if (rawPos < 0 || rawPos >= rawStr.size()) {
            throw OutOfRange("substring position access");
        }

        const auto rawLen = (len_ ? len.integer() : Value::Long(rawStr.size()));
        if (rawLen < 0 || rawLen > rawStr.size()) {
            throw OutOfRange("substring length access");
        }

        return Value(rawStr.substr(rawPos, rawLen));
    }
    return Value::Null;
}

// -------------------------------------------------------------
StringFind::StringFind(ByteCode::SharedPtr str, ByteCode::SharedPtr chr)
    : ByteCode()
    , str_(str)
    , chr_(chr)
    , pos_()
{
}

StringFind::StringFind(ByteCode::SharedPtr str, ByteCode::SharedPtr chr, ByteCode::SharedPtr pos)
    : ByteCode()
    , str_(str)
    , chr_(chr)
    , pos_(pos)
{
}

Value StringFind::exec(Environment::SharedPtr env) {
    if (str_.get() && chr_.get()) {
        Value str = str_->exec(env);
        Value chr = chr_->exec(env);
        Value pos = pos_ ? pos_->exec(env) : Value::Zero;

        if (!str.isString()) { throw InvalidOperandType("String", str.typeToString()); }
        if (!chr.isChar()) { throw InvalidOperandType("Character", chr.typeToString()); }
        if (!pos.isInt()) { throw InvalidOperandType("Integer", pos.typeToString()); }

        const auto &rawStr = str.text();

        const auto rawPos = pos.integer();
        if (rawPos < 0 || rawPos >= rawStr.size()) {
            throw OutOfRange("strfind position access");
        }

        auto result = rawStr.find(chr.character(), rawPos);
        return result != std::string::npos ? Value(Value::Long(result)) : Value(-1ll);
    }
    return Value::Null;
}

// -------------------------------------------------------------
MakeArray::MakeArray()
    : ByteCode()
    , values_()
{}

MakeArray::MakeArray(ByteCode::SharedPtrList values)
    : ByteCode()
    , values_(values)
{}

Value MakeArray::exec(Environment::SharedPtr env) {
    if (values_.empty()) {
        return Value(Sequence());
    }
    else {
        std::vector<Value> values;
        for (auto & valueCode : values_) {
            values.push_back(valueCode->exec(env));
        }
        return Value(Sequence(std::move(values)));
    }
}

// -------------------------------------------------------------
MakeArraySV::MakeArraySV(ByteCode::SharedPtr size, ByteCode::SharedPtr initValue)
    : ByteCode()
    , size_(size)
    , initValue_(initValue)
{}

Value MakeArraySV::exec(Environment::SharedPtr env) {
    if (size_.get()) {
        Value size = size_->exec(env);
        Value initValue = initValue_.get() ? initValue_->exec(env) : Value::Null;

        if (!size.isInt()) { throw InvalidOperandType("Integer", size.typeToString()); }

        return Value(Sequence(size.integer(), initValue));
    }
}

// -------------------------------------------------------------
ArrayLen::ArrayLen(ByteCode::SharedPtr expr)
    : ByteCode()
    , expr_(expr)
{}

Value ArrayLen::exec(Environment::SharedPtr env) {
    if (expr_.get()) {
        Value arr = expr_->exec(env);

        if (!arr.isArray()) { throw InvalidOperandType("Array", arr.typeToString()); }

        return Value(Value::Long(arr.array().size()));
    }
    return Value::Zero;
}

// -------------------------------------------------------------
ArrayGet::ArrayGet(ByteCode::SharedPtr arr, ByteCode::SharedPtr pos)
    : ByteCode()
    , arr_(arr)
    , pos_(pos)
{}

Value ArrayGet::exec(Environment::SharedPtr env) {
    if (arr_.get() && pos_.get()) {
        Value arr = arr_->exec(env);
        Value pos = pos_->exec(env);

        if (!arr.isArray()) { throw InvalidOperandType("Array", arr.typeToString()); }
        if (!pos.isInt()) { throw InvalidOperandType("Integer", pos.typeToString()); }

        const auto &rawArr = arr.array();
        const auto rawPos = pos.integer();
        if (rawPos < 0 || rawPos >= rawArr.size()) {
            throw OutOfRange("array get access");
        }

        return Value(rawArr.get(rawPos));
    }
    return Value::Null;
}

// -------------------------------------------------------------
ArraySet::ArraySet(ByteCode::SharedPtr arr, ByteCode::SharedPtr pos, ByteCode::SharedPtr val)
    : ByteCode()
    , arr_(arr)
    , pos_(pos)
    , val_(val)
{}

Value ArraySet::exec(Environment::SharedPtr env) {
    if (arr_.get() && pos_.get() && val_.get()) {
        Value arr = arr_->exec(env);
        Value pos = pos_->exec(env);
        Value val = val_->exec(env);

        if (!arr.isArray()) { throw InvalidOperandType("Array", arr.typeToString()); }
        if (!pos.isInt()) { throw InvalidOperandType("Integer", pos.typeToString()); }

        auto &rawArr = arr.array();
        const auto rawPos = pos.integer();
        if (rawPos < 0 || rawPos >= rawArr.size()) {
            throw OutOfRange("array set access");
        }

        rawArr.set(rawPos, val);
        return val;
    }
    return Value::Null;
}

// -------------------------------------------------------------
ArrayAdd::ArrayAdd(ByteCode::SharedPtr arr, ByteCode::SharedPtr val)
    : ByteCode()
    , arr_(arr)
    , val_(val)
{}

Value ArrayAdd::exec(Environment::SharedPtr env) {
    if (arr_.get() && val_.get()) {
        Value arr = arr_->exec(env);
        Value val = val_->exec(env);

        if (!arr.isArray()) { throw InvalidOperandType("Array", arr.typeToString()); }

        arr.array().add(val);
        return arr;
    }
    return Value::Null;
}
