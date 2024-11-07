#include "./meta_test.hpp"
#include "./sequence_test.hpp"
#include "./enum_test.hpp"
#include "./maybe_test.hpp"
#include "./prelude_test.hpp"
#include "./numeric_test.hpp"
#include "./scientific_test.hpp"
#include "./cyclic_test.hpp"
#include "./c_utility_test.hpp"
#include "./string_test.hpp"
#include "./sort_test.hpp"
#include "./format_test.hpp"
#include "./concurrency_test.hpp"

TEST_CASE("std::array instance Functor") {
    std::array<int, 0> a0 {};
    std::array<int, 5> a1 {1, 2, 3, 4, 5};

    CHECK(fmap([](int x) { return x + 1; }, a0) == std::array<int, 0> {});
    CHECK(fmap([](int x) { return x + 1; }, a1) == std::array<int, 5> {2, 3, 4, 5, 6});
}

TEST_CASE("std::vector instance Functor") {
    std::vector<int> v0 {};
    std::vector<int> v1 {1, 2, 3, 4, 5};

    CHECK(fmap([](int x) { return x + 1; }, v0) == std::vector<int> {});
    CHECK(fmap([](int x) { return x + 1; }, v1) == std::vector<int> {2, 3, 4, 5, 6});
}

TEST_CASE("std::vector instance Monad") {
    CHECK(pure<std::vector<int>>(42) == std::vector<int> {42});

    std::vector<int> v0 {};
    std::vector<int> v1 {1, 2, 3, 4, 5};

    CHECK(bind(v0, [](int x) { return std::vector<int> {x + 1}; }) == std::vector<int> {});
    CHECK(
        bind(v1, [](int x) { return std::vector<int> {x + 1}; }) == std::vector<int> {2, 3, 4, 5, 6}
    );
}