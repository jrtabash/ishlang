#include "exception.h"
#include <cstdio>
#include <iostream>

using namespace Int;

// -------------------------------------------------------------
FileContext::FileContext(const std::string filename, unsigned lineNo)
    : filename_(filename)
    , lineNo_(lineNo)
{}

// -------------------------------------------------------------
const std::string &FileContext::filename() const noexcept {
    return filename_;
}

// -------------------------------------------------------------
unsigned FileContext::lineNo() const noexcept {
    return lineNo_;
}

// -------------------------------------------------------------
Exception::Exception(const std::string &msg)
    : std::runtime_error(msg)
{}

// -------------------------------------------------------------
Exception::Exception(const std::string &msg, const std::string &variable)
    : std::runtime_error(msg + " - " + variable)
{}

// -------------------------------------------------------------
const std::optional<FileContext> &Exception::fileContext() const noexcept {
    return fileContext_;
}

// -------------------------------------------------------------
void Exception::setFileContext(const std::string &filename, unsigned lineNo) {
    fileContext_.emplace(filename, lineNo);
}

// -------------------------------------------------------------
void Exception::printError() const {
    if (fileContext_) {
        std::cerr << "***** " << fileContext_->filename() << ':' << fileContext_->lineNo() << '\n';
    }
    std::cerr << "Error: " << what() << std::endl;
}

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
