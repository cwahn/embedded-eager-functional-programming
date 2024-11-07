#ifndef CONCURRENCY_TEST_HPP_
#define CONCURRENCY_TEST_HPP_

#include "efp/concurrency.hpp"
#include "test_common.hpp"

using namespace efp;

TEST_CASE("Enqueue and Dequeue", "[BlockingQ]") {
    BlockingQ<int, 5> queue;

    SECTION("Enqueue then Dequeue") {
        queue.enqueue(1);
        CHECK(queue.dequeue() == 1);
    }

    SECTION("Multiple Enqueue and Dequeue") {
        queue.enqueue(1);
        queue.enqueue(2);
        CHECK(queue.dequeue() == 1);
        CHECK(queue.dequeue() == 2);
    }

    SECTION("Dequeue Order") {
        queue.enqueue(1);
        queue.enqueue(2);
        queue.enqueue(3);
        CHECK(queue.dequeue() == 1);
        CHECK(queue.dequeue() == 2);
        CHECK(queue.dequeue() == 3);
    }
}

TEST_CASE("Enqueue and Dequeue Dynamic Capacity", "[BlockingQ]") {
    BlockingQ<int> queue;

    SECTION("Enqueue then Dequeue") {
        queue.enqueue(1);
        CHECK(queue.dequeue() == 1);
    }

    SECTION("Multiple Enqueue and Dequeue") {
        queue.enqueue(1);
        queue.enqueue(2);
        CHECK(queue.dequeue() == 1);
        CHECK(queue.dequeue() == 2);
    }

    SECTION("Dequeue Order") {
        queue.enqueue(1);
        queue.enqueue(2);
        queue.enqueue(3);
        CHECK(queue.dequeue() == 1);
        CHECK(queue.dequeue() == 2);
        CHECK(queue.dequeue() == 3);
    }
}

TEST_CASE("NonBlockingQ Operations", "[NonBlockingQ]") {
    NonBlockingQ<int> queue;

    SECTION("Queue starts empty") {
        CHECK(queue.is_empty());
    }

    SECTION("Enqueue adds elements") {
        queue.enqueue(1);
        CHECK_FALSE(queue.is_empty());
    }

    SECTION("Dequeue returns elements in FIFO order") {
        queue.enqueue(1);
        queue.enqueue(2);
        auto maybeOne = queue.dequeue();
        CHECK(maybeOne.has_value());
        CHECK(maybeOne.value() == 1);

        auto maybeTwo = queue.dequeue();
        CHECK(maybeTwo.has_value());
        CHECK(maybeTwo.value() == 2);
    }

    SECTION("Dequeue on empty queue returns efp::Nothing") {
        auto result = queue.dequeue();
        CHECK_FALSE(result.has_value());
    }

    SECTION("Enqueue and dequeue multiple elements") {
        for (int i = 1; i <= 5; ++i) {
            queue.enqueue(i);
        }

        for (int i = 1; i <= 5; ++i) {
            auto result = queue.dequeue();
            CHECK(result.has_value());
            CHECK(result.value() == i);
        }
    }
}

TEST_CASE("DoubleBuffer Operations", "[DoubleBuffer]") {
    DoubleBuffer<int, 10> buffer; // Assuming a buffer size of 10 for testing

    SECTION("Initial State is Empty") {
        CHECK(buffer.empty());
    }

    SECTION("Enqueue Adds Elements") {
        buffer.enqueue(1);
        buffer.swap_buffer(); // Swap to make the enqueued element available for dequeue
        CHECK_FALSE(buffer.empty());
    }

    SECTION("Dequeue Removes Elements") {
        buffer.enqueue(1);
        buffer.swap_buffer();
        CHECK(buffer.dequeue() == 1);
        CHECK(buffer.empty());
    }

    SECTION("Elements Move to Read Buffer on Swap") {
        buffer.enqueue(1);
        buffer.enqueue(2);
        buffer.swap_buffer(); // Now, the elements should be in the read buffer
        CHECK(buffer.dequeue() == 1);
        CHECK(buffer.dequeue() == 2);
        CHECK(buffer.empty());
    }
}

#endif // CONCURRENCY_TEST_HPP_