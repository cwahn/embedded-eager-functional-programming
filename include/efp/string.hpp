#ifndef STRING_HPP_
#define STRING_HPP_

#if defined(__STDC_HOSTED__) && __STDC_HOSTED__ == 1
#include <cstring>
#include <string>
#endif

#include "efp/prelude.hpp"
#include "efp/sequence.hpp"

namespace efp {

    // Forward declarations for String and StringView
    template <typename Char, typename = EnableIf<detail::IsCharType<Char>::value>>
    using BasicStringView = VectorView<Char>;

    // BasicString

    template <typename Char, typename Traits, typename Allocator>
    class Vector<Char, Allocator, Traits, EnableIf<detail::IsCharType<Char>::value>> : public detail::VectorBase<Char, Allocator> {
    public:
        using Base = detail::VectorBase<Char>;
        using Base::Base;

        using traits_type = Traits;
        static const size_t npos = -1;

        Vector(const Char* c_str) {
            Base::_size = Traits::length(c_str);
            Base::_capacity = Base::_size + 1;
            // Base::_data = Base::_allocator.allocate(Base::_capacity);
            Base::_data = AllocatorTraits<Allocator>::allocate(Base::_allocator, Base::_capacity);
            _memcpy(Base::_data, c_str, Base::_size * sizeof(Char));
        }

        Vector(const Char* s, size_t count, const Allocator& alloc = Allocator()) {
            Base::_size = count;
            Base::_capacity = Base::_size + 1;
            // Base::_data = Base::_allocator.allocate(Base::_capacity);
            Base::_data = AllocatorTraits<Allocator>::allocate(Base::_allocator, Base::_capacity);
            _memcpy(Base::_data, s, Base::_size * sizeof(Char));
        }

        Vector(size_t size, Char c) {
            Base::_capacity = size;
            Base::_data = static_cast<Char*>(::operator new[](Base::_capacity * sizeof(Char)));
            Base::_size = size;
            for (size_t i = 0; i < size; ++i) {
                Base::_data[i] = c;
            }
        }

        // template<class InputIt>
        // Vector(InputIt first, InputIt last, const Allocator& alloc = Allocator())
        //     : allocator_(alloc), size_(std::distance(first, last)), capacity_(size_) {
        //     data_ = allocator_.allocate(capacity_);
        //     std::uninitialized_copy(first, last, data_);
        // }

        // Not using iterator
        template <typename InputIt>
        Vector(InputIt first, InputIt last, const Allocator& alloc = Allocator())
            : Base::allocator_(alloc), Base::size_(0), Base::capacity_(0) {
            // First pass: Count the number of elements to determine size
            for (InputIt it = first; it != last; ++it) {
                ++Base::size_;
            }
            Base::capacity_ = Base::size_ + 1;

            // Allocate memory for the elements
            // Base::data_ = Base::allocator_.allocate(Base::capacity_);
            Base::data_ = AllocatorTraits<Allocator>::allocate(Base::allocator_, Base::capacity_);

            // Second pass: Copy-construct elements from the range
            size_t i = 0;
            for (InputIt it = first; it != last; ++it, ++i) {
                Base::allocator_.construct(Base::data_ + i, *it);
            }
        }

        bool operator==(const Vector& other) const {
            return Base::operator==(other);
        }

        // Specialized equality comparison operator with const char *
        bool operator==(const Char* c_str) const {
            if (Base::_data == nullptr && c_str == nullptr) {
                return true;
            }

            if (Base::_data == nullptr || c_str == nullptr) {
                return false;
            }

            size_t c_str_len = Traits::length(c_str);
            if (Base::_size != c_str_len) {
                return false;
            }

            return Traits::compare(Base::_data, c_str, Base::_size) == 0;
        }

        Char at(size_t pos) const {
            if (pos >= Base::_size) {
                throw RuntimeError("Index out of range");
            }

            return Base::_data[pos];
        }

        Char front() const {
            return Base::_data[0];
        }

        Char back() const {
            return Base::_data[Base::_size - 1];
        }

        Vector& operator+=(const Vector& other) {
            for (size_t i = 0; i < other.size(); ++i) {
                Base::push_back(other[i]);
            }

            return *this;
        }

        Vector& append(const Char* c_str) {
            const size_t len = Traits::length(c_str);

            for (size_t i = 0; i < len; ++i) {
                Base::push_back(c_str[i]);
            }

            return *this;
        }

        Vector& append(size_t n, Char c) {
            for (size_t i = 0; i < n; ++i) {
                Base::push_back(c);
            }

            return *this;
        }

        Vector& assign(const Char* c_str) {
            Base::clear();
            return append(c_str);
        }

        Vector& assign(size_t n, Char c) {
            Base::clear();
            return append(n, c);
        }

        Vector& insert(size_t pos, const Char* c_str) {
            if (pos > Base::_size) {
                throw RuntimeError("Index out of range");
            }

            const size_t len = Traits::length(c_str);
            Base::reserve(Base::_size + len);

            for (size_t i = Base::_size; i > pos; --i) {
                Base::_data[i + len - 1] = Base::_data[i - 1];
            }

            for (size_t i = 0; i < len; ++i) {
                Base::_data[pos + i] = c_str[i];
            }

            Base::_size += len;

            return *this;
        }

        Vector& insert(size_t pos, const Char* c_str, size_t n) {
            if (pos > Base::_size) {
                throw RuntimeError("Index out of range");
            }

            // Always reserve one extra space for the null character
            Base::reserve(Base::_size + n + 1);

            for (size_t i = Base::_size; i > pos; --i) {
                Base::_data[i + n - 1] = Base::_data[i - 1];
            }

            for (size_t i = 0; i < n; ++i) {
                Base::_data[pos + i] = c_str[i];
            }

            Base::_size += n;

            return *this;
        }

        // todo replace(size_type pos, size_type len, const CharT* s)

        // todo replace(size_type pos, size_type len, const CharT* s, size_type n)

        Vector substr(size_t pos = 0, size_t len = npos) const {
            if (pos > Base::_size) {
                throw RuntimeError("Index out of range");
            }

            if (len > Base::_size - pos) {
                len = Base::_size - pos;
            }

            return Vector(Base::_data + pos, len);
        }

        // The resulting character string is not null-terminated.
        // https://en.cppreference.com/w/cpp/string/basic_string/copy
        size_t copy(Char* dest, size_t count, size_t pos = 0) const {
            if (pos > Base::_size) {
                throw RuntimeError("Index out of range");
            }

            if (count > Base::_size - pos) {
                count = Base::_size - pos;
            }

            _memcpy(dest, Base::_data + pos, count * sizeof(Char));
            return count;
        }

        // todo find, rfind, find_first_of, find_last_of, find_first_not_of, find_last_not_of

        int compare(const Vector& other) const {
            return Traits::compare(Base::_data, other.data(), Base::_size);
        }

        int compare(size_t pos, size_t len, const Vector& other) const {
            if (pos > Base::_size) {
                throw RuntimeError("Index out of range");
            }

            if (len > Base::_size - pos) {
                len = Base::_size - pos;
            }

            return Traits::compare(Base::_data + pos, other.data(), len);
        }

        const Char* c_str() const {
            Base::_data[Base::_size] = '\0';
            return Base::_data;
        }

        operator BasicStringView<Char, Traits>() const {
            return BasicStringView<Char, Traits>(Base::_data, Base::_size);
        }

#if defined(__STDC_HOSTED__) && __STDC_HOSTED__ == 1
        operator std::string() const {
            return std::string(Base::_data, Base::_size);
        }
#endif
    }; // class Vector

    template <
        typename Char,
        typename Traits = detail::DefaultCharTraits<Char>,
        typename Allocator = detail::DefaultAllocator<Char>,
        typename = EnableIf<detail::IsCharType<Char>::value>>
    using BasicString = Vector<Char, Allocator, Traits>;

    using String = BasicString<char>;

    using WString = BasicString<wchar_t>;

    using U16String = BasicString<char16_t>;

    using U32String = BasicString<char32_t>;

#if __cplusplus >= 202002L
    using U8String = BasicString<char8_t>;
#endif

    // BasicStringView specialization

    template <typename Char, typename Traits>
    class VectorView<Char, Traits, EnableIf<detail::IsCharType<Char>::value>> : public detail::VectorViewBase<Char> {
    public:
        using Base = detail::VectorViewBase<Char>;
        using Base::Base;

        VectorView(const Char* c_str) {
            Base::_size = Traits::length(c_str);
            Base::_data = c_str;
        }

        // todo basic_string_view(const efp::BasicString<Char>& s) noexcept
        VectorView(const Vector<Char>& s) noexcept {
            Base::_size = s.size();
            Base::_data = s.data();
        }

        bool operator==(const VectorView& other) const {
            return Base::operator==(other);
        }

        // Specialized equality comparison operator with const char *
        bool operator==(const Char* c_str) const {
            if (Base::_data == nullptr && c_str == nullptr) {
                return true;
            }

            if (Base::_data == nullptr || c_str == nullptr) {
                return false;
            }

            size_t c_str_len = Traits::length(c_str);
            if (Base::_size != c_str_len) {
                return false;
            }

            return Traits::compare(Base::_data, c_str, Base::_size) == 0;
        }

#if defined(__STDC_HOSTED__) && __STDC_HOSTED__ == 1
        operator std::string() const {
            return std::string(Base::_data, Base::_size);
        }
#endif

        const Char* c_str() const {
            return Base::_data;
        }
    };

    // BasicSstringView

    // Template is forwards declared at the top of the file

    using StringView = BasicStringView<char>;

    using WStringView = BasicStringView<wchar_t>;

    using U16StringView = BasicStringView<char16_t>;

    using U32StringView = BasicStringView<char32_t>;

#if __cplusplus >= 202002L
    using U8StringView = BasicStringView<char8_t>;
#endif

    // #if defined(__STDC_HOSTED__) && __STDC_HOSTED__ == 1

    // template<typename A>
    // auto operator<<(std::ostream& os, const A& seq) -> EnableIf<
    //     IsSequence<A>::value && detail::IsCharType<Element<A>>::value && !IsSame<A, std::string>::value,
    //     std::ostream&> {
    //     static_assert(IsSequence<A>(), "Argument should be an instance of Sequence trait.");

    //     for (const auto& elem : seq) {
    //         os << elem;
    //     }

    //     return os;
    // }

    // #endif

}; // namespace efp

#endif