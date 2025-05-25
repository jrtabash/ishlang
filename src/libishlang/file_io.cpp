#include "file_io.h"

#include <cassert>

using namespace Ishlang;

// -------------------------------------------------------------
FileMode FileModeNS::fromChar(char c) {
    switch (c) {
        case 'r':
        case 'w':
        case 'a':
            return static_cast<FileMode>(c);

        default:
            break;
    }
    throw FileIOError(Exception::format("Invalid mode '%c'", c));
}

// -------------------------------------------------------------
std::ios_base::openmode FileModeNS::toOpenMode(FileMode mode) {
    switch (mode) {
        case FileMode::Read:   return std::ios_base::in;
        case FileMode::Write:  return std::ios_base::out;
        case FileMode::Append: return std::ios_base::app;
        default:               break;
    }
    assert(false);
    return std::ios_base::in;
}

// -------------------------------------------------------------
std::optional<char> FileStruct::read() {
    char c = '\0';
    if (fs_.get(c)) {
        return std::make_optional<char>(c);
    }
    return std::nullopt;
}

// -------------------------------------------------------------
std::optional<std::string> FileStruct::readln() {
    std::string line;
    if (std::getline(fs_, line)) {
        return std::make_optional(std::move(line));
    }
    return std::nullopt;
}

// -------------------------------------------------------------
void FileStruct::write(char c) {
    if (!fs_.put(c)) {
        throw FileIOError("Failed to write char");
    }
}

// -------------------------------------------------------------
void FileStruct::write(const std::string &s) {
    if (!fs_.write(s.c_str(), s.size())) {
        throw FileIOError("Failed to write string");
    }
}

// -------------------------------------------------------------
void FileStruct::writeln(char c) {
    if (!fs_.put(c) || !fs_.put('\n')) {
        throw FileIOError("Failed to write line");
    }
}

// -------------------------------------------------------------
void FileStruct::writeln(const std::string &line) {
    if (!fs_.write(line.c_str(), line.size()) || !fs_.put('\n')) {
        throw FileIOError("Failed to write line");
    }
}
