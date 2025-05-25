#ifndef ISHLANG_FILE_IO_H
#define ISHLANG_FILE_IO_H

#include "exception.h"

#include <fstream>
#include <optional>
#include <string>

namespace Ishlang {

    enum class FileMode : char {
        Read   = 'r',
        Write  = 'w',
        Append = 'a'
    };

    namespace FileModeNS {
        inline char toChar(FileMode mode);
        FileMode fromChar(char c);

        std::ios_base::openmode toOpenMode(FileMode mode);
    }

    struct FileParams {
        std::string filename{};
        FileMode mode{FileMode::Read};
    };

    class FileStruct {
    public:
        inline FileStruct() = default;
        inline FileStruct(FileParams && params);
        inline FileStruct(const std::string &filename, FileMode mode);

        inline const FileParams &params() const noexcept;
        inline const std::string &filename() const noexcept;
        inline FileMode mode() const noexcept;

        inline bool isOpen() const noexcept;

        inline void close();
        inline void flush();

        std::optional<char> read();
        std::optional<std::string> readln();

        void write(char c);
        void write(const std::string &s);
        void writeln(char c);
        void writeln(const std::string &line);

        inline bool operator==(const FileStruct &rhs) const;
        inline bool operator!=(const FileStruct &rhs) const;

    public:
        FileStruct(const FileStruct &) = delete;
        FileStruct &operator=(const FileStruct &) = delete;

    private:
        FileParams params_{};
        std::fstream fs_{};
    };

    // --------------------------------------------------------------------------------
    // INLINE

    inline char FileModeNS::toChar(FileMode mode) {
        return static_cast<char>(mode);
    }

    inline FileStruct::FileStruct(FileParams && params)
        : params_(std::move(params))
        , fs_(params_.filename.c_str(), FileModeNS::toOpenMode(params_.mode))
    {
        if (!isOpen()) {
            throw FileIOError(params_.filename, "Failed to open file");
        }
    }

    inline FileStruct::FileStruct(const std::string &filename, FileMode mode)
        : FileStruct({.filename = filename, .mode = mode})
    {}

    inline const FileParams &FileStruct::params() const noexcept {
        return params_;
    }

    inline const std::string &FileStruct::filename() const noexcept {
        return params_.filename;
    }

    inline FileMode FileStruct::mode() const noexcept {
        return params_.mode;
    }

    inline bool FileStruct::isOpen() const noexcept {
        return fs_.is_open();
    }

    inline void FileStruct::close() {
        fs_.close();
    }

    inline void FileStruct::flush() {
        fs_.flush();
    }

    inline bool FileStruct::operator==(const FileStruct &rhs) const {
        return params_.filename == rhs.params_.filename && params_.mode == rhs.params_.mode;
    }

    inline bool FileStruct::operator!=(const FileStruct &rhs) const {
        return params_.filename != rhs.params_.filename || params_.mode != rhs.params_.mode;
    }

}

#endif // ISHLANG_FILE_IO_H
