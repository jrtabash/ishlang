#ifndef UTIL_H
#define	UTIL_H

#include <string>
#include <forward_list>
#include <istream>

namespace Int {

    class Util {
    public:
        static const double RealThreshold;

    public:
        typedef std::forward_list<std::string> TokenList;

    public:
        static bool isZero(double val);
        static bool isEqual(double lhs, double rhs);

    public:
        static std::string nextToken(std::istream &is);

        static std::string nextToken(const std::string &str, size_t &pos);
        static size_t tokenize(const std::string &str, TokenList &tokens);

    public:
        static bool setBoolFromString(bool &out, const std::string &str);
    };

}

#endif	// UTIL_H
