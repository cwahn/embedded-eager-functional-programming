#ifndef EFP_CONCURRENCY_HPP_
#define EFP_CONCURRENCY_HPP_

// ! Not for freestanding environments
#if defined(__STDC_HOSTED__) && __STDC_HOSTED__ == 1

#include <mutex>
#include <condition_variable>
#include <queue>
#include <cstring>

#include "efp/cpp_core.hpp"
#include "efp/maybe.hpp"
#include "efp/cyclic.hpp"

namespace efp {

    template <typename A>
    using Atomic = std::atomic<A>;

    template <typename A>
    using Arc = std::shared_ptr<A>;

    template <typename A>
    inline Arc<A> arc(A&& a) {
        return std::make_shared<A>(std::forward<A>(a));
    };

    template <typename A, size_t capacity = dyn>
    class BlockingQ {
    public:
        BlockingQ() {}

        BlockingQ(const BlockingQ& other) = delete;

        BlockingQ& operator=(const BlockingQ& other) = delete;

        BlockingQ(BlockingQ&& other) = delete;

        BlockingQ& operator=(BlockingQ&& other) = delete;

        ~BlockingQ() {}

        // Add an element to the queue.
        void enqueue(A t) {
            std::lock_guard<std::mutex> lock(_m);
            _q.push_back(t);
            _c.notify_one();
        }

        // Get the front element.
        // If the queue is empty, block till a element is avaiable.
        A dequeue() {
            std::unique_lock<std::mutex> lock(_m);
            while (_q.empty()) {
                // release lock as long as the wait and reaquire it afterwards.
                _c.wait(lock);
            }
            return _q.pop_front();
        }

    private:
        Vcq<A, capacity> _q;
        mutable std::mutex _m;
        std::condition_variable _c;
    };

    template <typename A>
    class BlockingQ<A, dyn> {
    public:
        BlockingQ() {}

        BlockingQ(const BlockingQ& other) = delete;

        BlockingQ& operator=(const BlockingQ& other) = delete;

        BlockingQ(BlockingQ&& other) = delete;

        BlockingQ& operator=(BlockingQ&& other) = delete;

        ~BlockingQ() {}

        // Add an element to the queue.
        void enqueue(A t) {
            std::lock_guard<std::mutex> lock(_m);
            _q.push(t);
            _c.notify_one();
        }

        // Get the front element.
        // If the queue is empty, block till a element is avaiable.
        A dequeue() {
            std::unique_lock<std::mutex> lock(_m);
            while (_q.empty()) {
                // release lock as long as the wait and reaquire it afterwards.
                _c.wait(lock);
            }
            A val = _q.front();
            _q.pop();
            return val;
        }

    private:
        std::queue<A> _q;
        mutable std::mutex _m;
        std::condition_variable _c;
    };

    template <typename A, size_t capacity = dyn>
    class NonBlockingQ {
    public:
        NonBlockingQ() {}

        NonBlockingQ(const NonBlockingQ& other) = delete;

        NonBlockingQ& operator=(const NonBlockingQ& other) = delete;

        NonBlockingQ(NonBlockingQ&& other) = delete;

        NonBlockingQ& operator=(NonBlockingQ&& other) = delete;

        ~NonBlockingQ() {}

        // Add an element to the queue.
        void enqueue(A t) {
            std::lock_guard<std::mutex> lock(_m);
            _q.push_back(t);
        }

        bool is_empty() {
            return _q.empty();
        }

        // Get the front element.
        // If the queue is empty, return nothing;
        Maybe<A> dequeue() {
            std::unique_lock<std::mutex> lock(_m);
            if (_q.empty()) {
                return nothing;
            } else {
                return _q.pop_front();
            }
        }

    private:
        Vcq<A, capacity> _q;
        mutable std::mutex _m;
    };

    template <typename A>
    class NonBlockingQ<A, dyn> {
    public:
        NonBlockingQ() {}

        NonBlockingQ(const NonBlockingQ& other) = delete;

        NonBlockingQ& operator=(const NonBlockingQ& other) = delete;

        NonBlockingQ(NonBlockingQ&& other) = delete;

        NonBlockingQ& operator=(NonBlockingQ&& other) = delete;

        ~NonBlockingQ() {}

        // Add an element to the queue.
        void enqueue(A t) {
            std::lock_guard<std::mutex> lock(_m);
            _q.push(t);
        }

        bool is_empty() {
            return _q.empty();
        }

        // Get the front element.
        // If the queue is empty, return nothing;
        Maybe<A> dequeue() {
            std::unique_lock<std::mutex> lock(_m);
            if (_q.empty()) {
                return nothing;
            } else {
                A val = _q.front();
                _q.pop();
                return val;
            }
        }

    private:
        std::queue<A> _q;
        mutable std::mutex _m;
    };

    // DoubleBuffer
    // A thread-safe, allocation-free double buffer implementation.
    template <typename A, size_t capacity>
    class DoubleBuffer {
    public:
        explicit DoubleBuffer()
            : _read_buffer(new Vcq<A, capacity>{}), _write_buffer(new Vcq<A, capacity>{}) {}

        DoubleBuffer(const DoubleBuffer& other) = default;

        DoubleBuffer& operator=(const DoubleBuffer& other) = default;

        DoubleBuffer(DoubleBuffer&& other) noexcept = default;

        DoubleBuffer& operator=(DoubleBuffer&& other) noexcept = default;

        ~DoubleBuffer() {
            delete _read_buffer;
            delete _write_buffer;
        }

        void enqueue(const A& a) {
            _spinlock.lock();
            _write_buffer->push_back(a);
            _spinlock.unlock();
        }

        void swap_buffer() {
            _spinlock.lock();
            swap(_write_buffer, _read_buffer);
            _spinlock.unlock();
        }

        A dequeue() {
            return _read_buffer->pop_front();
        }

        bool empty() {
            return _read_buffer->empty();
        }

    private:
        class Spinlock {
        public:
            inline void lock() {
                while (_flag.test_and_set(std::memory_order_acquire)) {
                }
            }

            inline void unlock() {
                _flag.clear(std::memory_order_release);
            }

        private:
            std::atomic_flag _flag = {0};
        };

        Spinlock _spinlock;
        Vcq<A, capacity>* _read_buffer;
        Vcq<A, capacity>* _write_buffer;
    };

    // todo Implement Sequence traits for DoubleBuffer
} // namespace efp

#endif // __STDC_HOSTED__ && __STDC_HOSTED__ == 1
#endif