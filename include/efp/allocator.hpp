#ifndef EFP_ALLOCATOR_HPP_
#define EFP_ALLOCATOR_HPP_

// #if !defined(__STDC_HOSTED__)
// Define a custom allocator for freestanding environments

#include "efp/cpp_core.hpp"

namespace efp {

    template <typename Alloc>
    struct AllocatorTraits {
        using allocator_type = Alloc;
        using value_type = typename Alloc::value_type;
        using pointer = typename Alloc::pointer;
        using const_pointer = typename Alloc::const_pointer;
        using size_type = typename Alloc::size_type;
        using difference_type = typename Alloc::difference_type;

        template <typename T>
        using rebind_alloc = typename Alloc::template rebind<T>::other;

        static pointer allocate(Alloc& alloc, size_type n) {
            return alloc.allocate(n);
        }

        static void deallocate(Alloc& alloc, pointer p, size_type n) {
            alloc.deallocate(p, n);
        }

        template <typename T, typename... Args>
        static void construct(Alloc& alloc, T* p, Args&&... args) {
            // Assuming your environment supports forwarding and placement new
            alloc.construct(p, std::forward<Args>(args)...);
        }

        template <typename T>
        static void destroy(Alloc& alloc, T* p) {
            alloc.destroy(p);
        }

        static size_type max_size(const Alloc& alloc) noexcept {
            return alloc.max_size();
        }
    };

    template <typename T>
    class Allocator {
    public:
        using value_type = T;
        using pointer = T*;
        using const_pointer = const T*;
        using reference = T&;
        using const_reference = const T&;
        using size_type = size_t;
        using difference_type = ptrdiff_t;

        template <typename U>
        struct rebind {
            using other = Allocator<U>;
        };

        Allocator() noexcept = default;

        template <typename U>
        Allocator(const Allocator<U>&) noexcept {}

        pointer allocate(size_type n) {
            return static_cast<pointer>(::operator new(n * sizeof(T)));
        }

        void deallocate(pointer p, size_type) {
            ::operator delete(p);
        }

        size_type max_size() const noexcept {
            return size_type(-1) / sizeof(T);
        }

        void construct(pointer p, const T& t) {
            new (p) T(t);
        }

        void destroy(pointer p) {
            p->~T();
        }
    };

} // namespace efp

// #endif

#endif