#ifndef SFINAE_TEST_HPP_
#define SFINAE_TEST_HPP_

#include "catch2/catch_test_macros.hpp"

#include "efp.hpp"
#include "test_common.hpp"

using namespace efp;

// TEST_CASE("bound_v")
// {
//     CHECK(bound_v(0, 2, IntegralConst<int, -1>{}) == 0);
//     CHECK(bound_v(0, 2, IntegralConst<int, 1>{}) == 1);
//     CHECK(bound_v(0, 2, IntegralConst<int, 3>{}) == 2);
//     CHECK(bound_v(0, IntegralConst<size_t, 3>{}, -1) == 0);
//     // ! issue
//     // CHECK((-1 > IntegralConst<size_t, 3>{}) == true);
//     // CHECK((-1 > IntegralConst<size_t, 3>{}) ? (IntegralConst<size_t, 3>{}) : ((-1 < 0) ? 0 : -1) == 0);
// }

TEST_CASE("all_v")
{
    CHECK(all_v() == true);
    CHECK(all_v(true) == true);
    CHECK(all_v(false) == false);
    CHECK(all_v(true, false) == false);
    CHECK(all_v(false, true) == false);
}

TEST_CASE("any_v")
{
    CHECK(any_v() == false);
    CHECK(any_v(true) == true);
    CHECK(any_v(false) == false);
    CHECK(any_v(true, false) == true);
    CHECK(any_v(false, true) == true);
}

TEST_CASE("maximum_v")
{
    CHECK(maximum_v(0) == 0);
    CHECK(maximum_v(0, 1) == 1);
    CHECK(maximum_v(-1, 1) == 1);
    CHECK(maximum_v(1., 2., 3) == 3.);
    CHECK(maximum_v(-1., 2, 3.) == 3.);
}

TEST_CASE("minimum_v")
{
    CHECK(minimum_v(0) == 0);
    CHECK(minimum_v(1, 0) == 0);
    CHECK(minimum_v(1u, -1) == 1);
    CHECK(minimum_v(1, 2., 3) == 1);
    CHECK(minimum_v(1, 2, -3.) == -3);
}

TEST_CASE("IsSame")
{
    CHECK(IsSame<void, double>::value == false);
    CHECK(IsSame<int, double>::value == false);
    CHECK(IsSame<int, int>::value == true);
    CHECK(IsSame<int, int &>::value == false);
    CHECK(IsSame<int, const int>::value == false);
}

TEST_CASE("PackAt")
{
    CHECK(IsSame<PackAt<0, int, double>, int>::value == true);
    CHECK(IsSame<PackAt<0, int, double>, double>::value == false);
    CHECK(IsSame<PackAt<1, int, double>, int>::value == false);
    CHECK(IsSame<PackAt<1, int, double>, double>::value == true);
}

template <typename A>
struct IsInt
{
    static constexpr bool value = false;
};

template <>
struct IsInt<int>
{
    static constexpr bool value = true;
};

TEST_CASE("Find")
{
    CHECK(Find<IsInt, int, int, float>::value == 0);
    CHECK(Find<IsInt, bool, int, float>::value == 1);
    CHECK(Find<IsInt, bool, float, int>::value == 2);
}

TEST_CASE("IsCallOperator")
{
    struct No
    {
    };

    struct Yes
    {
        void operator()(int x){};
    };

    auto lambda = [](int x) {};

    CHECK(IsCallOperator<No>::value == false);
    CHECK(IsCallOperator<Yes>::value == true);
    CHECK(IsCallOperator<decltype(lambda)>::value == true);
}

void argument_t_function0(int x0, float &&x1)
{
}

void argument_t_function1(const int x0, float &x1)
{
}

void argument_t_function2()
{
}

TEST_CASE("Arguments")
{
    auto argument_t_lambda0 = [](int x0, float &&x1) {};
    auto argument_t_lambda1 = [](const int x0, float &x1) {};
    auto argument_t_lambda2 = []() {};

    CHECK(IsSame<
              Tuple<int, float>,
              Arguments<decltype(&argument_t_function0)>>::value == false);

    // Can catch l-value reference r-value reference
    CHECK(IsSame<
              Tuple<int, float &&>,
              Arguments<decltype(&argument_t_function0)>>::value == true);

    // Can't catch const qualifier
    CHECK(IsSame<
              Tuple<const int, float &>,
              Arguments<decltype(&argument_t_function1)>>::value == false);

    // const quialifier will be removed at the result
    CHECK(IsSame<
              Tuple<int, float &>,
              Arguments<decltype(&argument_t_function1)>>::value == true);

    // Catching function not taking any argument with empty tuple
    CHECK(IsSame<
              Tuple<>,
              Arguments<decltype(&argument_t_function2)>>::value == true);

    CHECK(IsSame<
              Tuple<void>,
              Arguments<decltype(&argument_t_function2)>>::value == false);

    // l-value reference will be preserved
    CHECK(IsSame<
              Tuple<int, float>,
              Arguments<decltype(argument_t_lambda0)>>::value == false);

    // Can catch l-value reference r-value reference
    CHECK(IsSame<
              Tuple<int, float &&>,
              Arguments<decltype(argument_t_lambda0)>>::value == true);

    // Can't catch const qualifier
    CHECK(IsSame<
              Tuple<const int, float &>,
              Arguments<decltype(argument_t_lambda1)>>::value == false);

    CHECK(IsSame<
              Tuple<int, float &>,
              Arguments<decltype(argument_t_lambda1)>>::value == true);

    // Catching lambda not taking any argument with empty tuple
    CHECK(IsSame<
              Tuple<>,
              Arguments<decltype(argument_t_lambda2)>>::value == true);

    CHECK(IsSame<
              Tuple<void>,
              Arguments<decltype(argument_t_lambda2)>>::value == false);
}

double return_t_function0(int x0, float &&x1)
{
    return 0.;
}

double *return_t_function1(const int x0, float &x1)
{
    return nullptr;
}

TEST_CASE("Return")
{
    auto return_t_lambda0 = [](int x0, float x1)
    { return x1 + x0; };
    auto return_t_lambda1 = [](const int x0, float &x1)
    { return x0 + x1; };
    auto return_t_lambda2 = [](const int x0, float &x1) {};

    CHECK(IsSame<
              double,
              Return<decltype(&return_t_function0)>>::value == true);

    CHECK(IsSame<
              double &,
              Return<decltype(&return_t_function0)>>::value == false);

    CHECK(IsSame<
              double *,
              Return<decltype(&return_t_function1)>>::value == true);

    CHECK(IsSame<
              double,
              Return<decltype(&return_t_function1)>>::value == false);

    CHECK(IsSame<
              float,
              Return<decltype(return_t_lambda0)>>::value == true);

    CHECK(IsSame<
              int &,
              Return<decltype(return_t_lambda0)>>::value == false);

    CHECK(IsSame<
              float,
              Return<decltype(return_t_lambda1)>>::value == true);

    CHECK(IsSame<
              float *,
              Return<decltype(return_t_lambda1)>>::value == false);

    // Does catch void return

    CHECK(IsSame<
              void,
              Return<decltype(return_t_lambda2)>>::value == true);
}

int is_invocable_add(int a, int b)
{
    return a + b;
}

TEST_CASE("IsInvocable")
{
    CHECK(IsInvocable<decltype(&is_invocable_add), int, int>::value == true);
    CHECK(IsInvocable<decltype(&is_invocable_add), double, double>::value == true);
    CHECK(IsInvocable<decltype(&is_invocable_add), double, Unit>::value == false);
}

#endif