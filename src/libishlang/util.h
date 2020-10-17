#ifndef UTIL_H
#define UTIL_H

#include <filesystem>
#include <forward_list>
#include <optional>
#include <string>

namespace fs = std::filesystem;

namespace Ishlang {

    class Util {
    public:
        static const double RealThreshold;

    public:
        using TokenList = std::forward_list<std::string>;

    public:
        static bool isZero(double val);
        static bool isEqual(double lhs, double rhs);

    public:
        static std::string nextToken(const std::string &str, size_t &pos);
        static size_t tokenize(const std::string &str, TokenList &tokens);

    public:
        static bool setBoolFromString(bool &out, const std::string &str);

    public: // Filesystem
        struct TemporaryFile {
            TemporaryFile(const std::string &basename, const std::string &stuffToWrite = "");
            ~TemporaryFile();

            TemporaryFile(const TemporaryFile &) = delete;
            const TemporaryFile &operator=(const TemporaryFile &) = delete;

            const fs::path &path() const { return tempFile_; }

        private:
            fs::path tempFile_;
        };

        static bool pathExists(const std::string &path) { return fs::exists(path); }
        static bool isDirectory(const std::string &path) { return fs::is_directory(path); }
        static fs::path currentPath() { return fs::current_path(); }
        static fs::path temporaryPath() { return fs::temp_directory_path(); }

        static std::optional<fs::path> findFilePath(const fs::path &directory, const std::string &filename);
    };

}

#endif	// UTIL_H
