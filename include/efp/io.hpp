#ifndef IO_HPP_
#define IO_HPP_

#include "efp/prelude.hpp"
#include "efp/string.hpp"
#include "efp/format.hpp"

// If hosted environment
#if defined(__STDC_HOSTED__) && __STDC_HOSTED__ == 1
#include <cstdio>

namespace efp {

    class File {
    public:
        static Maybe<File> open(const char* path, const char* mode) {
            FILE* file = std::fopen(path, mode);

            if (file) {
                return efp::move(File{file, mode});
            }

            return nothing;
        }

        static Maybe<File> open(const String& path, const char* mode) {
            FILE* file = std::fopen(path.c_str(), mode);

            if (file) {
                return efp::move(File{file, mode});
            }

            return nothing;
        }

        File(const File&) = delete;

        File& operator=(const File&) = delete;

        File(File&& other) noexcept
            : _file(other._file) {
            other._file = nullptr;
        }

        File& operator=(File&& other) noexcept {
            if (this != &other) {
                _file = other._file;
                other._file = nullptr;
            }
            return *this;
        }

        ~File() {
            if (_file)
                std::fclose(_file);
        }

        Maybe<String> read_line() {
            if (_file) {
                String buffer{};
                int ch;

                while ((ch = std::fgetc(_file)) != '\n' && ch != EOF) {
                    buffer.push_back(static_cast<char>(ch));
                }

                // Check for EOF condition
                if (ch == EOF && buffer.empty())
                    return nothing;

                return buffer;
            }

            return nothing;
        }

        Vector<String> read_lines() {
            Vector<String> lines{};

            bool keep_read = true;

            while (keep_read) {
                const auto maybe_line = read_line();

                if (maybe_line) {
                    lines.push_back(maybe_line.value());
                } else {
                    keep_read = false;
                }
            }

            return lines;
        }

        bool write(const char* data, size_t length = dyn) {
            if (_file == nullptr)
                return false;

            if (std::strchr(_mode, 'b')) {
                // Write binary data
                if (length == dyn) {
                    length = std::strlen(data);
                }

                size_t written = std::fwrite(data, sizeof(char), length, _file);

                return written == length;
            } else {
                // Write text data
                if (length == dyn) {
                    length = std::strlen(data);
                }

                for (size_t i = 0; i < length; ++i) {
                    if (std::fputc(data[i], _file) == EOF)
                        return false;
                }

                return true;
            }
        }

        bool write(const String& data) {
            if (_file == nullptr)
                return false;

            if (std::strchr(_mode, 'b')) {
                // Write binary data
                size_t written = std::fwrite(data.data(), sizeof(char), data.size(), _file);
                return written == data.size();
            } else {
                return std::fputs(data.c_str(), _file) != EOF;
            }
        }

        int flush() {
            return std::fflush(_file);
        }

        bool seek(long offset) {
            return std::fseek(_file, offset, SEEK_SET) == 0;
        }

        int tell() {
            return std::ftell(_file);
        }

        bool close() {
            if (_file && std::fclose(_file) != EOF) {
                _file = nullptr;
                return true;
            }

            return false;
        }

    private:
        explicit File(FILE* file, const char* mode)
            : _file(file) {
            std::strncpy(_mode, mode, sizeof(_mode) - 1);
            _mode[sizeof(_mode) - 1] = '\0';
        }

        FILE* _file;
        char _mode[8];
    };

}; // namespace efp

#endif // __STDC_HOSTED__ == 1

#endif