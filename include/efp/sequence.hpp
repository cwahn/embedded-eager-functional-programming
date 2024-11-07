#ifndef SEQUENCE_HPP_
#define SEQUENCE_HPP_

#include "efp/meta.hpp"
#include "efp/trait.hpp"

#if defined(__STDC_HOSTED__) && __STDC_HOSTED__ == 1
#include <string>
#include <memory>

#include <array>
#include <vector>

namespace efp {
    // Use std::allocator_traits if available
    template <typename A>
    using AllocatorTraits = std::allocator_traits<A>;

} // namespace efp

#else
// todo remove STL
#include <string>
#include "efp/allocator.hpp"

#endif

// todo Move and copy assigment operator sort out

namespace efp {

    template <typename A, size_t ct_size>
    class Array {
    public:
        using Element = A;
        using CtSize = Size<ct_size>;
        using CtCapacity = Size<ct_size>;

        // STL compatible types
        using value_type = Element;
        using allocator_type = std::allocator<Element>;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using reference = value_type&;
        using const_reference = const value_type&;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using iterator = value_type*;
        using const_iterator = const value_type*;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        Array() {
            // By definition all of the data in Array should be valid
            for (size_t i = 0; i < ct_size; ++i) {
                new (_data + i) Element{};
            }
        }

        Array(const Array& other) {
            for (size_t i = 0; i < ct_size; ++i) {
                new (_data + i) Element{other._data[i]};
            }
        }

        Array& operator=(const Array& other) {
            if (this != &other) {
                for (size_t i = 0; i < ct_size; ++i) {
                    (_data + i)->~Element();                 // Destroy each element
                    new (_data + i) Element{other._data[i]}; // Copy-construct each element
                }
            }
            return *this;
        }

        Array(Array&& other) noexcept {
            for (size_t i = 0; i < ct_size; ++i) {
                new (_data + i) Element{efp::move(other._data[i])}; // Move-construct each element
            }
        }

        Array& operator=(Array&& other) noexcept {
            if (this != &other) {
                for (size_t i = 0; i < ct_size; ++i) {
                    (_data + i)->~Element();                            // Destroy each element
                    new (_data + i) Element{efp::move(other._data[i])}; // Move-construct each element
                }
            }
            return *this;
        }

        ~Array() {
            for (size_t i = 0; i < ct_size; ++i) {
                (_data + i)->~Element();
            }
        }

        Array(InitializerList<Element> il) {
            if (il.size() != ct_size) {
                throw RuntimeError("Array::Array: number of arguments must be equal to ct_size");
            }

            size_t index = 0;
            for (const auto& e : il) {
                new (_data + index++) Element{e};
            }
        }

        Element& operator[](size_t index) {
            return _data[index];
        }

        const Element& operator[](size_t index) const {
            return _data[index];
        }

        bool operator==(const Array& other) const {
            for (size_t i = 0; i < ct_size; ++i) {
                if (_data[i] != other._data[i]) {
                    return false;
                }
            }
            return true;
        }

        size_t size() const {
            return ct_size;
        }

        size_t capacity() const {
            return ct_size;
        }

        void resize(size_t length) {
            if (length != ct_size) {
                throw RuntimeError("Array::resize: length must be equal to ct_size");
            }
        }

        void reserve(size_t capacity) {
            if (capacity > ct_size) {
                throw RuntimeError("Array::reserve: capacity must be less than or equal to ct_size");
            }
        }

        const Element* data() const {
            return _data;
        }

        Element* data() {
            return _data;
        }

        Element* begin() {
            return _data;
        }

        const Element* begin() const {
            return _data;
        }

        Element* end() {
            return _data + ct_size;
        }

        const Element* end() const {
            return _data + ct_size;
        }

        bool empty() const {
            return ct_size == 0;
        }

    private:
        template <typename Head, typename... Tail>
        inline void _construct_elements(size_t& index, const Head& head, const Tail&... tail) {
            new (_data + index++) Element{head};
            _construct_elements(index, tail...);
        }

        template <typename Last>
        inline void _construct_elements(size_t& index, const Last& last) {
            new (_data + index) Element{last};
        }

        RawStorage<A, ct_size> _data;
    };

    template <typename A, size_t n>
    struct ElementImpl<Array<A, n>> {
        using Type = A;
    };

    template <typename A, size_t n>
    struct CtSizeImpl<Array<A, n>> {
        using Type = Size<n>;
    };

    template <typename A, size_t n>
    struct CtCapacityImpl<Array<A, n>> {
        using Type = Size<n>;
    };

    template <typename A, size_t n>
    constexpr auto length(const Array<A, n>&) -> Size<n> {
        return Size<n>{};
    }

    template <typename A, size_t n>
    constexpr auto nth(size_t i, const Array<A, n>& as) -> const A& {
        return as[i];
    }

    template <typename A, size_t n>
    constexpr auto nth(size_t i, Array<A, n>& as) -> A& {
        return as[i];
    }

    template <typename A, size_t n>
    constexpr auto data(const Array<A, n>& as) -> const A* {
        return as.data();
    }

    template <typename A, size_t n>
    constexpr auto data(Array<A, n>& as) -> A* {
        return as.data();
    }

    template <typename A, size_t ct_capacity>
    class ArrVec {
    public:
        using Element = A;
        using CtSize = Size<dyn>;
        using CtCapacity = Size<ct_capacity>;

        // STL compatible types
        using value_type = Element;
        // using allocator_type = std::allocator<Element>;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using reference = value_type&;
        using const_reference = const value_type&;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using iterator = value_type*;
        using const_iterator = const value_type*;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        ArrVec()
            : _size{0} {
            // By definition none of the data in ArrVec of size 0 should be valid
        }

        ArrVec(const ArrVec& other)
            : _size{other._size} {
            for (size_t i = 0; i < _size; ++i) {
                new (_data + i) Element{other._data[i]};
            }
        }

        ArrVec& operator=(const ArrVec& other) {
            if (this != &other) {
                for (size_t i = 0; i < _size; ++i) {
                    (_data + i)->~Element();
                }

                _size = other._size;

                for (size_t i = 0; i < _size; ++i) {
                    new (_data + i) Element{other._data[i]}; // Copy-construct each element
                }
            }
            return *this;
        }

        ArrVec(ArrVec&& other) noexcept
            : _size{other._size} {
            other._size = 0;
            for (size_t i = 0; i < _size; ++i) {
                new (_data + i) Element{efp::move(other._data[i])};
            }
        }

        ArrVec& operator=(ArrVec&& other) noexcept {
            if (this != &other) {
                // Destroy existing elements
                for (size_t i = 0; i < _size; ++i) {
                    (_data + i)->~Element();
                }

                // Move data from the source object
                _size = other._size;
                for (size_t i = 0; i < _size; ++i) {
                    new (_data + i) Element{efp::move(other._data[i])};
                }

                // Reset the source object
                other._size = 0;
            }
            return *this;
        }

        ~ArrVec() {
            for (size_t i = 0; i < _size; ++i) {
                (_data + i)->~Element();
            }
        }

        // Constructor from array
        template <size_t ct_size_, typename = EnableIf<ct_capacity >= ct_size_, void>>
        ArrVec(const ArrVec<Element, ct_size_>& as)
            : _size(ct_size_) {
            for (size_t i = 0; i < ct_size_; ++i) {
                new (_data + i) Element{as[i]};
            }
        }

        ArrVec(InitializerList<Element> il)
            : _size(il.size()) {
            if (il.size() > ct_capacity) {
                throw RuntimeError(
                    "ArrVec::ArrVec: number of arguments must be less than or equal to ct_capacity");
            }

            size_t index = 0;
            for (const auto& e : il) {
                new (_data + index++) Element{e};
            }
        }

        Element& operator[](size_t index) {
            return _data[index];
        }

        const Element& operator[](size_t index) const {
            return _data[index];
        }

        bool operator==(const ArrVec& other) const {
            if (_size != other._size) {
                return false;
            }

            for (size_t i = 0; i < _size; ++i) {
                if (_data[i] != other._data[i]) {
                    return false;
                }
            }

            return true;
        }

        size_t size() const {
            return _size;
        }

        size_t capacity() const {
            return ct_capacity;
        }

        void resize(size_t length) {
            if (length > ct_capacity) {
                throw RuntimeError("ArrVec::resize: length must be less than or equal to ct_capacity");
            }

            _size = length;
        }

        void reserve(size_t capacity) {
            if (capacity > ct_capacity) {
                throw RuntimeError("ArrVec::reserve: capacity must be less than or equal to ct_capacity");
            }
        }

        void push_back(const Element& value) {
            if (_size >= ct_capacity) {
                throw RuntimeError("ArrVec::push_back: size must be less than or equal to ct_capacity");
            } else {
                new (_data + _size++) Element{value};
            }
        }

        void push_back(Element&& value) {
            if (_size >= ct_capacity) {
                throw RuntimeError("ArrVec::push_back: size must be less than or equal to ct_capacity");
            } else {
                new (_data + _size++) Element{efp::move(value)};
            }
        }

        void insert(size_t index, const Element& value) {
            if (index > _size || _size == ct_capacity) {
                throw RuntimeError(
                    "ArrVec::insert: index must be less than or equal to size and size must be less than or equal to ct_capacity");
            }

            for (size_t i = _size; i > index; --i) {
                new (_data + i) Element{efp::move(_data[i - 1])};
                (_data + i - 1)->~Element();
            }

            new (_data + index) Element{value};

            ++_size;
        }

        void pop_back() {
            if (_size == 0) {
                throw RuntimeError("ArrVec::pop_back: size must be greater than 0");
            }

            (_data + _size-- - 1)->~Element();
        }

        void clear() {
            for (size_t i = 0; i < _size; ++i) {
                (_data + i)->~Element();
            }
            _size = 0;
        }

        void erase(size_t index) {
            if (index >= _size) {
                throw RuntimeError("ArrVec::erase: index must be less than or equal to size");
            }

            (_data + index)->~Element();

            for (size_t i = index; i < _size - 1; ++i) {
                new (_data + i) Element{efp::move(_data[i + 1])};
                (_data + i + 1)->~Element();
            }
            --_size;
        }

        const Element* data() const {
            return _data;
        }

        Element* data() {
            return _data;
        }

        Element* begin() {
            return _data;
        }

        const Element* begin() const {
            return _data;
        }

        Element* end() {
            return _data + _size;
        }

        const Element* end() const {
            return _data + _size;
        }

        bool empty() const {
            return _size == 0;
        }

    private:
        template <typename Head, typename... Tail>
        void _construct_elements(size_t& index, const Head& head, const Tail&... tail) {
            new (_data + index++) Element{head};
            _construct_elements(index, tail...);
        }

        template <typename Last>
        void _construct_elements(size_t& index, const Last& last) {
            new (_data + index++) Element{last};
        }

        // Element _data[ct_capacity];
        RawStorage<Element, ct_capacity> _data;
        size_t _size;
    };

    template <typename A, size_t n>
    struct ElementImpl<ArrVec<A, n>> {
        using Type = A;
    };

    template <typename A, size_t n>
    struct CtSizeImpl<ArrVec<A, n>> {
        using Type = Size<dyn>;
    };

    template <typename A, size_t n>
    struct CtCapacityImpl<ArrVec<A, n>> {
        using Type = Size<n>;
    };

    template <typename A, size_t n>
    constexpr auto length(const ArrVec<A, n>& as) -> size_t {
        return as.size();
    }

    template <typename A, size_t n>
    constexpr auto nth(size_t i, const ArrVec<A, n>& as) -> const A& {
        return as[i];
    }

    template <typename A, size_t n>
    constexpr auto nth(size_t i, ArrVec<A, n>& as) -> A& {
        return as[i];
    }

    template <typename A, size_t n>
    constexpr auto data(const ArrVec<A, n>& as) -> const A* {
        return as.data();
    }

    template <typename A, size_t n>
    constexpr auto data(ArrVec<A, n>& as) -> A* {
        return as.data();
    }

    namespace detail {

#if defined(__STDC_HOSTED__) && __STDC_HOSTED__ == 1
        // #include <memory>
        template <typename A>
        using DefaultAllocator = std::allocator<A>;

#else

        // #include "efp/allocator.hpp"
        template <typename A>
        using DefaultAllocator = efp::Allocator<A>;

#endif

        template <typename A, typename Allocator = DefaultAllocator<A>>
        class VectorBase {
        public:
            using Element = A;
            using CtSize = Size<dyn>;
            using CtCapacity = Size<dyn>;

            // STL compatible types
            using value_type = Element;
            // using traits_type = Traits;
            using allocator_type = Allocator;
            using size_type = std::size_t;
            using difference_type = std::ptrdiff_t;
            using reference = value_type&;
            using const_reference = const value_type&;
            using pointer = value_type*;
            using const_pointer = const value_type*;
            using iterator = value_type*;
            using const_iterator = const value_type*;
            using reverse_iterator = std::reverse_iterator<iterator>;
            using const_reverse_iterator = std::reverse_iterator<const_iterator>;

            VectorBase()
                : _allocator(Allocator()), _data(nullptr), _size(0), _capacity(0) {}

            VectorBase(const VectorBase& other)
                : _allocator(other._allocator), _size(other._size), _capacity(other._capacity) {
                if (other._data) {
                    // Member function call in not allowed in member initializer list
                    _data = AllocatorTraits<Allocator>::allocate(_allocator, _capacity);

                    for (size_t i = 0; i < _size; ++i) {
                        AllocatorTraits<Allocator>::construct(_allocator, _data + i, other._data[i]);
                    }
                }
            }

            // todo copy_and_swap
            VectorBase& operator=(const VectorBase& other) noexcept {
                if (this != &other) {
                    for (size_t i = 0; i < _size; ++i) {
                        AllocatorTraits<Allocator>::destroy(_allocator, _data + i);
                    }

                    if (_capacity < other._size + 1) {
                        resize(other._size);
                    }

                    for (size_t i = 0; i < other._size; ++i) {
                        AllocatorTraits<Allocator>::construct(_allocator, _data + i, other._data[i]);
                    }
                }

                return *this;
            }

            VectorBase(VectorBase&& other) noexcept
                : _allocator(other._allocator), _data(other._data), _size(other._size), _capacity(other._capacity) {
                other._data = nullptr;
                other._capacity = 0;
                other._size = 0;
            }

            VectorBase& operator=(VectorBase&& other) noexcept {
                if (this != &other) {
                    for (size_t i = 0; i < _size; ++i) {
                        AllocatorTraits<Allocator>::destroy(_allocator, _data + i);
                    }
                    AllocatorTraits<Allocator>::deallocate(_allocator, _data, _capacity);

                    _data = other._data;
                    _size = other._size;
                    _capacity = other._capacity;

                    other._data = nullptr;
                    other._size = 0;
                    other._capacity = 0;
                }

                return *this;
            }

            ~VectorBase() {
                if (_data) {
                    for (size_t i = 0; i < _size; ++i) {
                        AllocatorTraits<Allocator>::destroy(_allocator, _data + i);
                    }

                    AllocatorTraits<Allocator>::deallocate(_allocator, _data, _capacity);
                    _data = nullptr;
                }
            }

            VectorBase(InitializerList<Element> il, const Allocator& allocator = Allocator())
                : _allocator(allocator), _size(il.size()), _capacity(il.size() + 1) {
                _data = AllocatorTraits<Allocator>::allocate(_allocator, _capacity);

                size_t index = 0;
                for (const auto& e : il) {
                    AllocatorTraits<Allocator>::construct(_allocator, _data + index++, e);
                }
            }

            template <size_t ct_size_>
            VectorBase(const Array<Element, ct_size_>& as)
                : _allocator(Allocator()), _size(ct_size_), _capacity(ct_size_ + 1) {
                _data = AllocatorTraits<Allocator>::allocate(_allocator, _capacity);

                for (size_t i = 0; i < _size; ++i) {
                    AllocatorTraits<Allocator>::construct(_allocator, _data + i, as[i]);
                }
            }

            template <size_t ct_cap_>
            VectorBase(const ArrVec<Element, ct_cap_>& as)
                : _allocator(Allocator()), _size(as.size()), _capacity(as.size() + 1) {
                _data = AllocatorTraits<Allocator>::allocate(_allocator, _capacity);

                for (size_t i = 0; i < _size; ++i) {
                    AllocatorTraits<Allocator>::construct(_allocator, _data + i, as[i]);
                }
            }

            Element& operator[](size_t index) {
                return _data[index];
            }

            const Element& operator[](size_t index) const {
                return _data[index];
            }

            bool operator==(const VectorBase& other) const {
                if (_size != other._size) {
                    return false;
                }

                for (size_t i = 0; i < _size; ++i) {
                    if (_data[i] != other._data[i]) {
                        return false;
                    }
                }

                return true;
            }

            size_t size() const {
                return _size;
            }

            size_t capacity() const {
                return _capacity;
            }

            size_t max_size() const {
                return AllocatorTraits<Allocator>::max_size(_allocator);
            }

            void resize(size_t new_size) {
                // Always allocate one extra space for BasicString null terminator
                if (new_size + 1 > _capacity) {
                    reserve(new_size + 1);
                }

                _size = new_size;
            }

            void reserve(size_t new_capacity) {
                if (new_capacity > _capacity) {
                    Element* new_data = AllocatorTraits<Allocator>::allocate(_allocator, new_capacity);

                    for (size_t i = 0; i < _size; ++i) {
                        AllocatorTraits<Allocator>::construct(
                            _allocator,
                            new_data + i,
                            efp::move(_data[i]));
                    }

                    for (size_t i = 0; i < _size; ++i) {
                        AllocatorTraits<Allocator>::destroy(_allocator, _data + i);
                    }

                    AllocatorTraits<Allocator>::deallocate(_allocator, _data, _capacity);

                    _data = new_data;
                    _capacity = new_capacity;
                }
            }

            void shrink_to_fit() {
                if (_size < _capacity + 1) {
                    // Allocate new storage with exactly _size capacity
                    Element* new_data = _allocator.allocate(_size + 1);

                    // Move existing elements to the new storage
                    for (size_t i = 0; i < _size; ++i) {
                        AllocatorTraits<Allocator>::construct(
                            _allocator,
                            new_data + i,
                            efp::move(_data[i]));
                    }

                    // Destroy and deallocate old storage
                    for (size_t i = 0; i < _size; ++i) {
                        AllocatorTraits<Allocator>::destroy(_allocator, _data + i);
                    }
                    AllocatorTraits<Allocator>::deallocate(_allocator, _data, _capacity);

                    // Update data pointer and capacity
                    _data = new_data;
                    _capacity = _size + 1;
                }
            }

            void push_back(const Element& value) {
                if (_size + 1 >= _capacity) {
                    reserve(_size == 0 ? 2 : 2 * _size + 1);
                }

                AllocatorTraits<Allocator>::construct(_allocator, _data + _size, value);
                ++_size;
            }

            void push_back(Element&& value) {
                if (_size + 1 >= _capacity) {
                    reserve(_size == 0 ? 2 : 2 * _size + 1);
                }

                AllocatorTraits<Allocator>::construct(_allocator, _data + _size, efp::move(value));
                ++_size;
            }

            template <typename... Args>
            void emplace_back(Args&&... args) {
                if (_size + 1 >= _capacity) {
                    reserve(_size == 0 ? 2 : 2 * _size + 1);
                }

                AllocatorTraits<Allocator>::construct(
                    _allocator,
                    _data + _size,
                    efp::forward<Args>(args)...);
                ++_size;
            }

            void pop_back() {
                if (_size == 0) {
                    throw RuntimeError("VectorBase::pop_back: size must be greater than 0");
                }

                AllocatorTraits<Allocator>::destroy(_allocator, _data + _size-- - 1);
            }

            void insert(size_t index, const Element& value) {
                if (index > _size) {
                    throw RuntimeError("VectorBase::insert: index must be less than or equal to size");
                }

                if (_size + 1 >= _capacity) {
                    reserve(_capacity == 0 ? 2 : 2 * _capacity);
                }

                for (size_t i = _size; i > index; --i) {
                    _data[i] = efp::move(_data[i - 1]);
                }

                AllocatorTraits<Allocator>::construct(_allocator, _data + index, value);
                ++_size;
            }

            void erase(size_t index) {
                if (index >= _size) {
                    throw RuntimeError("VectorBase::erase: index must be less than or equal to size");
                }

                AllocatorTraits<Allocator>::destroy(_allocator, _data + index);

                for (size_t i = index; i < _size - 1; ++i) {
                    _data[i] = efp::move(_data[i + 1]);
                }

                --_size;
            }

            void clear() {
                for (size_t i = 0; i < _size; ++i) {
                    AllocatorTraits<Allocator>::destroy(_allocator, _data + i);
                }
                _size = 0;
            }

            const Element* data() const {
                return _data;
            }

            Element* data() {
                return _data;
            }

            Element* begin() {
                return _data;
            }

            const Element* begin() const {
                return _data;
            }

            Element* end() {
                return _data + _size;
            }

            const Element* end() const {
                return _data + _size;
            }

            bool empty() const {
                return _size == 0;
            }

        protected:
            template <typename Last>
            void _construct_elements(size_t& index, const Last& last) {
                AllocatorTraits<Allocator>::construct(_allocator, _data + index++, last);
            }

            template <typename Head, typename... Tail>
            void _construct_elements(size_t& index, const Head& head, const Tail&... tail) {
                AllocatorTraits<Allocator>::construct(_allocator, _data + index++, head);
                _construct_elements(index, tail...);
            }

            Allocator _allocator;
            Element* _data;
            size_t _size;
            size_t _capacity;
        };
    } // namespace detail

    namespace detail {
        template <typename T>
        struct IsCharType : False {};

        // Specializations for character types
        template <>
        struct IsCharType<char> : True {};

        template <>
        struct IsCharType<wchar_t> : True {};

        template <>
        struct IsCharType<char16_t> : True {};

        template <>
        struct IsCharType<char32_t> : True {};

#if __cplusplus >= 202002L
        template <>
        struct IsCharType<char8_t> : True {}; // C++20 char8_t support
#endif
    } // namespace detail

    namespace detail {

#if defined(__STDC_HOSTED__) && __STDC_HOSTED__ == 1

        template <typename Char, typename = void>
        struct DefaultCharTraitsImpl {
            using Type = void;
        };

        // Specialize only for valid character types
        template <typename Char>
        struct DefaultCharTraitsImpl<Char, EnableIf<IsCharType<Char>::value>> {
            using Type = std::char_traits<Char>;
        };

        template <typename Char>
        using DefaultCharTraits = typename DefaultCharTraitsImpl<Char>::Type;

#else
        // todo freestanding implementation
        template <typename Char>
        using DefaultCharTraits = std::char_traits<Char>;
#endif

    } // namespace detail

    template <
        typename A,
        typename Allocator = detail::DefaultAllocator<A>,
        typename CharTraits =
            Conditional<detail::IsCharType<A>::value, detail::DefaultCharTraits<A>, void>,
        typename = void>
    class Vector : public detail::VectorBase<A, Allocator> {
    public:
        using Base = detail::VectorBase<A>;
        using Base::Base;

        bool operator==(const Vector& other) const {
            return Base::operator==(other);
        }
    };

    template <typename A, typename Allocator, typename CharTraits>
    struct ElementImpl<Vector<A, Allocator, CharTraits>> {
        using Type = A;
    };

    template <typename A, typename Allocator, typename CharTraits>
    struct CtSizeImpl<Vector<A, Allocator, CharTraits>> {
        using Type = Size<dyn>;
    };

    template <typename A, typename Allocator, typename CharTraits>
    struct CtCapacityImpl<Vector<A, Allocator, CharTraits>> {
        using Type = Size<dyn>;
    };

    template <typename A, typename Allocator, typename CharTraits>
    constexpr auto length(const Vector<A, Allocator, CharTraits>& as) -> size_t {
        return as.size();
    }

    template <typename A, typename Allocator, typename CharTraits>
    constexpr auto nth(size_t i, const Vector<A, Allocator, CharTraits>& as) -> const A& {
        return as[i];
    }

    template <typename A, typename Allocator, typename CharTraits>
    constexpr auto nth(size_t i, Vector<A, Allocator, CharTraits>& as) -> A& {
        return as[i];
    }

    template <typename A, typename Allocator, typename CharTraits>
    constexpr auto data(const Vector<A, Allocator, CharTraits>& as) -> const A* {
        return as.data();
    }

    template <typename A, typename Allocator, typename CharTraits>
    constexpr auto data(Vector<A, Allocator, CharTraits>& as) -> A* {
        return as.data();
    }

    template <typename A, size_t ct_size>
    class ArrayView {
    public:
        using Element = A;
        using CtSize = Size<ct_size>;
        using CtCapacity = Size<ct_size>;

        ArrayView()
            : _data(nullptr) {}

        // ! length will not be used
        ArrayView(const Element* data, size_t = Size<ct_size>{})
            : _data(data) {
            // Ensure that data is not nullptr for a non-empty view.
            if (ct_size > 0 && _data == nullptr) {
                throw RuntimeError("ArrayView::ArrayView: data must not be nullptr for a non-empty view");
            }
        }

        const Element& operator[](size_t index) {
            return _data[index];
        }

        const Element& operator[](size_t index) const {
            return _data[index];
        }

        bool operator==(const ArrayView& other) const {
            return _data == other._data;
        }

        size_t size() const {
            return ct_size;
        }

        size_t capacity() const {
            return ct_size;
        }

        void resize(size_t length) {
            if (length > ct_size || length < 0) {
                throw RuntimeError(
                    "ArrayView::resize: length must be less than or equal to ct_size and greater than or equal to 0");
            }
        }

        void reserve(size_t capacity) {
            if (capacity > ct_size) {
                throw RuntimeError("ArrayView::reserve: capacity must be less than or equal to ct_size");
            }
        }

        const Element* data() const {
            return _data;
        }

        const Element* data() {
            return _data;
        }

        const Element* begin() const {
            return _data;
        }

        const Element* begin() {
            return _data;
        }

        const Element* end() const {
            return _data + ct_size;
        }

        const Element* end() {
            return _data + ct_size;
        }

        bool empty() const {
            return ct_size == 0;
        }

    private:
        const Element* _data;
    };

    template <typename A, size_t n>
    struct ElementImpl<ArrayView<A, n>> {
        using Type = A;
    };

    template <typename A, size_t n>
    struct CtSizeImpl<ArrayView<A, n>> {
        using Type = Size<n>;
    };

    template <typename A, size_t n>
    struct CtCapacityImpl<ArrayView<A, n>> {
        using Type = Size<n>;
    };

    template <typename A, size_t n>
    constexpr auto length(const ArrayView<A, n>&) -> Size<n> {
        return Size<n>{};
    }

    template <typename A, size_t n>
    constexpr auto nth(size_t i, const ArrayView<A, n>& as) -> const A& {
        return as[i];
    }

    template <typename A, size_t n>
    constexpr auto nth(size_t i, ArrayView<A, n>& as) -> const A& {
        return as[i];
    }

    template <typename A, size_t n>
    constexpr auto data(const ArrayView<A, n>& as) -> const A* {
        return as.data();
    }

    template <typename A, size_t n>
    constexpr auto data(ArrayView<A, n>& as) -> const A* {
        return as.data();
    }

    template <typename A, size_t ct_capacity>
    class ArrVecView {
    public:
        using Element = A;
        using CtSize = Size<dyn>;
        using CtCapacity = Size<ct_capacity>;

        ArrVecView()
            : _data(nullptr), _size(0) {}

        ArrVecView(const Element* data, size_t size)
            : _data(data), _size(size) {
            // Ensure that data is not nullptr for a non-empty view.
            if (size > 0 && _data == nullptr) {
                throw RuntimeError(
                    "ArrVecView::ArrVecView: data must not be nullptr for a non-empty view");
            }
        }

        ArrVecView(const ArrVecView& other)
            : _data(other._data), _size(other._size) {}

        ArrVecView& operator=(const ArrVecView& other) {
            if (this != &other) {
                _data = other._data;
                _size = other._size;
            }
            return *this;
        }

        // Constructor from ArrayView
        template <size_t ct_size_>
        ArrVecView(const ArrayView<Element, ct_size_>& as)
            : _data(as.data()), _size(length(as)) {}

        const Element& operator[](size_t index) {
            return _data[index];
        }

        const Element& operator[](size_t index) const {
            return _data[index];
        }

        bool operator==(const ArrVecView& other) const {
            return (_data == other._data) && (_size == other._size);
        }

        size_t size() const {
            return _size;
        }

        size_t capacity() const {
            return ct_capacity;
        }

        const Element* data() const {
            return _data;
        }

        const Element* data() {
            return _data;
        }

        const Element* begin() const {
            return _data;
        }

        const Element* begin() {
            return _data;
        }

        const Element* end() const {
            return _data + _size;
        }

        const Element* end() {
            return _data + _size;
        }

        bool empty() const {
            return _size == 0;
        }

    private:
        const Element* _data;
        size_t _size;
    };

    template <typename A, size_t n>
    struct ElementImpl<ArrVecView<A, n>> {
        using Type = A;
    };

    template <typename A, size_t n>
    struct CtSizeImpl<ArrVecView<A, n>> {
        using Type = Size<dyn>;
    };

    template <typename A, size_t n>
    struct CtCapacityImpl<ArrVecView<A, n>> {
        using Type = Size<n>;
    };

    template <typename A, size_t n>
    constexpr auto length(const ArrVecView<A, n>& as) -> size_t {
        return as.size();
    }

    template <typename A, size_t n>
    constexpr auto nth(size_t i, const ArrVecView<A, n>& as) -> const A& {
        return as[i];
    }

    template <typename A, size_t n>
    constexpr auto nth(size_t i, ArrVecView<A, n>& as) -> const A& {
        return as[i];
    }

    template <typename A, size_t n>
    constexpr auto data(const ArrVecView<A, n>& as) -> const A* {
        return as.data();
    }

    template <typename A, size_t n>
    constexpr auto data(ArrVecView<A, n>& as) -> const A* {
        return as.data();
    }

    namespace detail {

        // todo Custom allocator support
        // VectorViewBase

        template <typename A>
        class VectorViewBase {
        public:
            using Element = A;
            using CtSize = Size<dyn>;
            using CtCapacity = Size<dyn>;

            VectorViewBase()
                : _data{nullptr}, _size{0}, _capacity{0} {}

            VectorViewBase(const Element* data, size_t size)
                : _data(data), _size(size), _capacity(size) {
                // Ensure that data is not nullptr for a non-empty view.
                if (size > 0 && _data == nullptr) {
                    throw RuntimeError(
                        "VectorViewBase::VectorViewBase: data must not be nullptr for a non-empty view");
                }
            }

            VectorViewBase(const VectorViewBase& other)
                : _data(other._data), _size(other._size), _capacity(other._capacity) {}

            VectorViewBase& operator=(const VectorViewBase& other) {
                if (this != &other) {
                    _data = other._data;
                    _size = other._size;
                    _capacity = other._capacity;
                }
                return *this;
            }

            // Constructor from ArrayView
            template <size_t ct_size_>
            VectorViewBase(const ArrayView<Element, ct_size_>& as)
                : _data(as.data()), _size(length(as)) {}

            // Constructor from ArrVecView
            template <size_t ct_cap_>
            VectorViewBase(const ArrVecView<Element, ct_cap_>& as)
                : _data(as.data()), _size(length(as)) {}

            const Element& operator[](size_t index) {
                return _data[index];
            }

            const Element& operator[](size_t index) const {
                return _data[index];
            }

            bool operator==(const VectorViewBase& other) const {
                return (_data == other._data) && (_size == other._size) && (_capacity == other._capacity);
            }

            size_t size() const {
                return _size;
            }

            size_t capacity() const {
                return _capacity;
            }

            const Element* data() const {
                return _data;
            }

            const Element* data() {
                return _data;
            }

            const Element* begin() const {
                return _data;
            }

            const Element* begin() {
                return _data;
            }

            const Element* end() const {
                return _data + _size;
            }

            const Element* end() {
                return _data + _size;
            }

            bool empty() const {
                return _size == 0;
            }

        protected:
            const Element* _data;
            size_t _size;
            size_t _capacity;
        };
    } // namespace detail

    template <
        typename A,
        typename Traits = Conditional<detail::IsCharType<A>::value, detail::DefaultCharTraits<A>, void>,
        typename = void>
    class VectorView : public detail::VectorViewBase<A> {
    public:
        using Base = detail::VectorViewBase<A>;
        using Base::Base;

        bool operator==(const VectorView& other) const {
            return Base::operator==(other);
        }
    };

    template <typename A>
    struct ElementImpl<VectorView<A>> {
        using Type = A;
    };

    template <typename A>
    struct CtSizeImpl<VectorView<A>> {
        using Type = Size<dyn>;
    };

    template <typename A>
    struct CtCapacityImpl<VectorView<A>> {
        using Type = Size<dyn>;
    };

    template <typename A>
    constexpr auto length(const VectorView<A>& as) -> size_t {
        return as.size();
    }

    template <typename A>
    constexpr auto nth(size_t i, const VectorView<A>& as) -> const A& {
        return as[i];
    }

    template <typename A>
    constexpr auto nth(size_t i, VectorView<A>& as) -> const A& {
        return as[i];
    }

    template <typename A>
    constexpr auto data(const VectorView<A>& as) -> const A* {
        return as.data();
    }

    template <typename A>
    constexpr auto data(VectorView<A>& as) -> const A* {
        return as.data();
    }

    // ! Very problematic
    // #if defined(__STDC_HOSTED__) && __STDC_HOSTED__ == 1

    // template<typename A>
    // auto operator<<(std::ostream& os, const A& seq) -> EnableIf<
    //     IsSequence<A>::value && !detail::IsCharType<Element<A>>::value
    //         && !IsSame<A, std::string>::value,
    //     std::ostream&> {
    //     static_assert(IsSequence<A>(), "Argument should be an instance of Sequence trait.");

    //     // ? Interesting. Automatically consider it as VectorStream?
    //     os << "{ ";
    //     for (size_t i = 0; i < seq.size(); ++i) {
    //         os << seq[i];
    //         if (i != seq.size() - 1) {
    //             os << ", ";
    //         }
    //     }
    //     os << " }";
    //     return os;
    // }

    // #endif

    // Sequence trait implementation for std::array

    template <typename A, size_t n>
    struct ElementImpl<std::array<A, n>> {
        using Type = A;
    };

    template <typename A, size_t n>
    struct CtSizeImpl<std::array<A, n>> {
        using Type = Size<n>;
    };

    template <typename A, size_t n>
    struct CtCapacityImpl<std::array<A, n>> {
        using Type = Size<n>;
    };

    template <typename A, size_t n>
    constexpr auto length(const std::array<A, n>&) -> Size<n> {
        return Size<n>{};
    }

    template <typename A, size_t n>
    constexpr auto nth(size_t i, const std::array<A, n>& as) -> const A& {
        return as[i];
    }

    template <typename A, size_t n>
    constexpr auto nth(size_t i, std::array<A, n>& as) -> A& {
        return as[i];
    }

    template <typename A, size_t n>
    constexpr auto data(const std::array<A, n>& as) -> const A* {
        return as.data();
    }

    template <typename A, size_t n>
    constexpr auto data(std::array<A, n>& as) -> A* {
        return as.data();
    }

    // Sequence trait implementation for std::vector
    template <typename A>
    struct ElementImpl<std::vector<A>> {
        using Type = A;
    };

    template <typename A>
    struct CtSizeImpl<std::vector<A>> {
        using Type = Size<dyn>;
    };

    template <typename A>
    struct CtCapacityImpl<std::vector<A>> {
        using Type = Size<dyn>;
    };

    template <typename A>
    constexpr auto length(const std::vector<A>& as) -> size_t {
        return as.size();
    }

    template <typename A>
    constexpr auto nth(size_t i, const std::vector<A>& as) -> const A& {
        return as[i];
    }

    template <typename A>
    constexpr auto nth(size_t i, std::vector<A>& as) -> A& {
        return as[i];
    }

    template <typename A>
    constexpr auto data(const std::vector<A>& as) -> const A* {
        return as.data();
    }

    template <typename A>
    constexpr auto data(std::vector<A>& as) -> A* {
        return as.data();
    }

    // Sequence trait implementation for std::basic_string
    template <typename A>
    struct ElementImpl<std::basic_string<A>> {
        using Type = A;
    };

    template <typename A>
    struct CtSizeImpl<std::basic_string<A>> {
        using Type = Size<dyn>;
    };

    template <typename A>
    struct CtCapacityImpl<std::basic_string<A>> {
        using Type = Size<dyn>;
    };

    template <typename A>
    constexpr auto length(const std::basic_string<A>& as) -> size_t {
        return as.size();
    }

    template <typename A>
    constexpr auto nth(size_t i, const std::basic_string<A>& as) -> const A& {
        return as[i];
    }

    template <typename A>
    constexpr auto nth(size_t i, std::basic_string<A>& as) -> A& {
        return as[i];
    }

    template <typename A>
    constexpr auto data(const std::basic_string<A>& as) -> const A* {
        return as.data();
    }

    template <typename A>
    constexpr auto data(std::basic_string<A>& as) -> A* {
        return as.data();
    }

}; // namespace efp

#endif