#ifndef CYCLIC_HPP_
#define CYCLIC_HPP_

#include "efp/cpp_core.hpp"
#include "efp/sequence.hpp"

namespace efp {
    template <typename A, size_t n>
    class Vcb {
    public:
        using Element = A;
        using CtSize = Size<n>;
        using CtCapcity = Size<n>;
        using SizeType = size_t;

        static constexpr size_t ct_size = n;
        static constexpr size_t ct_capacity = n;

        Vcb() {
            for (size_t i = 0; i < ct_size * 2; ++i) {
                new (_buffer + i) A{};
            }
            _data = _buffer;
        }

        Vcb(const Vcb& other) {
            for (size_t i = 0; i < ct_size * 2; ++i) {
                new (_buffer + i) A(other._buffer[i]);
            }
            _data = _buffer + (other._data - other._buffer);
        }

        Vcb& operator=(const Vcb& other) {
            for (size_t i = 0; i < ct_size * 2; ++i) {
                (_buffer + i)->~A();
                new (_buffer + i) A(other._buffer[i]);
            }
            _data = _buffer + (other._data - other._buffer);
            return *this;
        }

        Vcb(Vcb&& other) noexcept {
            for (size_t i = 0; i < ct_size * 2; ++i) {
                new (_buffer + i) A(efp::move(other._buffer[i]));
            }

            _data = _buffer + (other._data - other._buffer);
        }

        Vcb operator=(Vcb&& other) noexcept {
            for (size_t i = 0; i < ct_size * 2; ++i) {
                (_buffer + i)->~A();
                new (_buffer + i) A(efp::move(other._buffer[i]));
            }

            _data = _buffer + (other._data - other._buffer);
            return *this;
        }

        ~Vcb() {
            for (size_t i = 0; i < ct_size * 2; ++i) {
                (_buffer + i)->~A();
            }
        }

        Vcb(InitializerList<Element> il) {
            if (il.size() != ct_size) {
                throw RuntimeError("Vcb::Vcb: number of arguments must be equal to ct_size");
            }

            size_t index = 0;
            for (const auto& e : il) {
                new (_buffer + index) A{e};
                new (_buffer + ct_size + index) A{e};
                ++index;
            }

            _data = _buffer;
        }

        A& operator[](const SizeType index) {
            return _data[index];
        }

        const A& operator[](const SizeType index) const {
            return _data[index];
        }

        void push_back(A value) {
            _data->~A();
            (_data + ct_size)->~A();

            new (_data) A{value};
            new (_data + ct_size) A{value};

            ++_data;
            _data -= ct_size * (_data == _buffer + ct_capacity);
        }

        constexpr SizeType size() const {
            return ct_size;
        }

        A* data() {
            return _data;
        }

        const A* data() const {
            return _data;
        }

        bool empty() const {
            return false;
        }

        const A* begin() const {
            return _data;
        }

        A* begin() {
            return _data;
        }

        const A* end() const {
            return _data + ct_size;
        }

        A* end() {
            return _data + ct_size;
        }

    private:
        RawStorage<A, 2 * ct_size> _buffer;
        A* _data;
    };

    template <typename A, size_t n>
    struct ElementImpl<Vcb<A, n>> {
        using Type = A;
    };

    template <typename A, size_t n>
    struct CtSizeImpl<Vcb<A, n>> {
        using Type = Size<n>;
    };

    template <typename A, size_t n>
    struct CtCapacityImpl<Vcb<A, n>> {
        using Type = Size<n>;
    };

    template <typename A, size_t n>
    constexpr auto length(const Vcb<A, n>& as) -> Size<n> {
        return Size<n>{};
    }

    template <typename A, size_t n>
    constexpr auto nth(size_t i, const Vcb<A, n>& as) -> const A& {
        return as[i];
    }

    template <typename A, size_t n>
    constexpr auto nth(size_t i, Vcb<A, n>& as) -> A& {
        return as[i];
    }

    template <typename A, size_t n>
    constexpr auto data(const Vcb<A, n>& as) -> const A* {
        return as.data();
    }

    template <typename A, size_t n>
    constexpr auto data(Vcb<A, n>& as) -> A* {
        return as.data();
    }

    template <typename A, size_t n>
    class Vcq {
    public:
        using Element = A;
        using CtSize = Size<dyn>;
        using CtCapcity = Size<n>;
        using SizeType = size_t;

        static constexpr size_t ct_size = dyn;
        static constexpr size_t ct_capacity = n;

        Vcq() {
            // Buffers are uninitialized
            _size = 0;
            _read = _buffer;
            _write = _buffer;
        }

        Vcq(const Vcq& other) {
            const auto read_offset = other._read - other._buffer;
            for (size_t i = 0; i < other._size; ++i) {
                const auto j =
                    read_offset + i < ct_capacity ? read_offset + i : read_offset + i - ct_capacity;
                new (_buffer + j) A(other._buffer[i]);
                new (_buffer + ct_capacity + j) A(other._buffer[i]);
            }

            _size = other._size;
            _read = _buffer + (other._read - other._buffer);
            _write = _buffer + (other._write - other._buffer);
        }

        Vcq& operator=(const Vcq& other) {
            const auto read_offset = _read - _buffer;
            for (size_t i = 0; i < _size; ++i) {
                const auto j =
                    read_offset + i < ct_capacity ? read_offset + i : read_offset + i - ct_capacity;
                (_buffer + j)->~A();
                (_buffer + ct_capacity + j)->~A();
            }

            const auto read_offset_other = other._read - other._buffer;
            for (size_t i = 0; i < other._size; ++i) {
                const auto j = read_offset_other + i < ct_capacity
                                   ? read_offset_other + i
                                   : read_offset_other + i - ct_capacity;
                new (_buffer + j) A(other._buffer[i]);
                new (_buffer + ct_capacity + j) A(other._buffer[i]);
            }

            _size = other._size;
            _read = _buffer + (other._read - other._buffer);
            _write = _buffer + (other._write - other._buffer);
            return *this;
        }

        Vcq(Vcq&& other) noexcept {
            const auto read_offset_other = other._read - other._buffer;
            for (size_t i = 0; i < other._size; ++i) {
                const auto j = read_offset_other + i < ct_capacity
                                   ? read_offset_other + i
                                   : read_offset_other + i - ct_capacity;
                new (_buffer + j) A(efp::move(other._buffer[i]));
                new (_buffer + ct_capacity + j) A(efp::move(other._buffer[i]));
            }

            _size = other._size;
            _read = _buffer + (other._read - other._buffer);
            _write = _buffer + (other._write - other._buffer);
        }

        Vcq& operator=(Vcq&& other) noexcept {
            const auto read_offset = _read - _buffer;
            for (size_t i = 0; i < _size; ++i) {
                const auto j =
                    read_offset + i < ct_capacity ? read_offset + i : read_offset + i - ct_capacity;
                (_buffer + j)->~A();
                (_buffer + ct_capacity + j)->~A();
            }

            const auto read_offset_other = other._read - other._buffer;
            for (size_t i = 0; i < other._size; ++i) {
                const auto j = read_offset_other + i < ct_capacity
                                   ? read_offset_other + i
                                   : read_offset_other + i - ct_capacity;
                new (_buffer + j) A(efp::move(other._buffer[i]));
                new (_buffer + ct_capacity + j) A(efp::move(other._buffer[i]));
            }

            _size = other._size;
            _read = _buffer + (other._read - other._buffer);
            _write = _buffer + (other._write - other._buffer);
            return *this;
        }

        ~Vcq() {
            const auto read_offset = _read - _buffer;
            for (size_t i = 0; i < _size; ++i) {
                const auto j =
                    read_offset + i < ct_capacity ? read_offset + i : read_offset + i - ct_capacity;
                (_buffer + j)->~A();
                (_buffer + ct_capacity + j)->~A();
            }
        }

        A& operator[](const SizeType index) {
            return _read[index];
        }

        const A& operator[](const SizeType index) const {
            return _read[index];
        }

        void push_back(const A& value) {
            if (_size == ct_capacity) { // Has to destroy the oldest element if the buffer is full
                _write->~A();
                (_write + ct_capacity)->~A();
            }

            new (_write) A{value};
            new (_write + ct_capacity) A{value};

            ++_write;
            _write -= ct_capacity * (_write == _buffer + ct_capacity);

            if (_size < ct_capacity) {
                ++_size;
            } else {
                _read++;
                _read -= ct_capacity * (_read == _buffer + ct_capacity);
            }
        }

        // ! Undefined if empty

        A pop_front() {
            A value{efp::move(*_read)};

            _read->~A();
            (_read + ct_capacity)->~A();
            _size--;

            _read++;
            _read -= ct_capacity * (_read == _buffer + ct_capacity);

            return value;
        }

        constexpr SizeType size() const {
            return _size;
        }

        bool empty() const {
            return _size == 0;
        }

        A* data() {
            return _read;
        }

        const A* data() const {
            return _read;
        }

        const A* begin() const {
            return _read;
        }

        A* begin() {
            return _read;
        }

        const A* end() const {
            return _write > _read ? _write : _write + ct_capacity;
        }

        A* end() {
            return _write > _read ? _write : _write + ct_capacity;
        }

    private:
        RawStorage<A, 2 * ct_capacity> _buffer;

        size_t _size = 0;
        A* _read;
        A* _write;
    };

    template <typename A, size_t n>
    struct ElementImpl<Vcq<A, n>> {
        using Type = A;
    };

    template <typename A, size_t n>
    struct CtSizeImpl<Vcq<A, n>> {
        using Type = Size<dyn>;
    };

    template <typename A, size_t n>
    struct CtCapacityImpl<Vcq<A, n>> {
        using Type = Size<n>;
    };

    template <typename A, size_t n>
    constexpr auto length(const Vcq<A, n>& as) -> size_t {
        return as.size();
    }

    template <typename A, size_t n>
    constexpr auto nth(size_t i, const Vcq<A, n>& as) -> const A& {
        return as[i];
    }

    template <typename A, size_t n>
    constexpr auto nth(size_t i, Vcq<A, n>& as) -> A& {
        return as[i];
    }

    template <typename A, size_t n>
    constexpr auto data(const Vcq<A, n>& as) -> const A* {
        return as.data();
    }

    template <typename A, size_t n>
    constexpr auto data(Vcq<A, n>& as) -> A* {
        return as.data();
    }

} // namespace efp

#endif
