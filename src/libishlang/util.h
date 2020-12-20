#ifndef UTIL_H
#define UTIL_H

#include <filesystem>
#include <forward_list>
#include <optional>
#include <ostream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace Ishlang {

    class Util {
    public:
        static const double RealThreshold;

    public:
        using TokenList = std::forward_list<std::string>;
        using StringVector = std::vector<std::string>;

    public:
        static bool isZero(double val);
        static bool isEqual(double lhs, double rhs);

    public:
        static std::string nextToken(const std::string &str, size_t &pos);
        static size_t tokenize(const std::string &str, TokenList &tokens);

    public:
        static StringVector split(const std::string &str, char delimiter);

    public:
        static bool setBoolFromString(bool &out, const std::string &str);

    public:
        template <typename Container, typename ItemPrinter>
        static inline std::ostream &printContainer(std::ostream &out,
                                                   const Container &container,
                                                   const ItemPrinter &itemPrinter,
                                                   char leftWrapper,
                                                   char rightWrapper,
                                                   std::size_t maxItems = 10);

    public: // Filesystem
        struct TemporaryFile {
            TemporaryFile(const std::string &basename, const std::string &stuffToWrite = "");
            ~TemporaryFile();

            TemporaryFile(const TemporaryFile &) = delete;
            const TemporaryFile &operator=(const TemporaryFile &) = delete;

            inline const fs::path &path() const;

        private:
            fs::path tempFile_;
        };

        static inline bool pathExists(const std::string &path);
        static inline bool isDirectory(const std::string &path);
        static inline fs::path currentPath();
        static inline fs::path temporaryPath();

        static std::optional<fs::path> findFilePath(const fs::path &directory, const std::string &filename);
    };

    // --------------------------------------------------------------------------------
    // INLINE

    template <typename Container, typename ItemPrinter>
    inline std::ostream &Util::printContainer(std::ostream &out,
                                              const Container &container,
                                              const ItemPrinter &itemPrinter,
                                              char leftWrapper,
                                              char rightWrapper,
                                              std::size_t maxItems) {
        out << leftWrapper;
        std::size_t count = 0;
        for (const auto &item : container) {
            if (count > 0) { out << ' '; }
            itemPrinter(out, item);
            if (++count >= maxItems && container.size() > maxItems) {
                out << " ...";
                break;
            }
        }
        out << rightWrapper;
        return out;
    }

    inline const fs::path &Util::TemporaryFile::path() const {
        return tempFile_;
    }

    inline bool Util::pathExists(const std::string &path) {
        return fs::exists(path);
    }

    inline bool Util::isDirectory(const std::string &path) {
        return fs::is_directory(path);
    }

    inline fs::path Util::currentPath() {
        return fs::current_path();
    }

    inline fs::path Util::temporaryPath() {
        return fs::temp_directory_path();
    }

}

#endif	// UTIL_H
