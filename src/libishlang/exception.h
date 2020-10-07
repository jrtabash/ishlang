#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <cstdarg>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>

namespace Ishlang {

    class FileContext {
    public:
        FileContext(const std::string filename, unsigned lineNo);

        const std::string &filename() const noexcept;
        unsigned lineNo() const noexcept;

    private:
        const std::string filename_;
        const unsigned lineNo_;
    };

    class Exception : public std::runtime_error {
    public:
        Exception(const std::string &msg);
        Exception(const std::string &msg, const std::string &variable);

        const std::optional<FileContext> &fileContext() const noexcept;
        void setFileContext(const std::string &filename, unsigned lineNo);

        void printError() const;

    public:
        static std::string format(const char *fmt, ...);

    private:
        std::optional<FileContext> fileContext_ = std::nullopt;
    };
    
    class NullEnvironment : public Exception {
    public:
        NullEnvironment() : Exception("Null Environment") {}
    };
    
    class DuplicateDef : public Exception {
    public:
        DuplicateDef(const std::string &name) 
            : Exception(format("Duplicate variable/function definition - '%s'", name.c_str()))
        {}
    };
    
    class UnknownSymbol : public Exception {
    public:
        UnknownSymbol(const std::string &name) : Exception("Unknown symbol", name) {}
    };

    class UnknownTokenType : public Exception {
    public:
        UnknownTokenType(const std::string &token, char tokenType)
            : Exception(format("Unknown token type '%c' for token '%s'", static_cast<char>(tokenType), token.c_str()))
        {}
    };

    class UnexpectedTokenType : public Exception {
    public:
        UnexpectedTokenType(const std::string &token, char tokenType, const std::string &where="")
            : Exception(format("Unexpected token type '%c'%s for token '%s'", 
                               static_cast<char>(tokenType), 
                               (where.empty() ? "" : format(" in %s", where.c_str()).c_str()),
                               token.c_str()))
        {}
    };
    
    class InvalidOperandType : public Exception {
    public:
        InvalidOperandType(const std::string &expected, const std::string &actual)
            : Exception(format("Invalid operand type, expected=%s actual=%s", expected.c_str(), actual.c_str()))
        {}
    };

    class DivByZero : public Exception {
    public:
        DivByZero() : Exception("Division by zero") {}
    };

    class IncompatibleTypes : public Exception {
    public:
        IncompatibleTypes(const std::string &type1, const std::string &type2)
            : Exception(std::string(std::string("Incompatible types, type1=") + type1 + " type2=" + type2))
        {}
    };
    
    class InvalidExpressionType : public Exception {
    public:
        InvalidExpressionType(const std::string &expected, const std::string &actual)
            : Exception(std::string("Invalid expression type, expected=") + expected + " actual=" + actual)
        {}
    };

    class InvalidExpression : public Exception {
    public:
        InvalidExpression(const std::string &msg)
            : Exception(std::string("Invalid expression - ") + msg)
        {}

        InvalidExpression(const std::string &msg, const std::string &var)
            : Exception(format("Invalid expression - %s '%s'", msg.c_str(), var.c_str()))
        {}
    };

    class TooManyOrFewForms : public InvalidExpression {
    public:
        TooManyOrFewForms(const std::string &name)
            : InvalidExpression(format("Too many/few forms in %s", name.c_str()))
        {}
    };

    class UnexpectedExpression : public Exception {
    public:
        UnexpectedExpression(const std::string &expected, const std::string &actual)
            : Exception(format("Unexpected expression %s, expecting %s", actual.c_str(), expected.c_str()))
        {}
    };

    class InvalidArgsSize : public Exception {
    public:
        InvalidArgsSize(size_t paramsSize, size_t argsSize) 
            : Exception(format("Invalid arguments list - params size(%lu) is not equal to args size(%lu)", paramsSize, argsSize))
        {}
    };

    class ExpectedParenthesis : public Exception {
    public:
        ExpectedParenthesis(char paren) : Exception(format("Missing %c from expression", paren)) {}
    };

    class UnknownFile : public Exception {
    public:
        UnknownFile(const std::string &filename)
            : Exception(format("Unknown file '%s'", filename.c_str()))
        {}
    };

    class UnknownMember : public Exception {
    public:
        UnknownMember(const std::string &sName, const std::string &mName)
            : Exception(format("Unknown member '%s:%s'", sName.c_str(), mName.c_str()))
        {}
    };

    class InvalidCommand : public Exception {
    public:
        InvalidCommand(const std::string &cmd, const std::string &msg)
            : Exception(format("Invalid command '%s': %s", cmd.c_str(), msg.c_str()))
        {}
    };

    class IncompleteExpression : public Exception {
    public:
        IncompleteExpression(const std::string &msg)
            : Exception(format("Incomplete expression: %s", msg.c_str()))
        {}
    };

    class OutOfRange : public Exception {
    public:
        OutOfRange(const std::string &msg)
            : Exception(format("Out of range: %s", msg.c_str()))
        {}
    };

    class InvalidAsType : public Exception {
    public:
        InvalidAsType(const std::string &from, const std::string &to)
            : Exception(format("Invalid astype from '%s' to '%s'", from.c_str(), to.c_str()))
        {}
    };
}

#endif	// EXCEPTION_H
