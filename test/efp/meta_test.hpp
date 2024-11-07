#ifndef META_TEST_HPP_
#define META_TEST_HPP_

#include "test_common.hpp"

using namespace efp;

// TEST_CASE("clamp")
// {
//     CHECK(clamp(0, 2, CtConst<size_t, -1>{}) == 0);
//     CHECK(clamp(0, 2, CtConst<size_t, 1>{}) == 1);
//     CHECK(clamp(0, 2, CtConst<size_t, 3>{}) == 2);
//     CHECK(clamp(0, CtConst<size_t, 3>{}, -1) == 0);
//     // ! issue
//     // CHECK((-1 > CtConst<size_t, 3>{}) == true);
//     // CHECK((-1 > CtConst<size_t, 3>{}) ? (CtConst<int, 3>{}) : ((-1 < 0) ? 0 : -1) == 0);
// }

TEST_CASE("_and") {
    // CHECK(_and({}) == true);
    CHECK(_and({true}) == true);
    CHECK(_and({false}) == false);
    CHECK(_and({true, false}) == false);
    CHECK(_and({false, true}) == false);
}

TEST_CASE("_or") {
    // CHECK(_or({}) == false);
    CHECK(_or({true}) == true);
    CHECK(_or({false}) == false);
    CHECK(_or({true, false}) == true);
    CHECK(_or({false, true}) == true);
}

TEST_CASE("_maximum") {
    CHECK(_maximum({0}) == 0);
    CHECK(_maximum({0, 1}) == 1);
    CHECK(_maximum({-1, 1}) == 1);
    CHECK(_maximum({1., 2., 3.}) == 3.);
    CHECK(_maximum({-1., 2., 3.}) == 3.);
}

TEST_CASE("_minimum") {
    CHECK(_minimum({0}) == 0);
    CHECK(_minimum({1, 0}) == 0);
    CHECK(_minimum({1, -1}) == -1);
    CHECK(_minimum({1, 2, 3}) == 1);
    CHECK(_minimum({1., 2., -3.}) == -3.);
}

TEST_CASE("IsSame") {
    CHECK(IsSame<void, double>::value == false);
    CHECK(IsSame<int, double>::value == false);
    CHECK(IsSame<int, int>::value == true);
    CHECK(IsSame<int, int&>::value == false);
    CHECK(IsSame<int, const int>::value == false);
}

TEST_CASE("PackAt") {
    CHECK(IsSame<PackAt<0, int, double>, int>::value == true);
    CHECK(IsSame<PackAt<0, int, double>, double>::value == false);
    CHECK(IsSame<PackAt<1, int, double>, int>::value == false);
    CHECK(IsSame<PackAt<1, int, double>, double>::value == true);
}

template <typename A>
struct IsInt {
    static constexpr bool value = false;
};

template <>
struct IsInt<int> {
    static constexpr bool value = true;
};

TEST_CASE("FindIndex") {
    CHECK(FindIndex<IsInt, int, int, float>::value == 0);
    CHECK(FindIndex<IsInt, bool, int, float>::value == 1);
    CHECK(FindIndex<IsInt, bool, float, int>::value == 2);
}

TEST_CASE("HasCallOperator") {
    struct No {};

    struct Yes {
        void operator()(int x){};
    };

    auto lambda = [](int x) {};

    CHECK(HasCallOperator<No>::value == false);
    CHECK(HasCallOperator<Yes>::value == true);
    CHECK(HasCallOperator<decltype(lambda)>::value == true);
}

void argument_t_function0(int x0, float&& x1) {}

void argument_t_function1(const int x0, float& x1) {}

void argument_t_function2() {}

TEST_CASE("Arguments") {
    auto argument_t_lambda0 = [](int x0, float&& x1) {};
    auto argument_t_lambda1 = [](const int x0, float& x1) {};
    auto argument_t_lambda2 = []() {};

    CHECK(IsSame<Tuple<int, float>, Arguments<decltype(&argument_t_function0)>>::value == false);

    // Can catch l-value reference r-value reference
    CHECK(IsSame<Tuple<int, float&&>, Arguments<decltype(&argument_t_function0)>>::value == true);

    // Can't catch const qualifier
    CHECK(
        IsSame<Tuple<const int, float&>, Arguments<decltype(&argument_t_function1)>>::value == false);

    // const quialifier will be removed at the result
    CHECK(IsSame<Tuple<int, float&>, Arguments<decltype(&argument_t_function1)>>::value == true);

    // Catching function not taking any argument with empty tuple
    CHECK(IsSame<Tuple<>, Arguments<decltype(&argument_t_function2)>>::value == true);

    CHECK(IsSame<Tuple<void>, Arguments<decltype(&argument_t_function2)>>::value == false);

    // l-value reference will be preserved
    CHECK(IsSame<Tuple<int, float>, Arguments<decltype(argument_t_lambda0)>>::value == false);

    // Can catch l-value reference r-value reference
    CHECK(IsSame<Tuple<int, float&&>, Arguments<decltype(argument_t_lambda0)>>::value == true);

    // Can't catch const qualifier
    CHECK(
        IsSame<Tuple<const int, float&>, Arguments<decltype(argument_t_lambda1)>>::value == false);

    CHECK(IsSame<Tuple<int, float&>, Arguments<decltype(argument_t_lambda1)>>::value == true);

    // Catching lambda not taking any argument with empty tuple
    CHECK(IsSame<Tuple<>, Arguments<decltype(argument_t_lambda2)>>::value == true);

    CHECK(IsSame<Tuple<void>, Arguments<decltype(argument_t_lambda2)>>::value == false);
}

double return_t_function0(int x0, float&& x1) {
    return 0.;
}

double* return_t_function1(const int x0, float& x1) {
    return nullptr;
}

// ! Deprecate. Use InvokeResult instead
// TEST_CASE("Return") {
//     auto return_t_lambda0 = [](int x0, float x1) { return x1 + x0; };
//     auto return_t_lambda1 = [](const int x0, float& x1) { return x0 + x1; };
//     auto return_t_lambda2 = [](const int x0, float& x1) {};

//     CHECK(IsSame<double, Return<decltype(&return_t_function0)>>::value == true);

//     CHECK(IsSame<double&, Return<decltype(&return_t_function0)>>::value == false);

//     CHECK(IsSame<double*, Return<decltype(&return_t_function1)>>::value == true);

//     CHECK(IsSame<double, Return<decltype(&return_t_function1)>>::value == false);

//     CHECK(IsSame<float, Return<decltype(return_t_lambda0)>>::value == true);

//     CHECK(IsSame<int&, Return<decltype(return_t_lambda0)>>::value == false);

//     CHECK(IsSame<float, Return<decltype(return_t_lambda1)>>::value == true);

//     CHECK(IsSame<float*, Return<decltype(return_t_lambda1)>>::value == false);

//     // Does catch void return

//     CHECK(IsSame<void, Return<decltype(return_t_lambda2)>>::value == true);
// }

int is_invocable_add(int a, int b) {
    return a + b;
}

TEST_CASE("IsInvocable") {
    CHECK(IsInvocable<decltype(&is_invocable_add), int, int>::value == true);
    CHECK(IsInvocable<decltype(&is_invocable_add), double, double>::value == true);
    CHECK(IsInvocable<decltype(&is_invocable_add), double, Unit>::value == false);
}

TEST_CASE("Tuple") {
    SECTION("const") {
        const Tuple<bool, int> tpl{true, 42};
        CHECK(tpl.template get<0>() == true);
        CHECK(tpl.template get<1>() == 42);
        CHECK(get<0>(tpl) == true);
        CHECK(get<1>(tpl) == 42);
        CHECK(p<0>(tpl) == true);
        CHECK(p<1>(tpl) == 42);
    }

    SECTION("non const") {
        Tuple<bool, int> tpl{true, 42};
        CHECK(tpl.template get<0>() == true);
        CHECK(tpl.template get<1>() == 42);
        CHECK(get<0>(tpl) == true);
        CHECK(get<1>(tpl) == 42);
        CHECK(p<0>(tpl) == true);
        CHECK(p<1>(tpl) == 42);

        tpl.template get<0>() = false;
        tpl.template get<1>() = 0;

        CHECK(tpl.template get<0>() == false);
        CHECK(tpl.template get<1>() == 0);
        CHECK(get<0>(tpl) == false);
        CHECK(get<1>(tpl) == 0);
        CHECK(p<0>(tpl) == false);
        CHECK(p<1>(tpl) == 0);
    }
}

// Define a function pointer type
using FuncPtrType = int (*)();

// A test function for the function pointer
int test_function() {
    return 42;
}

TEST_CASE("Tuple with Function Pointer and Lambda") {
    SECTION("Storing and retrieving function pointer and lambda") {
        auto test_lambda = []() { return 42; };

        // Create a tuple with a function pointer and a lambda
        Tuple<FuncPtrType, decltype(test_lambda)> my_tuple = {&test_function, test_lambda};

        // Retrieve and test the function pointer
        CHECK(my_tuple.get<0>()() == 42);
        CHECK(my_tuple.get<1>()() == 42);
    }
}

TEST_CASE("Tuple match") {
    SECTION("case 0") {
        const Tuple<bool, int> tpl{true, 42};
        CHECK(tpl.match([](bool p, int x) { return p ? x : -x; }) == 42);
    }

    SECTION("case 1") {
        const Tuple<bool, int> tpl{false, 42};
        CHECK(tpl.match([](bool p, int x) { return p ? x : -x; }) == -42);
    }

    SECTION("case empty") {
        const Tuple<> empty{};
        CHECK(empty.match([]() { return 42; }) == 42);
    }
}

TEST_CASE("tuple") {
    CHECK(Tuple<bool, int>{true, 42} == tuple(true, 42));
    CHECK(Tuple<>{} == tuple());
}

// ! Deprecate. Use invoke instead
// TEST_CASE("apply") {
//     SECTION("0") {
//         const auto tpl = tuple();
//         const auto f = []() { return unit; };

//         CHECK(apply(f, tpl) == unit);
//     }

//     SECTION("1") {
//         const auto tpl = tuple(true, 42);
//         const auto f = [](bool a, int b) { return unit; };

//         CHECK(apply(f, tpl) == unit);
//     }

//     SECTION("2") {
//         const auto tpl = tuple(40, 2);
//         const auto add = [](int a, int b) { return a + b; };

//         CHECK(apply(add, tpl) == 42);
//     }
// }

#endif