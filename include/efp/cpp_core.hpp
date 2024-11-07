#ifndef CPP_CORE_HPP
#define CPP_CORE_HPP

// C++ 11 Freestanding headers specified by N3242=11-0012

#include <cstddef>          // Defines int, ptrdiff_t, and nullptr_t
#include <limits>           // Defines numeric limits
#include <cstdlib>          // Defines general utilities: memory management, program utilities, string conversions, random numbers, and more.
#include <new>              // Defines operator new and operator delete, which can be overridden for custom memory management.
#include <typeinfo>         // Defines std::type_info, which is returned by typeid.
#include <exception>        // Defines std::exception, the base class for all exceptions.
#include <initializer_list> // Defines std::initializer_list, which is used to create a list of elements of the same type.
#include <cstdarg>          // Defines a variable argument list and macros for accessing it.
#include <type_traits>      // Defines compile-time type information.
#include <atomic>           // Defines atomic operations on data shared between threads.
#include <thread>           // Defines classes and functions for managing threads.

#if defined(__STDC_HOSTED__) && __STDC_HOSTED__ == 1

#include <cstring>

namespace efp {
    // Use a function alias for clarity and avoid direct assignment to avoid potential confusion.
    inline void* _memcpy(void* dest, const void* src, size_t size) {
        return std::memcpy(dest, src, size);
    }
} // namespace efp

#else

namespace efp {
    // Custom memcpy implementation for freestanding environments.
    // Added extern "C" to ensure C linkage for compatibility with C libraries or code.
    extern "C" void* _memcpy(void* dest, const void* src, size_t size) {
        auto* d = static_cast<char*>(dest);
        const auto* s = static_cast<const char*>(src);
        for (size_t i = 0; i < size; ++i) {
            d[i] = s[i];
        }
        return dest;
    }
} // namespace efp

#endif

// stdexcept

#if defined(__STDC_HOSTED__) && __STDC_HOSTED__ == 1

#include <stdexcept>

namespace efp {
    using LogicError = std::logic_error;
    using RuntimeError = std::runtime_error;
} // namespace efp

#else

namespace efp {

    class LogicError : public std::exception {
    public:
        explicit LogicError(const char* what_arg)
            : _what_arg(what_arg) {}

        LogicError(const LogicError& other) noexcept
            : std::exception(other), _what_arg(other._what_arg) {}

        const char* what() const noexcept override {
            return _what_arg;
        }

    protected:
        const char* _what_arg;
    };

    class RuntimeError : public std::exception {
    public:
        explicit RuntimeError(const char* what_arg)
            : _what_arg(what_arg) {}

        RuntimeError(const RuntimeError& other) noexcept
            : std::exception(other), _what_arg(other._what_arg) {}

        const char* what() const noexcept override {
            return _what_arg;
        }

    protected:
        const char* _what_arg;
    };

} // namespace efp

#endif

#endif