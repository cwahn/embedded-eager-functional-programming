#ifndef MAYBE_TEST_HPP_
#define MAYBE_TEST_HPP_

#include "catch2/catch_test_macros.hpp"

#include "efp.hpp"
#include "test_common.hpp"

using namespace efp;

TEST_CASE("maybe_construction") {
    SECTION("Nothing") {
        Maybe<int> a = nothing;

        CHECK(a.has_value() == false);
        CHECK(!a);
    }

    SECTION("Just") {
        Maybe<int> a = 42;

        CHECK(a.has_value() == true);
        CHECK(a.value() == 42);
        CHECK(a);
    }
}

TEST_CASE("maybe_functor") {
    SECTION("map") {
        Maybe<int> a = 2;
        const auto square = [](int x) { return x * x; };

        CHECK(a.fmap(square).index() == 1);
        CHECK(a.fmap(square).is_nothing() == false);
        CHECK(a.fmap(square).value() == 4);
    }

    SECTION("fmap") {
        Maybe<int> a = 2;
        const auto square = [](int x) { return x * x; };

        CHECK(fmap(square, a).index() == 1);
        CHECK(fmap(square, a).is_nothing() == false);
        CHECK(fmap(square, a).value() == 4);
    }
}

int times_2(int x) {
    return 2 * x;
}

TEST_CASE("maybe_applicative") {
    SECTION("pure") {
        CHECK(pure<Maybe<int>>(2).value() == 2);
    }

    // ! Can't copy assign lambda

    // auto times_2 = [](int x)
    // {
    //     return 2 * x;
    // };

    // ! Can't do sizeof to function pointer

    // Maybe<decltype(times_2)> maybe_times_2 = &times_2;

    // SECTION("ap")
    // {
    //     Maybe<int> ma = 2;
    //     CHECK(ap(maybe_times_2, ma).value() == 4);
    // }
}

TEST_CASE("maybe_monad") {
    SECTION("bind") {
        Maybe<int> ma = 2;

        const auto div_12_by = [](int x) -> Maybe<double> {
            if (x == 0) {
                return nothing;
            }

            return Maybe<double>{12. / x};
        };

        CHECK(bind(ma, div_12_by).is_nothing() == false);
        CHECK(bind(ma, div_12_by).value() == 6.);

        Maybe<int> mb = 0;

        CHECK(bind(mb, div_12_by).is_nothing() == true);

        Maybe<int> mc = nothing;

        CHECK(bind(mc, div_12_by).is_nothing() == true);
    }

    SECTION(">>=") {
        Maybe<int> ma = 2;

        const auto div_12_by = [](int x) -> Maybe<double> {
            if (x == 0) {
                return nothing;
            }

            return Maybe<double>{12. / x};
        };

        CHECK((ma >>= div_12_by).is_nothing() == false);
        CHECK(bind(ma, div_12_by).value() == 6.);

        Maybe<int> mb = 0;

        CHECK((mb >>= div_12_by).is_nothing() == true);

        Maybe<int> mc = nothing;

        CHECK((mc >>= div_12_by).is_nothing() == true);
    }
}

// Maybe<Unit> success()
// {
//     return unit;
// }

// Maybe<Unit> maybe_fail(bool is_fail)
// {
//     return nothing;
// }

// // Maybe<Unit> combined(bool is_fail)
// // {
// //     return success()
// //                ? maybe_fail(true)
// //                      ? maybe_fail(is_fail)
// //                      : nothing
// //                : nothing;
// // }

// Maybe<int> action_0(bool x)
// {
//     // if (x)
//     //     return 42;
//     // else
//     //     return nothing;
//     return x ? Maybe<int>{42} : Maybe<int>{nothing};
// }

// Maybe<Unit> action_1(bool x)
// {
//     if (x)
//         return unit;
//     else
//         return nothing;
// }

// Maybe<int> action_2(bool x, bool y)
// {
//     auto a = try_(action_0(x));
//     // auto b = try_(action_1(y));
//     return a;
// }

TEST_CASE("Maybe as bool") {
    SECTION("0") {
        Maybe<int> a = 42;
        Maybe<int> b = nothing;

        bool l = a ? true : false;
        CHECK(l);

        bool bis = false;
        if (!b)
            bis = true;

        CHECK(bis);
    }

    // SECTION("1")
    // {
    //     CHECK(action_2(true, true).value() == 42);
    // }

    // todo Make example test

    // SECTION("example") {
    //     Enum<bool, int, double> x = 2;  // Example with int

    //     int y = x.match(
    //         [](int x) { return x * 2; },  // Specific branch for int
    //         []() { return -1; }           // Wildcard branch
    //     );
    //     CHECK(y == 4);

    //     // Maybe<A> is a specialization of Enum<Nothing, A>
    //     Maybe<int> maybe_42 = 42;
    //     Maybe<int> no_value = nothing;

    //     // Using value() and isEmpty() for direct value access and checking
    //     CHECK(maybe_42.value() == 42);
    //     CHECK(no_value.is_nothing() == true);

    //     int result = maybe_42.match(
    //         [](int x) { return x; },  // Executes if there's a value
    //         []() { return 0; }        // Executes if empty (wildcard)
    //     );
    //     CHECK(result == 42);
    // }
}

TEST_CASE("EnumAt on Maybe") {
    CHECK(IsSame<EnumAt<0, Maybe<int>>, Nothing>::value);
    CHECK(IsSame<EnumAt<1, Maybe<int>>, int>::value);
}

#endif