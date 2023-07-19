#ifndef PRELUDE_HPP_
#define PRELUDE_HPP_

#include <array>
#include <vector>
#include <iterator>
#include <functional>
#include <algorithm>
#include <utility>
#include <type_traits>
#include <numeric>
#include <tuple>

#include "zero_copy.hpp"

namespace efp
{
    template <typename A>
    A id(const A &a)
    {
        return a;
    }

    // ! WIP
    // Composed

    // template <typename... Fs>
    // struct Composed;

    template <typename F, typename G>
    struct Composed
    {
        const F &f;
        const G &g;

        Composed(const F &f, const G &g) : f(f), g(g) {}

        template <typename Arg>

        auto operator()(Arg &&arg) const -> decltype(f(g(std::forward<Arg>(arg))))
        {
            return f(g(std::forward<Arg>(arg)));
        }
    };

    // template <typename F, typename... Fs>
    // struct Composed<F, Fs...>
    // {
    //     const F &f;
    //     const Composed<Fs...> fs;

    //     Composed(const F &f, const Fs &...fs) : f(f), fs(compose(fs...)) {}

    //     template <typename Arg>

    //     inline auto operator()(Arg &&arg) -> decltype(f(fs(std::forward<Arg>(arg))))
    //     {
    //         return f(fs(std::forward<Arg>(arg)));
    //     }
    // };

    // template <typename F, typename G>
    // inline auto operator*(F f, G g) -> Composed<F, G>
    // {
    //     return compose(f, g);
    // }

    // compose

    template <typename F, typename G>
    auto compose(const F &f, const G &g) -> Composed<F, G>
    {
        return Composed<F, G>(f, g);
    }

    // template <typename F, typename... Fs>
    // auto compose(const F &f, const Fs &...fs) -> Composed<F, Fs...>
    // {
    //     return Composed<F, Fs...>(f, fs...);
    // }

    // All

    template <typename... Args>
    struct All;

    template <>
    struct All<>
    {
        static constexpr bool value = true;
    };

    template <typename Head, typename... Tail>
    struct All<Head, Tail...>
    {
        static constexpr bool value = Head::value && All<Tail...>::value;
    };

    // Any

    template <typename... Args>
    struct Any;

    template <>
    struct Any<>
    {
        static constexpr bool value = false;
    };

    template <typename Head, typename... Tail>
    struct Any<Head, Tail...>
    {
        static constexpr bool value = Head::value || Any<Tail...>::value;
    };

    // RemoveReference

    template <typename A>
    using RemoveReference_t = typename std::remove_reference<A>::type;

    // CommonType

    template <typename... Args>
    using Common_t = typename std::common_type<Args...>::type;

    // IsStaticCapacity

    template <typename SeqA>
    struct IsStaticCapacity : std::false_type
    {
    };

    template <typename A, size_t N>
    struct IsStaticCapacity<StaticArray<A, N>> : std::true_type
    {
    };

    template <typename A, size_t N>
    struct IsStaticCapacity<StaticVector<A, N>> : std::true_type
    {
    };

    template <typename A, size_t N>
    struct IsStaticCapacity<std::array<A, N>> : std::true_type
    {
    };

    template <typename A, size_t N>
    struct IsStaticCapacity<A[N]> : std::true_type
    {
    };

    template <typename T>
    struct IsStaticCapacity<T &> : IsStaticCapacity<T>
    {
    };

    template <typename T>
    struct IsStaticCapacity<T &&> : IsStaticCapacity<T>
    {
    };

    // AreAllStaticCapacity

    template <typename... Seqs>
    struct AreAllStaticCapacity
    {
        static constexpr bool value = All<IsStaticCapacity<Seqs>...>::value;
    };

    // StaticCapacity

    template <typename A>
    struct StaticCapacity
    {
        static constexpr size_t value = 0;
    };

    template <typename A, size_t N>
    struct StaticCapacity<A[N]>
    {
        static constexpr size_t value = N;
    };

    template <typename A, size_t N>
    struct StaticCapacity<std::array<A, N>>
    {
        static constexpr size_t value = N;
    };

    template <typename A, size_t N>
    struct StaticCapacity<StaticArray<A, N>>
    {
        static constexpr size_t value = N;
    };

    template <typename A, size_t N>
    struct StaticCapacity<StaticVector<A, N>>
    {
        static constexpr size_t value = N;
    };

    template <typename A>
    struct StaticCapacity<A &> : StaticCapacity<A>
    {
    };

    template <typename A>
    struct StaticCapacity<A &&> : StaticCapacity<A>
    {
    };

    // IsStaticLength

    template <typename T>
    struct IsStaticLength : std::false_type
    {
    };

    template <typename A, size_t N>
    struct IsStaticLength<StaticArray<A, N>> : std::true_type
    {
    };

    template <typename A, size_t N>
    struct IsStaticLength<std::array<A, N>> : std::true_type
    {
    };

    template <typename A, size_t N>
    struct IsStaticLength<A[N]> : std::true_type
    {
    };

    template <typename T>
    struct IsStaticLength<T &> : IsStaticLength<T>
    {
    };

    template <typename T>
    struct IsStaticLength<T &&> : IsStaticLength<T>
    {
    };

    // StaticLength

    template <typename SeqA>
    struct StaticLength;

    template <typename A, size_t N>
    struct StaticLength<StaticArray<A, N>>
    {
        static constexpr size_t value = N;
    };

    template <typename A, size_t N>
    struct StaticLength<std::array<A, N>>
    {
        static constexpr size_t value = N;
    };

    template <typename A, size_t N>
    struct StaticLength<A[N]>
    {
        static constexpr size_t value = N;
    };

    template <typename SeqA>
    struct StaticLength<SeqA &> : StaticLength<SeqA>
    {
    };

    template <typename SeqA>
    struct StaticLength<SeqA &&> : StaticLength<SeqA>
    {
    };

    // IsIntegralConstant

    template <typename T>
    struct IsIntegralConstant : std::false_type
    {
    };

    template <typename T, T Value>
    struct IsIntegralConstant<std::integral_constant<T, Value>> : std::true_type
    {
    };

    // StaticSizeT

    template <size_t N>
    using StaticSizeT = std::integral_constant<size_t, N>;

    // min_size_t

    template <typename A>
    constexpr A min_size_t(const A &value)
    {
        return value;
    }

    template <typename Head, typename... Tail>
    constexpr auto min_size_t(const Head &head, const Tail &...tail)
        -> typename std::conditional<
            All<IsIntegralConstant<Head>, IsIntegralConstant<Tail>...>::value,
            Head,
            size_t>::type
    {
        return head < min_size_t(tail...) ? head : min_size_t(tail...);
    }

    // size_t_product

    template <typename A>
    constexpr A size_t_product(const A &value)
    {
        return value;
    }

    template <typename Head, typename... Tail>
    constexpr auto size_t_product(const Head &head, const Tail &...tail)
        -> typename std::conditional<
            All<IsIntegralConstant<Head>, IsIntegralConstant<Tail>...>::value,
            Head,
            size_t>::type
    {
        return head * size_t_product(tail...);
    }

    // MinStaticCapacity

    template <typename... Seqs>
    struct MinStaticCapacity;

    template <typename Seq>
    struct MinStaticCapacity<Seq>
    {
        static constexpr size_t value = StaticCapacity<Seq>::value;
    };

    template <typename Head, typename... Tail>
    struct MinStaticCapacity<Head, Tail...>
    {
        static constexpr size_t head = StaticCapacity<Head>::value;
        static constexpr size_t tail = min_size_t(StaticCapacity<Tail>::value...);
        static constexpr size_t value = head < tail ? head : tail;
    };

    // StaticCapacityProduct

    template <typename... Seqs>
    struct StaticCapacityProduct;

    template <typename SeqA>
    struct StaticCapacityProduct<SeqA>
    {
        static constexpr size_t value = StaticCapacity<SeqA>::value;
    };

    template <typename Head, typename... Tail>
    struct StaticCapacityProduct<Head, Tail...>
    {
        static constexpr size_t value = StaticCapacity<Head>::value * StaticCapacityProduct<Tail...>::value;
    };

    // length

    template <typename SeqA>
    size_t length(const SeqA &as)
    {
        return as.size();
    }

    template <typename A, size_t N>
    auto length(const StaticArray<A, N> &)
        -> std::integral_constant<size_t, N>
    {
        return std::integral_constant<size_t, N>{};
    }

    template <typename A, size_t N>
    auto length(const std::array<A, N> &)
        -> std::integral_constant<size_t, N>
    {
        return std::integral_constant<size_t, N>{};
    }

    template <typename A, size_t N>
    auto length(const A (&)[N])
        -> std::integral_constant<size_t, N>
    {
        return std::integral_constant<size_t, N>{};
    }

    // min_length

    template <typename SeqA>
    constexpr auto min_length(const SeqA &as)
        -> typename std::conditional<
            IsStaticLength<SeqA>::value,
            decltype(length(as)),
            size_t>::type
    {
        return length(as);
    }

    template <typename Head, typename... Tail>
    constexpr auto min_length(const Head &head, const Tail &...tail)
        -> typename std::conditional<
            All<IsIntegralConstant<Head>, IsIntegralConstant<Tail>...>::value,
            decltype(length(head)),
            size_t>::type
    {
        return length(head) < min_length(tail...) ? length(head) : min_length(tail...);
    }

    // for_each

    template <typename F, typename... Seqs>
    void for_each(const F &f, const Seqs &...seqs)
    {
        // ? Will it be optimized out to a compile time constatnt?
        const size_t seq_length = min_length(seqs...);

        for (int i = 0; i < seq_length; ++i)
        {
            f(seqs[i]...);
        }
    }

    // ElementType

    template <typename A>
    struct ElementType
    {
        using type = typename A::value_type;
    };

    template <typename A, size_t N>
    struct ElementType<A[N]>
    {
        using type = A;
    };

    template <typename A>
    struct ElementType<A &> : ElementType<A>
    {
    };

    template <typename A>
    struct ElementType<A &&> : ElementType<A>
    {
    };

    template <typename A>
    using Element_t = typename ElementType<A>::type;

    // MapSequence_t

    template <typename A, typename... Seqs>
    using MapSequence_t =
        typename std::conditional<
            All<IsStaticCapacity<Seqs>...>::value,
            typename std::conditional<
                All<IsStaticLength<Seqs>...>::value,
                StaticArray<A, MinStaticCapacity<Seqs...>::value>,
                StaticVector<A, MinStaticCapacity<Seqs...>::value>>::type,
            DynamicVector<A>>::type;

    // CallReturnType;

    template <typename, typename...>
    struct CallReturnType;

    template <typename F, typename... Args>
    struct CallReturnType
    {
        using type = decltype(std::declval<F>()(std::declval<Args>()...));
    };

    template <typename F, typename... Args>
    using CallReturn_t = typename CallReturnType<F, Args...>::type;

    // MapReturn_t

    template <typename F, typename... Seqs>
    using MapReturn_t = MapSequence_t<CallReturn_t<F, Element_t<Seqs>...>, Seqs...>;

    // map

    template <typename F, typename... Seqs>
    auto map(const F &f, const Seqs &...seqs)
        -> typename std::enable_if<
            All<IsStaticCapacity<Seqs>...>::value && All<IsStaticLength<Seqs>...>::value,
            StaticArray<CallReturn_t<F, Element_t<Seqs>...>, MinStaticCapacity<Seqs...>::value>>::type
    {
        using R = CallReturn_t<F, Element_t<Seqs>...>;

        StaticArray<R, MinStaticCapacity<Seqs...>::value> result;

        for (int i = 0; i < MinStaticCapacity<Seqs...>::value; ++i)
        {
            result[i] = f(seqs[i]...);
        }

        return result;
    }

    template <typename F, typename... Seqs>
    auto map(const F &f, const Seqs &...seqs)
        -> typename std::enable_if<
            All<IsStaticCapacity<Seqs>...>::value && !All<IsStaticLength<Seqs>...>::value,
            StaticVector<CallReturn_t<F, Element_t<Seqs>...>, MinStaticCapacity<Seqs...>::value>>::type
    {
        using R = CallReturn_t<F, Element_t<Seqs>...>;

        const size_t result_length = min_length(seqs...);

        StaticVector<R, MinStaticCapacity<Seqs...>::value> result(result_length);

        for (int i = 0; i < result_length; ++i)
        {
            result[i] = f(seqs[i]...);
        }

        return result;
    }

    template <typename F, typename... Seqs>
    auto map(const F &f, const Seqs &...seqs)
        -> typename std::enable_if<
            !All<IsStaticCapacity<Seqs>...>::value,
            DynamicVector<CallReturn_t<F, Element_t<Seqs>...>>>::type
    {
        using R = CallReturn_t<F, Element_t<Seqs>...>;

        const size_t result_length = min_length(seqs...);

        DynamicVector<CallReturn_t<F, Element_t<Seqs>...>> result(result_length);

        for (int i = 0; i < result_length; ++i)
        {
            result[i] = f(seqs[i]...);
        }

        return result;
    }

    // MapWithIndexReturn_t

    template <typename F, typename... Seqs>
    using MapWithIndexReturn_t = MapSequence_t<CallReturn_t<F, int, Element_t<Seqs>...>, Seqs...>;

    // FilterReturn_t

    template <typename SeqA>
    using FilterReturn_t =
        typename std::conditional<
            IsStaticCapacity<SeqA>::value,
            StaticVector<Element_t<SeqA>, StaticCapacity<SeqA>::value>,
            DynamicVector<Element_t<SeqA>>>::type;

    // filter

    template <typename F, typename SeqA>
    auto filter(const F &f, const SeqA &as)
        -> typename std::enable_if<
            IsStaticCapacity<SeqA>::value,
            StaticVector<Element_t<SeqA>, StaticCapacity<SeqA>::value>>::type
    {
        StaticVector<Element_t<SeqA>, StaticCapacity<SeqA>::value> result;

        for (int i = 0; i < length(as); ++i)
        {
            const auto a = as[i];
            if (f(a))
            {
                result.push_back(a);
            }
        }

        return result;
    }

    template <typename F, typename SeqA>
    auto filter(const F &f, const SeqA &as)
        -> typename std::enable_if<
            !IsStaticCapacity<SeqA>::value,
            DynamicVector<Element_t<SeqA>>>::type
    {
        const size_t sequance_length = length(as);

        DynamicVector<Element_t<SeqA>> result;
        result.reserve(sequance_length * 2);

        for (int i = 0; i < sequance_length; ++i)
        {
            const auto a = as[i];
            if (f(a))
            {
                result.push_back(a);
            }
        }

        return result;
    }

    // foldl

    template <typename F, typename R, typename SeqA>
    R foldl(const F &f, const R &initial_value, const SeqA &as)
    {
        R result = initial_value;

        for (int i = 0; i < length(as); ++i)
        {
            result = f(result, as[i]);
        }

        return result;
    }

    // foldr

    template <typename F, typename R, typename SeqA>
    R foldr(const F &f, const R &initial_value, const SeqA &as)
    {
        R result = initial_value;

        for (int i = length(as) - 1; i > -1; --i)
        {
            result = f(as[i], result);
        }

        return result;
    }

    // FromFunctionReturn_t

    template <typename N, typename F>
    using FromFunctionReturn_t = typename std::conditional<
        IsIntegralConstant<N>::value,
        StaticArray<CallReturn_t<F, int>, N::value>,
        DynamicVector<CallReturn_t<F, int>>>::type;

    // from_function

    template <typename N, typename F>
    auto from_function(const N &length, const F &f)
        -> typename std::enable_if<
            IsIntegralConstant<N>::value,
            StaticArray<CallReturn_t<F, int>, N::value>>::type
    {
        StaticArray<CallReturn_t<F, int>, N::value> result;

        for (int i = 0; i < N::value; ++i)
        {
            result[i] = f(i);
        }

        return result;
    }

    template <typename N, typename F>
    auto from_function(const N &length, const F &f)
        -> typename std::enable_if<
            !IsIntegralConstant<N>::value,
            DynamicVector<CallReturn_t<F, int>>>::type
    {
        DynamicVector<CallReturn_t<F, int>> result(length);

        // result.reserve(length * 2);

        for (int i = 0; i < length; ++i)
        {
            result[i] = f(i);
        }

        return result;
    }

    // for_each_with_index

    template <typename F, typename... Seqs>
    void for_each_with_index(const F &f, const Seqs &...seqs)
    {
        // ? Will it be optimized out to a compile time constatnt?
        const size_t seq_length = min_length(seqs...);

        for (int i = 0; i < seq_length; ++i)
        {
            f(i, seqs[i]...);
        }
    }

    // cartesian_for_each

    template <typename F, typename SeqA>
    void cartesian_for_each(const F &f, const SeqA &as)
    {
        for_each(f, as);
    }

    template <typename F, typename SeqA, typename... Seqs>
    void cartesian_for_each(const F &f, const SeqA &as, const Seqs &...seqs)
    {
        // ? Will it be optimized out to a compile time constatnt?
        const size_t as_length = length(as);
        Element_t<SeqA> a;

        for (int i = 0; i < as_length; ++i)
        {
            a = as[i];
            const auto inner = [=](Element_t<Seqs>... xs)
            {
                f(a, xs...);
            };

            cartesian_for_each<decltype(inner), Seqs...>(inner, seqs...);
        }
    }

    // map_with_index

    template <typename F, typename... Seqs>
    auto map_with_index(const F &f, const Seqs &...seqs)
        -> typename std::enable_if<
            All<IsStaticCapacity<Seqs>...>::value && All<IsStaticLength<Seqs>...>::value,
            StaticArray<CallReturn_t<F, int, Element_t<Seqs>...>, MinStaticCapacity<Seqs...>::value>>::type
    {
        using R = CallReturn_t<F, int, Element_t<Seqs>...>;

        StaticArray<R, MinStaticCapacity<Seqs...>::value> result;

        for (int i = 0; i < MinStaticCapacity<Seqs...>::value; ++i)
        {
            result[i] = f(i, seqs[i]...);
        }

        return result;
    }

    template <typename F, typename... Seqs>
    auto map_with_index(const F &f, const Seqs &...seqs)
        -> typename std::enable_if<
            All<IsStaticCapacity<Seqs>...>::value && !All<IsStaticLength<Seqs>...>::value,
            StaticVector<CallReturn_t<F, int, Element_t<Seqs>...>, MinStaticCapacity<Seqs...>::value>>::type
    {
        using R = CallReturn_t<F, int, Element_t<Seqs>...>;

        const size_t result_length = min_length(seqs...);

        StaticVector<R, MinStaticCapacity<Seqs...>::value> result(result_length);

        for (int i = 0; i < result_length; ++i)
        {
            result[i] = f(i, seqs[i]...);
        }

        return result;
    }

    template <typename F, typename... Seqs>
    auto map_with_index(const F &f, const Seqs &...seqs)
        -> typename std::enable_if<
            !All<IsStaticCapacity<Seqs>...>::value,
            DynamicVector<CallReturn_t<F, int, Element_t<Seqs>...>>>::type
    {
        using R = CallReturn_t<F, int, Element_t<Seqs>...>;

        const size_t result_length = min_length(seqs...);

        DynamicVector<CallReturn_t<F, int, Element_t<Seqs>...>> result(result_length);

        for (int i = 0; i < result_length; ++i)
        {
            result[i] = f(i, seqs[i]...);
        }

        return result;
    }

    // cartesian_map
    // ! Maybe need to bechmark and optimize
    template <typename F, typename... Seqs>
    auto cartesian_map(const F &f, const Seqs &...seqs)
        -> typename std::enable_if<
            All<IsStaticCapacity<Seqs>...>::value && All<IsStaticLength<Seqs>...>::value,
            StaticArray<CallReturn_t<F, Element_t<Seqs>...>, StaticCapacityProduct<Seqs...>::value>>::type
    {
        using R = CallReturn_t<F, Element_t<Seqs>...>;

        StaticArray<R, StaticCapacityProduct<Seqs...>::value> result;
        size_t i = 0;

        const auto inner = [&](Element_t<Seqs>... xs)
        {
            result[i++] = f(xs...);
        };

        cartesian_for_each(inner, seqs...);

        return result;
    }

    template <typename F, typename... Seqs>
    auto cartesian_map(const F &f, const Seqs &...seqs)
        -> typename std::enable_if<
            All<IsStaticCapacity<Seqs>...>::value && !All<IsStaticLength<Seqs>...>::value,
            StaticVector<CallReturn_t<F, Element_t<Seqs>...>, StaticCapacityProduct<Seqs...>::value>>::type
    {
        using R = CallReturn_t<F, Element_t<Seqs>...>;

        const size_t result_length = size_t_product(length(seqs)...);

        StaticVector<R, StaticCapacityProduct<Seqs...>::value> result(result_length);
        size_t i = 0;

        const auto inner = [&](Element_t<Seqs>... xs)
        {
            result[i++] = f(xs...);
        };

        cartesian_for_each(inner, seqs...);

        return result;
    }

    template <typename F, typename... Seqs>
    auto cartesian_map(const F &f, const Seqs &...seqs)
        -> typename std::enable_if<
            !All<IsStaticCapacity<Seqs>...>::value,
            DynamicVector<CallReturn_t<F, Element_t<Seqs>...>>>::type
    {
        using R = CallReturn_t<F, Element_t<Seqs>...>;

        const size_t result_length = size_t_product(length(seqs)...);

        DynamicVector<CallReturn_t<F, Element_t<Seqs>...>> result(result_length);
        size_t i = 0;

        const auto inner = [&](Element_t<Seqs>... xs)
        {
            result[i++] = f(xs...);
        };

        cartesian_for_each(inner, seqs...);

        return result;
    }

    // ! temp
    struct Lambda
    {
        template <typename Tret = void, typename Tfp = Tret (*)(void *), typename T>
        static Tfp ptr(T &t)
        {
            fn<T>(&t);                            // Save callable to static void pointer in fn function.
            return (Tfp)lambda_ptr_exec<Tret, T>; // Return pointer of static member function
        }

        template <typename Tret, typename T>
        static Tret lambda_ptr_exec(void *data)
        {                                       // This member function takes any type of one argument
            return (Tret)(*(T *)fn<T>())(data); // Calling the function without argument will return stored void pointer
            // Change the poiner type of T which is the type of original callable.
            // Dereference the pointer to get original callable
            // Done!
        }

        template <typename T>
        static void *fn(void *new_fn = nullptr)
        {
            static void *fn;
            if (new_fn != nullptr)
                fn = new_fn;
            return fn;
        }
    };

    // struct Callable
    // {
    //     // How to get argument types and return types of lambda
    //     template <typename F, typename Fptr>
    //     static void to_ptr(F &f, Fptr &fptr)
    //     {
    //         // fn<F>(&f);
    //         inner_ptr = (void *)&f;                     // Save callable to static void pointer in fn function.
    //         fptr = (Fptr)call_inner_ptr<Tret, F>; // Return pointer of static member function
    //     }

    //     template <typename Tret, typename T>
    //     static Tret call_inner_ptr(Arg)
    //     {                                       // This member function takes any type of one argument
    //         return (Tret)(*(T *)fn<T>())(data); // Calling the function without argument will return stored void pointer
    //         // Change the poiner type of T which is the type of original callable.
    //         // Dereference the pointer to get original callable
    //         // Done!
    //     }

    //     // template <typename T>
    //     // static void *fn(void *new_fn = nullptr)
    //     // {
    //     //     static void *fn;
    //     //     if (new_fn != nullptr)
    //     //         fn = new_fn;
    //     //     return fn;
    //     // }

    //     static void* inner_ptr;
    // };

    template <typename A>
    class IsCallOperator
    {
        typedef char one;
        typedef long two;

        template <typename B>
        static one test(decltype(&B::operator()));

        template <typename B>
        static two test(...);

    public:
        static const bool value = sizeof(test<A>(0)) == sizeof(one);
    };

    // ArgumentHelper

    template <typename, bool>
    struct ArgumentHelper
    {
    };

    template <typename F>
    struct ArgumentHelper<F, true> : ArgumentHelper<decltype(&F::operator()), false>
    {
        // using type = typename
    };

    template <typename R, typename... Args>
    struct ArgumentHelper<R (*)(Args...), false>
    {
        using type = std::tuple<Args...>;
    };

    template <typename R, typename A, typename... Args>
    struct ArgumentHelper<R (A::*)(Args...), false>
    {
        using type = std::tuple<Args...>;
    };

    template <typename R, typename A, typename... Args>
    struct ArgumentHelper<R (A::*)(Args...) const, false>
    {
        using type = std::tuple<Args...>;
    };

    // Arguement_t

    template <typename F>
    using Argument_t = typename ArgumentHelper<F, IsCallOperator<F>::value>::type;

    // ReturnTypeHelper

    template <typename, typename>
    struct ReturnTypeHelper
    {
    };

    template <typename F, typename... Args>
    struct ReturnTypeHelper<F, std::tuple<Args...>>
    {
        using type = CallReturn_t<F, Args...>;
    };

    // ReturnType

    template <typename F>
    struct ReturnType : ReturnTypeHelper<F, Argument_t<F>>
    {
    };

    // Return_t

    template <typename F>
    using Return_t = typename ReturnType<F>::type;

}
#endif