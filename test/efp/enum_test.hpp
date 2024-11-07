#ifndef ENUM_TYPE_TEST_HPP_
#define ENUM_TYPE_TEST_HPP_

// #include "catch2/catch_test_macros.hpp"

#include "efp.hpp"
#include "test_common.hpp"

using namespace efp;

struct None {};

TEST_CASE("Enum Rule of 5", "Enum") {
    // use MockHW and MockRaii to check if the rule of 5 is followed
    SECTION("New constructor", "Enum") {
        SECTION("Trivially copyable") {
            Enum<int, double> a = 42;
            CHECK(a.index() == 0);
            CHECK(a.get<int>() == 42);
        }

        SECTION("Non-trivially copyable") {
            {
                MockHW::reset();
                Enum<int, MockRaii> a = MockRaii{};
                CHECK(MockHW::resource_state_to_int() == 1);
            }
            CHECK(MockHW::is_sound());
        }
    }

    SECTION("Copy Constructor") {
        SECTION("Trivially copyable") {
            Enum<int, double> a = 42;
            Enum<int, double> b = a;

            CHECK(a.index() == b.index());
            CHECK(a.get<int>() == b.get<int>());
        }

        SECTION("Non-trivially copyable") {
            {
                MockHW::reset();
                Enum<int, MockRaii> a = MockRaii{};
                CHECK(MockHW::resource_state_to_int() == 1);

                Enum<int, MockRaii> b = a;
                CHECK(MockHW::resource_state_to_int() == 12);
            }
            CHECK(MockHW::is_sound());
        }
    }

    SECTION("Copy Assignment") {
        SECTION("Trivially copyable") {
            Enum<int, double> a = 42;
            Enum<int, double> b = 0;
            b = a;

            CHECK(a.index() == b.index());
            CHECK(a.get<int>() == b.get<int>());
        }

        SECTION("Non-trivially copyable") {
            {
                MockHW::reset();
                Enum<int, MockRaii> a = MockRaii{};
                CHECK(MockHW::resource_state_to_int() == 1);

                Enum<int, MockRaii> b = MockRaii{};
                CHECK(MockHW::resource_state_to_int() == 12);

                b = a;
                CHECK(MockHW::resource_state_to_int() == 13);
            }
            CHECK(MockHW::is_sound());
        }
    }

    SECTION("Move constructor") {
        SECTION("Trivially copyable") {
            Enum<int, double> a = 42;
            const auto a_index = a.index();
            Enum<int, double> b = efp::move(a);

            CHECK(b.index() == a_index);
            CHECK(b.get<int>() == 42);
        }

        SECTION("Non-trivially copyable") {
            {
                MockHW::reset();
                Enum<int, MockRaii> a = MockRaii{};
                CHECK(MockHW::resource_state_to_int() == 1);

                Enum<int, MockRaii> b = efp::move(a);
                CHECK(MockHW::resource_state_to_int() == 1);
            }
            CHECK(MockHW::is_sound());
        }
    }

    SECTION("Move Assignment") {
        SECTION("Trivially copyable") {
            Enum<int, double> a = 42;
            Enum<int, double> b = 0;
            b = efp::move(a);

            CHECK(b.index() == 0);
            CHECK(b.get<int>() == 42);
        }

        SECTION("Non-trivially copyable") {
            {
                MockHW::reset();
                Enum<int, MockRaii> a = MockRaii{};
                CHECK(MockHW::resource_state_to_int() == 1);

                Enum<int, MockRaii> b = MockRaii{};
                CHECK(MockHW::resource_state_to_int() == 12);

                b = efp::move(a);
                CHECK(MockHW::resource_state_to_int() == 1);
            }
            CHECK(MockHW::is_sound());
        }
    }
}

TEST_CASE("Equality", "Enum") {
    SECTION("Same type") {
        Enum<int, double> a = 42;
        Enum<int, double> b = 42;
        Enum<int, double> c = 0;

        CHECK(a == b);
        CHECK_FALSE(a == c);
    }

    SECTION("Different type") {
        Enum<int, double> a = 42;
        Enum<int, double> b = 42.;
        Enum<int, double> c = 0;

        CHECK(a != b);
        CHECK(a != c);
    }
}

TEST_CASE("WildCard") {
    SECTION("Void return") {
        bool wild_card_work = false;

        const auto wc = [&]() { wild_card_work = true; };

        const auto wrapped = detail::WildCardWrapper<CVRemoved<decltype(wc)>>{wc};
        wrapped(42);

        CHECK(wild_card_work == true);
    }

    SECTION("Non-void return") {
        const auto wc = [&]() { return 42; };

        const auto wrapped = detail::WildCardWrapper<CVRemoved<decltype(wc)>>{wc};

        CHECK(wrapped(unit) == 42);
    }
}

TEST_CASE("enum_type") {
    CHECK(sizeof(Enum<int, double>) == 16);
    Enum<int, double> a = 1;
    CHECK(a.index() == 0);
    CHECK(a.get<int>() == 1);
    CHECK(a.get<0>() == 1);

    Enum<int, double> b = 2.;
    CHECK(b.index() == 1);
    CHECK(b.get<double>() == 2.);
    CHECK(b.get<1>() == 2.);
    // ! Not catched because of abort()
    // CHECK(b.get<int>() == 1);
}

// ! Deprecated
// TEST_CASE("Enum Exteneded Constructor") {
//     SECTION("Explicit constructor") {
//         struct A {
//             A(bool arg) : value(arg) {}

//             bool value;
//         };

//         struct B {};

//         using SomeEnum = Enum<A, B>;

//         CHECK(IsConstructible<bool, bool>::value);
//         CHECK(IsConstructible<A, bool>::value);

//         CHECK(SomeEnum::IsUniquelyConstructible<bool>::value);

//         int idx_0 = (int)SomeEnum(true).index();
//         int idx_1 = (int)SomeEnum {A {true}}.index();

//         CHECK(idx_0 == idx_1);

//         // CHECK((int)SomeEnum{true}.index() == (int)SomeEnum{A{true}}.index());
//     }

// todo Make it works for aggregate construction as well
// SECTION("Non-explicit constructor") {
//     struct A {
//         bool value;
//     };
//     struct B {};

//     using SomeEnum = Enum<A, B>;

//     CHECK(IsConstructible<bool, bool>::value);
//     CHECK(IsConstructible<A, bool>::value);

//     CHECK(SomeEnum::IsUniquelyConstructible<bool>::value);

//     int idx_0 = (int)SomeEnum(true).index();
//     int idx_1 = (int)SomeEnum{A{true}}.index();

//     CHECK(idx_0 == idx_1);

//     // CHECK((int)SomeEnum{true}.index() == (int)SomeEnum{A{true}}.index());
// }

// ! Deprecated. Nested Enum does not support any shortcutss
// SECTION("Nested Enum") {
//     struct A {};

//     struct B {};

//     using Enum0 = Enum<A, B>;

//     struct C {};

//     using NestedEnum = Enum<C, Enum0>;

//     CHECK(NestedEnum {A {}}.index() == NestedEnum {Enum0 {A {}}}.index());
// }

// SECTION("Self referencing Enum") {
//     List<int> empty_list = Nil{};
//     List<int> one_list = Cons<int>{42, Nil{}};
// }
// }

// SECTION("Self referencing Enum") {
//     List<int> empty_list = Nil{};
//     List<int> one_list = Cons<int>{42, Nil{}};
// }

struct TemplateBranch {
    template <typename A>
    int operator()(const A& a) {
        return a * 2;
    }
};

TEST_CASE("enum_match") {
    SECTION("void0") {
        Enum<int, double> a = 42;
        double b = 0.;

        a.match([&](int x) { b += x; }, [&](double x) { b++; });

        //! Non-exhaustive match will not compile

        // a.match(
        //     [&](Unit x)
        //     { b = -1.; });

        CHECK(b == 42.);
    }

    SECTION("void1") {
        Enum<int, double> a = 42.;
        double b = 0.;

        a.match([&](int x) { b += x; }, [&](double x) { b++; });

        CHECK(b == 1.);
    }

    SECTION("non-void0") {
        Enum<Unit, int> a = unit;

        int b = a.match([&](Unit x) { return -1; }, [&](int x) { return 1; });

        CHECK(b == -1);
    }

    SECTION("non-void1") {
        Enum<Unit, int> a = 42;

        int b = a.match([&](Unit x) { return -1; }, [&](int x) { return x; });

        CHECK(b == 42);
    }

    SECTION("non-void2") {
        Enum<bool, int, double> a = 0;
        int b = 42;

        a.match(
            [&](bool x) { return b * 0; },
            [&](int x) { return b * 1; },
            [&](double x) { return b * 2; });

        CHECK(b == 42);
    }

    SECTION("non-void3") {
        Enum<bool, int, double, float, long> a = 0;
        int b = 42;

        a.match(
            [&](bool x) { return b * 0; },
            [&](int x) { return b * 1; },
            [&](double x) { return b * 2; },
            [&](float x) { return b * 3; },
            [&](long x) { return b * 4; });

        CHECK(b == 42);
    }

    SECTION("non-void3") {
        struct InitState {};

        struct AState {};

        struct BState {};

        struct ErrorState {};

        struct TerminatedState {};

        using MajorState = efp::Enum<InitState, AState, BState, ErrorState, TerminatedState>;

        MajorState a{InitState{}};

        int b = 42;

        a.match(
            [&](InitState x) { return b * 0; },
            [&](AState x) { return b * 1; },
            [&](BState x) { return b * 2; },
            [&](ErrorState x) { return b * 3; },
            [&](TerminatedState x) { return b * 4; });

        CHECK(b == 42);
    }

    SECTION("wild_card0") {
        Enum<Unit, int, double> a = unit;
        double b = 0.;

        a.match([&](int x) { b += 1; }, [&]() {});
        // ! Sould be failed to get compiled and show static assert message on top
        // a.match([&](int x) { b += 1; }, [&](double _) { b += 1; });
        // a.match([&](int x) { b += 1; }, [&](False _) {}, [&]() {});
        // a.match([&]() {}, [&](int x) { b += 1; });

        CHECK(b == 0.);
    }

    // todo Check priority of cosntructor argument then no match

    SECTION("wild_card1") {
        Enum<Unit, int, double> a = unit;

        const auto res = a.match([]() { return 42; });

        CHECK(res == 42);
    }

    SECTION("non-trivial0") {
#include <string>
        Enum<bool, std::string> a = std::string("Hello");
        int b = 42;

        a.match([&](bool x) { return b * 0; }, [&](std::string x) { return 42; });

        CHECK(b == 42);
    }

    SECTION("non-trivial0") {
#include <string>
        Enum<bool, std::string> a = std::string("Hello");
        int b = 42;

        a.match([&](bool x) { return b * 0; }, [&](const std::string& x) { return 42; });

        CHECK(b == 42);
    }

    // SECTION("Template Handler") {
    //     Enum<int, double> a = 42.;

    //     int b = a.match(TemplateBranch());
    //     CHECK(b == 84);
    // }
}

// // A test function to be pointed to
// int enum_test_function() {
//     return 42;
// }

// TEST_CASE("Function Pointer in Enum") {
//     // SECTION("Storing and retrieving function pointer") {
//     //     // Create an Enum instance holding a function pointer
//     //     Enum<int (*)()> my_enum_func_ptr = enum_test_function;

//     //     // Retrieve the function pointer and store it in a variable
//     //     int (*retrieved_func_ptr)() = my_enum_func_ptr.get<int (*)()>();

//     //     // Test if the retrieved function pointer is equal to the original one
//     //     CHECK(retrieved_func_ptr == &enum_test_function);
//     // }

//     // SECTION("Storing and retrieving lambda") {
//     //     // ! For some reason this does not work with const qualifier
//     //     auto enum_test_lambda = [&]() { return 84; };

//     //     // Create an Enum instance holding a lambda
//     //     Enum<decltype(enum_test_lambda)> my_enum_lambda(enum_test_lambda);

//     //     // Retrieve the lambda and store it in a variable
//     //     auto retrieved_lambda = my_enum_lambda.get<decltype(enum_test_lambda)>();

//     //     // Test if the retrieved lambda is equal to the original one
//     //     // This check confirms that the lambda is stored and retrieved correctly.
//     //     // Note: Direct lambda comparison might not be feasible; instead, test the behavior.
//     //     CHECK(retrieved_lambda() == enum_test_lambda());
//     // }

//     // SECTION("Using function name directly") {
//     //     // Create an Enum instance using the function name directly
//     //     Enum<int (*)()> my_enum_func_name = enum_test_function;

//     //     // Retrieve the function pointer and store it in a variable
//     //     int (*retrieved_func_name)() = my_enum_func_name.get<int (*)()>();

//     //     // Test if the retrieved function pointer is equal to the original one
//     //     CHECK(retrieved_func_name == &enum_test_function);
//     // }
// }

TEST_CASE("EnumAt") {
    CHECK(IsSame<EnumAt<0, Enum<int, double>>, int>::value);
    CHECK(IsSame<EnumAt<1, Enum<int, double>>, double>::value);
}

#endif