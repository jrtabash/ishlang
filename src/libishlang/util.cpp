#include "util.h"
#include "exception.h"

#include <cctype>
#include <cmath>
#include <fstream>

namespace fs = std::filesystem;

using namespace Ishlang;

const double Util::RealThreshold = 0.0000000001;

// -------------------------------------------------------------
bool Util::isZero(double val) { return std::fabs(val) <= RealThreshold; }

// -------------------------------------------------------------
bool Util::isEqual(double lhs, double rhs) { return std::fabs(lhs - rhs) <= RealThreshold; }

// -------------------------------------------------------------
std::string Util::nextToken(const std::string &str, size_t &pos) {
    std::string token;
    for (; pos < str.size(); ++pos) {
        if (isspace(str[pos])) {
            if (!token.empty()) {
                ++pos;
                break;
            }
            else { continue; }
        }
        else if (str[pos] == ';') {
            if (token.empty()) {
                if (++pos >= str.size() || str[pos] != ';') {
                    throw InvalidExpression("Incomplete comment");
                }
                while (str[pos] != '\n' && pos < str.size()) {
                    ++pos;
                }
                continue;
            }
            break;
        }
        else if (str[pos] == '\'') {
            if (token.empty()) {
                token += str[pos++];
                if (pos >= str.size()) {
                    throw InvalidExpression("character literal expected");
                }
                token += str[pos++];
                if (pos >= str.size() || str[pos] != '\'') {
                    throw InvalidExpression("character closing tick expected");
                }
                token += str[pos++];
            }
            break;
        }
        else if (str[pos] == '"') {
            if (token.empty()) {
                token += str[pos++];
                if (pos >= str.size()) {
                    throw InvalidExpression("string character expected");
                }
                while (str[pos] != '"') {
                    token += str[pos++];
                    if (pos >= str.size()) {
                        throw InvalidExpression("incomplete string");
                    }
                }
                if (str[pos] != '"') {
                    throw InvalidExpression("string closing double quotes expected");
                }
                token += str[pos++];
            }
            break;
        }
        else if (str[pos] == '(' || str[pos] == ')') {
            if (token.empty()) {
                token += str[pos];
                ++pos;
            }
            break;
        }
        else {
            token += str[pos];
        }
    }
    if (pos >= str.size()) { pos = std::string::npos; }
    return token;
}

// -------------------------------------------------------------
size_t Util::tokenize(const std::string &str, TokenList &tokens) {
    tokens.clear();
    TokenList::iterator iter = tokens.before_begin();
    std::string token;
    size_t size = 0;
    size_t pos = 0;
    while (pos != std::string::npos) {
        token = nextToken(str, pos);
        if (!token.empty()) {
            iter = tokens.insert_after(iter, std::move(token));
            ++size;
        }
    }
    return size;
}

// -------------------------------------------------------------
Util::StringVector Util::split(const std::string &str, char delimiter) {
    StringVector vec;
    if (!str.empty()) {
        std::string token;
        size_t pos = 0;
        size_t start = 0;
        do {
            pos = str.find(delimiter, start);
            token = str.substr(start, pos - start);
            vec.emplace_back(std::move(token));
            start = pos + 1;
        } while (pos != std::string::npos);
    }
    return vec;
}

// -------------------------------------------------------------
bool Util::setBoolFromString(bool &out, const std::string &str) {
    if (str == "true") {
        out = true;
    }
    else if (str == "false") {
        out = false;
    }
    else {
        return false;
    }
    return true;
}

// -------------------------------------------------------------
Util::TemporaryFile::TemporaryFile(const std::string &basename, const std::string &stuffToWrite)
    : tempFile_(temporaryPath() / basename)
{
    std::ofstream ofs(tempFile_.c_str());
    if (ofs.is_open()) {
        ofs << stuffToWrite;
        ofs.close();
    }
}

Util::TemporaryFile::~TemporaryFile() {
    fs::remove(tempFile_);
}

// -------------------------------------------------------------
std::optional<fs::path> Util::findFilePath(const fs::path &directory, const std::string &filename) {
    for (auto const & p : fs::directory_iterator(directory)) {
        if (p.path().filename() == filename) {
            return p.path();
        }
    }
    return std::nullopt;
}
