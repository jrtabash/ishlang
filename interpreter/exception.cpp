#include "exception.h"
#include <cstdio>

using namespace Int;

// -------------------------------------------------------------
Exception::Exception(const std::string &msg)
    : std::runtime_error(msg)
{}

// -------------------------------------------------------------
Exception::Exception(const std::string &msg, const std::string &variable)
    : std::runtime_error(msg + " - " + variable)
{}

// -------------------------------------------------------------
std::string Exception::format(const char *fmt, ...) {
    char buf[256] = { 0 };
    va_list vaList;
    va_start(vaList, fmt);
    std::vsnprintf(buf, sizeof(buf), fmt, vaList);
    va_end(vaList);
    return buf;
}
