#include "exception.h"
#include <cstdio>
#include <iostream>

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
    auto const size = std::vsnprintf(buf, sizeof(buf), fmt, vaList);
    va_end(vaList);

    if (size >= sizeof(buf)) {
        std::cerr << "Exception format message truncated - size=" << size
                  << " bufsize=" << sizeof(buf)
                  << '\n';
    }

    return buf;
}
