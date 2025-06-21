#include "unit_test_function.h"

#include "lexer.h"

using namespace Ishlang;

// -------------------------------------------------------------
DEFINE_TEST(testTokenType) {
    TEST_CASE(Lexer::tokenType("") == Lexer::Unknown);
    TEST_CASE(Lexer::tokenType("'") == Lexer::Unknown);
    TEST_CASE(Lexer::tokenType("''") == Lexer::Unknown);
    TEST_CASE(Lexer::tokenType("\"") == Lexer::Unknown);
    TEST_CASE(Lexer::tokenType("_") == Lexer::Unknown);
    TEST_CASE(Lexer::tokenType(".") == Lexer::Unknown);
    TEST_CASE(Lexer::tokenType("9ABC") == Lexer::Unknown);
    TEST_CASE(Lexer::tokenType("ABC-2") == Lexer::Unknown);
    TEST_CASE(Lexer::tokenType("#ABC") == Lexer::Unknown);
    TEST_CASE(Lexer::tokenType("ABC/3") == Lexer::Unknown);
    TEST_CASE(Lexer::tokenType("A@BC3") == Lexer::Unknown);

    TEST_CASE(Lexer::tokenType("-") == Lexer::Symbol);
    TEST_CASE(Lexer::tokenType("+") == Lexer::Symbol);
    TEST_CASE(Lexer::tokenType("*") == Lexer::Symbol);
    TEST_CASE(Lexer::tokenType("/") == Lexer::Symbol);
    TEST_CASE(Lexer::tokenType("%") == Lexer::Symbol);
    TEST_CASE(Lexer::tokenType("^") == Lexer::Symbol);
    TEST_CASE(Lexer::tokenType("=") == Lexer::Symbol);
    TEST_CASE(Lexer::tokenType("<") == Lexer::Symbol);
    TEST_CASE(Lexer::tokenType(">") == Lexer::Symbol);
    TEST_CASE(Lexer::tokenType("?") == Lexer::Symbol);
    TEST_CASE(Lexer::tokenType("==") == Lexer::Symbol);
    TEST_CASE(Lexer::tokenType("!=") == Lexer::Symbol);
    TEST_CASE(Lexer::tokenType("<=") == Lexer::Symbol);
    TEST_CASE(Lexer::tokenType(">=") == Lexer::Symbol);
    TEST_CASE(Lexer::tokenType("+=") == Lexer::Symbol);
    TEST_CASE(Lexer::tokenType("-=") == Lexer::Symbol);
    TEST_CASE(Lexer::tokenType("*=") == Lexer::Symbol);
    TEST_CASE(Lexer::tokenType("/=") == Lexer::Symbol);
    TEST_CASE(Lexer::tokenType("%=") == Lexer::Symbol);
    TEST_CASE(Lexer::tokenType("^=") == Lexer::Symbol);

    TEST_CASE(Lexer::tokenType("(") == Lexer::LeftP);
    TEST_CASE(Lexer::tokenType(")") == Lexer::RightP);
    TEST_CASE(Lexer::tokenType("'a'") == Lexer::Char);
    TEST_CASE(Lexer::tokenType("\"\"") == Lexer::String);
    TEST_CASE(Lexer::tokenType("\"a\"") == Lexer::String);
    TEST_CASE(Lexer::tokenType("\"text\"") == Lexer::String);
    TEST_CASE(Lexer::tokenType("123") == Lexer::Int);
    TEST_CASE(Lexer::tokenType("+123") == Lexer::Int);
    TEST_CASE(Lexer::tokenType("-123") == Lexer::Int);
    TEST_CASE(Lexer::tokenType("123.45") == Lexer::Real);
    TEST_CASE(Lexer::tokenType("+123.45") == Lexer::Real);
    TEST_CASE(Lexer::tokenType("-123.45") == Lexer::Real);
    TEST_CASE(Lexer::tokenType("123.") == Lexer::Real);
    TEST_CASE(Lexer::tokenType(".45") == Lexer::Real);
    TEST_CASE(Lexer::tokenType("true") == Lexer::Bool);
    TEST_CASE(Lexer::tokenType("false") == Lexer::Bool);
    TEST_CASE(Lexer::tokenType("null") == Lexer::Null);
    TEST_CASE(Lexer::tokenType("ABC") == Lexer::Symbol);
    TEST_CASE(Lexer::tokenType("ABC9") == Lexer::Symbol);
    TEST_CASE(Lexer::tokenType("ABC09sds") == Lexer::Symbol);
    TEST_CASE(Lexer::tokenType("ABC_3") == Lexer::Symbol);
}
