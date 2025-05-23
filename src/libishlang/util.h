#ifndef ISHLANG_UTIL_H
#define ISHLANG_UTIL_H

#include <concepts>
#include <filesystem>
#include <forward_list>
#include <optional>
#include <ostream>
#include <string>
#include <ranges>
#include <vector>

namespace fs = std::filesystem;

template <typename C>
concept Container = requires (C c) {
    std::ranges::range<C>;
    c.size();
    c.empty();
};

template <typename P, typename C>
concept ItemPrinter = std::invocable<P, std::ostream &, const std::ranges::range_value_t<C> &>;

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

        static inline int sign(std::integral auto n);

    public:
        static std::string nextToken(const std::string &str, size_t &pos);
        static size_t tokenize(const std::string &str, TokenList &tokens);

    public:
        static StringVector split(const std::string &str, char delimiter);

    public:
        static bool setBoolFromString(bool &out, const std::string &str);

    public:
        template <Container ContainerType, ItemPrinter<ContainerType> PrinterType>
        static inline std::ostream &printContainer(std::ostream &out,
                                                   ContainerType const &container,
                                                   PrinterType const &itemPrinter,
                                                   char leftWrapper,
                                                   char rightWrapper,
                                                   std::size_t maxItems = 10);

    public:
        template <typename MappingType>
        static inline bool isEqualMapping(MappingType const & lhs, MappingType const & rhs);

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

    inline int Util::sign(std::integral auto n) {
        return n >= 0 ? 1 : -1;
    }

    template <Container ContainerType, ItemPrinter<ContainerType> PrinterType>
    inline std::ostream &Util::printContainer(std::ostream &out,
                                              ContainerType const &container,
                                              PrinterType const &itemPrinter,
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

    template <typename MappingType>
    inline bool Util::isEqualMapping(MappingType const & lhs, MappingType const & rhs) {
        if (lhs.size() != rhs.size()) {
            return false;
        }

        for (auto const & [key, val] : lhs) {
            auto riter = rhs.find(key);
            if (riter == rhs.end() || riter->second != val) {
                return false;
            }
        }

        return true;
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

#endif	// ISHLANG_UTIL_H
